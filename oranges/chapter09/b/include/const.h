/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
					const.h
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


#ifndef 	_ORANGES_CONST_H_
#define 	_ORANGES_CONST_H_

/* the assert macro */
#define 	ASSERT 
#ifdef  	ASSERT 
void 	assertion_failure(char* exp, char *file, char* base_file, int line);
#define 	assert(exp) if(exp) ;\
			else assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)
#else
#define 	assert(exp)
#endif

#define EXTERN 	extern 

/*函数类型*/
#define PUBLIC
#define PRIVATE static

#define STR_DEFAULT_LEN 		1024

#include "config.h"

/* max() & min() */
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

/* Color */
#define BLACK 	0x0
#define WHITE 	0x7
#define RED 	0x4 
#define GREEN 	0X2 
#define BLUE 	0X1 
#define FLASH 	0X80 
#define BRIGHT  0X08
#define MAKE_COLOR(x,y) ((x<<4) | y) 	/* MAKE_COLOR(background, foreground) */

/* GDT 和 IDT 中描述符的个数 */
#define GDT_SIZE 	128
#define IDT_SIZE 	256

/* 权限 */
#define PRIVILEGE_KRNL 	0
#define PRIVILEGE_TASK 	1
#define PRIVILEGE_USER  3

/* RPL */
#define	RPL_KRNL	SA_RPL0
#define	RPL_TASK	SA_RPL1
#define	RPL_USER	SA_RPL3

/* struct proc */
#define SENDING 	0x02 		/* set when proc trying to send */
#define RECEIVING 	0x04 		/* set when proc trying to recv */


/* 8259A interrupt controller ports. */
#define INT_M_CTL 		0x20 	/* I/O port for interrupt controller */
#define INT_M_CTLMASK 	0x21	/* setting bits in this port disables ints */
#define INT_S_CTL 		0xa0
#define INT_S_CTLMASK 	0xa1


/* Hardware interrupts */
#define	NR_IRQ		16	/* Number of IRQs */
#define	CLOCK_IRQ	0
#define	KEYBOARD_IRQ	1
#define	CASCADE_IRQ	2	/* cascade enable for 2nd AT controller */
#define	ETHER_IRQ	3	/* default ethernet interrupt vector */
#define	SECONDARY_IRQ	3	/* RS232 interrupt vector for port 2 */
#define	RS232_IRQ	4	/* RS232 interrupt vector for port 1 */
#define	XT_WINI_IRQ	5	/* xt winchester */
#define	FLOPPY_IRQ	6	/* floppy disk */
#define	PRINTER_IRQ	7
#define	AT_WINI_IRQ	14	/* at winchester */


/* 8253/8254 PIT (Programmable Interval Timer) */
#define TIMER0         0x40 /* I/O port for timer channel 0 */
#define TIMER_MODE     0x43 /* I/O port for timer mode control */
#define RATE_GENERATOR 0x34 /* 00-11-010-0 :
			     * Counter0 - LSB then MSB - rate generator - binary
			     */
#define TIMER_FREQ     1193182L/* clock frequency for timer in PC and AT */
#define HZ             20  /* clock freq (software settable on IBM-PC) */


/* FALSE  TRUE */
#define TRUE 	1
#define FALSE 	0

/* AT keyboard */
/* 8042 ports */
#define KB_DATA		0x60	/* I/O port for keyboard data
					Read : Read Output Buffer
					Write: Write Input Buffer(8042 Data&8048 Command) */
#define KB_CMD		0x64	/* I/O port for keyboard command
					Read : Read Status Register
					Write: Write Input Buffer(8042 Command) */
#define LED_CODE 	0xED
#define KB_ACK 		0xFA


