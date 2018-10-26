; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                              klib.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

; 导入全局变量
extern disp_pos

[section .text]

; 导出函数
global 	disp_str
global 	disp_color_str
global 	out_byte
global 	in_byte

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

;=========================================================================
;					void disp_color_str(char* info, int color);
;=========================================================================
disp_color_str:
	push 	ebp
	mov 	ebp, esp

	mov 	esi, [ebp + 8] 		; pszInfo
	mov 	edi, [disp_pos] 		;显示位置
	mov 	ah,  [ebp + 12]		; color
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


;=========================================================================
;					void out_byte(u16 port, u8 value);
;=========================================================================
out_byte:
	mov 	edx, [esp + 4] 			; port
	mov 	al, [esp + 4 + 4] 		; value

	out 	dx, al
	nop 	
	nop
	ret

;=========================================================================
;					u8 in_byte(u16 port);
;=========================================================================
in_byte:
	mov 	edx, [esp + 4] 		;port
	xor 	eax, eax
	in 		al, dx
	nop
	nop
	ret