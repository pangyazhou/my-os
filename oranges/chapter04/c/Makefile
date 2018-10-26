# boot.bin:	boot.asm
# 	nasm boot.asm -o boot.bin

BOOT:=boot.asm
LDR:=pmtest9.asm
BOOT_BIN:=$(subst .asm,.bin,$(BOOT))
LDR_BIN:=$(subst .asm,.bin,$(LDR))

.PHONY 	: everything

everything: $(BOOT_BIN) $(LDR_BIN)
	dd if=/dev/zero of=a.img bs=512 count=2880
	dd if=$(BOOT_BIN) of=a.img bs=512 count=1 conv=notrunc
	sudo mount -o loop a.img /mnt/floppy/
	sudo cp $(LDR_BIN) /mnt/floppy/ -v
	sudo sleep 2s
	sudo umount /mnt/floppy	


start:
	make -r everything

$(BOOT_BIN) : $(BOOT)
	nasm $< -o $@
$(LDR_BIN) : $(LDR)
	nasm $< -o $@ 


clean:
	rm -f a.img
	rm -f $(BOOT_BIN) $(LDR_BIN)
	rm *.txt
	rm debugger.out