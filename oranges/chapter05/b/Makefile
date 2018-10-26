

.PHONY 	: everything

	
foo.o : foo.asm
	nasm -f elf  -o foo.o foo.asm

bar.o : bar.c
	gcc -m32 -c -o bar.o bar.c

everything: foo.o bar.o
	ld -s -m elf_i386 -o foobar foo.o bar.o

start: 
	make -r everything

clean:
	rm foo.o
	rm bar.o
