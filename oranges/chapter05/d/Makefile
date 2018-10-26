# boot.bin:	boot.asm
# 	nasm boot.asm -o boot.bin

BOOT:=boot.asm
LDR:=loader.asm
KERNEL:=kernel.asm
BOOT_BIN:=$(subst .asm,.bin,$(BOOT))
LDR_BIN:=$(subst .asm,.bin,$(LDR))
KERNEL_BIN:=$(subst .asm,.bin,$(KERNEL))

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
$(KERNEL_BIN) : $(KERNEL)
	nasm -f elf -o $(subst .asm,.o,$(KERNEL)) $<
	ld -s -m elf_i386 -Ttext 0x30400 -o $@ $(subst .asm,.o,$(KERNEL))


clean:
	rm -f $(BOOT_BIN) $(LDR_BIN) $(KERNEL_BIN) $(IMG) *.o 
	rm *.txt
	rm debugger.out