/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						hd.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
#include "hd.h"


PRIVATE void init_hd();
PRIVATE void hd_identify(int drive);
PRIVATE void print_identify_info(u16* hdinfo);
PRIVATE void hd_cmd_out(struct hd_cmd* cmd);
PRIVATE void interrupt_wait();
PRIVATE int waitfor(int mask, int val, int timeout);
PRIVATE void print_hdinfo(struct hd_info* hdi);
PRIVATE void hd_open(int device);
PRIVATE void get_part_table(int drive, int sect_nr, struct part_ent* entry);
PRIVATE void partition(int device, int style);
PRIVATE void hd_rdwt(MESSAGE* msg);
PRIVATE void hd_ioctl(MESSAGE* msg);
PRIVATE void hd_close(int device);


PRIVATE u8 hd_status;
PRIVATE u8 hdbuf[SECTOR_SIZE * 2];
PRIVATE struct hd_info hd_info[1];

#define DRV_OF_DEV(dev) (dev <= MAX_PRIM ? \
						dev / NR_PRIM_PER_DRIVE : \
						(dev - MINOR_hd1a) / NR_SUB_PER_DRIVE)
/*===============================================================================
					void task_hd();
=================================================================================*/
/**
* <Ring 1> The main loop of HD driver.
=================================================================================*/
PUBLIC void task_hd()
{
	MESSAGE msg;

	init_hd();

	while(1)
	{
		send_recv(RECEIVE, ANY, &msg);
		int src = msg.source;

		switch(msg.type)
		{
			case DEV_OPEN:
				hd_open(msg.DEVICE);
				break;
			case DEV_CLOSE:
				hd_close(msg.DEVICE);
				break;
			case DEV_READ:
			case DEV_WRITE:
				hd_rdwt(&msg);
				break;
			case DEV_IOCTL:
				hd_ioctl(&msg);
				break;
			default:
				dump_msg("HD driver: unknown msg", &msg);
				spin("FS:main_loop (invalid msg.type)");
				break;
		}
		send_recv(SEND, src, &msg);
	}
}
/*===============================================================================
					void init_hd();
=================================================================================*/
/**
* <Ring 1> Check hard drive, set IRQ handler, enable IRQ and initialize 
* data structures.
*
=================================================================================*/
PRIVATE void init_hd()
{
	int i;
	/* Get the number of drives from the BIOS data area. */
	u8 *pNrDrives = (u8*)0x475;
	printl("NrDrives:%d.\n", *pNrDrives);
	assert(*pNrDrives);

	put_irq_handler(AT_WINI_IRQ, hd_handler);
	enable_irq(CASCADE_IRQ);
	enable_irq(AT_WINI_IRQ);

	for(i = 0; i < (sizeof(hd_info) / sizeof(hd_info[0])); i++)
	{
		memset(&hd_info[i], 0, sizeof(hd_info[0]));
	}
	hd_info[0].open_cnt = 0;
}

/*===============================================================================
					void hd_open(int device);
=================================================================================*/
/**
* <Ring 1> This routine handles DEV_OPEN message. It identify the drive of 
* the given device and read the partition table of the drive if it has 
* not been read.
*
* @param device The device to be opened.
=================================================================================*/
PRIVATE void hd_open(int device)
{
	int drive = DRV_OF_DEV(device);
	assert(drive == 0); 			/* only one drive */

	hd_identify(drive);

	if(hd_info[drive].open_cnt++ == 0)
	{
		partition(drive * (NR_PART_PER_DRIVE + 1), P_PRIMARY);
		print_hdinfo(&hd_info[drive]);
	}
}

/*===============================================================================
			void hd_close(int device);
=================================================================================*/
/**
* <Ring 1> This routine handles DEV_CLOSE message.
*
* @param device 	The device to be opened. 
=================================================================================*/
PRIVATE void hd_close(int device)
{
	int drive = DRV_OF_DEV(device);
	assert(drive == 0);

	hd_info[drive].open_cnt--; 
}

