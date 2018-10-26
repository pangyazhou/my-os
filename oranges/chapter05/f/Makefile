# boot.bin:	boot.asm
# 	nasm boot.asm -o boot.bin

BOOT:=boot.asm
LDR:=loader.asm
KERNEL:=kernel.asm
STRING:=string.asm
KLIBA:=kliba.asm
START:=start.c
BOOT_BIN:=$(subst .asm,.bin,$(BOOT))
LDR_BIN:=$(subst .asm,.bin,$(LDR))
KERNEL_BIN:=$(subst .asm,.bin,$(KERNEL))
START_O:=$(subst .c,.o,$(START))

IMG:=a.img
FLOPPY:=/mnt/floppy


.PHONY 	: everything

everything: $(BOOT_BIN) $(LDR_BIN) $(KERNEL_BIN)
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	dd if=$(BOOT_BIN) of=$(IMG) bs=512 count=1 conv=notrunc
	sudo mount -o loop $(IMG) $(FLOPPY)
	sudo cp $(LDR_BIN) $(FLOPPY) -v
	sudo cp $(KERNEL_BIN) $(FLOPPY) -v
	sudo sleep 2s
	sudo umount $(FLOPPY)


start:
	make -r everything

$(BOOT_BIN) : $(BOOT)
	nasm $< -o $@
$(LDR_BIN) : $(LDR)
	nasm $< -o $@ 
$(KERNEL_BIN) : $(KERNEL) $(STRING) $(KLIBA)
	nasm -f elf -o $(subst .asm,.o,$(KERNEL)) $(KERNEL)
	nasm -f elf -o $(subst .asm,.o,$(STRING)) $(STRING)
	nasm -f elf -o $(subst .asm,.o,$(KLIBA)) $(KLIBA)
	gcc -c -fno-builtin -m32 -o $(START_O) $(START)
	ld -s -m elf_i386 -Ttext 0x30400 -o $@ $(subst .asm,.o,$(KERNEL)) \
	$(subst .asm,.o,$(STRING)) $(subst .asm,.o,$(KLIBA)) \
	$(START_O)


clean:
	rm -f $(BOOT_BIN) $(LDR_BIN) $(KERNEL_BIN) $(IMG) *.o 
	rm *.txt
	rm debugger.out