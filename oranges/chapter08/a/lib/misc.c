/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            misc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"

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