/* VGA */
#define	CRTC_ADDR_REG	0x3D4	/* CRT Controller Registers - Addr Register */
#define	CRTC_DATA_REG	0x3D5	/* CRT Controller Registers - Data Register */
#define	START_ADDR_H	0xC	/* reg index of video mem start addr (MSB) */
#define	START_ADDR_L	0xD	/* reg index of video mem start addr (LSB) */
#define	CURSOR_H	0xE	/* reg index of cursor position (MSB) */
#define	CURSOR_L	0xF	/* reg index of cursor position (LSB) */
#define	V_MEM_BASE	0xB8000	/* base of color video memory */
#define	V_MEM_SIZE	0x8000	/* 32K: B8000H -> BFFFFH */


/* tasks */
#define INVALID_DRIVER 	-20
#define INTERRUPT 		-10
#define TASK_TTY 		0
#define TASK_SYS 		1
#define TASK_HD 		2
#define TASK_FS 		3

/* TTY */
#define NR_CONSOLES	 3 		/* consoles */

/* */
#define ANY 		(NR_TASKS + NR_PROCS + 10)
#define NO_TASK 	(NR_TASKS + NR_PROCS + 20)

/* ipc */
#define SEND 		1
#define RECEIVE 	2
#define BOTH 		3 

/* magic chars used by 'printx' */
#define MAG_CH_PANIC 		'\002'
#define MAG_CH_ASSERT 		'\003'

enum msgtype {
	/* 
	 * when hard interrupt occurs, a msg (with type==HARD_INT) will
	 * be sent to some tasks
	 */
	HARD_INT = 1,

	/* SYS task */
	GET_TICKS,

	/* message type for drivers */
	DEV_OPEN = 1001,
	DEV_CLOSE,
	DEV_READ,
	DEV_WRITE,
	DEV_IOCTL
};

/* macros for messages */
#define RETVAL 		u.m3.m3i1
#define REQUEST 	u.m3.m3i2
#define CNT 		u.m3.m3i2
#define PROC_NR 	u.m3.m3i3
#define DEVICE 		u.m3.m3i4
#define POSITION 	u.m3.m3l1
#define BUF 		u.m3.m3p2



#define DIOCTL_GET_GEO 	1

/* Hard Drive */
#define SECTOR_SIZE 	512
#define SECTOR_BITS 	(SECTOR_SIZE * 8)
#define SECTOR_SIZE_SHIFT 	9

/* major device numbers */
#define NO_DEV 			0
#define DEV_FLOPPY 		1
#define DEV_CDROM 		2
#define DEV_HD 			3
#define DEV_CHAR_TTY	4
#define DEV_SCSI 		5

/* make device number from major and minor numbers */
#define MAJOR_SHIFT 	8
#define MAKE_DEV(a,b) 		((a << MAJOR_SHIFT) | b)

/* separate major and minor numbers from device number */
#define MAJOR(x) 			((x >> MAJOR_SHIFT) & 0xff)
#define MINOR(x) 			(x & 0xff)


#define MAX_DRIVES 		2
#define NR_PART_PER_DRIVE 	4
#define NR_SUB_PER_PART 	16
#define NR_SUB_PER_DRIVE 	(NR_PART_PER_DRIVE * NR_SUB_PER_PART)
#define NR_PRIM_PER_DRIVE 	(NR_PART_PER_DRIVE + 1)

/**
 * @def MAX_PRIM
 * Defines the max minor number of the primary partitions.
 * If there are 2 disks, prim_dev ranges in hd[0-9], this macro
 * will equals 9.
 */
#define MAX_PRIM 			(MAX_DRIVES * NR_PRIM_PER_DRIVE - 1)
#define MAX_SUBPARTITIONS 	(NR_SUB_PER_DRIVE * MAX_DRIVES)

/* device numbers of hard disk */
#define MINOR_hd1a 			0x10
#define MINOR_hd2a 			(MINOR_hd1a + NR_SUB_PER_PART)

#define ROOT_DEV 			MAKE_DEV(DEV_HD, MINOR_BOOT)

#define P_PRIMARY 			0
#define P_EXTENDED 			1

#define ORANGES_PART 		0x99 		/* Orange's partition */
#define NO_PART 			0x00 		/* unused entry */
#define EXT_PART 			0x05		/* extended partition */


#endif