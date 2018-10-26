/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            console.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef 	_ORANGES_CONSOLE_H_
#define 	_ORANGES_CONSOLE_H_

typedef struct s_console
{
	u32 	current_start_addr; 		/* 当前显示到的位置 */
	u32 	original_addr; 				/* 当前控制台对应的显存位置 */
	u32 	v_mem_limit; 				/* 当前控制台占的显存大小 */
	u32 	cursor; 					/* 当前光标位置 */
}CONSOLE;

#define 	SCR_UP				1
#define 	SCR_DN 				-1

#define 	SCREEN_WIDTH 		80
#define 	SCREEN_SIZE 		(80 * 25)

#define 	DEFAULT_CHAR_COLOR	(MAKE_COLOR(BLACK, WHITE))	/* 黑底白字 */
#define 	GRAY_CHAR 			(MAKE_COLOR(BLACK, BLACK) | BRIGHT)	
#define 	RED_CHAR 			(MAKE_COLOR(BLUE, RED) | BRIGHT)
#endif