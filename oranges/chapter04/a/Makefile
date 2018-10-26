# boot.bin:	boot.asm
# 	nasm boot.asm -o boot.bin

%.bin: %.asm
	nasm $*.asm -o $*.bin

boot.img: boot.bin loader.bin
	dd if=/dev/zero of=a.img bs=512 count=2880
	dd if=boot.bin of=a.img bs=512 count=1 conv=notrunc
	sudo mount -o loop a.img /mnt/floppy
	sudo cp loader.bin /mnt/floppy -v
	sudo sleep 1s
	sudo umount /mnt/floppy	


everything: boot.img

start:
	make -r everything

clean:
	rm a.img
	rm *.bin
	rm *.txt
	rm debugger.out