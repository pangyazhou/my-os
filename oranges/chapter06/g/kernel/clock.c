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
