/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  kernel console                                                    */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

#include <hal/board.h>

volatile uint32_t * const UART0DR = (uint32_t *)UART_BASE;
 
/** 一文字出力関数
    @param[in] ch 出力する文字
 */
void
aarch64_kputchar(int ch){
	psw_t psw;

	psw_disable_and_save_interrupt(&psw);
	*UART0DR = (unsigned int)(ch); /* Transmit char */
	psw_restore_interrupt(&psw);
}
