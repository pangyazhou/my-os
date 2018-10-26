/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						clock.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "sys/const.h"
#include "sys/protect.h"
#include "sys/proto.h"
#include "string.h"
#include "sys/proc.h"
#include "sys/global.h"

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
    out_byte(TIMER0, (u8) ((TIMER_FREQ/HZ) >> 8));
    /*out_byte(TIMER0, 0xff);
    out_byte(TIMER0, 0xff);*/

	put_irq_handler(CLOCK_IRQ, clock_handler); 		/* 设定时钟中断处理程序 */
	enable_irq(CLOCK_IRQ);							/* 让8259A可以接收时钟中断 */
}

/*======================================================================*
                               milli_delay
; 高精度延时函数
 *======================================================================*/
PUBLIC void milli_delay(int milli_sec)
{
	int t = get_ticks();
	while(((get_ticks() - t) * 1000 / (HZ * 8)) < milli_sec){}
}