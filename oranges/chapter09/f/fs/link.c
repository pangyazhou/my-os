/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						/fs/link.c
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
						do_unlink
===========================================================================*/
/**
* Remove a file.
*
* @note  We clear the i-node in inode_array[] although it is not really needed.
* 		 We don't clear the data bytes so the file is recoverable.
*
* @return On sucess, zero is returned. On error, -1 is returned.
==========================================================================*/
PUBLIC int do_unlink()
{
	char pathname[MAX_PATH];

	/* get parameters from the message */
	int name_len = fs_msg.NAME_LEN;
	int src = fs_msg.source;
	assert(name_len < MAX_PATH);
	phys_copy((void*)va2la(TASK_FS, pathname),
			(void*)va2la(src, fs_msg.PATHNAME),
			name_len);
	pathname[name_len] = 0;

	if(strcmp(pathname, "/") == 0)
	{
		printl("FS:do_unlink():: cannot unlink the root\n");
		return -1;
	}
	int inode_nr = search_file(pathname);
	if(inode_nr == INVALID_INODE)		/* file not found. */
	{
		printl("FS::do_unlink():: search_file() returns "
			"invalid inode: %s\n", pathname);
		return -1;
	}

	char filename[MAX_PATH];
	struct inode* dir_inode;
	if(strip_path(filename, pathname, &dir_inode) != 0)
		return -1;
	struct inode * pin = get_inode(dir_inode->i_dev, inode_nr);
	if(pin->i_mode != I_REGULAR)
	{
		printl("cannot remove file %s, because "
			"it is not a regular file.\n", pathname);
		return -1;
	}
	if(pin->i_cnt > 1)		/* the file was opened. */
	{
		printl("cannot remove file %s, because pin->i_cnt is %d\n", 
			pathname, pin->i_cnt);
		return -1;
	}

	struct super_block *sb = get_super_block(pin->i_dev);
	/********************************
	 * free the bit in i-map
	 ********************************/
	int byte_idx = inode_nr / 8;
	int bit_idx = inode_nr % 8;
	assert(byte_idx < SECTOR_SIZE);		/* we have only one i-map sector */
	/* read sector 2 */
	RD_SECT(pin->i_dev, 2);
	assert(fsbuf[byte_idx % SECTOR_SIZE] & (1 << bit_idx));
	fsbuf[byte_idx % SECTOR_SIZE] &= ~(1 << bit_idx);
	WR_SECT(pin->i_dev, 2);

	/********************************
	 * free the bit in s-map
	 ********************************/
	bit_idx = pin->i_start_sect - sb->n_1st_sect + 1;
	byte_idx = bit_idx / 8;
	int bits_left = pin->i_nr_sects;
	int byte_cnt = (bits_left - (8 - (bit_idx % 8))) / 8;

	/* current sector nr. */
	int s = 2 
		+ sb->nr_imap_sects + byte_idx / SECTOR_SIZE;

	RD_SECT(pin->i_dev, s);
	int i;
	/* clear the first byte */
	for(i = bit_idx % 8; (i < 8) && bits_left; i++, bits_left--)
	{
		assert((fsbuf[byte_idx % SECTOR_SIZE] >> i & 1) == 1);
		fsbuf[byte_idx % SECTOR_SIZE] &= ~(1 << i);
	}
	/* clear bytes from the second byte to the second to last */
	int k;
	i = (byte_idx % SECTOR_SIZE) + 1;	/* the second byte */
	for(k = 0; k < byte_cnt; k++,i++,bits_left-= 8)
	{
		if(i == SECTOR_SIZE)
		{
			i = 0;
			WR_SECT(pin->i_dev, s);
			RD_SECT(pin->i_dev, ++s);
		}
		assert(fsbuf[i] == 0xff);
		fsbuf[i] = 0;
	}
	/* clear the last byte */
	if(i == SECTOR_SIZE)
	{
		i = 0;
		WR_SECT(pin->i_dev, s);
		RD_SECT(pin->i_dev, ++s);
	}
	fsbuf[i] &= (~0) << bits_left;
	WR_SECT(pin->i_dev, s);


	/********************************
	 * clear the i-node itself
	 ********************************/
	pin->i_mode = 0;
	pin->i_size = 0;
	pin->i_start_sect = 0;
	pin->i_nr_sects = 0;
	sync_inode(pin);
	/* release slot in inode_table[] */
	put_inode(pin);

	/********************************
	 * set the inode-nr to 0 in the 
	 * directory entry
	 ********************************/
	int dir_blk0_nr = dir_inode->i_start_sect;
	int nr_dir_blks = (dir_inode->i_size + SECTOR_SIZE) / SECTOR_SIZE;
	int nr_dir_entries = 
		dir_inode->i_size / DIR_ENTRY_SIZE;

	int m = 0;
	struct dir_entry* pde = 0;
	int flg = 0;
	int dir_size = 0;

	for(i = 0; i < nr_dir_blks; i++)
	{
		RD_SECT(dir_inode->i_dev, dir_blk0_nr + i);

		pde = (struct dir_entry*)fsbuf;
		int j;
		for(j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++, pde++)
		{
			if(++m > nr_dir_entries)
				break;
			if(pde->inode_nr == inode_nr)
			{
				memset(pde, 0, DIR_ENTRY_SIZE);
				WR_SECT(dir_inode->i_dev, dir_blk0_nr + i);
				flg = 1;
				break;
			}
			if(pde->inode_nr != INVALID_INODE)
				dir_size += DIR_ENTRY_SIZE;
		}
		if(m > nr_dir_entries || flg)
			break;
	}
	assert(flg);
	if(m == nr_dir_entries)	/* the file is the last one */
	{
		dir_inode->i_size = dir_size;
		sync_inode(dir_inode);
	}
	return 0;
}