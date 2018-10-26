/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
					proc.h
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifndef 	_ORANGES_PROC_H_
#define 	_ORANGES_PROC_H_



typedef struct s_stackframe
{
	u32 	gs;
	u32 	fs;
	u32 	es;
	u32 	ds;
	u32 	edi;
	u32 	esi;
	u32 	ebp;
	u32 	kernel_esp;
	u32 	ebx;
	u32 	edx;
	u32 	ecx;
	u32 	eax;
	u32 	retaddr;
	u32 	eip;
	u32 	cs;
	u32 	eflags;
	u32 	esp;
	u32 	ss;
}STACK_FRAME;

struct proc
{
	STACK_FRAME regs; 		/* struct proc registers saved in stack frame */

	u16	 	ldt_sel; 		/* gdt selector giving ldt base and limit */
	struct descriptor 	ldts[LDT_SIZE]; 	/* local struct struct descriptors for code and data */

	int 	ticks; 			/* remiander ticks */
	int 	priority;
	u32 	pid; 			/* struct proc id passed in from MM */
	char 	p_name[32]; 	/* name of the struct proc */

	int 	p_flags; 		/* struct proc flags; A proc is runnable if p_flags == 0 */
	MESSAGE* 	p_msg;
	int 	p_recvfrom;
	int 	p_sendto;

	int 	has_int_msg; 	/* nonzero if an interrupt occurred when the task is not ready to deal with it */

	struct proc* q_sending;		/* queue of procs sending message to this proc */
	struct proc* next_sending; 	/* next proc in the sending queue (q_sending) */

	int 	nr_tty; 		/* 绑定的tty */

	struct file_desc * flip[NR_FILES];		/* 文件描述符指针 */
};

typedef struct s_task
{
	task_f 	initial_eip;
	int stacksize;
	char name[32];
}TASK;

#define proc2pid(x) (x - proc_table)

/* number of tasks */
#define NR_TASKS 	4
/* number of procs */
#define NR_PROCS 	3

/* number of system call */
#define NR_SYS_CALL 	2

/* stack of tasks */
#define STACK_SIZE_SYS		0x8000
#define STACK_SIZE_TTY 		0x8000
#define STACK_SIZE_HD 		0x8000
#define STACK_SIZE_FS 		0x8000
#define STACK_SIZE_TESTA 	0x8000
#define STACK_SIZE_TESTB 	0x8000
#define STACK_SIZE_TESTC 	0x8000


#define STACK_SIZE_TOTAL 	(STACK_SIZE_TESTA + STACK_SIZE_TESTB + STACK_SIZE_TESTC + \
 STACK_SIZE_TTY + STACK_SIZE_SYS + STACK_SIZE_HD + STACK_SIZE_FS)

#endif