
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifdef 	GLOBAL_VARIABLES_HERE
#undef 	EXTERN
#define EXTERN
#endif

EXTERN int 		nr_current_console;
EXTERN int 		ticks;
EXTERN int 		disp_pos;
EXTERN u32  	k_reenter;
EXTERN u8 		gdt_ptr[6];
EXTERN struct descriptor	gdt[GDT_SIZE];
EXTERN u8 		idt_ptr[6];
EXTERN GATE 	idt[IDT_SIZE];

EXTERN TSS 		tss;
EXTERN struct proc* 	p_proc_ready;

extern 	struct proc 	proc_table[];
extern 	char 		task_stack[];
extern  TASK 		task_table[];
extern 	TASK 		user_proc_table[];
extern 	irq_handler irq_table[];
extern 	TTY 		tty_table[];
extern 	CONSOLE 	console_table[];
