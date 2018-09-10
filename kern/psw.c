/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Processor Status Word                                             */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

/** Save processor status word and disable interrupt by CPU
    @param[in] pswp Memory area to store the processor status word
 */
void
psw_disable_and_save_interrupt(psw_t *pswp) {
#if defined(CONFIG_HAL)
	psw_t psw;

	__save_psw(psw);
	psw_disable_interrupt();
	*pswp = psw;
#endif  /*  CONFIG_HAL  */
}

/** Restore processor status word
    @param[in] pswp Memory area of the status of the processor status word
 */
void
psw_restore_interrupt(psw_t *pswp) {
#if defined(CONFIG_HAL)
	psw_t psw;

	psw = *pswp;
	__restore_psw(psw);
#endif  /*  CONFIG_HAL  */
}
