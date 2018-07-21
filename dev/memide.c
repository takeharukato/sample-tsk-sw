/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Pseudo IDE device                                                 */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

#define MEMIDE_BLK_SIZE (512)

extern char _fsimg_start;
extern char _fsimg_end;


typedef struct _ide_block_chain{
	mutex       mtx;
	wait_queue  wq;
}ide_block_chain;

static ide_block_chain memide_reqs;

static int
memide_read_sector(size_t offset, void *dest, size_t size) {
	void     *addr;
	size_t remains;
	size_t       n;
	psw_t      psw;

	psw_disable_and_save_interrupt(&psw);

	addr = (void *)(&_fsimg_start) + offset;
	
	for(n = 0, remains = size; remains > 0; addr += n, dest += n, remains -= n) {

		if ( MEMIDE_BLK_SIZE < remains )
			n = MEMIDE_BLK_SIZE;
		else
			n = remains;
		
		memmove(dest, addr, n);
	}

	psw_restore_interrupt(&psw);

	return 0;
}

static int
memide_write_sector(size_t offset, void *src, size_t size) {
	void     *addr;
	size_t remains;
	size_t       n;
	psw_t      psw;

	psw_disable_and_save_interrupt(&psw);

	addr = (void *)(&_fsimg_start) + offset;
	
	for(n = 0, remains = size; remains > 0; addr += n, src += n, remains -= n) {

		if ( MEMIDE_BLK_SIZE < remains )
			n = MEMIDE_BLK_SIZE;
		else
			n = remains;
		
		memmove(addr, src, n);
	}

	psw_restore_interrupt(&psw);

	return 0;
}
/** IDE read/write
    DIRTYなら書き込み, それ以外なら読み込み
 */
int 
iderw(blk_buf *b){
	blk_no no;
	size_t addr;

	kassert(b->flags & B_BUSY);

	mutex_hold(&memide_reqs.mtx);

	addr = b->blockno * BSIZE;

	if ( b->flags & B_DIRTY ) 
		memide_write_sector(addr, &b->data[0], BSIZE);
	else
		memide_read_sector(addr, &b->data[0], BSIZE);

	mutex_release(&memide_reqs.mtx);
	
	return 0;
}


void
memide_init(void){

	mutex_init(&memide_reqs.mtx);
	wque_init_wait_queue(&memide_reqs.wq);
}
