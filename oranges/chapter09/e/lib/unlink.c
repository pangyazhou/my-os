/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						unlink.c
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

/*=========================================================================
						unlink
===========================================================================*/
/**
* Delete a file.
*
* @param pathname 	The full path of the file to delete.
*
* @return 	Zero if successful, otherwise -1. 
==========================================================================*/
PUBLIC 	int unlink(const char * pathname)
{
	MESSAGE msg;

	msg.type 		= UNLINK;
	msg.PATHNAME 	= (void*)pathname;
	msg.NAME_LEN 	= strlen(pathname);

	send_recv(BOTH, TASK_FS, &msg);

	return msg.RETVAL;
}