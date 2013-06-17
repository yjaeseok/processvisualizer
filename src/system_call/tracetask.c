#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <asm/current.h>
#include <asm/mman.h>
#include <asm/fcntl.h>
#include <asm/uaccess.h>

#define NAME_LEN 11

int make_filename(char *filename, int n, pid_t pid){
	char pid_num[11];
	char *extend = ".task";
	int i=0, j=0, k=0;

	while(pid){
		pid_num[i++] = (pid%10) + 48;
		pid /= 10;
	}

	while(i--){
		if(k > n)
			return -1;
		filename[k++] = pid_num[i];
	}
	for(j=0; j<6; j++){
		if(k > n)
			return -1;
		filename[k++] = extend[j];
	}
	return 0;
}

struct task_struct_mini 
{
	pid_t pid;
	pid_t ppid;
	pid_t real_ppid;
	int state;
	long flags;
};

asmlinkage int sys_tracetask(pid_t pid)
{
	struct file *file;
	char filename[NAME_LEN];
	ssize_t (*write)(struct file*, const char*, size_t, loff_t*);
	struct task_struct_mini task;
	int n, ret;
	struct task_struct *target;

	target = pid_task(find_vpid(pid), PIDTYPE_PID);
	if(target == NULL) {
		printk("find task error()\n");
		return -1;
	}

	mm_segment_t old_fs = get_fs();
	set_fs(KERNEL_DS);

	if(make_filename(filename, NAME_LEN, target->pid)) {
		printk("make_filename error()\n");
		return -2;
	}

	file = filp_open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0600);
	if(!file){
		printk("filp_open error()\n");
		return -3;
	}

	if(!(file->f_op) || !(write=file->f_op->write)){
		printk("f_op write error\n");
		filp_close(file, target->files);
		return -4;
	}

	task.pid = target->pid;
	task.ppid = target->parent->pid;
	task.real_ppid = target->real_parent->pid;
	task.state = target->state;
	task.flags = target->flags;

	/* write target task structure */
	n = sizeof(struct task_struct_mini);
	if((ret = write(file, (char*)&task, n, &file->f_pos)) != n){
		printk("tast_struct_mini write() error[%d]\n", ret);
		return -5;
	}
	
	filp_close(file, target->files);
	set_fs(old_fs);

	return 0;
}
