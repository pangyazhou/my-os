; ==========================================
; pmtest1.asm
; 编译方法：nasm pmtest1.asm -o pmtest1.bin
; ==========================================

%include	"pm.inc"	; 常量, 宏, 以及一些说明

org	0100h
	jmp	LABEL_BEGIN

[SECTION .gdt]
; GDT
;                              段基址,       段界限 , 属性
LABEL_GDT:	   Descriptor       0,                0, 0               ; 空描述符
LABEL_DESC_NORMAL: Descriptor 	0,			 0ffffh, DA_DRW			 ; Normal 描述符
LABEL_DESC_CODE32: Descriptor   0, SegCode32Len - 1, DA_C + DA_32    ; 非一致代码段, 32
LABEL_DESC_CODE16: Descriptor   0, 			 0ffffh, DA_C		     ; 非一致代码段, 16
LABEL_DESC_DATA:   Descriptor 	0, 		DataLen - 1, DA_DRW			 ; DATA
LABEL_DESC_STACK:  Descriptor 	0, 		 TopOfStack, DA_DRWA + DA_32 ; STACK, 32
LABEL_DESC_TEST:   Descriptor 	0500000h,    0ffffh, DA_DRW
LABEL_DESC_VIDEO:  Descriptor 0B8000h,           0ffffh, DA_DRW	     ; 显存首地址
LABEL_DESC_LDT:	   Descriptor   0,       LDTLen - 1, DA_LDT 		 ; LDT
; GDT 结束

GdtLen		equ	$ - LABEL_GDT	; GDT长度
GdtPtr		dw	GdtLen - 1	; GDT界限
		dd	0		; GDT基地址

; GDT 选择子
SelectorNormal		equ	LABEL_DESC_NORMAL	- LABEL_GDT
SelectorCode32		equ	LABEL_DESC_CODE32	- LABEL_GDT
SelectorCode16		equ	LABEL_DESC_CODE16	- LABEL_GDT
SelectorData		equ	LABEL_DESC_DATA	    - LABEL_GDT
SelectorStack		equ	LABEL_DESC_STACK	- LABEL_GDT
SelectorTest		equ	LABEL_DESC_TEST		- LABEL_GDT
SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT
SelectorLDT 		equ LABEL_DESC_LDT 		- LABEL_GDT
; END of [SECTION .gdt]


;数据段
[SECTION .data1]		
ALIGN 	32
[BITS	32]
LABEL_DATA:
SPValueInRealMode 		dw 		0
;字符串
PMMessage:				db 		"In Protect Mode now. ^-^", 0 		;在保护模式中显示
OffsetPMMessage 		equ 	PMMessage - $$
StrTest: 				db 		"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0
OffsetStrTest 			equ 	StrTest - $$
DataLen 				equ 	$ - LABEL_DATA
;END OF [SECITON  .data1]

;全局堆栈段
[SECTION .gs]
ALIGN 	32
[BITS 	32]
LABEL_STACK:
		times 512 db 0
TopOfStack 		equ 	$ - LABEL_STACK - 1
;END OF [SECTION .gs]



;############################实模式#################################################
;程序开始处:
[SECTION .s16]
[BITS	16]
LABEL_BEGIN:
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, 0100h

	mov [LABEL_GO_BACK_TO_REAL + 3], ax
	mov [SPValueInRealMode], sp

	; 初始化 32 位代码段描述符
	xor	eax, eax
	mov	ax, cs
	shl	eax, 4
	add	eax, LABEL_SEG_CODE32
	mov	word [LABEL_DESC_CODE32 + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_CODE32 + 4], al
	mov	byte [LABEL_DESC_CODE32 + 7], ah

	; 初始化 数据段描述符
	xor eax, eax
	mov ax, ds
	shl eax, 4
	add eax, LABEL_DATA
	mov word [LABEL_DESC_DATA + 2], ax
	shr eax, 16
	mov byte [LABEL_DESC_DATA + 4], al
	mov byte [LABEL_DESC_DATA + 7], ah

	; 初始化 堆栈段描述符
	xor eax, eax
	mov ax, ds
	shl eax, 4
	add eax, LABEL_STACK
	mov word [LABEL_DESC_STACK + 2], ax
	shr eax, 16
	mov byte [LABEL_DESC_STACK + 4], al
	mov byte [LABEL_DESC_STACK + 7], ah

	; 初始化 16位代码段描述符
	mov ax, cs
	movzx eax, ax
	shl eax, 4
	add eax, LABEL_SEG_CODE16
	mov word [LABEL_DESC_CODE16 + 2], ax
	shr eax, 16
	mov byte [LABEL_DESC_CODE16 + 4], al
	mov byte [LABEL_DESC_CODE16 + 7], ah

	; 初始化 LDT在 GDT中的描述符
	xor eax, eax
	mov ax, ds
	shl eax, 4
	add eax, LABEL_LDT
	mov word [LABEL_DESC_LDT + 2], ax
	shr eax, 16
	mov byte [LABEL_DESC_LDT + 4], al
	mov byte [LABEL_DESC_LDT + 7], ah

	; 初始化 LDT 中的描述符
	xor eax, eax
	mov ax, ds
	shl eax, 4
	add eax, LABEL_CODE_A
	mov word [LABEL_LDT_DESC_CODEA + 2], ax
	shr eax, 16
	mov byte [LABEL_LDT_DESC_CODEA + 4], al
	mov byte [LABEL_LDT_DESC_CODEA + 7], ah

	; 为加载 GDTR 作准备
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_GDT		; eax <- gdt 基地址
	mov	dword [GdtPtr + 2], eax	; [GdtPtr + 2] <- gdt 基地址

	; 加载 GDTR
	lgdt	[GdtPtr]

	; 关中断
	cli

	; 打开地址线A20
	in	al, 92h
	or	al, 00000010b
	out	92h, al

	; 准备切换到保护模式
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax

	; 真正进入保护模式
	jmp	dword SelectorCode32:0	; 执行这一句会把 SelectorCode32 装入 cs,
					; 并跳转到 Code32Selector:0  处

