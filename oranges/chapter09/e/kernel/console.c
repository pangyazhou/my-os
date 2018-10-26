/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            console.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "sys/const.h"
#include "sys/protect.h"
#include "sys/proto.h"
#include "string.h"
#include "sys/proc.h"
#include "sys/global.h"
#include "sys/keyboard.h"
#include "sys/tty.h"
#include "sys/console.h"


PRIVATE void set_cursor(u32 position);
PRIVATE void set_video_start_addr(u32 addr);
PRIVATE void flush(CONSOLE* p_con);
/*====================================================================
							is_current_console
======================================================================*/
PUBLIC 	int is_current_console(CONSOLE* p_con)
{
	return (p_con == &console_table[nr_current_console]);
}


/*====================================================================
						void init_screen(TTY* p_tty);
======================================================================*/
PUBLIC void init_screen(TTY* p_tty)
{
	int nr_tty = p_tty - tty_table;
	p_tty->p_console = console_table + nr_tty;

	int v_mem_size = V_MEM_SIZE >> 1;			/* 显存总大小 (字为单位) */

	int con_v_mem_size 					= v_mem_size / NR_CONSOLES; /* 每个控制台显存大小 */
	p_tty->p_console->original_addr 	= nr_tty * con_v_mem_size;
	p_tty->p_console->v_mem_limit 		= con_v_mem_size;
	p_tty->p_console->current_start_addr= p_tty->p_console->original_addr;

	/* 默认光标在最开始处 */
	p_tty->p_console->cursor 			= p_tty->p_console->original_addr;

	if(nr_tty == 0)
	{
		/* 第一个控制台沿用原来的光标位置 */
		p_tty->p_console->cursor = disp_pos / 2;
		disp_pos = 0;
	}
	else
	{
		out_char(p_tty->p_console, nr_tty + '0');
		out_char(p_tty->p_console, '#');
	}


}

/*====================================================================
						void select_console(int nr_console);
======================================================================*/
PUBLIC void select_console(int nr_console)
{
	if((nr_console < 0) || (nr_console >= NR_CONSOLES))
	{
		return;
	}
	nr_current_console = nr_console;
	flush(&console_table[nr_console]);
}

/*====================================================================
				void scroll_screen(CONSOLE* p_con, int direction);
======================================================================*/
PUBLIC void scroll_screen(CONSOLE* p_con, int direction)
{
	/* 向上滚动一行 */
	if(direction == SCR_UP)
	{
		/* 显示起始地址大于控制台显存基址 */
		if(p_con->current_start_addr > p_con->original_addr)
		{
			p_con->current_start_addr -= SCREEN_WIDTH;
		}	
	}
	/* 向下滚动一行 */
	else if(direction == SCR_DN)
	{
		/* 当前显示最大地址小于控制台分配的显存最大地址 */
		if(p_con->current_start_addr + SCREEN_SIZE < p_con->original_addr + p_con->v_mem_limit)
		{
			p_con->current_start_addr += SCREEN_WIDTH;
		}
	}
	flush(p_con);
}


/*====================================================================
						void out_char(CONSOLE* p_con, char ch);
======================================================================*/
PUBLIC 	void out_char(CONSOLE* p_con, char ch)
{
	u8* p_vmem = (u8*) (V_MEM_BASE + p_con->cursor * 2);
	switch(ch)
	{
		case '\n':
			if(p_con->cursor < p_con->original_addr + p_con->v_mem_limit - SCREEN_WIDTH)
			{
				p_con->cursor = p_con->original_addr + SCREEN_WIDTH * 
						((p_con->cursor - p_con->original_addr) / SCREEN_WIDTH + 1);
			}
			break;
		case '\b':
			if(p_con->cursor > p_con->original_addr)
			{
				p_con->cursor--;
				*(p_vmem - 2) = ' ';
				*(p_vmem - 1) = DEFAULT_CHAR_COLOR;
			}
			break;
		default:
			if(p_con->cursor < p_con->original_addr + p_con->v_mem_limit -1)
			{
				*p_vmem++ = ch;
				*p_vmem++ = DEFAULT_CHAR_COLOR;
				p_con->cursor++;
			}
			break;
	}
	while(p_con->cursor >= p_con->current_start_addr + SCREEN_SIZE)
	{
		scroll_screen(p_con, SCR_DN);
	}
	flush(p_con);	
}

/*====================================================================
						void flush(CONSOLE* p_con);
======================================================================*/
PRIVATE void flush(CONSOLE* p_con)
{
	if(is_current_console(p_con))
	{
		set_cursor(p_con->cursor);
		set_video_start_addr(p_con->current_start_addr);
	}
}


/*====================================================================
						void set_cursor(u32 position);
======================================================================*/
PRIVATE void set_cursor(u32 position)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, CURSOR_H);
	out_byte(CRTC_DATA_REG, (position >> 8) & 0xff);
	out_byte(CRTC_ADDR_REG, CURSOR_L);
	out_byte(CRTC_DATA_REG, position & 0xff);
	enable_int();
}

/*====================================================================
						void set_video_start_addr(u32 position);
======================================================================*/
PRIVATE void set_video_start_addr(u32 addr)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, START_ADDR_H);
	out_byte(CRTC_DATA_REG, (addr >> 8) & 0xff);
	out_byte(CRTC_ADDR_REG, START_ADDR_L);
	out_byte(CRTC_DATA_REG, addr & 0xff);
	enable_int();
}