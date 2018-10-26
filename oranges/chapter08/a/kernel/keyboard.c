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
#include "keymap.h"
#include "tty.h"


PRIVATE 	KB_INPUT 	kb_in;

PRIVATE 	int 	code_with_E0 = 0;
PRIVATE 	int 	shift_l = 0;
PRIVATE 	int 	shift_r = 0;
PRIVATE 	int 	alt_l = 0;
PRIVATE  	int 	alt_r = 0;
PRIVATE 	int 	ctrl_l = 0;
PRIVATE 	int 	ctrl_r = 0;
PRIVATE 	int 	caps_lock = 0;
PRIVATE 	int 	num_lock = 0;
PRIVATE 	int 	scroll_lock = 0;
PRIVATE 	int 	column;

PRIVATE 	u8 		get_byte_from_kbuf();
PRIVATE 	void 	set_leds();
/*====================================================================
				void init_keyboard();
======================================================================*/
PUBLIC	void init_keyboard()
{
	kb_in.count = 0;
	kb_in.p_head = kb_in.p_tail = kb_in.buf;
	code_with_E0 = 0;
	shift_l = 0;
	shift_r = 0;
	alt_l = 0;
	alt_r = 0;
	ctrl_r = 0;
	ctrl_l = 0;
	caps_lock = 0;
	num_lock = 1;
	scroll_lock = 0;
	set_leds();
	put_irq_handler(KEYBOARD_IRQ, keyboard_handler);
	enable_irq(KEYBOARD_IRQ);
}


