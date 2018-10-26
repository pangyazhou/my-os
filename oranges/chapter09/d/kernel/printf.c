/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						printf.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "sys/const.h"
#include "sys/protect.h"
#include "sys/proto.h"
#include "string.h"
#include "sys/proc.h"
#include "sys/global.h"

/*=========================================================================
					int printf(const char* fmt, ...);
===========================================================================*/
PUBLIC 	int printf(const char* fmt, ...)
{
	int i;
	char buf[256];

	va_list arg = (va_list) ((char *)(&fmt) + 4);
	i = vsprintf(buf, fmt, arg);
	buf[i] = 0;
	printx(buf);
	return i;
}

