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

	/* ldt 使用 gdt 的代码段与数据段 */
	p_proc->ldt_sel = SELECTOR_LDT_FIRST;
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
	p_proc->regs.eip = (u32)TestA;
	p_proc->regs.esp = (u32) task_stack + STACK_SIZE_TOTAL;
	p_proc->regs.eflags = 0x1202; 	// IF=1, IOPL=1, bit 2 is always 1

	k_reenter = -1;
	p_proc_ready 	= proc_table;
	restart();

	while(1){}
}


/*=======================================================================
					testA
; 测试用
=========================================================================*/
PUBLIC void TestA()
{
	int i = 0; 
	disp_pos = 0;
	while(1)
	{
		disp_str("A");
		disp_int(i++);
		disp_str(".");
		delay(1);
	}
}

/*=======================================================================
					testB
; 测试用
=========================================================================*/
PUBLIC void TestB()
{
	int i = 0x1000; 
	while(1)
	{
		disp_str("B");
		disp_int(i++);
		disp_str(".");
		delay(1);
	}
}