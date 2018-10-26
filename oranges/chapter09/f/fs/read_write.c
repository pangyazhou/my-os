/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						/fs/read_write.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "sys/const.h"
#include "sys/protect.h"
#include "string.h"
#include "sys/fs.h"
#include "sys/proc.h"
#include "sys/tty.h"
#include "sys/console.h"
#include "sys/global.h"
#include "sys/proto.h"
#include "sys/hd.h"
#include "stdio.h"

/*=========================================================================
						do_rdwt
===========================================================================*/
/**
* Read/Write file and return byte count read/written.
*
* Sector map is not needed to update, since the sectors for the file have
* been allocated and the bits are set when the file was created.
* 
* @return How many bytes have been read/writen.
==========================================================================*/
PUBLIC int do_rdwt()
{
	int fd = fs_msg.FD;		/* file descriptor */
	void* buf = fs_msg.BUF;	/* R/W buffer */
	int len = fs_msg.CNT;	/* r/w bytes */

	int src = fs_msg.source;		/* caller proc nr. */
	assert((pcaller->flip[fd] >= &f_desc_table[0]) &&
			(pcaller->flip[fd] <= &f_desc_table[NR_FILE_DESC]));

	if(!(pcaller->flip[fd]->fd_mode & O_RDWR))
		return -1;
	int pos = pcaller->flip[fd]->fd_pos;
	struct inode* pin = pcaller->flip[fd]->fd_inode;
	assert(pin >= &inode_table[0] && pin <= &inode_table[NR_INODE]);
	int imode = pin->i_mode & I_TYPE_MASK;

	if(imode == I_CHAR_SPECIAL)
	{
		int t = fs_msg.type == READ ? DEV_READ : DEV_WRITE;
		fs_msg.type = t;

		int dev = pin->i_start_sect;
		assert(MAJOR(dev) == 4);

		fs_msg.DEVICE 	= MINOR(dev);
		fs_msg.BUF 		= buf;
		fs_msg.CNT 		= len;
		fs_msg.PROC_NR 	= src;
		assert(dd_map[MAJOR(dev)].driver_nr != INVALID_DRIVER);
		send_recv(BOTH, dd_map[MAJOR(dev)].driver_nr, &fs_msg);
		assert(fs_msg.CNT == len);

		return fs_msg.CNT;
	}
	else
	{
		assert(pin->i_mode == I_REGULAR || pin->i_mode == I_DIRECTORY);
		assert((fs_msg.type == READ) || (fs_msg.type == WRITE));

		int pos_end;
		if(fs_msg.type == READ)				/* 读写终止偏移量 */
			pos_end = min(pos + len, pin->i_size);
		else
			pos_end = min(pos + len, pin->i_nr_sects * SECTOR_SIZE);
		int off = pos % SECTOR_SIZE;		/* 扇区内偏移量 */
		int rw_sect_min = pin->i_start_sect + (pos >> SECTOR_SIZE_SHIFT); /* 读写起始扇区 */
		int rw_sect_max = pin->i_start_sect + (pos_end >> SECTOR_SIZE_SHIFT);	/* 读写终止扇区 */

		int chunk = min(rw_sect_max - rw_sect_min + 1, 
				FSBUF_SIZE >> SECTOR_SIZE_SHIFT);			/* 每次读写扇区数 */

		int bytes_rw = 0;			/* 已经读写的字节数 */
		int bytes_left = len;		/* 未读写的字节数 */
		int i;
		for(i = rw_sect_min; i <= rw_sect_max; i += chunk)
		{
			int bytes = min(bytes_left, chunk * SECTOR_SIZE - off);	/* 要读写的字节数 */
			/* 从磁盘中读取指定扇区 */
			rw_sector(DEV_READ,
				pin->i_dev,
				i * SECTOR_SIZE,
				chunk * SECTOR_SIZE,
				TASK_FS,
				fsbuf);
			if(fs_msg.type == READ)
			{
				phys_copy((void*)va2la(src, buf + bytes_rw),
						(void*)va2la(TASK_FS, fsbuf + off),
						bytes);
			}
			else
			{
				phys_copy((void*)va2la(TASK_FS, fsbuf + off),
						(void*)va2la(src, buf + bytes_rw),
						bytes);
				rw_sector(DEV_WRITE,
					pin->i_dev,
					i * SECTOR_SIZE,
					chunk * SECTOR_SIZE,
					TASK_FS, 
					fsbuf);
			}
			off = 0;
			bytes_rw += bytes;
			pcaller->flip[fd]->fd_pos += bytes;
			bytes_left -+ bytes;
		}
		/* 写文件超出原来大小,需更改i-node */
		if(pcaller->flip[fd]->fd_pos > pin->i_size)
		{
			pin->i_size = pcaller->flip[fd]->fd_pos;
			sync_inode(pin);
		}
		return bytes_rw;
	}
}
