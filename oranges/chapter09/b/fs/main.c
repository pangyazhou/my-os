/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						include/fs/main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
#include "hd.h"

/*=========================================================================
						task_fs
===========================================================================*/
/**
* <Ring 1> The main loop of TASK FS.
* 
==========================================================================*/
PUBLIC void task_fs()
{
	int i;
	printl("Task FS begins.\n");

	init_fs();

	spin("FS");
}

/*=========================================================================
						init_fs
===========================================================================*/
/**
* <Ring 1> Do some preparation.
* 
==========================================================================*/
PRIVATE void init_fs()
{
	/* open the device: hard disk */
	MESSAGE driver_msg;
	driver_msg.type = DEV_OPEN;
	driver_msg.DEVICE = MINOR(ROOT_DEV);
	assert(dd_map[MAJOR(ROOT_DEV)].driver_nr != INVALID_DRIVER);
	send_recv(BOTH, dd_map[MAJOR(ROOT_DEV)].driver_nr, &driver_msg);
	

}



PUBLIC void write(u64 position, u32 size, void* buf)
{
	MESSAGE driver_msg;
	driver_msg.type = DEV_WRITE;
	driver_msg.DEVICE = MINOR(ROOT_DEV);
	driver_msg.BUF = buf;
	driver_msg.PROC_NR = TASK_FS;
	driver_msg.CNT = size;
	driver_msg.POSITION = position;
	assert(dd_map[MAJOR(ROOT_DEV)].driver_nr != INVALID_DRIVER);
	send_recv(BOTH, dd_map[MAJOR(ROOT_DEV)].driver_nr, &driver_msg);
	spin("FS WRITE");
}

PUBLIC void read(u64 position, u32 size, void* buf)
{
	MESSAGE driver_msg;
	driver_msg.type = DEV_READ;
	driver_msg.DEVICE = MINOR(ROOT_DEV);
	driver_msg.BUF = buf;
	driver_msg.PROC_NR = TASK_FS;
	driver_msg.CNT = size;
	driver_msg.POSITION = position;
	assert(dd_map[MAJOR(ROOT_DEV)].driver_nr != INVALID_DRIVER);
	send_recv(BOTH, dd_map[MAJOR(ROOT_DEV)].driver_nr, &driver_msg);
	spin("FS READ");
}