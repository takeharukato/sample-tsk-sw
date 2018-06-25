/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Thread manager                                                    */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/** Enable dispatch
    @param[in] tinfo 
 */
static void
enable_dispatch(thread_info_t *tinfo) {

	tinfo->preempt &= ~THR_DISPATCH_DISABLE;
}

/** ディスパッチ禁止状態に設定する
    @param[in] tinfo スレッド情報のアドレス
 */
static void
disable_dispatch(thread_info_t *tinfo) {

	tinfo->preempt |= THR_DISPATCH_DISABLE;
}

/** 遅延ディスパッチ予約を立てる
    @param[in] tinfo スレッド情報のアドレス
 */
static void
set_delay_dispatch(thread_info_t *tinfo) {

	tinfo->preempt |= THR_DISPATCH_DELAYED;
}

/** 遅延ディスパッチ予約をクリアする
    @param[in] tinfo スレッド情報のアドレス
 */
static void
clr_delay_dispatch(thread_info_t *tinfo) {

	tinfo->preempt &= ~THR_DISPATCH_DELAYED;
}

/** 遅延ディスパッチ予約をクリアする
    @param[in] tinfo スレッド情報のアドレス
 */
static void
clr_thread_info(thread_info_t *tinfo) {

	tinfo->preempt &= ~THR_DISPATCH_MASK;
}

/** スレッドのスタックアドレスを設定する
    @param[in] thr       スレッド管理情報
    @param[in] stack_top スタックの先頭アドレス
    @param[in] size      スタックのサイズ
 */
static void
set_thread_stack(thread_t *thr, void *stack_top, size_t size) {
	thread_attr_t *attr = &thr->attr;
	thread_info_t *tinfo;

	attr->stack_top = stack_top;
	attr->stack_size = size;
	tinfo = thr_refer_thread_info(thr);
	clr_thread_info(tinfo);
	tinfo->magic = THR_THREAD_INFO_MAGIC;
	attr->stack = (void *)TRUNCATE_ALIGN(((void *)(tinfo)) - 1, MALIGN_SIZE);  /* スタック位置設定  */
}

/** スレッドを開始する
 */
void
thr_thread_start(void (*fn)(void *), void   *arg){

	__psw_enable_interrupt();  /* 割り込みを許可する  */
	fn(arg);                /* スレッド開始関数を呼び出す  */
	thr_exit_thread(0);        /* スレッド開始関数から帰ったら自スレッドを終了する  */
	/* ここには来ない. */

	return ;
}

/** スレッド管理情報のリンクを取り外す
    @param[in] thr スレッド管理情報
 */
void
thr_unlink_thread(thread_t *thr){
	psw_t psw;

	psw_disable_interrupt(&psw);
	list_del(&thr->link);
	psw_restore_interrupt(&psw);
}

/** スレッド管理情報のスタック配置部分を参照する
    @param[in] thr スレッド管理情報
 */
thread_info_t *
thr_refer_thread_info(thread_t *thr) {
	thread_attr_t *attr = &thr->attr;

	return 	(thread_info_t *)(attr->stack_top + attr->stack_size - sizeof(thread_info_t));
}

/** スレッドのコンテキストスイッチを行う
    @param[in] prev スイッチされるスレッドのスレッド管理情報
    @param[in] next スイッチするスレッドのスレッド管理情報
 */
void 
thr_thread_switch(thread_t *prev, thread_t *next) {

	hal_do_context_switch(&(((thread_attr_t *)(&prev->attr))->stack),
	    &(((thread_attr_t *)(&next->attr))->stack));
	return;
}
/** スレッドを生成する
    @param[in] thrp  スレッド管理情報のポインタ変数のアドレス
    @param[in] attrp スレッド属性情報
    @param[in] start スレッド開始関数のアドレス
    @param[in] arg   スレッド開始関数の引数
    @retval 0 正常終了
    @retval ENOENT スレッドIDをすべて使い切った
    @retval ENOMEM スレッド生成に必要なメモリが不足している
 */
int
thr_create_thread(thread_t **thrp, thread_attr_t *attrp, void (*start)(void *), void *arg){
	int               rc;
	void   *thread_stack;
	thread_t        *thr;
	size_t    stack_size;

	/*
	 * スタックのセットアップ
	 */
	if ( (attrp == NULL) || (attrp->stack_top == NULL) || (attrp->stack_size == 0) ) {

		stack_size = STACK_SIZE;
		thread_stack = kmalloc(stack_size);
		if (thread_stack == NULL) {
			rc = ENOMEM;
			goto out;
		}
	} else {
		thread_stack = attrp->stack_top;
		stack_size = attrp->stack_size;
	}

	thr = kmalloc(sizeof(thread_t));
	if (thr == NULL) {
		rc = ENOMEM;
		goto free_stack_out;
	}

	memset(thr, 0, sizeof(thread_t));

	set_thread_stack(thr, thread_stack, stack_size);

	rc = thrmgr_get_threadid(&thr->tid);
	if (rc != 0)
		goto free_thread_out;  /* ID獲得に失敗した  */

	/*
	 * 属性情報のチェック
	 */
	if ( (attrp != NULL) &&
	    ( (attrp->prio < RDQ_PRIORITY_MAX) && (attrp->prio > RDQ_USER_QUE_IDX) ) )
		(&thr->attr)->prio = attrp->prio;  /*  優先度を設定する  */
	else
		(&thr->attr)->prio = 0;

	hal_setup_thread_function(thr, start, arg);  /* スレッド開始アドレスを設定する  */
	
	init_list_node(&thr->link);
	thr->exit_code = 0;

	/*
	 * メッセージ送受信機構の初期化
	 */
	wque_init_wait_queue(&thr->recv_wq);
	wque_init_wait_queue(&thr->send_wq);
	init_list_head(&thr->recv_que);
	msg_init_message_buffer(&thr->msg_buf);

	thr->status = THR_TSTATE_RUN;

	thrmgr_thread_manager_add(thrmgr_refer_thread_manager(), thr);

	sched_set_ready(thr);  /* レディーキューに追加する  */

	rc = 0;
	*thrp = thr;
	
out:
	return rc;

free_thread_out:
	kfree(thr);

free_stack_out:
	kfree(thread_stack);

	return rc;
}

