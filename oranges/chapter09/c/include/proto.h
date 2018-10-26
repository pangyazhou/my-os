/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
					proto.h
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include "tty.h"
#include "console.h"
#include "proc.h"

/* klib.asm */
PUBLIC 	void 	out_byte(u16 port, u8 value);
PUBLIC 	u8 		in_byte(u16 port);
PUBLIC 	void    disp_str(char * info);
PUBLIC 	void 	disp_color_str(char * info, int color);
PUBLIC 	void 	enable_irq(int irq);
PUBLIC 	void 	disable_irq(int irq);
PUBLIC	void 	enable_int();
PUBLIC 	void 	disable_int();
PUBLIC 	void	port_read(u16 port, u8* buf, int size);
PUBLIC 	void 	port_write(u16 port, u8* buf, int size);

/* protect.c */
PUBLIC 	void 	init_prot();
PUBLIC 	u32 	seg2phys(u16 seg);


/* i8259.c */
PUBLIC  void 	init_8259A();
PUBLIC  void 	spurious_irq(int irq);
PUBLIC 	void 	put_irq_handler(int irq, irq_handler handler);

/* main.c */
PUBLIC 	void 	TestA();
PUBLIC 	void 	TestB();
PUBLIC 	void 	TestC();
PUBLIC 	int 	get_ticks();
PUBLIC 	void 	panic(const char* fmt, ...);

/* klib.c */
PUBLIC 	void 	disp_int(int input);
PUBLIC 	void 	delay(int time);
PUBLIC 	void 	milli_delay(int milli_sec);
PUBLIC 	char* 	itoa(char * str, int num);
PUBLIC 	void 	pause();

/* kernel.asm */
PUBLIC 	void 	restart();
PUBLIC  void 	sys_call();

/* clock.c */
PUBLIC 	void 	clock_handler(int irq); 
PUBLIC  void 	init_clock();	


/* keyboard.c */
PUBLIC	void 	init_keyboard();
PUBLIC  void 	keyboard_handler(int irq);
PUBLIC 	void 	keyboard_read(TTY* p_tty);

/* tty.c */
PUBLIC  void 	task_tty();
PUBLIC 	void 	in_process(TTY* p_tty, u32 key);
PUBLIC	void 	tty_write(TTY* p_tty, char* buf, int len);

/* console.c */
PUBLIC 	int 	is_current_console(CONSOLE* p_con);
PUBLIC 	void	out_char(CONSOLE* p_con, char ch);
PUBLIC 	void 	select_console(int nr_console);
PUBLIC 	void 	init_screen(TTY* p_tty);
PUBLIC 	void 	scroll_screen(CONSOLE* p_con, int direction);

/* printf.c */
PUBLIC 	int 	printf(const char* fmt, ...);
#define printl 	printf

/* vsprintf */
PUBLIC 	int 	vsprintf(char* buf, const char* fmt, va_list args);
PUBLIC 	int 	sprintf(char* buf, const char* fmt, ...);

/* 系统调用相关*/
/* proc.c */
PUBLIC 	void 	reset_msg(MESSAGE* msg);
PUBLIC 	void 	schedule();
PUBLIC 	int 	ldt_seg_linear(struct proc* p, int idx);
PUBLIC 	void* 	va2la(int pid, void* va);
PUBLIC 	void 	task_sys();
PUBLIC 	int 	sys_sendrec(int function, int src_dest, MESSAGE* msg, struct proc* p);
PUBLIC 	int 	send_recv(int function, int src_dest, MESSAGE* msg);
PUBLIC 	void 	dump_msg(const char * title, MESSAGE* m);
PUBLIC 	void 	inform_int(int task_nr);

/* tty.c */
PUBLIC 	int 	sys_printx(int _unused1, int _unused2, char* s, struct proc* p_proc);

/* syscall.asm */
PUBLIC 	void 	printx(char* str);
PUBLIC 	int 	sendrec(int function, int src_dest, MESSAGE* msg);

/* hd.c */
PUBLIC 	void 	task_hd();
PUBLIC 	void 	hd_handler();
PUBLIC 	u8 		get_hd_status();

/* fs/main.c */
PUBLIC 	void 	task_fs();
PUBLIC int rw_sector(int io_type, int dev, u64 pos, int bytes, int proc_nr,
			void* buf);

/* misc.c */
PUBLIC 	void 	spin(char* func_name);