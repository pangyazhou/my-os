;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;						kernel.asm
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

;------------------------------------------------------------------------
; 编译链接方法
; $ nasm -f elf kernel.asm -o kernel.o
; $ ld -s -m elf_i386 kernel.o -o kernel.bin    #‘-s’选项意为“strip all”
;------------------------------------------------------------------------

SELECTOR_KERNEL_CS 		equ 	8

;导入函数
extern 	cstart

;导入全局变量
extern 	gdt_ptr

;堆栈段
[section .bss]
StackSpace 		resb 	2 * 1024 		; 2K 堆栈空间
StackTop: 								;栈顶

[section .text] ;代码段

	global _start		;导出 _start

_start:
		; 此时内存看上去是这样的（更详细的内存情况在 LOADER.ASM 中有说明）：
	;              ┃                                    ┃
	;              ┃                 ...                ┃
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃■■■■■■Page  Tables■■■■■■┃
	;              ┃■■■■■(大小由LOADER决定)■■■■┃ PageTblBase
	;    00101000h ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃■■■■Page Directory Table■■■■┃ PageDirBase = 1M
	;    00100000h ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃□□□□ Hardware  Reserved □□□□┃ B8000h ← gs
	;       9FC00h ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃■■■■■■■LOADER.BIN■■■■■■┃ somewhere in LOADER ← esp
	;       90000h ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃■■■■■■■KERNEL.BIN■■■■■■┃
	;       80000h ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃■■■■■■■■KERNEL■■■■■■■┃ 30400h ← KERNEL 入口 (KernelEntryPointPhyAddr)
	;       30000h ┣━━━━━━━━━━━━━━━━━━┫
	;              ┋                 ...                ┋
	;              ┋                                    ┋
	;           0h ┗━━━━━━━━━━━━━━━━━━┛ ← cs, ds, es, fs, ss
	;
	;
	; GDT 以及相应的描述符是这样的：
	;
	;		              Descriptors               Selectors
	;              ┏━━━━━━━━━━━━━━━━━━┓
	;              ┃         Dummy Descriptor           ┃
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃         DESC_FLAT_C    (0～4G)     ┃   8h = cs
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃         DESC_FLAT_RW   (0～4G)     ┃  10h = ds, es, fs, ss
	;              ┣━━━━━━━━━━━━━━━━━━┫
	;              ┃         DESC_VIDEO                 ┃  1Bh = gs
	;              ┗━━━━━━━━━━━━━━━━━━┛
	;
	; 注意! 在使用 C 代码的时候一定要保证 ds, es, ss 这几个段寄存器的值是一样的
	; 因为编译器有可能编译出使用它们的代码, 而编译器默认它们是一样的. 比如串拷贝操作会用到 ds 和 es.
	;
	;

	; 把 esp 从 LOADER 挪到 KERNEL 
;	xchg	bx, bx 		;@
	mov 	esp, StackTop 		;堆栈在bss段中

	sgdt 	[gdt_ptr]
	call 	cstart
	lgdt 	[gdt_ptr]

	jmp 	SELECTOR_KERNEL_CS:csinit
csinit:
	
	push 	0
	popfd 			;
	hlt

	

