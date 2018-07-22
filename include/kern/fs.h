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

#define FS_IADDR_DIRECT_MIN      (0)
#define FS_IADDR_DIRECT_NR       (12)
#define FS_IADDR_IN_DIRECT_MIN   (FS_IADDR_DIRECT_NR)
#define FS_IADDR_IN_DIRECT_NR    (3)
#define FS_IADDR_DOUBLE_MIN      (FS_IADDR_IN_DIRECT_MIN + FS_IADDR_IN_DIRECT_NR)
#define FS_IADDR_DOUBLE_NR       (1)
#define FS_IADDR_NR              \
	(FS_IADDR_DIRECT_NR + FS_IADDR_IN_DIRECT_NR + FS_IADDR_DOUBLE_NR )

#define FS_IMODE_NONE            (0)
#define FS_IMODE_DIR             (1)
#define FS_IMODE_FILE            (2)
#define FS_IMODE_DEV             (3)

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
typedef struct _dinode{
	imode_t                 i_mode;
	ref_cnt                i_nlink;
	obj_id_t                 i_uid;
	obj_id_t                 i_gid;
	obj_size_t              i_size;
	obj_size_t i_addr[FS_IADDR_NR];
}dinode;


#define RESV_BLOCK_NR        (1)  /*  First block is reserved */

#define SUPER_BLOCK_BLK_NO   (1)
#define SUPER_BLOCK_BLK_NR   (1)

#define BITS_PER_BLOCK       (BSIZE*8)

// Inodes per block.
#define INODES_PER_BLOCK     (BSIZE / sizeof(struct _dinode))
// Block containing inode i
#define INUM2BLOCK(i)        (((i) / INODES_PER_BLOCK) + RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR)

// Block containing bit for block b
#define BITMAP_BLOCK(blkno, ninodes) \
	(b/BITS_PER_BLOCK + (ninodes)/INODES_PER_BLOCK + RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR)
#endif  /*  _KERN_FS_H   */
