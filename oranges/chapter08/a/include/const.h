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
#define HZ             100  /* clock freq (software settable on IBM-PC) */


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

#endif