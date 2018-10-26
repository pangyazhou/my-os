
pmtest1.bin : pmtest1.asm pm.inc
	nasm pmtest1.asm -o pmtest1.bin

a.img: pmtest1.bin
	dd if=/dev/zero of=a.img bs=512 count=2880
	dd if=createboot.bin of=a.img bs=512 count=1 conv=notrunc
	dd if=pmtest1.bin of=a.img bs=512 count=1 conv=notrunc

boot.bin : boot.asm
	nasm boot.asm -o boot.bin

boot.img : boot.bin
	dd if=/dev/zero of=boot.img bs=512 count=2880
	dd if=boot.bin of=boot.img bs=512 count=1 conv=notrunc

start: 
	make -r a.img

boot:
	make -r boot.img

clean:
	rm pmtest1.bin
	rm a.img

