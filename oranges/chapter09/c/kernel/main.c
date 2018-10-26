/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

/*=======================================================================
					kernel_main
=========================================================================*/
PUBLIC void kernel_main()
{
	disp_str("-----\"kernel_main\" begins-----\n");

	struct proc* p_proc = proc_table;	/* p_proc 指向进程表第一个进程 */
	TASK* 	 p_task = task_table;
	char* 	 p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16 	 selector_ldt = SELECTOR_LDT_FIRST;
	int i;
	u8 		privilege;
	u8 		rpl;
	int 	eflags;
	int 	priority;
	for(i = 0; i < NR_TASKS + NR_PROCS; i++){
		if(i < NR_TASKS){
			p_task 		= task_table + i;
			privilege 	= PRIVILEGE_TASK;
			rpl 		= RPL_TASK;
			eflags 		= 0x1202;
			priority 	= 15;
		}
		else 
		{
			p_task 		= user_proc_table + (i - NR_TASKS);
			privilege 	= PRIVILEGE_USER;
			rpl 		= RPL_USER;
			eflags 		= 0x202; 			/* 无I/O操作权限 */
			priority 	= 5;
		}

		strcpy(p_proc->p_name, p_task->name);
		p_proc->pid = i;
		p_proc->ldt_sel = selector_ldt;
		/* ldt 使用 gdt 的代码段与数据段 */
		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(struct descriptor));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5; 		//change the DPL
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(struct descriptor));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;

		/* 设置段寄存器, 特权级1, LDT 描述符*/
		p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32) p_task_stack;
		p_proc->regs.eflags = eflags; 	// IF=1, IOPL=1, bit 2 is always 1	

		p_proc->nr_tty = 0;		/* 所属终端 */

		p_proc->p_flags = 0;
		p_proc->p_msg = 0;
		p_proc->p_recvfrom = NO_TASK;
		p_proc->p_sendto = NO_TASK;
		p_proc->has_int_msg = 0;
		p_proc->q_sending = 0;
		p_proc->next_sending = 0;
		
		p_proc->ticks = p_proc->priority = priority;
		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	k_reenter = 0;
	ticks = 0;
	p_proc_ready 	= proc_table;
	proc_table[4].nr_tty = 0;
	proc_table[5].nr_tty = 1;
	proc_table[6].nr_tty = 2;


    init_clock();
	init_keyboard();

	restart();

	while(1){}
}


/*=======================================================================
					testA
; 测试用
=========================================================================*/
PUBLIC void TestA()
{
	while(1)
	{
		//printf("<Ticks:%d>", get_ticks());
		milli_delay(200);
	}
}

/*=======================================================================
					testB
; 测试用
=========================================================================*/
PUBLIC void TestB()
{
	int i = 0;
	while(1)
	{
		//printf("B:%6d", i++);
		milli_delay(200);
	}
}

/*=======================================================================
					testC
; 测试用
=========================================================================*/
PUBLIC void TestC()
{
	int i = 0;
	char ch = 'C';
	while(1)
	{
		printf("<Ticks:%d>", get_ticks());
		milli_delay(2000);
	}
}

/*=======================================================================
					get_ticks
; 测试用
=========================================================================*/
PUBLIC int get_ticks()
{
	MESSAGE msg;
	reset_msg(&msg);
	msg.type = GET_TICKS;
	assert(msg.type == 2);
	send_recv(BOTH, TASK_SYS, &msg);
	return msg.RETVAL;
}

/*=======================================================================
					panic
; 测试用
=========================================================================*/
PUBLIC void panic(const char* fmt, ...)
{
	int i;
	char buf[256];

	va_list arg = (va_list)((char*)&fmt + 4);
	i = vsprintf(buf, fmt, arg);
	printl("%c !!panic!! %s", MAG_CH_PANIC, buf);

	/* should never arrive here */
	__asm__ __volatile__("ud2");		/* 内联汇编 */
}