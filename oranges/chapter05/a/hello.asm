;编译连接方法
;(ld 的 '-s' 选项意思为 "strip all")
;
; $ nasm -f elf hello.asm -o hello.o
; $ ld -s hello.o -o hello
; $ ./hello
; Hello, world!
; $

[section .data]	;数据段

strHello 		db 		"Hello,world!", 0ah
STRLEN 			equ 	$ - strHello

[section .text]	;代码段

global	_start 		;导出入口

_start:
	mov 	edx, STRLEN
	mov 	ecx, strHello
	mov 	ebx, 1
	mov 	eax, 4 			;系统调用 sys_write
	int 	0x80
	mov 	ebx, 0
	mov 	eax, 1 			; 系统调用 sys_exit
	int 	0x80