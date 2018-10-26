; ==========================================
; pmtest1.asm
; 编译方法：nasm pmtest1.asm -o pmtest1.bin
; ==========================================

%include	"pm.inc"	; 常量, 宏, 以及一些说明

PageDirBase 	equ 	200000h		;页目录开始地址:	2M
PageTblBase 	equ 	201000h		;页表开始地址: 	2M + 4K

org	0100h
	xchg 	bx, bx	
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
LABEL_DESC_VIDEO:  Descriptor 0B8000h,           0ffffh, DA_DRW	     ; 显存首地址
LABEL_DESC_PAGE_DIR: Descriptor PageDirBase,   4095, DA_DRW 	     ;Page Directory
LABEL_DESC_PAGE_TBL: Descriptor PageTblBase,  4096 * 8 - 1, DA_DRW 		 ; Page Tables

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
SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT
SelectorPageDir 	equ LABEL_DESC_PAGE_DIR - LABEL_GDT
SelectorPageTbl 	equ LABEL_DESC_PAGE_TBL - LABEL_GDT
; END of [SECTION .gdt]


;数据段
[SECTION .data1]		
ALIGN 	32
[BITS	32]
LABEL_DATA:

;实模式下使用的符号
;字符串
_szPMMessage:				db 		"In Protect Mode now. ^-^", 0Ah, 0Ah, 0 		;在保护模式中显示
_szMemChkTitle: 			db 		"BaseAddrL BaseAddrH LengthLow LengthHigh    Type", 0Ah, 0 	;进入保护模式显示
_szRAMSize 					db 		"RAM size:", 0
_szReturn 					db 		0Ah, 0
;变量
_wSPValueInRealMode 		dw 		0			;实模式的sp
_dwMCRNumber				dd 		0			;内存检测结果数量
_dwDispPos					dd 		(80 * 6 + 0) * 2	;屏幕第6行,第0列
_dwMemSize 					dd 		0					;内存大小
_ARDStruct:					;地址范围描述符结构
	_dwBaseAddrLow:			dd 		0
	_dwBaseAddrHigh:		dd 		0
	_dwLengthLow:			dd 		0
	_dwLengthHigh:			dd 		0
	_dwType: 				dd 		0

_MemChkBuf:	times 		256	db 	0			;缓冲区,存放ARDStruct数组

;保护模式下使用的符号
szPMMessage 		equ _szPMMessage - $$
szMemChkTitle		equ	_szMemChkTitle	- $$
szRAMSize			equ	_szRAMSize	- $$
szReturn			equ	_szReturn	- $$
dwDispPos			equ	_dwDispPos	- $$
dwMemSize			equ	_dwMemSize	- $$
dwMCRNumber			equ	_dwMCRNumber	- $$
ARDStruct			equ	_ARDStruct	- $$
	dwBaseAddrLow	equ	_dwBaseAddrLow	- $$
	dwBaseAddrHigh	equ	_dwBaseAddrHigh	- $$
	dwLengthLow		equ	_dwLengthLow	- $$
	dwLengthHigh	equ	_dwLengthHigh	- $$
	dwType			equ	_dwType		- $$
MemChkBuf			equ	_MemChkBuf	- $$


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
	mov [_wSPValueInRealMode], sp

	;########获取内存信息###############
	;调用BIOS中断15h
	;参数:ax:0e820h, es:di指向地址范围描述符结构体, edx:0534d4150h, ecx:20
	;返回:eax:0534d4150h, es:di指向返回的内存信息结构体, ecx:20, cf:0为正常,1为错误
	;ebx:下一描述符所需要的后续值, 为0时表示最后一个
	;##################################
	mov 	ebx, 0
	mov 	di, _MemChkBuf
.loop:
	mov 	eax, 0e820h
	mov 	ecx, 20
	mov 	edx, 0534d4150h
	int 	15h
	jc 		LABEL_MEM_CHK_FAIL
	add 	di, 20
	inc 	dword [_dwMCRNumber]
	cmp 	ebx, 0
	jne 	.loop
	jmp 	LABEL_MEM_CHK_OK
LABEL_MEM_CHK_FAIL:
	mov 	dword [_dwMCRNumber], 0
