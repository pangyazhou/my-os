/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            fs.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef 	_ORGANGS_FS_H_
#define 	_ORGANGS_FS_H_

/**
 * @struct dev_drv_map fs.h "include/fs.h"
 * @brief The Device_nr.
 *
 * 
 */
struct dev_drv_map 
{
	int driver_nr; 	/* The proc nr of the device driver. */
};

/**
 * @def 	MAGIC_V1
 * @brief 	Magic number of FS V1.O
 */
#define 	MAGIC_V1 	0x111

/**
 * @struct 	super_block fs.h
 * @brief 	The 2nd sector of the FS
 *
 * Remember to change SUPER_BLOCK_SIZE if the members are changed.
 */
struct super_block
{
	u32 magic;			/* Magic number */
	u32 nr_inodes; 		/* How many inodes */
	u32 nr_sects; 		/* How many sectors */
	u32 nr_imap_sects; 	/* How many inode-map sectors */
	u32 nr_smap_sects; 	/* How many sector-map sectors */
	u32 n_1st_sect;		/* Number of the 1st data sector */
	u32 nr_inode_sects; /* How many inode sectors */
	u32 root_inode; 	/* Inode nr of root directory */
	u32 inode_size; 	/* INODE_SIZE */
	u32 inode_isize_off;	/* Offset of 'struct inode::i_size' */
	u32 inode_start_off; 	/* Offset of 'struct inode::i_start_sect' */
	u32 dir_ent_size;		/* DIR_ENTRY_SIZE */
	u32 dir_ent_inode_off; 	/* Offset of 'struct dir_entry::inode_nr' */
	u32 dir_ent_fname_off; 	/* Offset of 'struct dir_entry::fname' */

	/* the following item(s) are only present in memory */
	int sb_dev; 			/* the super block's home device */
};

#define SUPER_BLOCK_SIZE 	56

struct inode
{
	u32 i_mode; 	/* Access mode */
	u32 i_size;		/* File size */
	u32 i_start_sect; 	/* The first sector of the data */
	u32 i_nr_sects;		/* How many sectors the file occupies */
	u8  _unused[16];	/* Stuff for alignment */

	/* The following items are only present in memory */
	int i_dev;		
	int i_cnt;			/* How many procs share this inode */
	int i_num; 			/* inode nr. */

};

#define INODE_SIZE 	32

#define MAX_FILENAME_LEN	12

struct dir_entry
{
	int inode_nr;
	char 	fname[MAX_FILENAME_LEN];
};

#define DIR_ENTRY_SIZE 	sizeof(struct dir_entry)

/**
 * @struct 	file_desc
 * @brief 	File Descriptor
 */
struct file_desc
{
	int 	fd_mode;		/* R or W */
	int 	fd_pos;			/* Current position for R/W */
	struct inode * fd_inode;	/* Ptr to the i-node */
};


/**
 * Since all invocations of "rw_sector()" in FS look similar
 * (most of the params are the same), we use this macro to 
 * make code more readable.
 */
#define RD_SECT(dev,sect_nr) rw_sector(DEV_READ, \
					dev,	\
					(sect_nr) * SECTOR_SIZE, 	\
					SECTOR_SIZE, 	\
					TASK_FS,	\
					fsbuf);
#define WR_SECT(dev,sect_nr) rw_sector(DEV_WRITE,	\
					dev,	\
					(sect_nr) * SECTOR_SIZE, 	\
					SECTOR_SIZE,	\
					TASK_FS,	\
					fsbuf);

#endif