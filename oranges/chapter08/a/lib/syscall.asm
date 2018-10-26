; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                              syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

_NR_printx					equ 	0 			; 写显存功能号
_NR_sendrec 				equ 	1 			; 消息发送接收系统调用功能号
INT_VECTIOR_SYS_CALL 		equ 	0x90 		; 中断调用号


global 	sendrec
global 	printx

bits 32
[section .text]

;=========================================================================
;							void printx(char* str);
;=========================================================================
printx:
	mov 	eax, _NR_printx
	mov  	edx, [esp + 4]			; char* str
	int 	INT_VECTIOR_SYS_CALL
	ret

;=========================================================================
;				int sendrec(int function, int src_dest, MESSAGE* msg)
;=========================================================================
sendrec:
	mov 	eax, _NR_sendrec
	mov 	ebx, [esp + 4]			; function
	mov 	ecx, [esp + 8]			; src_dest
	mov 	edx, [esp + 8] 			; MESSAGE* msg
	int 	INT_VECTIOR_SYS_CALL
	ret
