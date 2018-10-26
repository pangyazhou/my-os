; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

%include "sconst.inc"

_NR_get_ticks 			equ 	0 			; 系统调用功能号
INT_VECTOR_SYS_CALL 	equ 	0x90 		; 系统调用号

global 	get_ticks

bits 32
[section .text]

get_ticks:
	mov 	eax, _NR_get_ticks
	int 	INT_VECTOR_SYS_CALL
	ret