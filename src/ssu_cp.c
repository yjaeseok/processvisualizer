#include<stdio.h>
#include<stdlib.h>

int ssu_cp(pid_t);//cp with no -reverse option

//copying the pid information file 
int ssu_cp(pid_t pid)
{
		FILE *s_fp,*d_fp;
		char s_buf[256]="\0";
		char d_buf[256]="\0";
		int c;

		sprintf(s_buf,"/proc/%d/status",pid);

		printf("copy start pid:%d\n",pid);

		sprintf(d_buf,"/sdcard/nfsroot/%d.status",pid);
		if((s_fp=fopen(s_buf,"r"))==NULL){
				printf("%s fopen error\n",s_buf);
				return -1;
		}//open /proc/pid/status file
		if((d_fp=fopen(d_buf,"w"))==NULL){
				printf("%s fopen error\n",d_buf);
				return -1;
		}//make pid.status file
		//open SOURCE file and DEST file

		while((c=fgetc(s_fp))!=EOF)
				fputc(c,d_fp);
		//read from the source file and write to dest file

		printf("end copy\n");
		fclose(d_fp);
		fclose(s_fp);
		return ;
		
}

