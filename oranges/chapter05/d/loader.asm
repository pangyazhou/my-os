;; loader.asm

org 	0100h

	jmp 	LABEL_START
;===========================================================================
;FAT12 磁盘的头
%include 		"fat12hdr.inc"
%include "load.inc"
%include "pm.inc"

;===========================================================================
;GDT设置
[SECTION .gdt]
; GDT
;                                         段基址,       段界限     , 属性
LABEL_GDT:		Descriptor	       0,                 0, 0				; 空描述符
LABEL_DESC_FLAT_C:	Descriptor             0,           0fffffh, DA_CR | DA_32 | DA_LIMIT_4K	; 0 ~ 4G
LABEL_DESC_FLAT_RW:	Descriptor             0,           0fffffh, DA_DRW | DA_32 |DA_LIMIT_4K	; 0 ~ 4G
LABEL_DESC_VIDEO:	Descriptor	 	 0B8000h,            0ffffh, DA_DRW	| DA_DPL3		; 显存首地址

; GDT 结束

GdtLen		equ	$ - LABEL_GDT	; GDT长度
GdtPtr		dw	GdtLen - 1	; GDT界限
			dd	BaseOfLoaderPhyAddr + LABEL_GDT		; GDT基地址

; GDT 选择子
SelectorFlatC		equ	LABEL_DESC_FLAT_C	- LABEL_GDT
SelectorFlatRW		equ	LABEL_DESC_FLAT_RW	- LABEL_GDT
SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT + SA_RPL3

BaseOfStack 		equ 		0100h
PageDirBase 		equ 		100000h 		; 页目录开始地址: 1M
PageTblBase 		equ 		101000h 		; 页表开始地址: 	1M + 4K


;********************程序开始********************
LABEL_START:
	mov 	ax, cs
	mov 	ds, ax
	mov 	es, ax
	mov 	ss, ax
	mov 	sp, BaseOfStack

	mov 	dh, 0
	call 	DispStrRealMode 			;显示"Loading"

	; 得到内存数
	mov	ebx, 0
	mov	di, _MemChkBuf
.MemChkLoop:
	mov	eax, 0E820h
	mov	ecx, 20
	mov	edx, 0534D4150h
	int	15h
	jc	.MemChkFail
	add	di, 20
	inc	dword [_dwMCRNumber]
	cmp	ebx, 0
	jne	.MemChkLoop
	jmp	.MemChkOK
.MemChkFail:
	mov	dword [_dwMCRNumber], 0
.MemChkOK:


;在A盘目录中寻找 KERNEL.BIN
	mov	word [wSectorNo], SectorNoOfRootDirectory	
	;软驱复位
	xor		ah, ah
	xor 	dl, dl
	int 	13h
LABEL_SEARCH_IN_ROOT_DIR_BEGIN:
	cmp		word [wRootDirSizeForLoop], 0 		;判断根目录区是否已经读完
	jz 		LABEL_NO_KERNELBIN 					;未发现loader.bin文件
	dec 	word [wRootDirSizeForLoop]
	mov 	ax, BaseOfKernelFile			;es
	mov 	es, ax
	mov 	bx, OffsetOfKernelFile			;bx
	mov 	ax, [wSectorNo]				;ax
	mov 	cl, 1
	call 	ReadSector					;从根目录区读取一个扇区

;	xchg 	bx, bx 			;@调试点

	mov 	si, KernelFileName 				;ds:si -> "KERNEL  BIN"
	mov 	di, OffsetOfKernelFile 			;es:di -> BaseOfKernelFile:0h
	cld 	
	mov 	dx, 08h						;每个根目录扇区包含的目录条目数
LABEL_SEARCH_FOR_KERNELBIN:
	cmp 	dx, 0 						;循环次数控制
	jz 		LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR 			;读完一个扇区
	dec 	dx
	mov 	cx, 11 						;文件名11个字节
LABEL_CMP_FILENAME:
	cmp 	cx, 0 	
	jz 		LABEL_FILENAME_FOUND 		;发现文件
	dec 	cx
	lodsb 				;ds:si -> al
	cmp 	al, byte [es:(di + 020h)]
	jz 		LABEL_GO_ON 				;字符相等
	jmp 	LABEL_DIFFERENT 			;字符不相等

LABEL_GO_ON:
	inc 	di
	jmp 	LABEL_CMP_FILENAME 		

