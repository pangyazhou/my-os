/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						printf.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

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

