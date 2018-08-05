/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Block I/O                                                         */
/*                                                                    */
/*  The following codes are derived from xv6.                         */
/*  https://pdos.csail.mit.edu/6.828/2016/xv6.html                    */
/*  I might replace these codes in the future.                        */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

static buffer_cache global_buffer_cache;

/* equivalent to bget */
static blk_buf *
buffer_cache_blk_get(dev_id dev, blk_no blockno){
	wq_reason rc;
	blk_buf   *b;
	list_t   *lp;
	psw_t    psw;

	psw_disable_and_save_interrupt(&psw);
	do{
		mutex_hold(&global_buffer_cache.mtx);
		reverse_for_each(lp, &global_buffer_cache, head){  

			/*
			 * Search the block in chached buffers.
			 */
			b = CONTAINER_OF(lp, blk_buf, mru);
			if ( ( b->dev == dev ) && ( b->blockno == blockno) ) {

				if ( !(b->flags & B_BUSY) ) {

					b->flags |= B_BUSY;
					mutex_release(&global_buffer_cache.mtx);
					goto found;
				} else {

					rc = wque_wait_on_event_with_mutex(&b->waiters, 
					    &global_buffer_cache.mtx);
					mutex_release(&global_buffer_cache.mtx);
					continue;
				}
			}
		}

		/*
		 * Re-use buffer
		 */
		reverse_for_each(lp, &global_buffer_cache, head) {
	
			b = CONTAINER_OF(lp, blk_buf, mru);
			if ( ( !(b->flags & B_BUSY) ) && ( !(b->flags & B_DIRTY) ) ) {
			
				b->dev = dev;
				b->blockno = blockno;
				b->flags = B_BUSY;
				mutex_release(&global_buffer_cache.mtx);
				goto found;
			}
		}

		mutex_release(&global_buffer_cache.mtx);
	} while(rc != WQUE_REASON_DESTROY );
	panic("buffer_cache_blk_get: no buffers");
found:
	psw_restore_interrupt(&psw);
	return b;
}

/* equivalent to bread */
blk_buf *
buffer_cache_blk_read(dev_id dev, blk_no blockno){
	blk_buf *b;

	b = buffer_cache_blk_get(dev, blockno);
	if ( !(b->flags & B_VALID) ) {

		iderw(b); /* Read buffer */
	}

	return b;
}

/* equivalent to bwrite */
blk_buf *
buffer_cache_blk_write(blk_buf *b){


	kassert(b->flags & B_BUSY);

	b->flags |= B_DIRTY;
	iderw(b);  /* Write buffer */

	return b;
}

/*  equivalent to brelse */
void
buffer_cache_blk_release(blk_buf *b){
	psw_t psw;

	if ( (b->flags & B_BUSY) == 0 )
		panic("buffer_cache_blk_release");

	mutex_hold(&global_buffer_cache.mtx);
	psw_disable_and_save_interrupt(&psw);

	list_del(&b->mru);

	list_add_top(&global_buffer_cache.head, &b->mru);

	b->flags &= ~B_BUSY;

	/*
	 * Wake up waiters
	 */
	wque_wakeup(&b->waiters, WQUE_REASON_WAKEUP);

	mutex_release(&global_buffer_cache.mtx);
	psw_restore_interrupt(&psw);
}

/* equivalent to binit */
void
buffer_cache_init(void){
	blk_buf *b;

	mutex_init(&global_buffer_cache.mtx);

	/*
	 * Create linked list of buffers
	 */
	init_list_head(&global_buffer_cache.head);

	for(b = &global_buffer_cache.buf[0]; &global_buffer_cache.buf[NBUF] > b; ++b){
		
		init_list_node(&b->mru);
		init_list_node(&b->link);
		list_add_top(&global_buffer_cache.head, &b->mru);
		b->dev = BLK_DEV_NODEV;  /* no device own this block */
		wque_init_wait_queue(&b->waiters);
	}
}
