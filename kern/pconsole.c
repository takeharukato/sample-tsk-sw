/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Pseudo console                                                    */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

#if !defined(CONFIG_HAL)
#include <stdio.h>

static kconsole_t pseudo_console = KCONSOLE_INITILIZER(pseudo_console);

static void
pseudo_putchar (int c) {
	
	putchar(c);
}
/** 擬似コンソールの初期化
 */
void
init_pseudo_console(void) {

	pseudo_console.putchar = pseudo_putchar;
	pseudo_console.cls = NULL;
	pseudo_console.locate = NULL;
	register_kconsole(&pseudo_console);
}
#endif  /*  !CONFIG_HAL  */