;;;;;;;;;;;;;;;;;;;;;;;;;;从保护模式跳回到实模式入口;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
LABEL_REAL_ENTRY:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax

	mov sp, [SPValueInRealMode]

	;in al, 92h				;关闭A20地址线
	;and al, 11111101b
	;out 92h, al

	sti 				;开中断

	mov ax, 4c00h		;回到dos
	int 21h 
; END of [SECTION .s16]





;############################保护模式#####################################
; 32 位代码段. 由实模式跳入.
[SECTION .s32]
[BITS	32]

LABEL_SEG_CODE32:
	mov ax, SelectorData
	mov ds, ax			;数据段选择子
	mov ax, SelectorTest
	mov es, ax			;测试段选择子
	mov	ax, SelectorVideo
	mov	gs, ax			; 视频段选择子(目的)

	mov ax, SelectorStack
	mov ss, ax 			;堆栈段选择子

	mov esp, TopOfStack

	;显示一个字符串
	mov ah, 0ch 		;0000: 黑底 , 1100: 红字
	xor esi, esi
	xor edi, edi
	mov esi, OffsetPMMessage	;源数据偏移
	mov edi, (80 * 10 + 0) * 2 	;目的数据偏移,10行0列
	cld
.1:
	lodsb
	test al, al
	jz .2
	mov [gs:edi], ax
	add edi, 2
	jmp .1
.2:				;显示完毕
	call DispReturn

	;load LDT
	mov 	ax, SelectorLDT
	lldt 	ax

	; 跳入局部任务
	jmp	SelectorLDTCodeA:0

;----------------------------------换行-----------------------------------------------------------
DispReturn:
	push 	eax
	push 	ebx
	mov 	eax, edi
	mov 	bl, 160
	div 	bl
	and 	eax, 0ffh
	inc 	eax
	mov 	bl, 160
	mul 	bl
	mov 	edi, eax
	pop 	ebx
	pop 	eax

	ret
;----------------------------------DispReturn结束-----------------------------------------------------------

SegCode32Len	equ	$ - LABEL_SEG_CODE32
; END of [SECTION .s32]


;###########################LDT段:局部描述符表#################################################
[SECTION .ldt]
ALIGN 	32
LABEL_LDT:
;							段基址  			段界限			属性
LABEL_LDT_DESC_CODEA: 	Descriptor 	0,	CodeALen - 1, 	DA_C + DA_32 	; 32位

LDTLen 		equ 	$ - LABEL_LDT

;LDT选择子
SelectorLDTCodeA 	equ 	LABEL_LDT_DESC_CODEA - LABEL_LDT + SA_TIL
;END OF [SECTION 	.ldt]


;##############################32位局部代码段#################################################
[SECTION 	.la]
ALIGN 	32
[BITS 	32]
LABEL_CODE_A:
	mov 	ax, SelectorVideo
	mov 	gs, ax

	mov 	edi, (80 * 12 + 0) * 2	;屏幕第12行0列
	mov 	ah, 0ch					;0000:黑底, 1100:红字
	mov 	al, 'L'
	mov 	[gs:edi], ax

	;跳回实模式
	jmp 	SelectorCode16:0
CodeALen 	equ 	$ - LABEL_CODE_A


;###################################跳回实模式#######################################################
;16位代码段. 由32为代码段跳入, 跳出后回到实模式
[SECTION .s16code]
ALIGN 	32
[BITS 	16]
LABEL_SEG_CODE16:
	mov ax, SelectorNormal
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov eax, cr0
	and al, 11111110b
	mov cr0, eax

LABEL_GO_BACK_TO_REAL:
	jmp 0:LABEL_REAL_ENTRY

Code16Len 		equ 	$ - LABEL_SEG_CODE16
;END OF [SECTION .s16code]
