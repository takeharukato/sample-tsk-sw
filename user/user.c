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
	message_t msg;


	while(1) {
		msg.buf[0] = (msg.buf[0] + 1) % 100;
		kprintf("threadA Send : %d\n", msg.buf[0]);
		msg_send(3, &msg);
		msg_recv(3, &msg);
		kprintf("threadA Recv : %d\n", msg.buf[0]);
	}
}

void
threadB(void *arg) {
	message_t msg;

	while(1) {
	  	msg_recv(MSG_RECV_ANY, &msg);
		kprintf("threadB Recv : %d\n", msg.buf[0]);
		msg.buf[0] += 10;
		kprintf("threadB Send : %d\n", msg.buf[0]);
		msg_send(2, &msg);
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
