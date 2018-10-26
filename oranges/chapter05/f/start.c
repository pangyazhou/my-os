/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						start.c
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"

PUBLIC void cstart()
{
	disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
		"-----------\"cstart\" begins-------\n");

	/*将LOADER中的gdt复制到新的gdt中*/
	memcpy(&gdt,  						/* New GDT */
			(void*)(*((u32*)(&gdt_ptr[2]))), 	/* Base of Old GDT */
			*((u16*)(&gdt_ptr[0])) + 1 			/* Limit of Old GDT */
		);
	u16* p_gdt_limit = (u16*)(&gdt_ptr[0]);
	u32* p_gdt_base = (u32*)(&gdt_ptr[2]);
	*p_gdt_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
	*p_gdt_base = (u32)&gdt;
}