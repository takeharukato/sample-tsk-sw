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

static device_driver memide_driver;

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
static int 
memide_rw(device_driver *drv, blk_buf *b){
	size_t addr;

	kassert(b->flags & B_BUSY);

	mutex_hold(&drv->mtx);

	addr = b->blockno * BSIZE;

	if ( b->flags & B_DIRTY ) 
		memide_write_sector(addr, &b->data[0], BSIZE);
	else
		memide_read_sector(addr, &b->data[0], BSIZE);

	mutex_release(&drv->mtx);

	return 0;
}


void
memide_init(void){

	memide_driver.blkrw = memide_rw;
	register_device_driver(ROOT_DEV, &memide_driver, NULL);
}
