/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						proc.c
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"



/*===============================================================================
					void schedule();
=================================================================================*/
PUBLIC void schedule()
{
	struct proc* p;
	int greatest_ticks = 0;

	while(!greatest_ticks){ 	// greatest_ticks = 0;
		for(p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p++)
		{
			if(p->ticks > greatest_ticks)
			{
				greatest_ticks = p->ticks;
				p_proc_ready = p;
			}
		}
		if(!greatest_ticks)
		{
			for(p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p++)
			{
				p->ticks = p->priority;
			}
		}
	}
}

/*===============================================================================
			int sys_sendrec(int function, int src_dest, MESSAGE* msg, struct proc* p_proc);
=================================================================================*/
PUBLIC int sys_sendrec(int function, int src_dest, MESSAGE* msg, struct proc* p)
{

}


/*===============================================================================
					int ldt_seg_linear(struct proc* p, int idx);
; 计算进程局部描述符表中指定段的线性地址基址
=================================================================================*/
PUBLIC int ldt_seg_linear(struct proc* p, int idx)
{
	DESCRIPTOR* desc = &p->ldts[idx];
	return desc->base_high << 24 | desc->base_mid << 16 | desc->base_low;
}

/*===============================================================================
					void* va2la(int pid, void* va);
=================================================================================*/
PUBLIC void* va2la(int pid, void* va)
{
	struct proc* proc = &proc_table[pid];

	u32 seg_base = ldt_seg_linear(proc, INDEX_LDT_RW);
	u32 la = seg_base + (u32)va;

	if(pid < NR_TASKS + NR_PROCS)
	{
		assert(la == (u32)va);
	}
	return (void*)la;
}