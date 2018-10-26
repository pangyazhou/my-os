/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						read.c
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
						read
=================================================================================*/
/**
* Read from a file descriptor
*
* @param fd		File descriptor
* @param buf 	Buffer to accept the bytes read.
* @param count 	How many bytes to read.
*
* @return On success, the number of bytes read are returned.
*		  On error, -1 is returned.
=================================================================================*/
PUBLIC int read(int fd, void* buf, int count)
{
	MESSAGE msg;
	msg.type 	= READ;
	msg.FD 		= fd;
	msg.BUF 	= buf;
	msg.CNT 	= count;

	send_recv(BOTH, TASK_FS, &msg);

	return msg.CNT;
}
