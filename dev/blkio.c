/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Block I/O                                                         */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

static buffer_cache global_buffer_cache;

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return B_BUSY buffer.
/* equivalent to bget */
static blk_buf *
buffer_cache_blk_get(int32_t dev, uint64_t blockno){
	blk_buf *b;
	list_t  *lp;
	wq_reason rc;
	psw_t psw;

loop:
	//acquire(&global_buffer_cache.lock);
	psw_disable_and_save_interrupt(&psw);
	list_for_each(lp, &global_buffer_cache, head){  // Is the block already cached?
		
		b = CONTAINER_OF(lp, blk_buf, mru);
		if ( ( b->dev == dev ) && ( b->blockno == blockno) ) {

			if ( !(b->flags & B_BUSY) ) {

				b->flags |= B_BUSY;
				//release(&global_buffer_cache.lock);
				goto found;
			} else {

				//sleep(b, &global_buffer_cache.lock);
				rc = wque_wait_on_queue(&b->waiters);
				/* This buffer_cached might be updated */
				goto loop;
			}
		}
	}

	// Not cached; recycle some non-busy and clean buffer.
	// "clean" because B_DIRTY and !B_BUSY means log.c
	// hasn't yet committed the changes to the buffer.

	//  for(b = bcache.head.prev; b != &bcache.head; b = b->prev){
	reverse_for_each(lp, &global_buffer_cache, head) {
	
		b = CONTAINER_OF(lp, blk_buf, mru);
		if ( ( (b->flags & B_BUSY) == 0 ) && ( (b->flags & B_DIRTY) == 0) ) {
			
			b->dev = dev;
			b->blockno = blockno;
			b->flags = B_BUSY;
			//release(&bcache.lock);
			goto found;
		}
	}

	panic("buffer_cache_blk_get: no buffers");
found:
	psw_restore_interrupt(&psw);
	return b;
}

// Return a B_BUSY buf with the contents of the indicated block.
/* equivalent to bread */
blk_buf *
buffer_cache_blk_read(int32_t dev, uint64_t blockno){
	blk_buf *b;

	b = buffer_cache_blk_get(dev, blockno);
	if ( !(b->flags & B_VALID) ) {

		//iderw(b); DIRTYなら書き込み, それ以外なら読み込み
	}

	return b;
}

// Write b's contents to disk.  Must be B_BUSY.
/* equivalent to bwrite */
blk_buf *
buffer_cache_blk_write(blk_buf *b){


	if ( (b->flags & B_BUSY) == 0 )
		panic("buffer_cache_blk_write");

	b->flags |= B_DIRTY;
	//iderw(b); // DIRTYなら書き込み, それ以外なら読み込み

	return b;
}

// Release a B_BUSY buffer.
// Move to the head of the MRU list.
/*  equivalent to brelse */
void
buffer_cache_blk_release(blk_buf *b){
	psw_t psw;

	if ( (b->flags & B_BUSY) == 0 )
		panic("buffer_cache_blk_release");

	//acquire(&bcache.lock);
	psw_disable_and_save_interrupt(&psw);

	list_del(&b->mru);

	list_add_top(&global_buffer_cache.head, &b->mru);

	b->flags &= ~B_BUSY;

	/*
	 * Wake up waiters
	 */
	wque_wakeup(&b->waiters, WQUE_REASON_WAKEUP);

	//release(&bcache.lock);
	psw_restore_interrupt(&psw);
}

/* equivalent to binit */
void
buffer_cache_init(void){
	blk_buf *b;

	//initlock(&bcache.lock, "bcache");

	/*
	 * Create linked list of buffers
	 */
	init_list_head(&global_buffer_cache.head);
	mutex_init(&global_buffer_cache.mtx);

	for(b = &global_buffer_cache.buf[0]; b < &global_buffer_cache.buf[NBUF]; ++b){

		list_add_top(&global_buffer_cache.head, &b->mru);
		b->dev = BLK_DEV_NODEV;  /* no device own this block */
		wque_init_wait_queue(&b->waiters);
	}
}
