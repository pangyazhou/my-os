;编译连接方法
;(ld 的 '-s' 选项意思为 "strip all")
;
; $ nasm -f elf foo.asm -o foo.o
; $ ld -s -m elf_i386 foo.o -o foo 	 	; 编译32位汇编代码 -m elf_i386
; $ ./foo
; $

extern choose 		;int choose(int a, int b);

[section .data]

	num1st 			dd 	6
	num2st 			dd  4

[section .text] 

	global _start 			;导出_start入口,便于链接器识别
	global myprint 			;导出让bar.c使用

_start:
	push 	dword [num1st]
	push 	dword [num2st]
	call 	choose
	add 	esp, 8

	mov 	ebx, 0
	mov 	eax, 1
	int 	0x80 			;系统调用sys_exit

; void myprint(char* msg, int len)
myprint:
	mov 	edx, [esp + 8]  	; len
	mov 	ecx, [esp + 4]		; msg
	mov 	ebx, 1
	mov 	eax, 4
	int 	0x80
	ret
