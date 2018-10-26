SRC:=pmtest4.asm
BIN:=$(subst .asm,.com,$(SRC))

.PHONY : everything


everything: $(BIN)
	sudo mount -o loop pm.img /mnt/floppy/
	sudo cp $(BIN) /mnt/floppy/ -v
	sudo umount /mnt/floppy

$(BIN) : $(SRC)
	nasm $< -o $@

clean:
	rm $(BIN)

cleanall:
	make -r clean
	rm *.txt
	rm *.out