/** 自スレッドの終了
    @param[in] code 終了コード
 */
void
thr_exit_thread(int code){
	psw_t psw;

	psw_disable_interrupt(&psw);
	thr_unlink_thread(current);   /* レディーキューから外す  */
	current->exit_code = (exit_code_t)code; /* 終了コードを設定  */
	current->status = THR_TSTATE_EXIT;  /* スレッドを終了状態にする  */
	thrmgr_thread_manager_remove(thrmgr_refer_thread_manager(), current);  /* スレッド一覧から取り除く  */

	reaper_add_exit_thread(current);  /*< 回収スレッドにスレッドの回収を依頼する  */
out:	
	psw_restore_interrupt(&psw);

	sched_schedule();  /* 自スレッド終了に伴うスケジュール  */

	return;
}

/** スレッドの破棄
   @param[in] thr 破棄するスレッドのスレッド管理情報 
   @retval EBUSY 終了していないスレッドを破棄しようとした
 */
int
thr_destroy_thread(thread_t *thr){
	int rc;
	psw_t psw;
	thread_attr_t *attr = &thr->attr;

	psw_disable_interrupt(&psw);
	if ( thr->status != THR_TSTATE_DEAD ) {
		rc = EBUSY;
		goto out;
	}

	thrmgr_put_threadid(thr->tid); /* IDの返却          */
	kfree(attr->stack_top);  /* スタックの開放          */
	kfree(thr);              /* スレッド管理情報の開放  */
out:	
	psw_restore_interrupt(&psw);
	return rc;
}

/** スレッドIDの獲得
    @param[in] thr スレッド管理情報
    @retval    スレッドID
 */
tid_t
thr_get_tid(thread_t *thr) {

	return thr->tid;
}

/** 自スレッドIDの獲得
    @param[in] thr スレッド管理情報
    @retval    スレッドID
 */
tid_t
thr_get_current_tid(void) {

	return current->tid;
}

/** スレッドキューの初期化
    @param[in] que 初期化対象のスレッドキュー
 */
void
thr_init_thread_queue(thread_queue_t *que) {
	psw_t psw;

	psw_disable_interrupt(&psw);
	init_list_head(&que->head);
	psw_restore_interrupt(&psw);
}


/** スレッドキューにスレッドを追加する
    @param[in] que 追加先のスレッドキュー
    @param[in] thr スレッド管理情報
 */
void 
thr_add_thread_queue(thread_queue_t *que, thread_t *thr){
	psw_t psw;

	psw_disable_interrupt(&psw);
	list_add(&que->head, &thr->link);
	psw_restore_interrupt(&psw);
}

/** スレッドキューからスレッドを取り外す
    @param[in] que 追加先のスレッドキュー
    @param[in] thr スレッド管理情報
 */
void 
thr_remove_thread_queue(thread_queue_t *que, thread_t *thr){
	psw_t psw;

	psw_disable_interrupt(&psw);
	list_del(&thr->link);
	psw_restore_interrupt(&psw);
}

/** スレッドキューが空であることを確認する
    @param[in] que 確認対象のスレッドキュー
    @retval 真 スレッドキューが空である
    @retval 偽 スレッドキューが空でない
 */
int 
thr_thread_queue_empty(thread_queue_t *que) {
	int rc;
	psw_t psw;

	psw_disable_interrupt(&psw);
	rc = list_is_empty(&que->head);
	psw_restore_interrupt(&psw);

	return rc;
}

/** スレッドキューの先頭要素を取り出す
    @param[in] que 取り出す対象のスレッドキュー
    @return    スレッド管理情報のアドレス  スレッドキューの先頭要素がある場合
    @return    NULL                        スレッドキューが空の場合
 */
thread_t *
thr_thread_queue_get_top(thread_queue_t *que) {
	thread_t *thr;
	psw_t psw;
	
	psw_disable_interrupt(&psw);

	if (list_is_empty(&que->head)) 
		thr =  NULL;
	else 
		thr = CONTAINER_OF(list_get_top(&que->head), thread_t, link);
	
	psw_restore_interrupt(&psw);

	return thr;
}

/** メッセージ受信待ち中であることを確認する
    @param[in] thr スレッド管理情報
    @retval 真 メッセージ受信待ち中である
    @retval 偽 メッセージ受信待ち中でない
 */
int
thr_can_receive_message(thread_t *thr) {
	int rc;
	psw_t psw;
	
	psw_disable_interrupt(&psw);

	rc = is_wque_empty(&thr->recv_wq);

	psw_restore_interrupt(&psw);
	return rc;
}

