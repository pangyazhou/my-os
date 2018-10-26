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

/*======================================================================*
                           task_tty
 *======================================================================*/
 PUBLIC void task_tty()
 {
 	while(1)
 	{
 		keyboard_read();
 	}
 }

 /*======================================================================*
                           in_process
 *======================================================================*/
 PUBLIC void in_process(u32 key)
 {
 	char output[2] = {'\0', '\0'};

 	if(!(key & FLAG_EXT))
 	{
 		output[0] = key & 0xff;
 		disp_color_str(output, 0x0f);

 		disable_int();
 		out_byte(CRTC_ADDR_REG, CURSOR_H);
 		out_byte(CRTC_DATA_REG, ((disp_pos/2)>>8)&0xff);
 		out_byte(CRTC_ADDR_REG, CURSOR_L);
 		out_byte(CRTC_DATA_REG, ((disp_pos/2))&0xff);
 		enable_int();
 	}
 	else
 	{
 		int raw_code = key & MASK_RAW;
 		switch(raw_code)
 		{
 			case UP:
 				if((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R))
 				{
 					disable_int();
 					out_byte(CRTC_ADDR_REG, START_ADDR_H);
 					out_byte(CRTC_DATA_REG, ((80*15) >> 8) & 0xff);
 					out_byte(CRTC_ADDR_REG, START_ADDR_L);
 					out_byte(CRTC_DATA_REG, ((80*15)) & 0xff);
 					enable_int();
 				}
 				break;
 			case DOWN:
 				if((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R))
 				{
 					/* do nothing */
 				}
 				break;
 			default:
 				break;
 		}

 	}
 }