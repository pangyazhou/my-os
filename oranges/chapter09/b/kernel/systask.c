/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						systask.c
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"

/*===============================================================================
					void task_sys();
=================================================================================*/
/**
* <Ring 1> The main loop of TASK SYS.
=================================================================================*/

PUBLIC void task_sys()
{
	MESSAGE msg;
	while(1)
	{
		send_recv(RECEIVE, ANY, &msg);
		int src = msg.source;

		switch(msg.type)
		{
			case GET_TICKS:
				msg.RETVAL = ticks;
				send_recv(SEND, src, &msg);
				break;
			default:
				assert(src == 2);
				assert(msg.type == 2);
				__asm__ __volatile__("hlt");
				panic("unknown msg type");
				break;
		}
	}
}
