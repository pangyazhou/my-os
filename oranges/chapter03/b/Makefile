
pmtest2.com: pmtest2.asm
	nasm pmtest2.asm -o pmtest2.com

pmtest1.com: pmtest1.asm
	nasm pmtest1.asm -o pmtest1.com

everything: pmtest2.com
	sudo mount -o loop pm.img /mnt/floppy/
	sudo cp pmtest2.com /mnt/floppy/ -v
	sudo umount /mnt/floppy

start: 
	make -r everything

clean:
	rm pmtest2.com
