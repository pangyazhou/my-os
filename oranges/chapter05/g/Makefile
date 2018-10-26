#########################
# Makefile for Orange'S #
#########################

# Entry point of Orange'S
# It must have the save value with 'KernelEntryPointPhyAddr' in load.inc
ENTRYPOINT 		= 0x30400

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
CFLAGS 		= -I include/ -c -fno-builtin -m32		#编译内核C文件
LDFLAGS 	= -s -Ttext $(ENTRYPOINT) -e $(ENTRYOFFSET) -m elf_i386 	#连接目标文件
DASMFLAGS 	= -u -o $(ENTRYPOINT) -e $(ENTRYOFFSET)

# This Program
ORANGESBOOT 	= boot/boot.bin boot/loader.bin
ORANGESKERNEL 	= kernel.bin
OBJS 			= kernel/kernel.o kernel/start.o lib/kliba.o lib/string.o
DASMOUTPUT 		= kernel.bin.asm

# All Phony Targets
.PHONY : everything final image clean realclean disasm all building

# Default starting position
everything: $(ORANGESBOOT) $(ORANGESKERNEL)

all : realclean everything

final : all clean

image : final building

clean: 
	rm -f $(OBJS)

realclean :
	rm -f $(OBJS) $(ORANGESBOOT) $(ORANGESKERNEL) $(IMG)

disasm :
	$(DASM) $(DASMFLAGS) $(ORANGESKERNEL) > $(DASMOUTPUT)

# We assume that "a.img" exists in current folder
building:
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

kernel/kernel.o : kernel/kernel.asm 
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/start.o : kernel/start.c
	$(CC) $(CFLAGS) -o $@ $<

lib/kliba.o : lib/kliba.asm 
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/string.o : lib/string.asm 
	$(ASM) $(ASMKFLAGS) -o $@ $<