/*====================================================================
				void keyboard_handler(int irq);
======================================================================*/
PUBLIC	void keyboard_handler(int irq)
{
	u8 scan_code = in_byte(KB_DATA);
	
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
PUBLIC void keyboard_read(TTY* p_tty)
{
	u8 scan_code;
	char output[2];
	int make; 		/* TRUE: make; FALSE: break; */

	u32 key = 0; 	/* 表示一个键 */
	u32* keyrow; 	/* 指向keymap[] 的某一行 */

	// memset(output, 0, 2);
	if(kb_in.count > 0)
	{
		code_with_E0 = 0;
		scan_code = get_byte_from_kbuf();

		/* 解析扫描码 */
		if(scan_code == 0xE1)
		{
			int i;
			u8 pausebrk_scode[] ={0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5};
			int is_pausebreak = 1;
			for(i = 1; i < 6; i++)
			{
				if(get_byte_from_kbuf() != pausebrk_scode[i])
				{
					is_pausebreak = 0;
					break;
				}
			}
			if(is_pausebreak)
			{
				key = PAUSEBREAK;
			}
		}
		else if(scan_code == 0xE0)
		{
			scan_code = get_byte_from_kbuf();
			/* PrintScreen 被按下 */
			if(scan_code == 0x2A)
			{
				if(get_byte_from_kbuf() == 0xE0)
				{
					if(get_byte_from_kbuf() == 0x37)
					{
						key = PRINTSCREEN;
						make = 1;
					}
				}
			}
			if(scan_code == 0xB7)
			{
				if(get_byte_from_kbuf() == 0xE0)
				{
					if(get_byte_from_kbuf() == 0xAA)
					{
						key = PRINTSCREEN;
						make = 0;
					}
				}
			}
			/* 不是PRINTSCREEN */
			if(key == 0)
			{
				code_with_E0 = 1;
			}
		}
		if((key != PAUSEBREAK) && (key != PRINTSCREEN))
		{
			/* 先判断是make 还是 break */
			make = (scan_code & FLAG_BREAK ? FALSE : TRUE);

			/* 定位到 keymap 中的行 */
			keyrow = &keymap[(scan_code & 0x7f) * MAP_COLS];

			column = 0;

			int caps = shift_l || shift_r;
			if(caps_lock)
			{
				if((keyrow[0] >= 'a') && (keyrow[0] <= 'z'))
				{
					caps = !caps;
				}
			}
			if(caps)
			{
				column = 1;
			}
			if(code_with_E0)
			{
				column = 2;
				code_with_E0 = 0;
			}
			key = keyrow[column];
			switch(key)
			{
				case SHIFT_L:
					shift_l = make;
					break;
				case SHIFT_R:
					shift_r = make;
					break;
				case CTRL_L:
					ctrl_l = make;
					break;
				case CTRL_R:
					ctrl_r = make;
					break;
				case ALT_L:
					alt_l = make;
					break;
				case ALT_R:
					alt_l = make;
					break;
				case CAPS_LOCK:
					if(make)
					{
						caps_lock = !caps_lock;
						set_leds();
					}
					break;
				case NUM_LOCK:
					if(make)
					{
						num_lock = !num_lock;
						set_leds();
					}
					break;
				case SCROLL_LOCK:
					if(make)
					{
						scroll_lock = !scroll_lock;
						set_leds();
					}
					break;
				default:
					break;
			}

			if(make)
			{
				int pad = 0;

				/* 处理小键盘 */
				if((key >= PAD_SLASH) && (key <= PAD_9))
				{
					pad = 1;
					switch(key)
					{
						case PAD_SLASH:
							key = '/';
							break;
						case PAD_STAR:
							key = '*';
							break;
						case PAD_MINUS:
							key = '-';
							break;
						case PAD_PLUS:
							key = '+';
							break;
						case PAD_ENTER:
							key = ENTER;
							break;
						default:
							if(num_lock && (key >= PAD_0) && (key <= PAD_9))
							{
								key = key - PAD_0 + '0';
							}
							else if(num_lock && (key == PAD_DOT))
							{
								key = '.';
							}
							else 
							{
								switch(key)
								{
									case PAD_HOME:
										key = HOME;
										break;
									case PAD_END:
										key = END;
										break;
									case PAD_PAGEUP:
										key = PAGEUP;
										break;
									case PAD_PAGEDOWN:
										key = PAGEDOWN;
										break;
									case PAD_INS:
										key = INSERT;
										break;
									case PAD_UP:
										key = UP;
										break;
									case PAD_DOWN:
										key = DOWN;
										break;
									case PAD_LEFT:
										key = LEFT;
										break;
									case PAD_RIGHT:
										key = RIGHT;
										break;
									case PAD_DOT:
										key = DELETE;
										break;
									default:
										break;
								}
							}
							break;
					}
				}

				key |= shift_l 	? FLAG_SHIFT_L 	: 	0;
				key |= shift_r 	? FLAG_SHIFT_R 	: 	0;
				key |= ctrl_l 	? FLAG_CTRL_L 	: 	0;
				key |= ctrl_r 	? FLAG_CTRL_R 	: 	0;
				key |= alt_l 	? FLAG_ALT_L 	: 	0;
				key |= alt_r 	? FLAG_ALT_R 	: 	0;
				key |= pad 		? FLAG_PAD 		: 	0;
				in_process(p_tty, key);
			}
		}

	}
}

/*====================================================================
				void get_byte_from_kbuf();
======================================================================*/
PRIVATE u8 get_byte_from_kbuf()
{
	u8 scan_code;
	while(kb_in.count <= 0){} 	/* 等待下一个字符到来 */
	disable_int();
	scan_code = *(kb_in.p_tail);
	kb_in.p_tail++;
	if(kb_in.p_tail == kb_in.buf + KB_IN_BYTES)
	{
		kb_in.p_tail = kb_in.buf;
	}
	kb_in.count--;
	enable_int();
	return scan_code;
}

/*====================================================================
				void kb_wait();
; 等待8042输入缓冲区为空
======================================================================*/
PRIVATE void kb_wait()
{
	u8 kb_stat;
	do
	{
		kb_stat = in_byte(KB_CMD);
	}while(kb_stat & 0x02);
}

/*====================================================================
				void kb_ack();
; 等待键盘响应
======================================================================*/
PRIVATE void kb_ack()
{
	u8 kb_read;
	do
	{
		kb_read = in_byte(KB_DATA);
	}while(kb_read != KB_ACK);
}

/*====================================================================
				void set_leds();
; 设置led灯状态
======================================================================*/
PRIVATE void set_leds()
{
	u8 leds = (caps_lock << 2) | (num_lock << 1) | scroll_lock;

	kb_wait();
	out_byte(KB_DATA, LED_CODE);
	kb_ack();
	kb_wait();
	out_byte(KB_DATA, leds);
	kb_ack();
}
