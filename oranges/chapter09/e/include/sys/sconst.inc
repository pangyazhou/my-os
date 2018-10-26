
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               sconst.inc
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

P_STACKBASE	equ	0
GSREG		equ	P_STACKBASE
FSREG		equ	GSREG		+ 4
ESREG		equ	FSREG		+ 4
DSREG		equ	ESREG		+ 4
EDIREG		equ	DSREG		+ 4
ESIREG		equ	EDIREG		+ 4
EBPREG		equ	ESIREG		+ 4
KERNELESPREG	equ	EBPREG		+ 4
EBXREG		equ	KERNELESPREG	+ 4
EDXREG		equ	EBXREG		+ 4
ECXREG		equ	EDXREG		+ 4
EAXREG		equ	ECXREG		+ 4
RETADR		equ	EAXREG		+ 4
EIPREG		equ	RETADR		+ 4
CSREG		equ	EIPREG		+ 4
EFLAGSREG	equ	CSREG		+ 4
ESPREG		equ	EFLAGSREG	+ 4
SSREG		equ	ESPREG		+ 4
P_STACKTOP	equ	SSREG		+ 4
P_LDT_SEL 	EQU P_STACKTOP
P_LDT 		EQU P_LDT_SEL 	+ 4

TSS3_S_SP0 	EQU 4

SELECTOR_FLAT_C 	EQU 	0x08
SELECTOR_TSS 		EQU 	0X20
SELECTOR_KERNEL_CS 	EQU 	SELECTOR_FLAT_C

INT_M_CTL 		EQU 	0X20		; I/O port for interrupt controller
INT_M_CTLMASK 	EQU 	0X21		; setting bits in this port disable ints
INT_S_CTL 		EQU 	0XA0
INT_S_CTLMASK 	EQU 	0XA1

EOI 			EQU 	0X20