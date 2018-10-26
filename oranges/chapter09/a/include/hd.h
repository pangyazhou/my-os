/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            hd.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef 	_ORGANGS_HD_H_
#define 	_ORGANGS_HD_H_


/*******************************************************
/* I/O Ports used by hard disk controllers.
********************************************************/
/* slave disk not supported yet, all master registers below */

/* Command Block Registers */
/* 		MACRO 			PORT 			DESCRIPTON 				INPUT/OUTPUT   	*/
#define REG_DATA		0X1F0 			/* DATA 					I/O 		*/
#define REG_FEATURES	0X1F1			/* Features 				O 			*/
#define REG_ERROR 		REG_FEATURES 	/* Error 					I 			*/

#define REG_NSECTOR		0x1F2			/*	Sector Count			I/O			*/
#define REG_LBA_LOW		0x1F3			/*	Sector Number / LBA Bits 0-7	I/O		*/
#define REG_LBA_MID		0x1F4			/*	Cylinder Low / LBA Bits 8-15	I/O		*/
#define REG_LBA_HIGH	0x1F5			/*	Cylinder High / LBA Bits 16-23	I/O		*/
#define REG_DEVICE		0x1F6			/*	Drive | Head | LBA bits 24-27	I/O		*/

#define REG_STATUS	0x1F7		/*	Status				I		*/

#define	STATUS_BSY	0x80
#define	STATUS_DRDY	0x40
#define	STATUS_DFSE	0x20
#define	STATUS_DSC	0x10
#define	STATUS_DRQ	0x08
#define	STATUS_CORR	0x04
#define	STATUS_IDX	0x02
#define	STATUS_ERR	0x01

#define REG_CMD		REG_STATUS	/*	Command				O		*/

/* Control Block Registers */
/*	MACRO		PORT			DESCRIPTION			INPUT/OUTPUT	*/
/*	-----		----			-----------			------------	*/
#define REG_DEV_CTRL	0x3F6		/*	Device Control			O		*/
#define REG_ALT_STATUS	REG_DEV_CTRL	/*	Alternate Status		I		*/
					/*	This register contains the same information as the Status Register.
						The only difference is that reading this register does not imply interrupt acknowledge or clear a pending interrupt.
					*/

#define REG_DRV_ADDR	0x3F7		/*	Drive Address			I		*/


struct hd_cmd
{
	u8 	features;
	u8 	count;
	u8 	lba_low;
	u8 	lba_mid;
	u8 	lba_high;
	u8 	device;
	u8 	command;
};


/* definetions */
#define HD_TIMEOUT 			10000 		/* in millisec */
#define PARTITION_TABLE_OFFSET 	0x1be
#define ATA_IDENTIFY 		0xec
#define ATA_READ 			0x20
#define ATA_WRITE 			0x30
/* for DEVICE register. */
#define MAKE_DEVICE_REG(lba, drv, lba_highest) (((lba) << 6) | \
							((drv) << 4) | 		\
							(lba_highest & 0x0f) | 0xa0)

#endif