LABEL_MEM_CHK_OK:
	;#########内存信息获取成功###########

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

;;;;;;;;;;;;;;;;;;;;;;;;;;从保护模式跳回的实模式入口;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
LABEL_REAL_ENTRY:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax

	mov sp, [_wSPValueInRealMode]

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
;	xchg 	bx, bx				;调试用
	mov ax, SelectorData	
	mov ds, ax			; 数据段选择子
	mov ax, SelectorData
	mov es, ax
	mov	ax, SelectorVideo
	mov	gs, ax			; 视频段选择子(目的)

	mov ax, SelectorStack
	mov ss, ax 			;堆栈段选择子

	mov esp, TopOfStack

	;显示一个字符串
	push 	szPMMessage
	call	DispStr
	add 	esp, 4

	push 	szMemChkTitle
	call 	DispStr
	add 	esp, 4

	call 	DispMemSize 		;显示内存信息
	call 	SetupPaging			;启动分页机制
	
	; 到此停止
	jmp	SelectorCode16:0
;----------------------------------启动分页机制------------------------------------------------
SetupPaging:
	;根据内存大小计算应初始化多少PDE及P多少页表
	xor 	edx, edx
	mov 	eax, [dwMemSize]
	mov 	ebx, 400000h			;4M 一个页表对应的内存大小
	div 	ebx
	mov 	ecx, eax				;页表个数
	test 	edx, edx
	jne 	.no_remainder
	inc 	ecx
.no_remainder:
	push 	ecx						;暂时保存页表个数


	;首先初始化页目录
	mov 	ax, SelectorPageDir
	mov 	es, ax
	xor 	edi, edi
	xor 	eax, eax
	mov 	eax, PageTblBase | PG_P | PG_USU | PG_RWW
.1:
	stosd				; mov [es:edi], eax
	add 	eax, 4096
	loop 	.1

	;再初始化所有页表(1K个, 4M内存空间)
	mov 	ax, SelectorPageTbl
	mov 	es, ax
	pop 	eax
	mov 	ebx, 1024
	mul 	ebx
	mov 	ecx, eax
	xor 	edi, edi
	xor 	eax, eax
	mov 	eax, PG_P | PG_USU | PG_RWW
.2:
	stosd
	add 	eax, 4096
	loop 	.2

	;设置页目录入口
	mov 	eax, PageDirBase
	mov 	cr3, eax
	mov 	eax, cr0
	or 		eax, 80000000h
	mov 	cr0, eax
	jmp 	short .3
.3:
	nop

	ret
;-------------------------------------分页机制完毕----------------------------------------

;----------------------------------显示内存信息------------------------------------
DispMemSize:
	push 	esi
	push 	edi
	push 	ecx

	mov 	esi, MemChkBuf
	mov 	ecx, [dwMCRNumber];
.loop:
	mov 	edx, 5
	mov 	edi, ARDStruct
.1:
	push 	dword [esi]
	call 	DispInt			;
	pop 	eax
	stosd
	add 	esi, 4
	dec 	edx
	cmp 	edx, 0
	jne 	.1
	call 	DispReturn 		;printf("\n")
	cmp 	dword [dwType], 1
	jne 	.2
	mov 	eax, [dwBaseAddrLow]
	add 	eax, [dwLengthLow]
	cmp 	eax, [dwMemSize]
	jb		.2
	mov 	[dwMemSize], eax
.2:
	loop 	.loop

	call 	DispReturn
	push 	szRAMSize
	call 	DispStr			;printf("RAM size:")
	add 	esp, 4

	push 	dword [dwMemSize]
	call 	DispInt
	add 	esp, 4

	pop 	ecx
	pop 	edi
	pop 	esi
	ret
;----------------------------------内存信息显示完毕---------------------------------
%include 	"lib.inc"		;库函数

SegCode32Len	equ	$ - LABEL_SEG_CODE32
; END of [SECTION .s32]




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
	and eax, 7ffffffeh
	mov cr0, eax

LABEL_GO_BACK_TO_REAL:
	jmp 0:LABEL_REAL_ENTRY

Code16Len 		equ 	$ - LABEL_SEG_CODE16
;END OF [SECTION .s16code]