LABEL_DIFFERENT:
	and 	di, 0ffc0h
	add 	di, 040h 					;指向下一个条目
	mov 	si, KernelFileName
	jmp 	LABEL_SEARCH_FOR_KERNELBIN

LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR:
	add 	word [wSectorNo], 1
	jmp 	LABEL_SEARCH_IN_ROOT_DIR_BEGIN

LABEL_NO_KERNELBIN:
	mov 	dh, 2
	call 	DispStrRealMode 		; 显示 "NO KERNEL"

%ifdef 	_BOOT_DEBUG_
	mov 	ax, 4c00h
	int 	21h
%else
	jmp 	$
%endif

;找到kernel.bin后
LABEL_FILENAME_FOUND:  			
;	xchg 	bx, bx 		;@
	mov	 	ax, RootDirSectors
	and 	di, 0ffc0h 		; di -> 当前条目开始处

	push 	eax
	mov 	eax, [es:di + 01ch + 020h]
	mov 	dword [dwKernelSize], eax  			;保存KERNEL.BIN文件大小
	pop 	eax 

	add 	di, 01ah + 020h ; di -> 首Sector
	mov 	cx, word [es:di]  	;数据cluster
	push 	cx
	add 	cx, ax
	add 	cx, DeltaSectorNo 		;cl <- loader.bin 的数据起始扇区
	mov 	ax, BaseOfKernelFile
	mov 	es, ax
	mov 	bx, OffsetOfKernelFile
	mov 	ax, cx

LABEL_GO_ON_LOADING_FILE:
	push 	ax
	push 	bx
	mov 	ah, 0eh
	mov 	al, '.'
	mov 	bl, 0fh 			; 每读一个扇区, 在 Loading后面添加一个 '.'
	int 	10h
	pop 	bx
	pop 	ax

	mov 	cl, 1
	call 	ReadSector
	pop 	ax
	call 	GetFATEntry
	cmp 	ax, 0fffh
	jz 		LABEL_FILE_LOADED
	push 	ax 					;保存Sector在FAT中的序号
	mov 	dx, RootDirSectors
	add 	ax, dx
	add 	ax, DeltaSectorNo
	add 	bx, [BPB_BytsPerSec]
	jmp 	LABEL_GO_ON_LOADING_FILE
LABEL_FILE_LOADED:
	
	call 	KillMotor 		;关闭软驱马达

	mov 	dh, 1
	call 	DispStrRealMode	 		; 显示"Ready."字符串

	; 下面准备跳入保护模式


	; 加载 GDTR
	lgdt	[GdtPtr]

	; 关中断
	;cli

	; 打开地址线A20
	in	al, 92h
	or	al, 00000010b
	out	92h, al

	; 准备切换到保护模式
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax

	; 真正进入保护模式
	jmp 	dword SelectorFlatC:(BaseOfLoaderPhyAddr + LABEL_PM_START)
	jmp 	$

;============================================================================
;变量
wRootDirSizeForLoop 	dw 		RootDirSectors 	;Root Directory 占用的扇区数
								;还有多少扇区没读
wSectorNo 				dw 		0 				;要读取的扇区号
bOdd 					db 		0 				;奇数还是偶数
dwKernelSize 			dd 		0 				;KERNEL.BIN 文件大小

;字符串
KernelFileName 		db 		"KERNEL  BIN", 0
MessageLength 		equ 	9
BootMessage:		db	"Loading  "
Message1 			db 	"Ready.   "
Message2 			db 	"No KERNEL"


;============================================================================
;子程序
;----------------------------------------------------------------------------
;函数名:DispStrRealMode
;----------------------------------------------------------------------------
;作用:
;	显示一个字符串, 函数开始时 dh 中应该是字符串序号
DispStrRealMode:
	push 	es
	mov 	ax, MessageLength
	mul 	dh
	add 	ax, BootMessage
	mov		bp, ax						; ES:BP = 串地址
	mov 	ax, ds
	mov 	es, ax 
	mov		cx, MessageLength			; CX = 串长度
	mov		ax, 01301h					; AH = 13,  AL = 01h
	mov		bx, 000ch					; 页号为0(BH = 0) 黑底红字(BL = 0Ch,高亮)
	mov		dl, 0
	add	 	dh, 3
	int		10h							; int 10h
	pop 	es
	ret

