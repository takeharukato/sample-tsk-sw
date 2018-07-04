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

#define KERN_PHY_BASE             (0x0000000000000000)
#define KERN_VMA_BASE             (0xFFFF800000000000)

#if !defined(ASM_FILE)
void boot_main(uint64_t _magic, uint64_t _mbaddr);
#endif /* ASM_FILE */
#endif  /*  _HAL_BOOT64_H   */
