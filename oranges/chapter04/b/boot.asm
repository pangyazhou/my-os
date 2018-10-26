
;%define	_BOOT_DEBUG_	; 做 Boot Sector 时一定将此行注释掉!将此行打开后用 nasm Boot.asm -o Boot.com 做成一个.COM文件易于调试

%ifdef	_BOOT_DEBUG_
	org  0100h			; 调试状态, 做成 .COM 文件, 可调试
%else
	org  07c00h			; Boot 状态, Bios 将把 Boot Sector 加载到 0:7C00 处并开始执行
%endif
;===============================================================================
%ifdef 		_BOOT_DEBUG_
BaseOfStack 	equ 	0100h		;调试状态
%else
BaseOfStack 	equ 	07c00h		;Boot 状态堆栈基地址
%endif

BaseOfLoader 			equ 	09000h 		;LOADER.BIN 被加载到的位置  ---段地址
OffsetOfLoader 			equ 	0100h		;LOADER.BIN 被加载到的位置  ---偏移地址
RootDirSectors 			equ 	14 			;根目录占用空间
SectorNoOfRootDirectory 		equ 	19		;根目录扇区号
;===============================================================================

	%include "boot.inc"

LABEL_START:
	mov		ax, cs
	mov		ds, ax
	mov		es, ax
	mov 	ss, ax
	mov 	sp, BaseOfStack
	mov 	dh, 0
	xchg 	bx, bx 			;@调试点

	;软驱复位
	xor		ah, ah
	xor 	dl, dl
	int 	13h

;在A盘目录中寻找 LOADER.BIN
	mov 	word [wSectorNo], SectorNoOfRootDirectory
LABEL_SEARCH_IN_ROOT_DIR_BEGIN:
	cmp		word [wRootDirSizeForLoop], 0 		;判断根目录区是否已经读完
	jz 		LABEL_NO_LOADERBIN 					;未发现loader.bin文件
	dec 	word [wRootDirSizeForLoop]
	mov 	ax, BaseOfLoader			;es
	mov 	es, ax
	mov 	bx, OffsetOfLoader			;bx
	mov 	ax, [wSectorNo]				;ax
	mov 	cl, 1
	call 	ReadSector					;从根目录区读取一个扇区

	mov 	si, LoaderFileName 			;ds:si -> "LOADER  BIN"
	mov 	di, OffsetOfLoader 			;es:di -> BaseOfLoader:0100h
	cld 	
	mov 	dx, 08h						;每个根目录扇区包含的目录条目数
LABEL_SEARCH_FOR_LOADERBIN:
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
	mov 	si, LoaderFileName
	jmp 	LABEL_SEARCH_FOR_LOADERBIN

LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR:
	add 	word [wSectorNo], 1
	jmp 	LABEL_SEARCH_IN_ROOT_DIR_BEGIN

LABEL_NO_LOADERBIN:
	mov 	dh, 2
	call 	DispStr

%ifdef 	_BOOT_DEBUG_
	mov 	ax, 4c00h
	int 	21h
%else
	jmp 	$
%endif

LABEL_FILENAME_FOUND:
	mov 	dh, 3
	call 	DispStr
	jmp	$




	

	jmp	$					; 无限循环
;============================================================================
;变量
wRootDirSizeForLoop 	dw 		RootDirSectors 	;Root Directory 占用的扇区数
								;还有多少扇区没读
wSectorNo 				dw 		0 				;要读取的扇区号
bOdd 					db 		0 				;奇数还是偶数

;字符串
LoaderFileName 		db 		"LOADER  BIN", 0
MessageLength 		equ 	9
BootMessage:		db	"Booting  "
Message1 			db 	"Ready.   "
Message2 			db 	"No LOADER"
Message3 			db 	"LOADER..."
;============================================================================

;============================================================================
;子程序
;----------------------------------------------------------------------------
;函数名:DispStr
;----------------------------------------------------------------------------
;作用:
;	显示一个字符串, 函数开始时 dh 中应该是字符串序号
DispStr:
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
	int		10h							; int 10h
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



;============================================================================

times 	510-($-$$)	db	0	; 填充剩下的空间，使生成的二进制代码恰好为512字节
dw 	0xaa55				; 结束标志
