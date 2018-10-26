/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						systask.c
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



#include "type.h"
#include "sys/const.h"
#include "sys/protect.h"
#include "string.h"
#include "sys/proc.h"
#include "sys/tty.h"
#include "sys/console.h"
#include "sys/global.h"
#include "sys/keyboard.h"
#include "sys/proto.h"

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