;----------------------------------------------------------------------------
;函数名: ReadSector
;----------------------------------------------------------------------------
;作用:
;	从第 ax 个 Sector 开始, 将 cl 个Sector 读入 es:bx 中
; -----------------------------------------------------------------------
	; 怎样由扇区号求扇区在磁盘中的位置 (扇区号 -> 柱面号, 起始扇区, 磁头号)
	; -----------------------------------------------------------------------
	; 设扇区号为 x
	;                           ┌ 柱面号 = y >> 1
	;       x           ┌ 商 y ┤
	; -------------- => ┤      └ 磁头号 = y & 1
	;  每磁道扇区数     │
	;                   └ 余 z => 起始扇区号 = z + 1
ReadSector:
	push 	bp
	mov 	bp, sp
	sub 	esp, 2		;空出2个字节, 保存要读的扇区数

	mov 	byte [bp -2], cl
	push 	bx
	mov 	bl, [BPB_SecPerTrk]		;每磁道扇区数
	div 	bl
	inc 	ah 						;起始扇区号
	mov	 	cl, ah
	mov 	dh, al
	shr 	al, 1
	mov 	ch, al 					;柱面号
	and 	dh, 1 					;磁头号
	pop 	bx 						;恢复bx
	;至此, "柱面号, 磁头号, 起始扇区" 全部获取
	mov 	dl, [BS_DrvNum]			;驱动器号
.GoOnReading:
	mov 	ah, 2 					;读
	mov	 	al, [bp - 2]			;读al个扇区
	int 	13h
	jc 		.GoOnReading 			;如果读错 cf=1 继续读,知道正确为止

	add 	esp, 2
	pop 	bp
	ret

;---------------------------------------------------------------------------
; 函数名: GetFATEntry
;---------------------------------------------------------------------------
; 作用:
; 	找到序号为 ax 的 Sector 在 FAT 中的条目, 结果放在 ax 中
; 	注意: 中间需要读 FAT 的扇区到 ES:BX 处, 所以开始要保存es, bx
GetFATEntry:
	push 	es
	push 	bx
	push 	ax
	mov 	ax, BaseOfKernelFile
	sub 	ax, 01100h 			;在BaseOfLoader 后面留出4K空间用于存放FAT
	mov 	es, ax
	pop 	ax
	mov 	byte [bOdd], 0
	mov 	bx, 3
	mul 	bx
	mov 	bx, 2
	div 	bx
	cmp 	dx, 0
	jz 		LABEL_EVEN
	mov 	byte [bOdd], 1
LABEL_EVEN:
	;当前 AX 是 FATEntry 在 FAT 中的偏移量, 计算 FATEntry 在哪个扇区
	xor 	dx, dx
	mov 	bx, [BPB_BytsPerSec]
	div 	bx 			; dx:ax / 512 
						; ax <- 商 (FATEntry 所在扇区相对于 FAT 的扇区号)
						; dx <- 余数 (FATEntry 在扇区内的偏移)
	push 	dx
	mov 	bx, 0 		; es:bs -> (BaseOfLoader - 01100h):00
	add 	ax, SectorNoOfFAT1 		; ax 为FATEntry 所在的扇区号
	mov 	cl, 2 					;读2个扇区
	call    ReadSector

	pop 	dx
	add 	bx, dx
	mov 	ax, [es:bx]
	cmp 	byte [bOdd], 1
	jnz 	LABEL_EVEN_2
	shr 	ax, 4
LABEL_EVEN_2:
	and 	ax, 0fffh

LABEL_GET_FAT_ENTRY_OK:
	pop 	bx
	pop 	es
	ret

;---------------------------------------------------------------------------
; 函数名: KillMotor
;---------------------------------------------------------------------------
; 作用:
; 	关闭软驱马达
KillMotor:
	push 	dx
	mov		dx, 03f2h
	mov 	al, 0
	out 	dx, al
	pop 	dx
	ret


;#########################此后代码在保护模式下运行##################################
;==========================代码段================================================
;32位代码段, 由实模式跳入
[SECTION .s32]
ALIGN 	32
[BITS 	32]

LABEL_PM_START:
	mov 	ax, SelectorVideo
	mov 	gs, ax

	mov 	ax, SelectorFlatRW
	mov 	ds, ax
	mov 	es, ax
	mov 	fs, ax
	mov 	ss, ax
	mov 	esp, TopOfStack 

