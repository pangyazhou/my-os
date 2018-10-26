/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						fs/misc.c
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
#include "sys/keyboard.h"
#include "sys/proto.h"
#include "stdio.h"


/*=========================================================================
						search_file
===========================================================================*/
/**
* <Ring 1> Search the file and return the inode_nr
*
* @param path 	The full path of the file to search.
* @return 		Ptr to the i-node of the file if successful,
* 				otherwise zero.
==========================================================================*/
PUBLIC int search_file(char* path)
{
	int i, j;
	char filename[MAX_PATH];
	memset(filename, 0, MAX_FILENAME_LEN);
	struct inode* dir_inode;
	if(strip_path(filename, path, &dir_inode) != 0)
		return 0;
	if(filename[0] == 0)		/* path: '/' */
		return dir_inode->i_num;

	/* Search the dir for the file */
	int dir_blk0_nr = dir_inode->i_start_sect;
	int nr_dir_blks = (dir_inode->i_size + SECTOR_SIZE - 1) / SECTOR_SIZE;
	int nr_dir_entries = 
		dir_inode->i_size / DIR_ENTRY_SIZE;


	int m = 0;
	struct dir_entry *pde;
	for(i = 0; i < nr_dir_blks; i++)
	{
		int dev = root_inode->i_dev;
		root_inode->i_dev = dev;
		RD_SECT(dir_inode->i_dev, dir_blk0_nr + i);
		pde = (struct dir_entry*)fsbuf;
		for(j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++, pde++)
		{
			if(memcmp(filename, pde->fname, MAX_FILENAME_LEN) == 0)
			{
				return pde->inode_nr;
			}
			if(++m > nr_dir_entries)
				break;
		}
		/* all entries have been iterated */
		if(m > nr_dir_blks)
			break;
	}
	/* file not found */
	return 0;
}


/*=========================================================================
						strip_path
===========================================================================*/
/**
* <Ring 1> Get the basename from the fullpath.
* 
* In Orange's FS v1.0, all files are stored in the root directory,
* There is no sub-folder thing.
==========================================================================*/
PUBLIC int strip_path(char * filename, const char* pathname, 
					struct inode** ppinode)
{

	const char* s = pathname;
	char* t = filename;

	if(s == 0)
		return -1;
	if(*s == '/')
		s++;
	while(*s)
	{
		if(*s == '/')
			return -1;
		*t++ = *s++;
		/* if filename is too long, just truncate it */
		if(t - filename >= MAX_FILENAME_LEN)
			break;
	}
	*t = 0;
	*ppinode = root_inode;
	return 0;
}