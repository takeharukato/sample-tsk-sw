/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  kernel mutex                                                      */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

void 
mutex_init(mutex *mtx){
	psw_t psw;	

	psw_disable_and_save_interrupt(&psw);
	mtx->count = 0;
	mtx->owner = NULL;
	wque_init_wait_queue( &mtx->wq );
	psw_restore_interrupt(&psw);
}

void 
mutex_destroy(mutex *mtx){
	psw_t psw;	

	psw_disable_and_save_interrupt(&psw);
	wque_wakeup(&mtx->wq, WQUE_REASON_DESTROY);
	mtx->count = 0;
	mtx->owner = NULL;
	psw_restore_interrupt(&psw);
}
int 
mutex_try_hold(mutex *mtx){
	int    rc;
	psw_t psw;	

	psw_disable_and_save_interrupt(&psw);
	if ( mtx->count > 0 ) {
		rc = EAGAIN;
		goto out;
	}
	++mtx->count;
	mtx->owner = current;
	rc = 0;
out:
	psw_restore_interrupt(&psw);
	return rc;
}
int
mutex_hold(mutex *mtx){
	int           rc;
	psw_t        psw;	
	wq_reason reason;

	psw_disable_and_save_interrupt(&psw);
	while ( mtx->count > 0 ) {

		reason = wque_wait_on_queue( &mtx->wq );
		if ( reason != WQUE_REASON_WAKEUP ) {

			rc = ENODEV;
			goto out;
		}
	}
	++mtx->count;
	mtx->owner = current;
	rc = 0;
out:
	psw_restore_interrupt(&psw);
	return rc;
}
void
mutex_release(mutex *mtx){
	int           rc;
	psw_t        psw;	
	wq_reason reason;

	psw_disable_and_save_interrupt(&psw);
	--mtx->count;
	wque_wakeup(&mtx->wq, WQUE_REASON_WAKEUP );
	mtx->owner = NULL;
out:
	psw_restore_interrupt(&psw);
	return;
}
