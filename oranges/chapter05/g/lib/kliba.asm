; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                              klib.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

[section .data]
disp_pos	dd 	0

[section .text]

; 导出函数
global 	disp_str

;=========================================================================
;					void disp_str(char* info)
;=========================================================================
disp_str:
	push 	ebp
	mov 	ebp, esp

	mov 	esi, [ebp + 8] 		;pszInfo
	mov 	edi, [disp_pos] 		;显示位置
	mov 	ah, 0fh
.1:
	lodsb
	test	al, al
	jz 		.2 				; 字符串结束
	cmp 	al, 0ah 		; 是不是回车?
	jnz 	.3
	push 	eax
	mov 	eax, edi
	mov 	bl, 160
	div 	bl
	and 	eax, 0ffh
	inc 	eax
	mov 	bl, 160
	mul 	bl
	mov 	edi, eax
	pop 	eax
	jmp 	.1
.3:
	mov 	[gs:edi], ax
	add 	edi, 2
	jmp 	.1

.2:
	mov 	[disp_pos], edi 	;保存当前光标位置

	pop 	ebp
	ret