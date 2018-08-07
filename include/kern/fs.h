/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  File system definitions                                           */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_FS_H)
#define  _KERN_FS_H

#include <kern/freestanding.h>
#include <kern/kern_types.h>
#include <kern/blkio.h>

#define FS_MAGIC_NR              (0xfeedf5)
#define NINODE                   (1024)

#define FS_IADDR_DIRECT_MIN      (0)
#define FS_IADDR_DIRECT_NR       (12)
#define FS_IADDR_IN_DIRECT_MIN   (FS_IADDR_DIRECT_NR)
#define FS_IADDR_IN_DIRECT_NR    (3)
#define FS_IADDR_DOUBLE_MIN      (FS_IADDR_IN_DIRECT_MIN + FS_IADDR_IN_DIRECT_NR)
#define FS_IADDR_DOUBLE_NR       (1)
#define FS_IADDR_NR              \
	(FS_IADDR_DIRECT_NR + FS_IADDR_IN_DIRECT_NR + FS_IADDR_DOUBLE_NR )
#define FS_MAXFILE_SIZE          (FS_IADDR_DIRECT_NR*BSIZE + \
				  FS_IADDR_IN_DIRECT_NR*(BSIZE/sizeof(blk_no)) + \
				  FS_IADDR_DOUBLE_NR*(BSIZE/sizeof(blk_no))*(BSIZE/sizeof(blk_no)))
#define FS_IMODE_NONE            (0)
#define FS_IMODE_DIR             (1)
#define FS_IMODE_FILE            (2)
#define FS_IMODE_DEV             (3)
#if !defined(O_CREATE)
#define O_CREATE                 (0x10)
#endif 
#if !defined(O_RDONLY)
#define O_RDONLY                 (0x1)
#endif
#if !defined(O_WRONLY)
#define O_WRONLY                 (0x2)
#endif
#if !defined(O_RDWR)
#define O_RDWR                   (0x3)
#endif
#if !defined(SEEK_SET)
#define        SEEK_SET          (0x0)
#endif
#if !defined(SEEK_CUR)
#define        SEEK_CUR          (0x1)
#endif
#if !defined(SEEK_END)
#define        SEEK_END          (0x2)
#endif

typedef struct _superblock {
	uint32_t s_max_size;    /*< Size of file system image in blocks */
	uint32_t s_ninodes;     /*< Number of inode blocks              */
	uint32_t s_imap_blocks; /*< inodes bitmap length in blocks      */
	uint32_t s_bmap_blocks; /*< block bitmap length in blocks       */
	uint32_t s_firstdata_block;  /*< first data block no            */
	uint32_t s_nblocks;     /*< Number of data blocks               */
	uint32_t s_magic;       /*< Magic number                        */
	uint32_t s_pad;         /*< Padding                             */
}superblock;

/* inode in disk
 */
typedef struct _d_inode{
	imode_t                 i_mode;
	dev_id                   i_dev;  /* Device number   */
	ref_cnt                i_nlink;
	obj_id_t                 i_uid;
	obj_id_t                 i_gid;
	obj_size_t              i_size;
	blk_no     i_addr[FS_IADDR_NR];
}d_inode;

#define RESV_BLOCK_NR        (1)  /*  First block is reserved */

#define SUPER_BLOCK_BLK_NO   (1)
#define SUPER_BLOCK_BLK_NR   (1)

#define BITS_PER_BLOCK       (BSIZE*8)

// Inodes per block.
#define INODES_PER_BLOCK      (BSIZE / sizeof(struct _d_inode))
#define IADDRS_PER_BLOCK      (BSIZE / sizeof(blk_no))
#define FNAME_MAX             (59)

#define ROOT_DEV              (0)
#define ROOT_DENT_INO         (2)

#define I_BUSY                 (0x1)
#define I_VALID                (0x2)

// in-memory copy of an inode
typedef struct _inode {
	uint32_t    inum;  /* Inode number    */
	ref_cnt      ref;  /* Reference count */
	int        flags;  /* I_BUSY, I_VALID */
	wait_queue waiters;  /* Waiters on this inode */

	/* copy of disk inode  */
	imode_t                 i_mode;
	dev_id                   i_dev;  /* Device number   */
	ref_cnt                i_nlink;
	obj_id_t                 i_uid;
	obj_id_t                 i_gid;
	obj_size_t              i_size;
	blk_no     i_addr[FS_IADDR_NR];
}inode;

struct _stat {
	imode_t     mode;  /* Type of file */
	dev_id       dev;  /* File system's disk device */
	uint32_t     ino;  /* Inode number */
	ref_cnt    nlink;  /* Number of links to file */
	obj_size_t  size;  /* Size of file in bytes */
};

/** Directory entry in a disk
 */
typedef struct _d_dirent{
	uint32_t              d_ino;
	char    d_name[FNAME_MAX+1];
}d_dirent;

typedef struct _dirent {
	uint32_t  d_ino;   /* inode number */
	off_t     d_off;   /* offset to this entry in i-node address space */
	char     d_name[FNAME_MAX+1]; /* filename (null-terminated) */
}dirent;

typedef struct _inode_cache{
	mutex mtx;
	inode inode[NINODE];
}inode_cache;

void inode_update(inode *_ip);
void inode_put(inode *_ip);
inode *inode_allocate(dev_id _dev, imode_t _mode);
inode *inode_duplicate(inode *_ip);
void inode_lock(inode *_ip);
void inode_unlock(inode *_ip);
off_t inode_read(inode *_ip, void *_dst, off_t _off, size_t _counts);
off_t inode_write(inode *_ip, void *_src, off_t _off, size_t _counts);
void inode_get_stat(struct _inode *_ip, struct _stat *_st);
int inode_add_directory_entry(inode *_dp, char *_name, uint32_t _inum);
inode *inode_dirlookup(inode *_dp, char *_name, dirent *_ent);
inode *inode_namei(char *_path);
inode *inode_nameiparent(char *_path, char *_name);
void inode_cache_init(void);

int fs_dup(int _fd);
int fs_read(int _fd, char *_dst, size_t _count);
int fs_write(int _fd, char *_src, size_t _count);
int fs_close(int _fd);
int fs_open(char *_path, omode_t _omode);
int fs_link(char *_old, char *_new);
int fs_unlink(char *_path);
off_t fs_lseek(int _fd, off_t _off, int _where);
int fs_fstat(int fd, struct _stat *st);
#endif  /*  _KERN_FS_H   */
