/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  カーネルコンソール処理                                            */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

static kconsole_list_t kcons = KCONSOLE_LIST_INITILIZER(kcons);

/** 一文字出力関数
    @param[in] ch 出力する文字
 */
void
kputchar(int ch){
	psw_t psw;
	kconsole_list_t *kcp = &kcons;
	kconsole_t *con;
	list_t *cp;

	psw_disable_interrupt(&psw);
	list_for_each(cp, kcp, head) {
		con = CONTAINER_OF(cp, kconsole_t, link);
		if (con->putchar != NULL) {
			con->putchar(ch);
		}
	}
	psw_restore_interrupt(&psw);
}

/** 画面クリア
 */
void
kcls(void){
	psw_t psw;
	kconsole_list_t *kcp = &kcons;
	kconsole_t *con;
	list_t *cp;

	psw_disable_interrupt(&psw);
	list_for_each(cp, kcp, head) {
		con = CONTAINER_OF(cp, kconsole_t, link);
		if (con->cls != NULL) {
			con->cls();
		}
	}
	psw_restore_interrupt(&psw);
}

/** カーソル位置更新
    @param[in] x X座標
    @param[in] y Y座標
 */
void
klocate(int x, int y){
	psw_t psw;
	kconsole_list_t *kcp = &kcons;
	kconsole_t *con;
	list_t *cp;

	psw_disable_interrupt(&psw);
	list_for_each(cp, kcp, head) {
		con = CONTAINER_OF(cp, kconsole_t, link);
		if (con->locate != NULL) {
			con->locate(x, y);
		}
	}
	psw_restore_interrupt(&psw);
}

/** カーネルコンソール登録
    @param[in] conp コンソール情報
    @retval     0  正常終了
    @retval EINVAL conpがNULLまたはputcharハンドラが登録されていない
 */
int 
register_kconsole(kconsole_t *conp) {
	int    rc;
	psw_t psw;	
	kconsole_list_t *kcp = &kcons;
	
	psw_disable_interrupt(&psw);
	if ( (conp == NULL) || (conp->putchar == NULL) ) {
		rc = EINVAL;
		goto out;
	}
	list_add(&kcp->head, &conp->link);
	rc = 0;
out:
	psw_restore_interrupt(&psw);
	return rc;
}

/** カーネルコンソール登録抹消
    @param[in] conp コンソール情報
 */
void
unregister_kconsole(kconsole_t *conp) {
	int    rc;
	psw_t psw;	
	kconsole_list_t *kcp = &kcons;
	
	psw_disable_interrupt(&psw);
	list_del(&conp->link);
	psw_restore_interrupt(&psw);

	return ;
}
