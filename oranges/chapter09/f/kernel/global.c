
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define 	GLOBAL_VARIABLES_HERE

#include 	"sys/config.h"
#include 	"type.h"
#include 	"sys/const.h"
#include 	"sys/protect.h"
#include	"sys/proto.h"
#include 	"sys/proc.h"
#include	"sys/global.h"
#include 	"sys/tty.h"
#include 	"sys/console.h"
#include 	"sys/hd.h"
#include 	"sys/fs.h"

PUBLIC 	struct proc 		proc_table[NR_TASKS + NR_PROCS];

PUBLIC  char 			task_stack[STACK_SIZE_TOTAL];

PUBLIC TASK 			task_table[NR_TASKS] = {
								{task_tty, STACK_SIZE_TTY, "TTY"},
								{task_sys, STACK_SIZE_SYS, "SYS"},
								{task_hd, STACK_SIZE_SYS, "HD"},
								{task_fs, STACK_SIZE_SYS, "FS"}};
PUBLIC TASK 			user_proc_table[NR_PROCS] = {
								{TestA, STACK_SIZE_TESTA, "TestA"},
								{TestB, STACK_SIZE_TESTB, "TestB"},
								{TestC, STACK_SIZE_TESTC, "TestC"}};

PUBLIC 	irq_handler 	irq_table[NR_IRQ];
PUBLIC  system_call 	sys_call_table[NR_SYS_CALL] = {sys_printx, sys_sendrec};

PUBLIC 	TTY 			tty_table[NR_CONSOLES];
PUBLIC 	CONSOLE 		console_table[NR_CONSOLES];

/* FS related below */
/************************************************************************************
 *	For dd_map[k],
 *  'k' is the driver nr. 
 *  
 *  Remeber to modify include/sys/const.h if the order is changed.
 ************************************************************************************/
struct dev_drv_map dd_map[] = {
	{INVALID_DRIVER},		/* 0: Unused */
	{INVALID_DRIVER},		/* 1: Reserved for floppy driver */
	{INVALID_DRIVER},		/* 2: Reserved for cdrom driver */
	{TASK_HD},				/* 3: Hard disk */
	{TASK_TTY},				/* 4: TTY */
	{INVALID_DRIVER}		/* 5: Reserved for scsi disk driver */
};

/**
 * 	6MB~7MB: buffer for FS
 */
PUBLIC u8 * 		fsbuf 		= (u8*)0x600000;
PUBLIC const int 	FSBUF_SIZE 	= 0x100000;
