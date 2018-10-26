;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;						kernel.asm
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

;------------------------------------------------------------------------
; 编译链接方法
; $ nasm -f elf kernel.asm -o kernel.o
; $ ld -s -m elf_i386 kernel.o -o kernel.bin    #‘-s’选项意为“strip all”
;------------------------------------------------------------------------

SELECTOR_KERNEL_CS	equ	8

;导入函数
extern 	cstart
extern 	exception_handler

;导入全局变量
extern 	gdt_ptr
extern 	idt_ptr
extern 	disp_pos

;堆栈段
[section .bss]
StackSpace 		resb 	2 * 1024 		; 2K 堆栈空间
StackTop: 								;栈顶

[section .text] ;代码段

	global _start		;导出 _start
	global divide_error
	global single_step_exception
	global nmi
	global breakpoint_exception
	global overflow
	global bounds_check
	global inval_opcode
	global copr_not_available
	global double_fault
	global copr_seg_overrun
	global inval_tss
	global segment_not_present
	global stack_exception
	global general_protection
	global page_fault
	global copr_error

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

	mov 	dword [disp_pos], 0

	sgdt 	[gdt_ptr] 		; 将原 gdtr 保存到[gdt_ptr]
	call 	cstart 			; 更新 [gdt_ptr]
	lgdt 	[gdt_ptr] 		; 使用新的 gdt
	lidt 	[idt_ptr] 		; 使用idt

	jmp 	SELECTOR_KERNEL_CS:csinit
csinit:
	;ud2
	mov 	ax, 2
	mov 	dl, 0
	div 	dl
	
	
;	push 	0
;	popfd 			;
	hlt

; 中断和异常 -- 异常
divide_error:
	push 	0xffffffff		; no error code
	push 	0 				; vector_no = 0
	jmp 	exception
single_step_exception:
	push 	0xffffffff		; no error code
	push 	1 				; vector_no = 1
	jmp 	exception
nmi:
	push 	0xffffffff		; no error code
	push 	2 				; vector_no = 2
	jmp 	exception
breakpoint_exception:
	push 	0xffffffff		; no error code
	push 	3 				; vector_no = 3
	jmp 	exception
overflow:
	push 	0xffffffff		; no error code
	push 	4 				; vector_no = 4
	jmp 	exception
bounds_check:
	push 	0xffffffff		; no error code
	push 	5 				; vector_no = 5
	jmp 	exception
inval_opcode:
	push 	0xffffffff		; no error code
	push 	6 				; vector_no = 6
	jmp 	exception
copr_not_available:
	push 	0xffffffff		; no error code
	push 	7 				; vector_no = 7
	jmp 	exception
double_fault:
	push 	8 				; vector_no = 8
	jmp 	exception
copr_seg_overrun:
	push 	0xffffffff		; no error code
	push 	9 				; vector_no = 9
	jmp 	exception
inval_tss:
	push 	10 				; vector_no = A
	jmp 	exception
segment_not_present:
	push 	11 				; vector_no = B
	jmp 	exception
stack_exception:
	push 	12 				; vector_no = C
	jmp 	exception
general_protection:
	push 	13 				; vector_no = D
	jmp 	exception
page_fault:
	push 	14 				; vector_no = E
	jmp 	exception
copr_error:
	push 	0xffffffff		; no error code
	push 	16 				; vector_no = 10h
	jmp 	exception

exception:
	call 	exception_handler
	add 	esp, 4 * 2
	hlt


	

