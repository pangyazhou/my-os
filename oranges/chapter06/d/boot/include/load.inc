
BaseOfLoader 			equ 	09000h 		;LOADER.BIN 被加载到的位置  ---段地址
OffsetOfLoader 			equ 	0100h		;LOADER.BIN 被加载到的位置  ---偏移地址

BaseOfLoaderPhyAddr 	equ 	BaseOfLoader * 10h

BaseOfKernelFile 		equ 	08000h 			; KERNEL.BIN 被加载的段地址
OffsetOfKernelFile 		equ 	0h 				; KERNEL.BIN 被加载的偏移地址

BaseOfKernelFilePhyAddr 	equ 	BaseOfKernelFile * 10h
KernelEntryPointPhyAddr 	equ 	0x30400
