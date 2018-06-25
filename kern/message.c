/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Message passing                                                   */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/** 特定のスレッドからのメッセージがあるか確認する
    @param[in] src 受け付け対象のスレッドID
    @param[in] mbp メッセージバッファのアドレスを参照するポインタ変数のアドレス
    @retval 0 メッセージが見つかり, mbpに配置されたポインタ変数でメッセージバッファを参照できるようにした
    @retval ENOENT メッセージは見つからなかった
 */
static int
msg_recv_from(tid_t src, message_buffer_t **mbp) {
	int               rc;
	message_buffer_t *mb;
	list_t           *lp;
	psw_t            psw;

	psw_disable_interrupt(&psw);
	list_for_each(lp, current, recv_que) {
		mb = CONTAINER_OF(lp, message_buffer_t, link);
		if (mb->sender == src) { /* メッセージが見つかったので, メッセージを取り出す  */
			list_del(lp);
			*mbp = mb;
			rc = 0;
			goto out;
		}
	}
	rc = ENOENT;
out:
	psw_restore_interrupt(&psw);
	return rc;
}


/** メッセージバッファの初期化
    @param[in] mbuf メッセージバッファへのポインタ
 */
void 
msg_init_message_buffer(message_buffer_t *mbuf){
	
	memset(mbuf, 0, sizeof(message_buffer_t));
	wque_init_wait_queue(&mbuf->wq);
	init_list_node(&mbuf->link);
}

/** メッセージ送信
    @param[in] dst 送信先スレッドID
    @param[in] msg メッセージのアドレス
    @retval     0  正常終了
    @retval ESRCH  スレッドが見つからなかった
    @retval EINVAL 自分自身に送信しようとした
 */
int  
msg_send(tid_t dst, message_t *msg){
	int               rc;
	psw_t            psw;
	thread_t        *thr;
	message_buffer_t *mb;

	mb = &current->msg_buf;
	psw_disable_interrupt(&psw);

	if (dst == current->tid) {
		rc = EINVAL;
		goto out;
	}
	
	rc = thrmgr_find_thread_by_tid(dst , &thr);
	if (rc != 0)
		goto out;

	memmove(&mb->msg, msg, sizeof(message_t));
	mb->sender = thr_get_current_tid();
	mb->receiver = dst;
	list_add(&thr->recv_que, &mb->link);
	
	/* 相手が受信待ちでなければ, 相手のウエイトキューで待ち合わせる  */
	while (!thr_can_receive_message(thr)) {
		wque_wakeup(&thr->recv_wq);         /* 相手のスレッドを起床する  */
		wque_wait_on_queue(&thr->send_wq); /* 相手の送信待ちキューで休眠する  */
	}

	wque_wait_on_queue(&mb->wq); /* メッセージの受付を待つためにメッセージのキューで待ち合わせる  */

out:
	psw_restore_interrupt(&psw);

	return rc;
}
/** メッセージ受信
    @param[in] src 送信元スレッドID
    @param[in] msg メッセージのアドレス
    @retval     0  正常終了
    @retva EINVAL  自スレッドのメッセージを待ち合わせようとした
 */
int
msg_recv(tid_t src, message_t *msg) {
	int               rc;
	psw_t            psw;
	thread_t        *thr;
	message_buffer_t *mb;
	list_t           *lp;

	if (src ==thr_get_current_tid()) {
		rc = EINVAL;
		goto out;
	}
		
	psw_disable_interrupt(&psw);

	while ( (list_is_empty(&current->recv_que)) ||
	    ( ( src != MSG_RECV_ANY ) && (msg_recv_from(src, &mb) != 0 ) ) ) {
		/* 受信バッファにメッセージがなければ,
		 * 送信待ちスレッドを起こして休眠する
		 */
		wque_wakeup(&current->send_wq);         /* 相手のスレッドを起床する  */
		wque_wait_on_queue(&current->recv_wq);  /* 自分の受信待ちキューで休眠する  */
	}

	if (src == MSG_RECV_ANY)  { /* 任意のスレッドのメッセージを取り出す  */
		mb = CONTAINER_OF(list_get_top(&current->recv_que), message_buffer_t, link);
	} 

	memmove(msg, &mb->msg, sizeof(message_t));      /*  メッセージを読み取る  */	

	wque_wakeup(&mb->wq);           /* メッセージの受信を待っているスレッドを起床する  */
	
	rc = 0;
out:
	psw_restore_interrupt(&psw);

	return rc;
}
