#########################
# Makefile for Orange'S #
#########################

# Entry point of Orange'S
# It must have the save value with 'KernelEntryPointPhyAddr' in load.inc
ENTRYPOINT 		= 0x1000

# Offset of entry point in kernel file
# It depends on ENTRYPOINT
ENTRYOFFSET 	= 0x400 

# Programs, flags, etc.
IMG 	= a.img
ASM 	= nasm
DASM 	= ndisasm
CC 		= gcc
LD 		= ld 
ASMBFLAGS 	= -I boot/include/      				#编译boot
ASMKFLAGS 	= -I include/ -f elf 					#编译内核asm文件
CFLAGS 		= -I include/ -c -fno-builtin -fno-stack-protector -m32 		#编译内核C文件
LDFLAGS 	= -s -Ttext $(ENTRYPOINT) -Map krnl.map -m elf_i386 	#连接目标文件
DASMFLAGS 	= -u -o $(ENTRYPOINT) -e $(ENTRYOFFSET)

# This Program
ORANGESBOOT 	= boot/boot.bin boot/loader.bin
ORANGESKERNEL 	= kernel.bin
OBJS 			= kernel/kernel.o kernel/start.o kernel/i8259.o kernel/global.o\
				kernel/protect.o kernel/main.o kernel/clock.o kernel/proc.o kernel/keyboard.o\
				kernel/tty.o kernel/console.o kernel/printf.o kernel/vsprintf.o kernel/systask.o\
				kernel/hd.o\
				fs/main.o fs/open.o fs/misc.o fs/read_write.o fs/link.o\
				lib/kliba.o lib/klib.o lib/string.o lib/syscall.o lib/misc.o lib/close.o\
				lib/open.o lib/read.o lib/write.o lib/unlink.o
DASMOUTPUT 		= kernel.bin.asm

# All Phony Targets
.PHONY : everything final image clean realclean disasm all buildimg

# Default starting position
everything: $(ORANGESBOOT) $(ORANGESKERNEL)

all : realclean everything

final : all clean

image : final buildimg

clean: 
	rm -f $(OBJS) *.lock

realclean :
	rm -f $(OBJS) $(ORANGESBOOT) $(ORANGESKERNEL) $(IMG)

cleanall: realclean
	rm bochsout.txt debugger.out *.lock

disasm :
	$(DASM) $(DASMFLAGS) $(ORANGESKERNEL) > $(DASMOUTPUT)

# We assume that "a.img" exists in current folder
buildimg:
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	dd if=boot/boot.bin of=$(IMG) bs=512 count=1 conv=notrunc
	sudo mount -o loop $(IMG) /mnt/floppy/
	sudo cp -fv boot/loader.bin /mnt/floppy/
	sudo cp -fv kernel.bin /mnt/floppy/
	sudo sleep 2s
	sudo umount /mnt/floppy/

boot/boot.bin : boot/boot.asm boot/include/load.inc boot/include/fat12hdr.inc \
			boot/include/lib.inc 
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/loader.bin : boot/loader.asm  boot/include/load.inc boot/include/fat12hdr.inc \
			boot/include/lib.inc boot/include/pm.inc 
	$(ASM) $(ASMBFLAGS) -o $@ $<

# 链接目标程序,生成可执行程序
$(ORANGESKERNEL) : $(OBJS)
	$(LD) $(LDFLAGS) -o $(ORANGESKERNEL) $(OBJS)

kernel/kernel.o : kernel/kernel.asm include/sys/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/start.o : kernel/start.c include/type.h include/sys/const.h include/sys/protect.h \
		include/sys/proto.h include/string.h include/sys/proc.h include/sys/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/main.o : kernel/main.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/i8259.o : kernel/i8259.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/global.o : kernel/global.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/protect.o : kernel/protect.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/clock.o : kernel/clock.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/proc.o : kernel/proc.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/keyboard.o : kernel/keyboard.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/tty.o : kernel/tty.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/console.o : kernel/console.c  
	$(CC) $(CFLAGS) -o $@ $<

kernel/printf.o : kernel/printf.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/vsprintf.o : kernel/vsprintf.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/systask.o : kernel/systask.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/hd.o : kernel/hd.c include/sys/hd.h
	$(CC) $(CFLAGS) -o $@ $<


fs/main.o : fs/main.c include/sys/fs.h
	$(CC) $(CFLAGS) -o $@ $<

fs/open.o : fs/open.c 
	$(CC) $(CFLAGS) -o $@ $<

fs/misc.o : fs/misc.c 
	$(CC) $(CFLAGS) -o $@ $<

fs/read_write.o : fs/read_write.c 
	$(CC) $(CFLAGS) -o $@ $<

fs/link.o : fs/link.c 
	$(CC) $(CFLAGS) -o $@ $<

lib/klib.o : lib/klib.c  include/type.h include/sys/const.h include/sys/protect.h \
		include/sys/proto.h include/string.h include/sys/proc.h include/sys/global.h
	$(CC) $(CFLAGS) -o $@ $<

lib/misc.o : lib/misc.c
	$(CC) $(CFLAGS) -o $@ $<

lib/open.o : lib/open.c
	$(CC) $(CFLAGS) -o $@ $<

lib/close.o : lib/close.c
	$(CC) $(CFLAGS) -o $@ $<

lib/read.o : lib/read.c
	$(CC) $(CFLAGS) -o $@ $<
	
lib/write.o : lib/write.c
	$(CC) $(CFLAGS) -o $@ $<

lib/unlink.o : lib/unlink.c
	$(CC) $(CFLAGS) -o $@ $<


lib/kliba.o : lib/kliba.asm 
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/string.o : lib/string.asm 
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/syscall.o : lib/syscall.asm 
	$(ASM) $(ASMKFLAGS) -o $@ $<

