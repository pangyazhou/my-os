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

	PROCESS* p_proc = proc_table;	/* p_proc 指向进程表第一个进程 */
	TASK* 	 p_task = task_table;
	char* 	 p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16 	 selector_ldt = SELECTOR_LDT_FIRST;
	int i;
	for(i = 0; i < NR_TASKS; i++){
		strcpy(p_proc->p_name, p_task->name);
		p_proc->pid = i;

		p_proc->ldt_sel = selector_ldt;
		/* ldt 使用 gdt 的代码段与数据段 */
		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5; 		//change the DPL
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;

		/* 设置段寄存器, 特权级1, LDT 描述符*/
		p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | RPL_TASK;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32) p_task_stack;
		p_proc->regs.eflags = 0x1202; 	// IF=1, IOPL=1, bit 2 is always 1	

		p_proc->ticks = p_proc->priority = (NR_TASKS - i) * 10;
		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	k_reenter = 0;
	ticks = 0;
	p_proc_ready 	= proc_table;



    init_clock();
	init_keyboard();
	
    disp_pos = 0;
    for(i = 0; i < 80 * 25; i++)
    {
    	disp_str(" ");
    }
    disp_pos = 0;

	restart();

	while(1){}
}


/*=======================================================================
					testA
; 测试用
=========================================================================*/
PUBLIC void TestA()
{
	disp_pos = 0;
	while(1)
	{
		// disp_str("A.");
		milli_delay(10);
	}
}

/*=======================================================================
					testB
; 测试用
=========================================================================*/
PUBLIC void TestB()
{
	while(1)
	{
		// disp_str("B.");
		milli_delay(10);
	}
}

/*=======================================================================
					testC
; 测试用
=========================================================================*/
PUBLIC void TestC()
{
	while(1)
	{
		// disp_str("C.");
		milli_delay(10);
	}
}