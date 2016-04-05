/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  thread management routines                                        */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

static thread_manager_t all_threads;
static id_bitmap_t thread_id_map = ID_BITMAP_INITIALIZER;  /*< スレッドIDのプール  */


/** スレッドマネージャを初期化する
 */
static void
thread_manager_init(thread_manager_t *tm) {

	init_list_head(&tm->head);
}

/** スレッドマネージャのアドレスを取得する
 */
thread_manager_t *
thrmgr_refer_thread_manager(void) {
	return &all_threads; 
}

/** スレッドマネージャにスレッドを追加する
    @param[in] tm スレッドマネージャのアドレス
    @param[in] thr スレッド管理情報
 */
void
thrmgr_thread_manager_add(thread_manager_t *tm, thread_t *thr) {
	psw_t psw;

	psw_disable_interrupt(&psw);
	list_add(&tm->head, &thr->mgr_link);
	psw_restore_interrupt(&psw);
}
/** スレッドマネージャからスレッドを取り除く
    @param[in] tm スレッドマネージャのアドレス
    @param[in] thr スレッド管理情報
 */
void
thrmgr_thread_manager_remove(thread_manager_t *tm, thread_t *thr) {
	psw_t psw;

	psw_disable_interrupt(&psw);
	list_del(&thr->mgr_link);
	psw_restore_interrupt(&psw);
}

/** スレッドIDからスレッド管理情報を得る
    @param[in] tid  検索対象スレッドのスレッドID
    @param[in] thrp スレッド管理情報を参照するポインタ変数のアドレス
    @retval 0       正常終了
    @retval ESRCH   スレッドが見つからなかった
 */
int
thrmgr_find_thread_by_tid(tid_t tid, thread_t **thrp) {
	int     rc;
	list_t *ptr;
	psw_t psw;
	thread_manager_t *tm;
	thread_t *thr;

	tm = thrmgr_refer_thread_manager();
	
	psw_disable_interrupt(&psw);

	rc = ESRCH;
	if (list_is_empty(&tm->head)) 
		goto out;

	list_for_each(ptr, tm, head) {
		thr = CONTAINER_OF(ptr, thread_t, mgr_link);
		if (thr->tid == tid) {
			rc = 0;
			*thrp = thr;
			goto out;
		}
	}

out:
	psw_restore_interrupt(&psw);
	return rc;
}


/** スレッドIDの予約
    @param[in] id 予約するID
    @retval EBUSY 既に使用されている
 */
int
thrmgr_reserve_threadid(tid_t id) {
	return reserve_id(&thread_id_map, id);
}

/** スレッドIDの獲得
    @param[in] tidp スレッドID返却領域
    @retval ENOENT IDを使い切った
 */
int
thrmgr_get_threadid(tid_t *tidp) {
	int rc;

	rc = get_new_id(&thread_id_map, tidp);

	return rc;
}

/** スレッドIDの開放
    @param[in] tid  開放するスレッドID
 */
void
thrmgr_put_threadid(tid_t tid) {

	put_id(&thread_id_map, tid);

	return;
}

/** スレッド管理系の初期化
 */
void
thrmgr_init_thread_manager(void) {

	thread_manager_init(&all_threads);
}
