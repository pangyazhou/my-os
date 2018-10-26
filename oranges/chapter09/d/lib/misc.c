/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            misc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "sys/const.h"
#include "sys/protect.h"
#include "string.h"
#include "sys/proc.h"
#include "sys/tty.h"
#include "sys/console.h"
#include "sys/global.h"
#include "sys/keyboard.h"
#include "sys/proto.h"

/*======================================================================
				spin
========================================================================*/
/**
 * Compare memory areas.
 *
 * @param s1 	The 1st area.
 * @param s2 	The 2nd area.
 * @param n 	The first n bytes to be compared.
========================================================================*/
PUBLIC int memcmp(const void* s1, const void* s2, int n)
{
	if((s1 == 0) || (s2 == 0))
		return (s1 - s2);
	const char* p1 = (const char*)s1;
	const char* p2 = (const char*)s2;
	int i;
	for(i = 0; i < n; i++, p1++, p2++)
	{
		if(*p1 != *p2)
			return (*p1 - *p2);
	}
	return 0;
}

/*======================================================================
				spin
; 程序原地循环
========================================================================*/
PUBLIC 	void spin(char* func_name)
{
	printl("\nspinning in %s ...\n", func_name);
	while(1){}
}

/*======================================================================
					assertion_failure
========================================================================
* Invoked by assert().

* @param exp 				The failure expression itself.
* @param file 				__FILE__
* @param base_file 			__BASE_FILE__
* @param line 				__LINE__
========================================================================*/
PUBLIC void assertion_failure(char* exp, char* file, char* base_file, int line)
{
	printl("%c   assert(%s) failed: file: %s, base_file: %s, line: %d", 
		MAG_CH_ASSERT,
		exp, file, base_file, line);

	spin("assertion_failure()");

	/* should never arrive here */
	__asm__ __volatile__("ud2");
}

