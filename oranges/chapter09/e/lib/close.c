/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						close.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "sys/const.h"
#include "sys/protect.h"
#include "string.h"
#include "sys/fs.h"
#include "sys/proc.h"
#include "sys/tty.h"
#include "sys/console.h"
#include "sys/global.h"
#include "sys/proto.h"
#include "sys/hd.h"
#include "stdio.h"

/*===============================================================================
						close
=================================================================================*/
/**
* <Ring 3> close a file descriptor.
=================================================================================*/
PUBLIC 	int close(int fd)
{
	MESSAGE msg;
	msg.type 	= CLOSE;
	msg.FD 		= fd;

	send_recv(BOTH, TASK_FS, &msg);
	return msg.RETVAL;
}