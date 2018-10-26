;; lib.inc

;------------------显示AL中的数字----------------------------
DispAL:
	push 	ecx
	push 	edx
	push 	edi

	mov 	edi, [dwDispPos]

	mov 	ah, 0fh
	mov 	dl, al
	shr 	al, 4
	mov 	ecx, 2
.begin:
	and 	al, 01111b
	cmp 	al, 9
	ja 		.1
	add 	al, '0'
	jmp 	.2
.1:
	sub 	al, 0ah
	add 	al, 'A'
.2:
	mov 	[gs:edi], ax
	add 	edi, 2

	mov 	al, dl
	loop 	.begin

	mov 	[dwDispPos], edi

	pop 	edi
	pop 	edx
	pop 	ecx

	ret
;-------------------DispAL结束-------------------------------

;-------------------显示一个整型数----------------------------
DispInt:
	mov 	eax, [esp + 4]		;传递的整型参数
	shr 	eax, 24
	call 	DispAL

	mov 	eax, [esp + 4]
	shr 	eax, 16
	call 	DispAL

	mov 	eax, [esp + 4]
	shr 	eax, 8
	call 	DispAL

	mov 	eax, [esp + 4]
	call 	DispAL

	mov 	ah, 07h
	mov 	al, 'h'
	push 	edi
	mov 	edi, [dwDispPos]
	mov 	[gs:edi], ax
	add 	edi, 4
	mov 	[dwDispPos], edi
	pop 	edi

	ret

;-------------------DispInt结束------------------------------

;-------------------显示一个字符串----------------------------
DispStr:
	push 	ebp
	mov 	ebp, esp
	push 	ebx
	push 	esi
	push 	edi

	mov 	esi, [ebp + 8]
	mov 	edi, [dwDispPos]
	mov 	ah, 0fh
.1:
	lodsb		;mov al, [ds:esi], inc esi
	test 	al, al 		;是否结束
	jz 		.2
	cmp 	al, 0ah		;是否为回车
	jnz 	.3
	push 	eax
	mov 	eax, edi
	mov 	bl, 160
	div 	bl
	and 	eax, 0ffh		;当前行
	inc 	eax				;增加一行
	mov 	bl, 160
	mul 	bl
	mov 	edi, eax
	pop 	eax
	jmp 	.1
.3:
	mov 	[gs:edi], ax
	add 	edi, 2
	jmp		.1
.2:
	mov 	[dwDispPos], edi

	pop 	edi
	pop 	esi
	pop 	ebx
	pop 	ebp

	ret
;-------------------DispStr结束------------------------------
;-------------------换行-------------------------------------
DispReturn:
	push 	szReturn
	call 	DispStr
	add 	esp, 4

	ret
;-------------------DispReturn结束---------------------------


;------------------------------------------------------------
;内存拷贝, 模拟memcpy
;------------------------------------------------------------
;void *MemCpy(void* es:pDest, void* ds:pSrc, int iSize);
;------------------------------------------------------------
MemCpy:
	push 	ebp
	mov 	ebp, esp

	push 	esi
	push 	edi
	push 	ecx

	mov 	edi, [ebp + 8]		;Destination  	参数
	mov 	esi, [ebp + 12]		;Source
	mov 	ecx, [ebp + 16] 	;Counter
.1:	
	cmp 	ecx, 0
	jz 		.2

	mov 	al, [ds:esi]
	inc 	esi

	mov 	byte [es:edi], al      ;逐字节复制
	inc 	edi

	dec 	ecx
	jmp 	.1
.2:
	mov 	eax, [bp + 8]		;返回值:目的地址

	pop 	ecx
	pop 	edi
	pop 	esi
	mov 	esp, ebp
	pop 	ebp

	ret
;---------------MemCpy结束-----------------------------------