/*===============================================================================
			void hd_rdwt(MESSAGE* msg);
=================================================================================*/
/**
* <Ring 1> This routine handles DEV_READ and DEV_WRITE message.
*
* @param p 	Message ptr.
=================================================================================*/
PRIVATE void hd_rdwt(MESSAGE* msg)
{
	int i;
	int drive = DRV_OF_DEV(msg->DEVICE);

	u64 pos = msg->POSITION;
	assert((pos >> SECTOR_SIZE_SHIFT) < (1 << 31));

	/* We only allow to R/W from a SECTOR boundary */
	assert((pos & 0x1ff) == 0);

	u32 sect_nr = (u32)(pos >> SECTOR_SIZE_SHIFT); 	/* 读/写的起始扇区 */
	int logidx = (msg->DEVICE - MINOR_hd1a) % NR_SUB_PER_DRIVE;
	sect_nr += msg->DEVICE < MAX_PRIM ? 
		hd_info[drive].primary[msg->DEVICE].base :
		hd_info[drive].logical[logidx].base;

	struct hd_cmd cmd;
	cmd.features = 0;
	cmd.count = (msg->CNT + SECTOR_SIZE - 1) / SECTOR_SIZE;		//读/写字节数
	cmd.lba_low = sect_nr & 0xff;
	cmd.lba_mid = (sect_nr >> 8) & 0xff;
	cmd.lba_high = (sect_nr >> 16) & 0xff;
	cmd.device = MAKE_DEVICE_REG(1, drive, (sect_nr >> 24) & 0x0f);
	cmd.command = (msg->type == DEV_READ) ? ATA_READ : ATA_WRITE;
	hd_cmd_out(&cmd);

	int bytes_left = msg->CNT;
	void * la = (void*)va2la(msg->PROC_NR, msg->BUF);

	while(bytes_left)
	{
		int bytes = min(SECTOR_SIZE, bytes_left);
		if(msg->type == DEV_READ)
		{
			interrupt_wait();
			port_read(REG_DATA, hdbuf, SECTOR_SIZE);
			phys_copy(la, (void*)va2la(TASK_HD, hdbuf), bytes);
		}
		else
		{
			if(!(waitfor(STATUS_DRQ, STATUS_DRQ, HD_TIMEOUT)))
			{
				panic("hd writing error.");
			}
			port_write(REG_DATA, la, bytes);
			interrupt_wait();
		}
		bytes_left -= SECTOR_SIZE;
		la += SECTOR_SIZE;
	}
}

/*===============================================================================
			void hd_ioctl(MESSAGE* msg);
=================================================================================*/
/**
* <Ring 1> This routine handles DEV_IOCTL message.
*
* @param p 	Message ptr.
=================================================================================*/
PRIVATE void hd_ioctl(MESSAGE* msg)
{
	int device = msg->DEVICE;
	int drive = DRV_OF_DEV(device);

	struct hd_info* hdi = &hd_info[drive];
	if(msg->REQUEST == DIOCTL_GET_GEO)
	{
		void* dst = va2la(msg->PROC_NR, msg->BUF);
		void* src = va2la(TASK_HD, 
					device < MAX_PRIM ?
					&hdi->primary[device] :
					&hdi->logical[(device - MINOR_hd1a) % NR_SUB_PER_DRIVE]);
		phys_copy(dst, src, sizeof(struct part_info));
	}
	else
	{
		assert(0);
	}
}


/*===============================================================================
			void get_part_table(int drive, int sect_nr, struct part_ent *entry);
=================================================================================*/
/**
* <Ring 1> Get a partition table of a drive.
*
* @param drive 		Drive nr (0 for the 1st disk, 1 for the 2nd, ...)n
* @param sect_nr 	The sector at which the partition table is located.
* @param entry 		Ptr to part_ent struct 
=================================================================================*/
PRIVATE void get_part_table(int drive, int sect_nr, struct part_ent* entry)
{
	int i;
	struct hd_cmd cmd;
	cmd.features = 0;
	cmd.count = 1;
	cmd.lba_low = sect_nr & 0xff;
	cmd.lba_mid = (sect_nr >> 8) & 0xff;
	cmd.lba_high = (sect_nr >> 16) & 0xff;
	cmd.device = MAKE_DEVICE_REG(1,				/* LBA mode */
					drive,
					(sect_nr >> 24) & 0x0f);
	cmd.command = ATA_READ;
	hd_cmd_out(&cmd);
	interrupt_wait();

	port_read(REG_DATA, hdbuf, SECTOR_SIZE);
	memcpy(entry, 
			hdbuf + PARTITION_TABLE_OFFSET,
			sizeof(struct part_ent) * NR_PART_PER_DRIVE);
}

