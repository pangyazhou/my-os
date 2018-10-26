/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            keyboard.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "keyboard.h"


PRIVATE 	KB_INPUT 	kb_in;
/*====================================================================
				void init_keyboard();
======================================================================*/
PUBLIC	void init_keyboard()
{
	kb_in.count = 0;
	kb_in.p_head = kb_in.p_tail = kb_in.buf;
	put_irq_handler(KEYBOARD_IRQ, keyboard_handler);
	enable_irq(KEYBOARD_IRQ);
}


/*====================================================================
				void keyboard_handler(int irq);
======================================================================*/
PUBLIC	void keyboard_handler(int irq)
{
	u8 scan_code = in_byte(0x60);
	
	if(kb_in.count < KB_IN_BYTES)
	{
		*(kb_in.p_head) = scan_code;
		kb_in.p_head++;
		if(kb_in.p_head == kb_in.buf + KB_IN_BYTES)
		{
			kb_in.p_head = kb_in.buf;
		}
		kb_in.count++;
	}
}

/*====================================================================
				void keyboard_read();
======================================================================*/
PUBLIC void keyboard_read()
{
	u8 scan_code;
	if(kb_in.count > 0)
	{
		disable_int();
		scan_code = *(kb_in.p_tail);
		kb_in.p_tail++;
		if(kb_in.p_tail == kb_in.buf + KB_IN_BYTES)
		{
			kb_in.p_tail = kb_in.buf;
		}
		kb_in.count--;
		enable_int();

		disp_int(scan_code);
	}
}