;	xchg 	bx, bx 			;@
	push 	szMemChkTitle
	call 	DispStr
	add 	esp, 4

	call 	DispMemInfo
	call 	SetupPaging

	mov 	ah, 0fh 		; 0000:黑底, 1111: 白字
	mov 	al, 'P'
	mov 	[gs:((80 * 0 + 39) * 2)], ax

	call 	InitKernel
	;jmp $

	;***************************************************************
	jmp 	SelectorFlatC:KernelEntryPointPhyAddr 		;正式进入内核 *
	;***************************************************************
	; 内存看上去是这样的：
	;              ┃                                    ┃
	;              ┃                 .                  ┃
	;              ┃                 .                  ┃
	;              ┃                 .                  ┃
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃■■■■■■■■■■■■■■■■■■┃
	;              ┃■■■■■■Page  Tables■■■■■■┃
	;              ┃■■■■■(大小由LOADER决定)■■■■┃
	;    00101000h ┃■■■■■■■■■■■■■■■■■■┃ PageTblBase
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃■■■■■■■■■■■■■■■■■■┃
	;    00100000h ┃■■■■Page Directory Table■■■■┃ PageDirBase  <- 1M
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃□□□□□□□□□□□□□□□□□□┃
	;       F0000h ┃□□□□□□□System ROM□□□□□□┃
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃□□□□□□□□□□□□□□□□□□┃
	;       E0000h ┃□□□□Expansion of system ROM □□┃
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃□□□□□□□□□□□□□□□□□□┃
	;       C0000h ┃□□□Reserved for ROM expansion□□┃
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃□□□□□□□□□□□□□□□□□□┃ B8000h ← gs
	;       A0000h ┃□□□Display adapter reserved□□□┃
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃□□□□□□□□□□□□□□□□□□┃
	;       9FC00h ┃□□extended BIOS data area (EBDA)□┃
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃■■■■■■■■■■■■■■■■■■┃
	;       90000h ┃■■■■■■■LOADER.BIN■■■■■■┃ somewhere in LOADER ← esp
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃■■■■■■■■■■■■■■■■■■┃
	;       80000h ┃■■■■■■■KERNEL.BIN■■■■■■┃
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃■■■■■■■■■■■■■■■■■■┃
	;       30000h ┃■■■■■■■■KERNEL■■■■■■■┃ 30400h ← KERNEL 入口 (KernelEntryPointPhyAddr)
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃                                    ┃
	;        7E00h ┃              F  R  E  E            ┃
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃■■■■■■■■■■■■■■■■■■┃
	;        7C00h ┃■■■■■■BOOT  SECTOR■■■■■■┃
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃                                    ┃
	;         500h ┃              F  R  E  E            ┃
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃□□□□□□□□□□□□□□□□□□┃
	;         400h ┃□□□□ROM BIOS parameter area □□┃
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇┃
	;           0h ┃◇◇◇◇◇◇Int  Vectors◇◇◇◇◇◇┃
	;              ┗━━━━━━━━━━━━━━━━━━┛ ← cs, ds, es, fs, ss
	;
	;
	;		┏━━━┓		┏━━━┓
	;		┃■■■┃ 我们使用 	┃□□□┃ 不能使用的内存
	;		┗━━━┛		┗━━━┛
	;		┏━━━┓		┏━━━┓
	;		┃      ┃ 未使用空间	┃◇◇◇┃ 可以覆盖的内存
	;		┗━━━┛		┗━━━┛
	;
	; 注：KERNEL 的位置实际上是很灵活的，可以通过同时改变 LOAD.INC 中的
	;     KernelEntryPointPhyAddr 和 MAKEFILE 中参数 -Ttext 的值来改变。
	;     比如把 KernelEntryPointPhyAddr 和 -Ttext 的值都改为 0x400400，
	;     则 KERNEL 就会被加载到内存 0x400000(4M) 处，入口在 0x400400。
	;


%include "lib.inc"

;=================================================================================
;保护模式下的子程序
;---------------------------------------------------------------------------------
; 函数名: DispMemInfo
;---------------------------------------------------------------------------------
; 作用: 
;	显示内存信息
DispMemInfo:
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
	jnz 	.1
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

;---------------------------------------------------------------------------------
; 函数名: SetupPaging
;---------------------------------------------------------------------------------
; 作用:
;	启动分页机制
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
	mov 	[PageTableNumber], ecx					;暂时保存页表个数


	;首先初始化页目录
	mov 	ax, SelectorFlatRW
	mov 	es, ax
	mov  	edi, PageDirBase
	xor 	eax, eax
	mov 	eax, PageTblBase | PG_P | PG_USU | PG_RWW
