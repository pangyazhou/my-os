/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						clock.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

/*=======================================================================
					clock_handler
=========================================================================*/
PUBLIC void clock_handler(int irq)
{
	ticks++;
	p_proc_ready->ticks--;
	if(k_reenter != 0)
	{
		return;
	}
	if(p_proc_ready->ticks > 0)
	{
		return;
	}
	schedule();
}

/*=======================================================================
					void init_clock();
=========================================================================*/
PUBLIC void init_clock()
{
		 /* 初始化 8253 PIT */
    out_byte(TIMER_MODE, RATE_GENERATOR);
    out_byte(TIMER0, (u8) (TIMER_FREQ/HZ));
    out_byte(TIMER0, (u8) ((TIMER_FREQ/HZ)));

	put_irq_handler(CLOCK_IRQ, clock_handler); 		/* 设定时钟中断处理程序 */
	enable_irq(CLOCK_IRQ);							/* 让8259A可以接收时钟中断 */
}