/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						start.c
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "sys/const.h"
#include "sys/protect.h"
#include "sys/proto.h"
#include "string.h"
#include "sys/proc.h"
#include "sys/global.h"



/*======================================================================*
                            cstart
 *======================================================================*/
PUBLIC void cstart()
{
	disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
		"-----------\"cstart\" begins-------\n");

	/*将LOADER中的gdt复制到新的gdt中*/
	memcpy(&gdt,  						/* New GDT */
			(void*)(*((u32*)(&gdt_ptr[2]))), 	/* Base of Old GDT */
			*((u16*)(&gdt_ptr[0])) + 1 			/* Limit of Old GDT */
		);
	/* gdt */
	u16* p_gdt_limit = (u16*)(&gdt_ptr[0]);
	u32* p_gdt_base = (u32*)(&gdt_ptr[2]);
	*p_gdt_limit = GDT_SIZE * sizeof(struct descriptor) - 1;
	*p_gdt_base = (u32)&gdt;

	/* idt */
	u16* p_idt_limit = (u16*)(&idt_ptr[0]);
	u32* p_idt_base = (u32*)(&idt_ptr[2]);
	*p_idt_limit = IDT_SIZE * sizeof(GATE) - 1;
	*p_idt_base = (u32)&idt;

	init_prot();	//初始化中断向量表

	disp_str("-----------\"cstart\" ends-------\n");
}