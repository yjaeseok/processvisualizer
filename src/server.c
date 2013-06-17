#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/unistd.h>
#include "sysproject.h"
#include "pstree.h"
#include "pstat.h"

#define __NR_TRACETASK (__NR_SYSCALL_BASE+376)
void send_msg(int ,char *);//send the message to client
void device_start(int flag, pid_t pid);//start device driver
void device_end(void);//end device driver
void error_print(char *);//print error message
pid_t dot_pid, led_pid;//attributes for device driver

void main(int argc, char **argv)
{
		int i;
		pid_t pid, lcd_pid;
		int serv_sock;
		int clnt_sock;
		struct sockaddr_in serv_addr;
		struct sockaddr_in clnt_addr;
		char prot[MSGSIZE]="\0";
		int str_len;
		char tmpbuf[16];
		socklen_t clnt_addr_size;
		dot_pid = led_pid = 0;

		if(argc!=2){//check input arguments count
				fprintf(stderr,"Usage : %s [PORT]\n",argv[0]);
				exit(1);
		}
		serv_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

		if(serv_sock==-1)
				error_print("socket() error");

		memset(&serv_addr,0,sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		serv_addr.sin_port = htons(atoi(argv[1]));

		if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
				error_print("bind() error");

		printf("SERVER IS RUNNING...\n");
		while(1){
				if(listen(serv_sock,5)==-1)//listen client
						error_print("listen() error");
				clnt_addr_size = sizeof(clnt_addr);
				clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
				if(clnt_sock == -1)
						error_print("accept() error");

				while(1){//iterate the require
						device_start(0,0);//start device driver
						printf("starting device success!\n");
						str_len = read(clnt_sock,prot,sizeof(prot));
						printf("%d\n",str_len);
						if(str_len<0)//check protocol length from client
								error_print("read() error");
						printf("protocol : %s\n",prot);

						if(!strncmp(prot,GET_LIST,14)) {//check protocol
								printf("Getting All Process List Start...\n");
								device_start(1,0);
								if(pstree()!=0){//call pstree function that is making pstree.dat file
										printf("pstree() error\n");
										send_msg(clnt_sock,"error");//if it is error, send error message to client
								}
								else
										send_msg(clnt_sock,"success");//if it is made successfully, send success message
						}
						else if(!strncmp(prot,GET_INFO,PROT_LEN))
						{//check protocol
								printf("Getting  Process Information Start...\n");
								pid = atoi(&prot[PROT_LEN]);
								printf("PID : %d\n",pid);
								device_start(2,pid);
								if(ssu_cp(pid)<0){//copy pid information
										printf("copy error\n");
										send_msg(clnt_sock,"error");//if error, send error message to client
								}
								if(syscall(__NR_TRACETASK, pid)<0){//system call
										printf("system call error\n");
										send_msg(clnt_sock,"error");
								}else{
										send_msg(clnt_sock,"success");//send success message
								}
						}
						else if(!strncmp(prot,DO_EXIT,14))
						{//getting exit message from client
								printf("Client Exited..\n");
								if((lcd_pid = fork()) < 0){
										printf("fork() error\n");
										send_msg(clnt_sock,"error");
								}
								else if(lcd_pid == 0){
										execl("./lcd", "./lcd", "Good", "Bye!", NULL);
										printf("lcd failed\n");
										send_msg(clnt_sock,"error");
								}
								while(wait((int*)0) != lcd_pid);
								sleep(1);
								device_end();
								close(clnt_sock);//close client socket
								break;
						}
						device_end();//device end
				}
		}
		printf("closed!\n");
		close(serv_sock);//close server socket
		exit(0);//terminate server program
}

//sending message to client
void send_msg(int sock,char *str)
{

		char prot[MSGSIZE]="\0";

		printf("send_msg\n");
		if(!strcmp(str,"error"))//if there is error
		{
				printf("error sending msessage\n");
				strcpy(prot,ERROR_MSG);//send error message to client
				write(sock,prot,sizeof(prot));
		}
		else if(!strcmp(str,"success"))//if everything is successfully done
		{
				printf("send success\n");
				strcpy(prot,SEND_FILE);//send success message to client
				write(sock,prot,sizeof(prot));
		}
		printf("send message done\n");
		sleep(1);
}

//starting device driver
void device_start(int flag, pid_t pid){
		char str[6];
		char vector[][3][16]={
				{"./lcd", "Hello,Process", "Visualizer"}, 
				{"./lcd", "Get", "ProcessTree"}, 
				{"./lcd", "Get", "ProcessInfo"}
		};

		sprintf(str, "%d", pid);

		device_end();

		if((pid = fork()) < 0){
				printf("fork error\n");
				exit(0);
		}
		else if(pid == 0){
				
				if(atoi(str) >= 1)
						execl("./fnd", "./fnd", str, NULL);
				else
						exit(0);
				printf("fnd faild\n");
				exit(0);
		}
		while(wait((int*)0) != pid);

		if((pid = fork()) < 0){
				printf("fork error\n");
				exit(0);
		}
		else if(pid == 0){
				execl("./motor", "./motor", "1");
				printf("motor faild\n");
				exit(0);
		}
		while(wait((int*)0) != pid);
		
		if((pid = fork()) < 0){
				printf("fork error\n");
				exit(0);
		}
		else if(pid == 0){

				execl("./lcd", vector[flag][0], vector[flag][1], vector[flag][2], NULL);
				printf("lcd faild\n");
				exit(0);
		}
		while(wait((int*)0) != pid);

	
		if((pid = fork()) < 0){
				printf("fork error\n");
				exit(0);
		}
		else if(pid == 0){
				execl("./dot", "./dot");
				printf("dot faild\n");
				exit(0);
		}
		dot_pid = pid;
	
		if((pid = fork()) < 0){
				printf("fork error\n");
				exit(0);
		}
		else if(pid == 0){
				execl("./led", NULL);
				printf("led faild\n");
				exit(0);
		}
		led_pid = pid;
		
		return ;

}


//ending device driver
void device_end(void){
		pid_t pid;

		if(led_pid > 0){
				kill(led_pid, SIGKILL);
				led_pid = 0;
		}
		if(dot_pid > 0){
				kill(dot_pid, SIGKILL);
				dot_pid = 0;
		}
		if((pid = fork()) < 0){
				printf("fork() for stopping motor error\n");
				exit(0);
		}
		else if(pid == 0){
				execl("./motor", "./motor", "0");
				printf("stopping motor error\n");
				exit(0);
		}
		while(wait((int *)0) != pid);
	
		return ;
}

void error_print(char *mesg)
{
		fputs(mesg,stderr);
		fputc('\n',stderr);
		exit(1);
}
