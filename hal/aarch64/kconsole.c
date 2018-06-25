/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  kernel console                                                    */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

static kconsole_list_t kcons = KCONSOLE_LIST_INITILIZER(kcons);
volatile unsigned int * const UART0DR = (unsigned int *) 0x09000000;
 
/** 一文字出力関数
    @param[in] ch 出力する文字
 */
void
kputchar(int ch){
	psw_t psw;

	psw_disable_interrupt(&psw);
	*UART0DR = (unsigned int)(ch); /* Transmit char */
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
