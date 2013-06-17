#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include "pstree.h"


#define BUFF_SIZE 1024

process *header;

FILE *pstreefp;

int pstree() {
		char buff[BUFF_SIZE];
		char cmd[BUFF_SIZE];
		char ppname[BUFF_SIZE];
		char path[BUFF_SIZE];
		FILE *fp;
		DIR *dp;
		struct dirent *dirp;
		int start_pid = 1;
		process *proc;
		process *prev = NULL;

		printf("pstree() start\n");

		// 프로세스 목록 정보를 저장할 파일열기 
		if ((pstreefp = fopen("/sdcard/nfsroot/pstree.dat", "w")) == NULL) {
				printf("fopen error()\n");
				return -1;
		}

		// /proc의 pid 리스트 얻기
		if ((dp = opendir("/proc")) == NULL) {
				printf("opendir error()\n");
				return -1;
		}

		// proc 파일 시스템안에 들어있는 폴더 검색
		while ((dirp = readdir(dp)) != NULL) {
				if (strcmp(dirp->d_name, ".") == 0 ||
								strcmp(dirp->d_name, "..") == 0)
						continue;

				// 숫자폴더인지 확인 (process 정보)
				if (isnumber(dirp->d_name) == 0)
						continue;

				// stat 파일 패스구하기
				sprintf(path, "/proc/%d/stat", atoi(dirp->d_name));
				if ((fp = fopen(path, "r")) == NULL)
						continue;

				fgets( buff, BUFF_SIZE, fp );
				proc = (process *)malloc(sizeof(process));
				proc->prev_process = NULL;
				proc->next_process = NULL;

				// stat 파일을 읽은 내용 (buff)을 process 구조체에 저장
				get_proc( buff, proc );

				if( header == NULL ) {
						header = proc; 
				} else {
						prev->next_process = proc;
				}
				proc->prev_process = prev;
				prev = proc;
		}
		closedir(dp);

		// 프로세스 이름으로 오름차순 정렬
		quick_sort( header->next_process, proc );
		// 프로세스 트리내용 출력
		print_process_tree(start_pid);
		fclose(pstreefp);

		// 변수 및 메모리 정보 초기화
		proc = header;
		while (proc != NULL) {
				prev = proc;
				proc = proc->next_process;
				free(prev);
				prev = NULL;
		}
		header = NULL;

		printf("pstree done\n");
		return 0;
}

// 프로세스 정보를 가진 문자열로 프로세스 구조체에 저장
void get_proc( char *buff, process * proc )
{
		char cmd[BUFF_SIZE];
		long unsigned int tmp_lu;
		int tmp_d;
		char tmp_c;
		char tmp_s[BUFF_SIZE];

		sscanf( buff, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld", &proc->pid, proc->pname, &tmp_c, &proc->ppid, &tmp_d, &tmp_d, &tmp_d, &tmp_d, &tmp_d, &tmp_lu, &tmp_lu,
						&tmp_lu, &tmp_lu, &tmp_lu, &tmp_lu, &tmp_lu, &tmp_lu, &tmp_lu, &tmp_lu, &proc->num_threads );

		if( strlen(proc->pname) > 0 
						&& proc->pname[strlen(proc->pname)-1] == ')' )
				proc->pname[strlen(proc->pname)-1] = '\0';

		if( strlen(proc->pname) > 0 
						&& proc->pname[0] == '(' )
				sprintf( proc->pname, "%s", &(proc->pname[1]) );
}

void print_process_tree(int start_pid)
{
		print_process_tree_child( "", start_pid, "", 1 );
}

void print_process_tree_child( char *buf, int pid, char *pname, int num_threads )
{
		char str[BUFF_SIZE];
		char white_space[BUFF_SIZE];
		char tmp[BUFF_SIZE];
		int child_cnt = 0;
		int i = 0 ;
		process *t = header;

		//printf("print_process_tree_child space : %d pid : %d \n", space, pid );
		memset(str, 0x20, sizeof(str));
		memset(white_space, 0x20, sizeof(white_space));

		while( t->next_process != NULL ) 
		{
				if( strlen(buf) == 0 && t->pid == pid ) {
						// init
						//sprintf( str,"%s(%d)", t->pname, t->pid );
						sprintf( str,"%s", t->pname );
						fprintf(pstreefp, "%s", str );
						sprintf( str,"%s%.*s", buf, strlen(t->pname) + 1, white_space );
						print_process_tree_child( str, t->pid, t->pname, 1 );
						return;
				} else {	
						if( t->ppid == pid ) {
								// 자식 프로세스를 찾는다.
								sprintf( tmp,"%s(%d)", t->pname, t->pid );
								if( child_cnt == 0 ) {
										sprintf( str,"─┬─%s", tmp );
								} else {
										sprintf( str,"%s├─%s", buf, tmp );
								}
								fprintf(pstreefp, "%s", str );
								sprintf( str,"%s│ %.*s", buf, strlen(tmp)+ 1, white_space );
								print_process_tree_child( str, t->pid, t->pname, t->num_threads );
								child_cnt++;
						}
				}
				t = (process*)t->next_process;
		}
		if( num_threads > 1 ) {
				// 쓰레드 정보를 출력
				for( i = 1; i < num_threads; i++ ) {
						if( child_cnt == 0 ) {
								sprintf( str,"─┬─{%s}", pname );
						} else if( i == num_threads - 1 ) {
								sprintf( str,"%s└─{%s}", buf, pname );
						} else {
								sprintf( str,"%s├─{%s}", buf, pname );
						}
						fprintf(pstreefp, "%s\n", str );
						child_cnt++;
				}
		} 

		if( child_cnt == 0 )
				fprintf(pstreefp, "\n");
}

// 두 프로세스 구조체의 정보를 교환
void swap( process *left, process *right ) 
{
		int t_pid, t_ppid, t_num_threads;
		char t_pname[BUFF_SIZE];
		memset(t_pname,0,sizeof(t_pname));

		t_pid = left->pid;
		t_ppid = left->ppid;
		t_num_threads = left->num_threads;
		strcpy( t_pname, left->pname );

		left->pid = right->pid;
		left->ppid = right->ppid;
		left->num_threads = right->num_threads;
		strcpy( left->pname, right->pname );

		right->pid = t_pid;
		right->ppid = t_ppid;
		right->num_threads = t_num_threads;
		strcpy( right->pname, t_pname );
}

// 프로세스 구조체를 이름 순으로 퀵소트 정렬하는 함수
void quick_sort( process *left, process *right )
{
		process *start;
		process *current, *old_current;
		char t_pname[BUFF_SIZE];

		if( left == right ) return;

		start = left;
		current = start->next_process;

		while( current->next_process != NULL )
		{
				if( strcmp( start->pname, current->pname ) < 0 ) {
						swap( start, current );
				} 

				if( current == right ) break;

				current = current->next_process;
		}

		swap( left, current );
		old_current = current;

		current = current->prev_process;
		if( current != NULL )
		{
				if(( left->prev_process != current ) && ( current->next_process != left ))
						quick_sort( left, current );
		}

		current = old_current;
		current = current->next_process;

		if( current != NULL )
		{
				if(( current->prev_process != right ) && (right->next_process != current ))
						quick_sort( current, right );
		}
}

// 숫자인지 여부를 판별하는 함수
int isnumber(const char* num)
{
		int i = 0;
		int ret = 1;
		while(num[i]) {
				if (!isdigit(num[i])) {
						ret = 0;
						break;
				}
				i++;
		}
		return ret;
}
