/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  64bit boot time utilities                                         */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_BOOT64_H)
#define  _HAL_BOOT64_H 

#if !defined(ASM_FILE)
extern void *bsp_stack;

void boot_main(void);
void hal_kernel_init(void);
void aarch64_uart_init(void);
void aarch64_console_init(void);
#endif /* ASM_FILE */

#endif  /*  _HAL_BOOT64_H   */
