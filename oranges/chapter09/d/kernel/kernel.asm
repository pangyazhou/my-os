;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;						kernel.asm
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

;------------------------------------------------------------------------
; 编译链接方法
; $ nasm -f elf kernel.asm -o kernel.o
; $ ld -s -m elf_i386 kernel.o -o kernel.bin    #‘-s’选项意为“strip all”
;------------------------------------------------------------------------

%include "sys/sconst.inc"

;导入函数
extern 	cstart
extern 	exception_handler
extern 	spurious_irq
extern 	kernel_main
extern 	debug
extern 	disp_str
extern 	delay
extern 	clock_handler
extern 	irq_table

;导入全局变量
extern 	gdt_ptr
extern 	idt_ptr
extern 	p_proc_ready
extern 	tss
extern 	disp_pos
extern 	k_reenter
extern 	sys_call_table


;堆栈段
[section .bss]
StackSpace 		resb 	2 * 1024 		; 2K 堆栈空间
StackTop: 								;栈顶

;数据段
[section .data]
clock_int_msg 			db 		"^", 0

[section .text] ;代码段

	global _start		;导出 _start

	global restart

	global sys_call

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
	global hwint00
	global hwint01
	global hwint02
	global hwint03
	global hwint04
	global hwint05
	global hwint06
	global hwint07
	global hwint08
	global hwint09
	global hwint10
	global hwint11
	global hwint12
	global hwint13
	global hwint14
	global hwint15

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
	;		              struct struct descriptors               Selectors
	;              ┏━━━━━━━━━━━━━━━━━━┓
	;              ┃         Dummy struct struct descriptor           ┃
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
	xor 	eax, eax
	mov 	ax, SELECTOR_TSS
	ltr 	ax

	jmp 	kernel_main
	;hlt

; 中断和异常 -- 硬件中断
;---------------------------------
%macro 	hwint_master 	1
		call 	save		
		in 		al, INT_M_CTLMASK 		; 不允许硬件中断
		or 		al, (1 << %1)
		out 	INT_M_CTLMASK, al
		mov 	al, EOI
		out 	INT_M_CTL, al
		sti
		push 	%1
		call 	[irq_table + 4 * %1]
		pop 	ecx
		cli
		in 		al, INT_M_CTLMASK 		; 允许硬件中断
		and 	al, ~(1 << %1)
		out 	INT_M_CTLMASK, al
		ret 			; 重入时跳到.restart_reenter_v2 , 通常情况跳到.restart_v2
%endmacro
;---------------------------------

ALIGN 	16
hwint00:				; Interrupt routine for irq 0 (the clock);
		hwint_master 	0

ALIGN 	16
hwint01:				; Interrupt routine for irq 1 (keyboard);
		hwint_master 	1

ALIGN 	16
hwint02:				; Interrupt routine for irq 2 (cascade);
		hwint_master 	2

ALIGN 	16
hwint03:				; Interrupt routine for irq 3 (second serial);
		hwint_master 	3

ALIGN 	16
hwint04:				; Interrupt routine for irq 4 (first serial);
		hwint_master 	4

ALIGN 	16
hwint05:				; Interrupt routine for irq 5 (XT winchester);
		hwint_master 	5

ALIGN 	16
hwint06:				; Interrupt routine for irq 6 (floppy);
		hwint_master 	6

ALIGN 	16
hwint07:				; Interrupt routine for irq 7 (printer);
		hwint_master 	7

; 中断和异常 -- 硬件中断		
;---------------------------------
%macro 	hwint_slave 	1
		call 	save		
		in 		al, INT_S_CTLMASK 		; 不允许时钟中断
		or 		al, (1 << (%1 - 8))
		out 	INT_S_CTLMASK, al
		mov 	al, EOI
		out 	INT_M_CTL, al
		nop
		out 	INT_S_CTL, al
		sti
		push 	%1
		call 	[irq_table + 4 * %1]
		pop 	ecx
		cli
		in 		al, INT_S_CTLMASK 		; 允许时钟中断
		and 	al, ~(1 << (%1 - 8))
		out 	INT_S_CTLMASK, al
		ret 			; 重入时跳到.restart_reenter_v2 , 通常情况跳到.restart_v2
%endmacro
;---------------------------------

ALIGN 	16
hwint08:				; Interrupt routine for irq 8 (realtime clock).
		hwint_slave 	8

ALIGN 	16
hwint09:				; Interrupt routine for irq 9 (irq 2 redirected).
		hwint_slave 	9

ALIGN 	16
hwint10:				; Interrupt routine for irq 10 
		hwint_slave 	10

ALIGN 	16
hwint11:				; Interrupt routine for irq11
		hwint_slave 	11

ALIGN 	16
hwint12:				; Interrupt routine for irq 12
		hwint_slave 	12

ALIGN 	16
hwint13:				; Interrupt routine for irq 13 (FPU exception)
		hwint_slave 	13

ALIGN 	16
hwint14:				; Interrupt routine for irq 14 (AT winchester).
		hwint_slave 	14

ALIGN 	16
hwint15:				; Interrupt routine for irq 15
		hwint_slave 	15




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
	mov 	eax, dword [disp_pos]
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
	iretd


; ====================================================================================
;                                   sys_call
; 中断和异常 -- 软件中断 -- 系统调用
; ====================================================================================
sys_call:
	call 	save
	sti
	push 	esi

	push 	dword [p_proc_ready]
	push 	edx
	push 	ecx
	push 	ebx
	call 	[sys_call_table + eax * 4]
	add 	esp, 4 * 4

	pop 	esi
	mov 	[esi + EAXREG - P_STACKBASE], eax
	cli
	ret

; ====================================================================================
;                                   save
; ====================================================================================
save:
		pushad
		push 	ds
		push 	es
		push 	fs
		push 	gs
		mov 	esi, edx
		mov 	dx, ss
		mov 	ds, dx
		mov 	es, dx
		mov 	edx, esi

		mov 	esi, esp 		; 进程表起始地址

		inc 	dword [k_reenter] 			;判断是否重入
		cmp 	dword [k_reenter], 0
		jne 	.1

		mov 	esp, StackTop 		;进入内核栈
		push 	restart
		jmp 	[esi + RETADR - P_STACKBASE]
.1:
		push 	restart_reenter
		jmp 	[esi + RETADR - P_STACKBASE]

; ====================================================================================
;                                   restart
; ====================================================================================
restart:
	mov 	esp, [p_proc_ready] 		; 退出内核栈
	lldt 	[esp + P_LDT_SEL]
	lea 	eax, [esp + P_STACKTOP]
	mov 	dword [tss + TSS3_S_SP0], eax 			; esp0 

restart_reenter:
	dec 	dword [k_reenter]
	pop 	gs
	pop 	fs
	pop 	es
	pop 	ds
	popad 	

	add 	esp, 4
	iretd

