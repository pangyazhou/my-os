;说明----------------------------------------------------------------------------
;


;--------------------------------------------------------------------------------
;类型命名原则
;		DA_		:Descriptor Attribute
;		D 		:数据段
; 		C 		;代码段
; 		S  		;系统段
; 		R 		;只读
; 		RW		;读写
; 		A 		;已访问
; 		其他
;---------------------------------------------------------------------------------

;描述符类型
DA_32 			EQU 	4000H 	;32位段

DA_DPL0 		EQU 	00H 	;DPL = 0
DA_DPL1 		EQU 	20H 	;DPL = 1
DA_DPL2 		EQU 	40H 	;DPL = 2
DA_DPL2 		EQU 	60H 	;DPL = 3


;存储段描述符类型
DA_DR		EQU 		90h		;存在的只读数据段类型值
DA_DRW		EQU			92H		;存在的可读写数据段类型值
DA_DRWA 	EQU			93H 	;存在的已访问可读写数据段值类型
DA_C 		EQU 		98H		;存在的只执行代码段属性值
DA_CR 		EQU 		9AH 	;存在的可执行可读取代码段属性值
DA_CCO 		EQU 		9CH 	;存在的只执行一致性代码段属性值
DA_CCOR		EQU 		9EH 	;存在的可执行可读一致性代码段属性值

;系统段描述符类型
DA_LDT 		EQU 		82H 	;局部描述符表段类型值
DA_TaskGate EQU 		85H 	;任务门类型值
DA_386TSS 	EQU 		89H 	;可用386任务状态段类型值
DA_386CGate EQU 		8CH 	;386 调用门类型值
DA_386IGate EQU 		8EH 	;386 中断门类型值
DA_386TGate EQU 		8FH 	;386 陷阱门类型值


;宏------------------------------------------------------------------------------
;描述符
;usage: Descriptor Base, Limit, Attr
;		Base: 	dd
;		Limit:	dd (low 20 bits available)
;		Attr:	sw (lower 4 bits of high byte are always 0)
%macro	Descriptor 3 
		dw 			%2 & 0ffffh				;段界限2
		dw 			%1 & 0ffffh				;段基址1
		db			(%1 >> 16) & 0ffh 		;段基址2
		dw 			((%2 >> 8) & 0f00h) | (%3 & 0f0ffh);	;属性1 + 段界限2 + 属性2
		db 			(%1 >> 24) & 0ffh 		;段基址3
%endmacro	;共8个字节
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^