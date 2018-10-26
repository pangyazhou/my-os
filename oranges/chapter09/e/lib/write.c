/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						write.c
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
						write
=================================================================================*/
/**
* Write to a file descriptor.
*
* @param fd 		File descriptor.
* @param buf 		Buffer including the bytes to write.
* @param count 		How many bytes to write.
*
* @return 	On success, the number of bytes written are returnen.
* 			On error, -1 is returned.
=================================================================================*/
PUBLIC int write(int fd, const void* buf, int count)
{
	MESSAGE msg;

	msg.type 	= WRITE;
	msg.FD 		= fd;
	msg.BUF 	= (void*)buf;
	msg.CNT 	= count;

	send_recv(BOTH, TASK_FS, &msg);
	return msg.CNT;
}
