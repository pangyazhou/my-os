; 编译链接方法
; $ nasm -f elf kernel.asm -o kernel.o
; $ ld -s -m elf_i386 kernel.o -o kernel.bin    #‘-s’选项意为“strip all”

[section .text] ;代码段

	global _start		;导出 _start

_start:
	mov 	ah, 0fh
	mov 	al, 'K'
	mov 	[gs:((80 * 1 + 39) * 2)], ax
	jmp 	$

