; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                              syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

NK_get_ticks 				equ 	0 			; 功能号
INT_VECTIOR_SYS_CALL 		equ 	0x90 		; 中断调用号

	
global 	get_ticks

bits 32
[section .text]
;=========================================================================
;							int get_ticks();
;=========================================================================
get_ticks:
	mov 	eax, NK_get_ticks
	int 	INT_VECTIOR_SYS_CALL
	ret