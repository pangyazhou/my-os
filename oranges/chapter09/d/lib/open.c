/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						open.c
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
						open
=================================================================================*/
/**
* <Ring 3> open/create a file.
=================================================================================*/
PUBLIC 	int open(char *pathname, int flags)
{
	MESSAGE msg;

	msg.type =	OPEN;
	msg.PATHNAME 	= (void*)pathname;
	msg.FLAGS 		= flags;
	msg.NAME_LEN 	= strlen(pathname); 

	send_recv(BOTH, TASK_FS, &msg);
	assert(msg.type == SYSCALL_RET);

	return msg.FD;
}