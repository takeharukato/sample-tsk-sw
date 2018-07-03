/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Block I/O devices                                                 */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_BLKIO_H)
#define  _KERN_BLKIO_H 

#include <kern/freestanding.h>
#include <kern/kern_types.h>

#define BLK_DEV_NODEV     (-1)
#define B_BUSY            (0x1)  /*< buffer is locked by some process   */
#define B_VALID           (0x2)  /*< buffer has been read from disk     */
#define B_DIRTY           (0x4)  /*< buffer needs to be written to disk */
#define BSIZE             (512)  /*< Sector                             */
#define NBUF              (10)   /*< buffers                            */

typedef struct _blk_buf {
	int           flags;
	dev_id          dev;  /*< device id           */
	blk_no      blockno;  /*< block no            */
	list_t          mru;  /*< Most recently used  */
	list_t         link;  /*< disk queue          */
	wait_queue  waiters;  /*< Wait queue          */
	uint8_t data[BSIZE];
}blk_buf;

typedef struct _buffer_cache{
	//spinlock lock;
	mutex                  mtx;
	struct _blk_buf  buf[NBUF];
	list_head_t head;   // Linked list of all buffers, through prev/next. head.next is most recently used.
} buffer_cache;

#endif  /*  _KERN_BLKIO_H   */
