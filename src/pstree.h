#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 1024

typedef struct process {
	int pid;
	int ppid;
	char pname[BUFF_SIZE];
	
	struct process *prev_process;	/* Prev process pointer */
	struct process *next_process;	/* Next process pointer */
	int num_threads;	
} process ;
void print_process( process *proc );		/* print process */
void print_process_all();					/* print process all */
void print_process_tree( int stat_pid );	/* print process tree */
void print_process_tree_child( char *, int pid, char *pname, int num_threads );

void quick_sort( process *left, process *right );
void swap( process *left, process *right );
void get_proc( char *buff, process * proc );
int isnumber(const char*str);
int pstree();

