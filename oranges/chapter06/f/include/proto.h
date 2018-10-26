/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
					proto.h
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* klib.asm */
PUBLIC 	void 	out_byte(u16 port, u8 value);
PUBLIC 	u8 		in_byte(u16 port);
PUBLIC 	void    disp_str(char * info);
PUBLIC 	void 	disp_color_str(char * info, int color);
PUBLIC 	void 	enable_irq(int irq);
PUBLIC 	void 	disable_irq(int irq);

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

/* klib.c */
PUBLIC 	void 	disp_int(int input);
PUBLIC 	void 	delay(int time);
PUBLIC 	void 	milli_delay(int milli_sec);

/* kernel.asm */
PUBLIC 	void 	restart();
PUBLIC  void 	sys_call();

/* clock.c */
PUBLIC 	void 	clock_handler(int irq); 	

/* proc.c */
PUBLIC int sys_get_ticks();

/* syscall.asm */
PUBLIC int get_ticks();