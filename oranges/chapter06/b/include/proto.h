/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
					proto.h
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* klib.asm */
PUBLIC 	void 	out_byte(u16 port, u8 value);
PUBLIC 	u8 		in_byte(u16 port);
PUBLIC 	void    disp_str(char * info);
PUBLIC 	void 	disp_color_str(char * info, int color);

/* protect.c */
PUBLIC 	void 	init_prot();
PUBLIC u32 seg2phys(u16 seg);


/* i8259.c */
PUBLIC  void 	init_8259A();

/* main.c */
PUBLIC 	void 	TestA();

/* klib.c */
PUBLIC 	void 	disp_int(int input);
PUBLIC 	void 	delay(int time);

/* kernel.asm */
void restart();