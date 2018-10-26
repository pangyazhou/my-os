; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                              string.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

[section .text]

;导出函数
global 	memcpy
global 	memset
global 	strcpy
global 	debug
global 	strlen

;-------------------------------------------------------------------------
; void* memcpy(void* es:pDest, void* ds:pSrc, int iSize)
;-------------------------------------------------------------------------
memcpy:
	push 	ebp
	mov 	ebp, esp

	push 	esi
	push 	edi
	push 	ecx

	mov 	edi, [ebp + 8]		; Destination
	mov 	esi, [ebp + 12] 	; Source
	mov		ecx, [ebp + 16] 	; Counter
.1:
	cmp 	ecx, 0
	jz 		.2

	mov 	al, [ds:esi]
	inc 	esi

	mov 	byte [es:edi], al
	inc 	edi

	dec 	ecx
	jmp 	.1
.2:
	mov 	eax, [ebp + 8]		;返回值

	pop 	ecx
	pop 	edi
	pop 	esi

	mov 	esp, ebp
	pop 	ebp

	ret 		; 函数结束, 返回

;----------------------------------------------------------------
; 	void memset(void* p_dest, char ch, int size);
;----------------------------------------------------------------
memset:
	push 	ebp
	mov 	ebp, esp

	push 	edi
	push	edx
	push	ecx

	mov 	edi, [ebp + 8] 			; Destination
	mov 	edx, [ebp + 12]			; Char to be putted
	mov 	ecx, [ebp + 16] 		; Counter
.1:
	cmp 	ecx, 0
	jz 		.2

	mov 	byte [edi], dl
	inc 	edi

	dec 	ecx
	jmp 	.1
.2:
	pop 	ecx
	pop 	edx
	pop 	edi
	mov 	esp, ebp
	pop 	ebp
	ret 				; 函数结束,返回


;----------------------------------------------------------------
; 	char* strcpy(void* p_dest, void* p_src);
;----------------------------------------------------------------
strcpy:
	push 	ebp
	mov 	ebp, esp

	push 	esi
	push 	edi

	mov 	edi, [ebp + 8]
	mov 	esi, [ebp + 12]

.1:	
	mov 	al, [esi]
	inc 	esi
	mov 	[edi], al
	inc 	edi
	cmp 	al, 0
	jnz 	.1

	mov 	eax, [ebp + 8]

	pop 	edi
	pop 	esi
	pop 	ebp
	ret



	
; ====================================================================================
;                                   debug
; 调试接口
; ====================================================================================
debug:
	xchg 	bx, bx
	ret


; ====================================================================================
;                                   strlen
; 字符串长度
; ====================================================================================
strlen:
	push 	ebp
	mov 	ebp, esp

	mov 	eax, 0				; 长度为0
	mov 	esi, [ebp + 8]
.1:
	cmp 	byte [esi], 0 		; 是否到字符串末尾
	jz 		.2
	inc 	esi
	inc 	eax
	jmp 	.1
.2:
	pop 	ebp
	ret