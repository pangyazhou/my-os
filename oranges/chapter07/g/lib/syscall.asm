; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                              syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

_NR_get_ticks 				equ 	0 			; 获取时钟中断发生次数功能号
_NR_write					equ 	1 			; 写显存功能号
INT_VECTIOR_SYS_CALL 		equ 	0x90 		; 中断调用号

	
global 	get_ticks
global 	write

bits 32
[section .text]
;=========================================================================
;							int get_ticks();
;=========================================================================
get_ticks:
	mov 	eax, _NR_get_ticks
	int 	INT_VECTIOR_SYS_CALL
	ret

;=========================================================================
;							int write(char* buf, int len);
;=========================================================================
write:
	mov 	eax, _NR_write
	mov  	ebx, [esp + 4]			; char* buf
	mov 	ecx, [esp + 8]			; int len
	int 	INT_VECTIOR_SYS_CALL
	ret