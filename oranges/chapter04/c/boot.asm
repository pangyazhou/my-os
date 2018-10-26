
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
SectorNoOfFAT1 			equ 	1 			;fat1 的第一个扇区号
DeltaSectorNo 			equ 	17 			; DeltaSectorNo = BPB_RsvdSecCnt + (BPB_NumFATs * FATSz) - 2
;===============================================================================

	%include "boot.inc"

LABEL_START:
	mov		ax, cs
	mov		ds, ax
	mov		es, ax
	mov 	ss, ax
	mov 	sp, BaseOfStack
	mov 	dh, 0
;	xchg 	bx, bx 			;@调试点

	;清屏
	mov 	ax, 0600h 			; ah = 6, al = 0h
	mov 	bx, 0700h 			; 黑底白字 (bl = 07h)
	mov 	cx, 0 				; 左上角(0, 0)
	mov		dx, 0184fh			; 右下角(80, 50)
	int 	10h

	mov 	dh, 0 				; "Booting  "
	call 	DispStr 

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

;找到loader.bin后
LABEL_FILENAME_FOUND:  			
;	xchg 	bx, bx 		;@
	mov	 	ax, RootDirSectors
	and 	di, 0ffc0h 		; di -> 当前条目开始处
	add 	di, 01ah + 020h ; di -> 首Sector
	mov 	cx, word [es:di]  	;数据cluster
	push 	cx
	add 	cx, ax
	add 	cx, DeltaSectorNo 		;cl <- loader.bin 的数据起始扇区
	mov 	ax, BaseOfLoader
	mov 	es, ax
	mov 	bx, OffsetOfLoader
	mov 	ax, cx

LABEL_GO_ON_LOADING_FILE:
	push 	ax
	push 	bx
	mov 	ah, 0eh
	mov 	al, '.'
	mov 	bl, 0fh
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
	mov 	dh, 1
	call 	DispStr	

;============================================================================
;跳转到加载到内存的loader.bin开始处,
;开始执行loader.bin的代码
;boot Sector 的使命到此结束
	jmp 	BaseOfLoader:OffsetOfLoader

;============================================================================


;============================================================================
;变量
wRootDirSizeForLoop 	dw 		RootDirSectors 	;Root Directory 占用的扇区数
								;还有多少扇区没读
wSectorNo 				dw 		0 				;要读取的扇区号
bOdd 					db 		0 				;奇数还是偶数

;字符串
LoaderFileName 		db 		"PMTEST9 BIN", 0
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
	mov 	ax, BaseOfLoader
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


;============================================================================

times 	510-($-$$)	db	0	; 填充剩下的空间，使生成的二进制代码恰好为512字节
dw 	0xaa55				; 结束标志