.1:
	stosd				; mov [es:edi], eax
	add 	eax, 4096
	loop 	.1

	;再初始化所有页表
	mov 	eax, [PageTableNumber]
	mov 	ebx, 1024
	mul 	ebx
	mov 	ecx, eax
	mov 	edi, PageTblBase
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


;---------------------------------------------------------------------------------
; 函数名: InitKernel
;---------------------------------------------------------------------------------
; 作用:
; 	将kernel.bin 根据 elf 文件信息转移到正确的位置
InitKernel:
	;从文件头获取信息
	xor 	esi, esi
	mov 	cx, word [BaseOfKernelFilePhyAddr + 2ch]		; 从Elf头中或取e_phnum
	movzx 	ecx, cx
	mov 	esi, [BaseOfKernelFilePhyAddr + 1ch] 			; esi <-  pELFHdr->e_phoff
	add 	esi, BaseOfKernelFilePhyAddr  					; esi 是programheader地址
.Begin:
	mov 	eax, [esi + 0] 		; 程序段类型
	cmp 	eax, 0
	jz 		.NoAction
	push 	dword [esi + 010h] 		;程序段大小 size
	mov 	eax, [esi + 04h] 		;程序段在文件中偏移量
	add 	eax, BaseOfKernelFilePhyAddr
	push 	eax 					; src
	push 	dword [esi + 08h] 		; dst
	call 	MemCpy
	add 	esp, 12
.NoAction:
	add 	esi, 020h
	dec 	ecx
	jnz 	.Begin

	ret

;===========================数据段=================================================
[SECTION .data1]	 ; 数据段
ALIGN	32
[BITS	32]
LABEL_DATA:
; 实模式下使用这些符号
; 字符串
_szMemChkTitle:			db	"BaseAddrL BaseAddrH LengthLow LengthHigh   Type", 0Ah, 0	; 进入保护模式后显示此字符串
_szRAMSize			db	"RAM size:", 0
_szReturn			db	0Ah, 0
; 变量
_dwMCRNumber:			dd	0	; Memory Check Result
_dwDispPos:			dd	(80 * 6 + 0) * 2	; 屏幕第 6 行, 第 0 列。
_dwMemSize:			dd	0
_ARDStruct:			; Address Range Descriptor Structure
	_dwBaseAddrLow:		dd	0
	_dwBaseAddrHigh:	dd	0
	_dwLengthLow:		dd	0
	_dwLengthHigh:		dd	0
	_dwType:		dd	0
_PageTableNumber:		dd	0
_SavedIDTR:			dd	0	; 用于保存 IDTR
				dd	0
_SavedIMREG:			db	0	; 中断屏蔽寄存器值
_MemChkBuf:	times	256	db	0

; 保护模式下使用这些符号
szMemChkTitle		equ	BaseOfLoaderPhyAddr + _szMemChkTitle
szRAMSize		equ	BaseOfLoaderPhyAddr + _szRAMSize
szReturn		equ	BaseOfLoaderPhyAddr + _szReturn
dwDispPos		equ	BaseOfLoaderPhyAddr + _dwDispPos
dwMemSize		equ	BaseOfLoaderPhyAddr + _dwMemSize
dwMCRNumber		equ	BaseOfLoaderPhyAddr + _dwMCRNumber
ARDStruct		equ	BaseOfLoaderPhyAddr + _ARDStruct
	dwBaseAddrLow	equ	BaseOfLoaderPhyAddr + _dwBaseAddrLow
	dwBaseAddrHigh	equ	BaseOfLoaderPhyAddr + _dwBaseAddrHigh
	dwLengthLow		equ	BaseOfLoaderPhyAddr + _dwLengthLow
	dwLengthHigh	equ	BaseOfLoaderPhyAddr + _dwLengthHigh
	dwType		equ	BaseOfLoaderPhyAddr + _dwType
MemChkBuf		equ	BaseOfLoaderPhyAddr + _MemChkBuf
PageTableNumber  	equ BaseOfLoaderPhyAddr + _PageTableNumber

; 堆栈在数据段的末尾
StackSpace: 	times 	1024 	db 0
TopOfStack 			equ 	BaseOfLoaderPhyAddr + $ 		; 栈顶

; END of [SECTION .data1]

