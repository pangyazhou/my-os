/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            tty.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef 	_ORANGES_TTY_H_
#define 	_ORANGES_TTY_H_

#define 	TTY_IN_BYTES 	256 	/* tty input queue size */

struct s_console;

/* TTY */
typedef struct s_tty
{
	u32 	in_buf[TTY_IN_BYTES]; 		/* tty 输入缓冲区 */
	u32* 	p_inbuf_head; 				/* 指向缓冲区下一个空闲位置 */
	u32* 	p_inbuf_tail; 				/* 指向键盘任务应该处理的键值 */
	int 	inbuf_count; 				/* 缓冲区已经填充了多少 */

	struct s_console * 	p_console; 		/* 所属的控制台 */
}TTY;

#endif