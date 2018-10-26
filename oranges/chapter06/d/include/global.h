
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifdef 	GLOBAL_VARIABLES_HERE
#undef 	EXTERN
#define EXTERN
#endif

EXTERN int 		disp_pos;
EXTERN u32  	k_reenter;
EXTERN u8 		gdt_ptr[6];
EXTERN DESCRIPTOR	gdt[GDT_SIZE];
EXTERN u8 		idt_ptr[6];
EXTERN GATE 	idt[IDT_SIZE];

EXTERN TSS 		tss;
EXTERN PROCESS* 	p_proc_ready;

extern 	PROCESS 	proc_table[];
extern 	char 		task_stack[];
extern  TASK 		task_table[];
extern 	irq_handler irq_table[];
