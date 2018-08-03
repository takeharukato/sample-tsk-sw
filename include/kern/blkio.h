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
#include <kern/mutex.h>

#define BLK_DEV_NODEV     (-1)
#define B_BUSY            (0x1)  /*< buffer is locked                   */
#define B_VALID           (0x2)  /*< buffer has been read from disk     */
#define B_DIRTY           (0x4)  /*< buffer needs to be written to disk */
#define BSIZE             (512)  /*< Sector                             */
#define NBUF             (1024)  /*< buffers                            */

typedef struct _blk_buf {
	blk_state     flags;
	dev_id          dev;  /*< device id           */
	blk_no      blockno;  /*< block no            */
	list_t          mru;  /*< Most recently used  */
	list_t         link;  /*< disk queue          */
	wait_queue  waiters;  /*< Wait queue          */
	uint8_t data[BSIZE];
}blk_buf;

typedef struct _buffer_cache{
	mutex                  mtx;
	struct _blk_buf  buf[NBUF]; /* Block Buffers */
	list_head_t           head; /* Linked list of all buffers(most recently used)  */
} buffer_cache;


void memide_init(void);
int iderw(blk_buf *_b);

void buffer_cache_init(void);
void buffer_cache_blk_release(blk_buf *_b);
blk_buf *buffer_cache_blk_write(blk_buf *_b);
blk_buf * buffer_cache_blk_read(dev_id dev, blk_no blockno);
#endif  /*  _KERN_BLKIO_H   */
