/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  64bit boot code                                                   */
/*                                                                    */
/**********************************************************************/

#include <kern/freestanding.h>
#include <kern/printf.h>

#include <hal/hal.h>

void
hal_kernel_init(void){
}

/** 64bit モードでのブートアップ
 */
void 
boot_main(uint64_t __attribute__ ((unused)) magic, 
    uint64_t __attribute__ ((unused)) mbaddr) {

	while(1);
}
