
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            klib.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "sys/const.h"
#include "sys/protect.h"
#include "sys/proto.h"
#include "string.h"
#include "sys/proc.h"
#include "sys/global.h"

/*======================================================================
				itoa : 显示数字
========================================================================*/
/* 数字前面的 0 不被显示出来, 比如 0000B800 被显示成 B800 */
PUBLIC char * itoa(char * str, int num)
{
	char *	p = str;
	char	ch;
	int	i;
	int	flag = 0;

	*p++ = '0';
	*p++ = 'x';

	if(num == 0){
		*p++ = '0';
	}
	else{	
		for(i=28;i>=0;i-=4){
			ch = (num >> i) & 0xF;
			if(flag || (ch > 0)){
				flag = 1;
				ch += '0';
				if(ch > '9'){
					ch += 7;
				}
				*p++ = ch;
			}
		}
	}

	*p = 0;

	return str;
}
/*======================================================================*
                               disp_int
 *======================================================================*/
PUBLIC void disp_int(int input)
{
	char output[16];
	itoa(output, input);
	disp_str(output);
}

/*======================================================================*
                               delay
; 延时函数
 *======================================================================*/
PUBLIC void delay(int time)
{
	int i, j, k;
	for(k = 0; k < time; k++)
	{
		for(i = 0; i < 30; i++)
		{
			for(j = 0; j < 10000; j++)
			{
				
			}
		}
	}
}

/* 暂停函数 */
PUBLIC void pause()
{
	__asm__ __volatile__("hlt");
}
