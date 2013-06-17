#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include"sysproject.h"


int read_ps(int);//read pstree.dat pstree of file that is from server
int require_ps(int);//send protocol of making pstree file to server
int require_pid(int,char *);//send protocol of making process information to server
int require_exit(int);//send protocol that client is exiting the program
int read_file(int, char *);//read the process information file from server
void error_print(int,char *);//print error message and send the error protocol to server

void main(int argc,char **argv)
{
		int sock;
		int select;//selecting the menu
		char pid[4];
		struct sockaddr_in serv_addr;

		if(argc!=3)//check the input 
		{
				fprintf(stderr,"Usage :%s [IP] [PORT]\n",argv[0]);
				exit(1);
		}
		sock = socket(PF_INET,SOCK_STREAM,0);
		//make socket
		if(sock==-1){
				fprintf(stderr,"socket() error");
				exit(1);
		}

		memset(&serv_addr,0,sizeof(serv_addr));

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
		serv_addr.sin_port = htons(atoi(argv[2]));

		if(connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr))==ERROR)
		{
				fprintf(stderr,"connect() error\n");
				exit(1);
		}//check connection

		while(1)
		{
				//Menu
				system("clear");
				printf("*****PROCESS VISUALIZER*****\n");
				printf("1.GET PROCESS LIST\n");
				printf("2.GET PROCESS INFO\n");
				printf("3.EXIT PROGRAM\n");
				printf("SELECT MENU : ");
				scanf("%d",&select);
				switch(select)
				{
						case 1://requiring pstree file to server
								if(require_ps(sock)==ERROR)
										error_print(sock,"FAIL TO GET LIST");
								if(read_ps(sock)==ERROR)//read pstree file from server
										error_print(sock,"FIAL TO GET LIST");
								break;
						case 2://require process information file to server
								if(require_pid(sock,pid)==ERROR)
										error_print(sock,"FAIL TO GET INFO");
								if(read_file(sock,pid)==ERROR)//read pid info file 
										error_print(sock,"FAIL TO GET FILE");
								break;
						case 3://send exiting protocol to server
								if(require_exit(sock)==ERROR)
										error_print(sock,"FAIL TO EXIT");
								printf("EXIT PROCESS VISUALIZER..\n");
								close(sock);//close socket
								sleep(2);
								exit(0);
				}
				printf("Success!!\n");
				sleep(1);

		}
}

//read pstree file that is from server synchronized by nfs
int read_ps(int sock)
{
		int fd;
		char prot[MSGSIZE] = "\0";
		char buf[1024]="\0";
		int str_len;
		int n;

		str_len=read(sock,prot,sizeof(prot));
		//wait until get SEND_FILE protocol from server
		//that mean is done that is making the pstree file by server
		if(str_len==ERROR)//check protocol length sended from server 
		{
				printf("read() error\n");
				return -1;
		}

		printf("GET MESSAGE\n");

		if(strncmp(prot,SEND_FILE,10))//check protocol
		{
				printf("PROTOCOL ERROR\n");
				return -1;
		}

		if((fd=open("/nfsroot/pstree.dat",O_RDONLY))==ERROR){
		//open pstree.dat file that is made by server and synchronized by nfs
				printf("read_ps() error\n");
				return ERROR;
		}

		while((n=read(fd,buf,sizeof(buf)))>0)
				printf("%s\n",buf);
		//read and print the pstree.dat file 

		getchar();
		getchar();
		//wait until user type enter or any key
		close(fd);
		return 0;
}

//read the file that has process information that user wanted
//server get pid from client, and server make the file and send by using nfs
int read_file(int sock,char *pid)
{
		int n;
		char prot[MSGSIZE]="\0";
		int str_len;
		char buf[MSGSIZE]="\0";
		int num;


		str_len=read(sock,prot,sizeof(prot));
		//wait until server send the SEND_FILE protocol that means 
		//server is done to make the file which is ordered from client
		if(str_len==ERROR)//check protocol length
		{
				printf("read() error\n");
				return -1;
		}

		printf("GET MESSAGE\n");

		if(strncmp(prot,SEND_FILE,10))//check is it right protocol
		{
				printf("PROTOCOL ERROR\n");
				return -1;
		}

		num = atoi(pid);//make char to integer that is pid typed by client

		if(pstat(num)<0)//read file and display the information
		{
				printf("pstat() error\n");
				return -1;
		}
		return 0;
}

//send the protocol that require pstree.dat file to server 
int require_ps(int sock)
{
		char prot[MSGSIZE] = GET_LIST;
		prot[MSGSIZE]='\0';
		int len=0;


		if(write(sock,prot,sizeof(prot))==ERROR)//send protocol GET_LIST which means client wants to see process list tree that is running in the board
		{
				printf("SENDING PROTOCOL ERROR\n");
				return -1;
		}
		return 0;
}

//send protocol that client will exit server to server
int require_exit(int sock)
{
		char prot[MSGSIZE] = DO_EXIT;
		prot[MSGSIZE] = '\0';

		//send protocol DO_EXIT that means client will exit the server
		if(write(sock,prot,sizeof(prot))==ERROR)
		{
				printf("SENDING PROTOCOL ERROR\n");
				return -1;
		}
		return 0;
}

//send protocol to server that client need pid's information.
//pid is from user that is typed
int require_pid(int sock,char *pid)
{
		int i,j,str_len;
		char tmp[4]="\0";
		char prot[MSGSIZE] = GET_INFO;

		int len;

		printf("PID : ");//get the pid from user
		scanf("%s",tmp);

		str_len = strlen(tmp);

		if(str_len!= 4)
		{
				if(str_len==0)
				{
						printf("Wrong pid NUMBER\n");
						return -1;
				}

				for(i=str_len-1,j=3;j>-1;j--)
				{
						if(i<0)
								pid[j]='0';
						else
								pid[j]=tmp[i--];
				}
		}
		else
				strcpy(pid,tmp);
		//change the pid value to format to add protocol


		strcat(prot,pid);//add each protocol and pid value

		if(write(sock,prot,sizeof(prot))==ERROR)//send the protocol
		{
				printf("write() error\n");
				return -1;
		}
		return 0;

}

//print the error message and send the error message 
//to server 
void error_print(int sock,char *str)
{
		require_exit(sock);//send the exit message to server
		fprintf(stderr,"%s\n",str);//print error message
		exit(1);//exit the client program
}