/*===============================================================================
					void partition(int device, int style);
=================================================================================*/
/**
* <Ring 1> This routine is called when a device is opened. It reads the 
* partition table(s) and fills the hd_info struct.
*
* @param device 	Device nr.
* @param style 		P_PRIMARY OR P_EXTENDED
=================================================================================*/
PRIVATE void partition(int device, int style)
{
	int i;
	int drive = DRV_OF_DEV(device);
	struct hd_info* hdi = &hd_info[drive];

	struct part_ent part_tbl[NR_SUB_PER_DRIVE];

	if(style == P_PRIMARY)
	{
		get_part_table(drive, drive, part_tbl);

		int nr_prim_parts = 0;
		for(i = 0; i < NR_PART_PER_DRIVE; i++)
		{
			if(part_tbl[i].sys_id == NO_PART)
				continue;
			nr_prim_parts++;
			int dev_nr = i + 1;
			hdi->primary[dev_nr].base = part_tbl[i].start_sect;
			hdi->primary[dev_nr].size = part_tbl[i].nr_sects;

			if(part_tbl[i].sys_id == EXT_PART)
			{
				partition(device + dev_nr, P_EXTENDED);
			}
		}
		assert(nr_prim_parts != 0);
	}
	else if(style == P_EXTENDED)
	{
		int j = device % NR_PRIM_PER_DRIVE; 	/* 1~4 */
		int ext_start_sect = hdi->primary[j].base;
		int s = ext_start_sect;
		int nr_1st_sub = (j - 1) * NR_SUB_PER_PART;

		for(i = 0; i < NR_SUB_PER_DRIVE; i++)
		{
			int dev_nr = nr_1st_sub + i;
			get_part_table(drive, s, part_tbl);
			hdi->logical[dev_nr].base = s + part_tbl[0].start_sect;
			hdi->logical[dev_nr].size = part_tbl[0].nr_sects;

			s = ext_start_sect + part_tbl[1].start_sect;

			/* no more logical partigion in this extended partition */
			if(part_tbl[1].sys_id == NO_PART)
				break;
		}
	}
	else
	{
		assert(0);
	}
}

/*===============================================================================
					void print_hdinfo(struct hd_info* hdi);
=================================================================================*/
/**
* <Ring 1> Print disk info.
* 
* @param hdi 	Ptr to struct hd_info.
=================================================================================*/
PRIVATE void print_hdinfo(struct hd_info* hdi)
{
	int i;
	for(i = 0; i < NR_PART_PER_DRIVE + 1; i++)
	{
		printl("%sPART_%d: base %d(0x%x), size %d(0x%x) (int sector)\n",
				i == 0 ? " " : "    ",
				i,
				hdi->primary[i].base,
				hdi->primary[i].base,
				hdi->primary[i].size,
				hdi->primary[i].size);
	}
	for(i = 0; i < NR_SUB_PER_DRIVE; i++)
	{
		if(hdi->logical[i].size == 0)
			continue;
		printl("        "
			"%d: base %d(0x%x) size %d(0x%x) (in sector)\n",
			i,
			hdi->logical[i].base,
			hdi->logical[i].base,
			hdi->logical[i].size,
			hdi->logical[i].size);
	}
}


/*===============================================================================
					void hd_identify(int drive);
=================================================================================*/
/**
* <Ring 1> Get the disk information.
*
* @param drive 	Drive Nr.
=================================================================================*/
PRIVATE void hd_identify(int drive)
{
	struct hd_cmd cmd;
	cmd.device = MAKE_DEVICE_REG(0, drive, 0);
	cmd.command = ATA_IDENTIFY;
	hd_cmd_out(&cmd);
	interrupt_wait();
	port_read(REG_DATA, hdbuf, SECTOR_SIZE);

	print_identify_info((u16*)hdbuf);

	u16* hdinfo = (u16*)hdbuf;
	hd_info[drive].primary[0].base = 0;
	/* Total Nr of User Addressable Sectors */
	hd_info[drive].primary[0].size = ((int)hdinfo[61] << 16) + hdinfo[60];
}


