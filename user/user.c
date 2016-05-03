/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  user routines                                                     */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"
wait_queue_t wque;

void
threadA(void *arg) {
	int i = 0;

	while(1) {

		kprintf("threadA\n");

		sched_rotate_queue();
		sched_set_ready(current) ;
		sched_schedule();
	}
}

void
threadB(void *arg) {

	while(1) {

		kprintf("threadB\n");

		sched_rotate_queue();
		sched_set_ready(current) ;
		sched_schedule();
	}
}
void
threadC(void *arg) {
	kprintf("threadC\n");
}

void
user_init(void) {
	thread_t *thrA, *thrB, *thrC;

	wque_init_wait_queue(&wque);
	thr_create_thread(&thrA, NULL, threadA, NULL);
	thr_create_thread(&thrB, NULL, threadB, NULL);
	thr_create_thread(&thrC, NULL, threadC, (void *)0x5a5a5a5a5a);
}