/*===============================================================================
					void print_identify_info();
=================================================================================*/
/**
* <Ring 1> Print the hdinfo retrieved via ATA_IDENTIFY command.
*
* @param hdinfo The buffer read from the disk i/o port.
=================================================================================*/
PRIVATE void print_identify_info(u16* hdinfo)
{
	int i, k;
	char s[64];

	struct iden_info_ascii
	{
		int idx;
		int len;
		char * desc;
	}iinfo[] = {{10, 20, "HD SN"},		/* Serial number in ASCII */
				{27, 40, "HD Model"}};	/* Model number in ASCII */

	for(k = 0; k < sizeof(iinfo) / sizeof(iinfo[0]); k++)
	{
		char *p = (char*)&hdinfo[iinfo[k].idx];
		for(i = 0; i < iinfo[k].len/2; i++)
		{
			s[i * 2 + 1] = *p++;
			s[i * 2] = *p++;
		}
		s[i * 2] = 0;
		printl("%s: %s\n", iinfo[k].desc, s);
	}

	int capabilities = hdinfo[49];
	printl("LBA supported: %s\n",
		(capabilities & 0x0200) ? "Yes" : "No");

	int cmd_set_supported = hdinfo[83];
	printl("LBA48 supported: %s\n", 
		(cmd_set_supported & 0x0400) ? "Yes" : "No");

	int sectors = ((int)hdinfo[61] << 16) + hdinfo[60];
	printl("HD size: %dMB\n", sectors * 512 / 1000000);
}


/*===============================================================================
					void hd_cmd_out();
=================================================================================*/
/**
* <Ring 1> Output a command to HD controller.
*
* @param cmd The command struct ptr.
=================================================================================*/
PRIVATE void hd_cmd_out(struct hd_cmd* cmd)
{
	/**
	 * For all commands, the host must first check if BSY=1,
	 * and should proceed no further unless and until BSY=0.
	 */
	if(!waitfor(STATUS_BSY, 0, HD_TIMEOUT))
	{
		panic("hd error.");
	}

	/* Activate the Interrupt Enable bit */
	out_byte(REG_DEV_CTRL, 0);
	/* Load required parameters in the Command block Registers */
	out_byte(REG_FEATURES, cmd->features);
	out_byte(REG_NSECTOR, cmd->count);
	out_byte(REG_LBA_LOW, cmd->lba_low);
	out_byte(REG_LBA_MID, cmd->lba_mid);
	out_byte(REG_LBA_HIGH, cmd->lba_high);
	out_byte(REG_DEVICE, cmd->device);
	/* Write the command code to the Command Register. */
	out_byte(REG_CMD, cmd->command);
}

/*===============================================================================
					void interrupt_wait();
=================================================================================*/
/**
* <Ring 1> Wait until a disk interrupt occurs.
*
=================================================================================*/
PRIVATE void interrupt_wait()
{
	MESSAGE msg;
	send_recv(RECEIVE, INTERRUPT, &msg);
}

/*===============================================================================
					void waitfor();
=================================================================================*/
/**
* <Ring 1> Wait for a certain status.
*
* @param mask 		Status mask
* @param val 		Required status
* @param timeout 	Timeout in milliseconds
*
* @return One if sucess, zero if timeout.
=================================================================================*/
PRIVATE int waitfor(int mask, int val, int timeout)
{
	int t = get_ticks();
	u8 ch;
	while(((get_ticks() - t) * 1000/ HZ) < timeout)
	{
		if(((ch = in_byte(REG_STATUS)) & mask) == val)
			return 1;
	}
	return 0;
}

/*===============================================================================
					void hd_handler();
=================================================================================*/
/**
 * <Ring 0> Interrupt handler.
 * 
 * @param irq IRQ nr of the disk interrupt.
=================================================================================*/
PUBLIC void hd_handler()
{
	/*
	 *	Interrupts are cleared when the host
	 *  	- reads the Status Register,
	 * 		- issues a reset, or
	 * 		- writes to the Command Register.
	 */
	hd_status = in_byte(REG_STATUS);
	inform_int(TASK_HD);
}

/* get hard disk status */
PUBLIC u8 get_hd_status()
{
	return in_byte(REG_STATUS);
}