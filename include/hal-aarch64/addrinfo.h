/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Aliignments and an address space definitions                      */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_ADDR_INFO_H)
#define  _HAL_ADDR_INFO_H 

#include <kern/freestanding.h>

#define STACK_ALIGN       (16)       /*<  スタックアラインメント  */

/** 指定されたアラインメントにそっていないことを確認するためのマスク値を算出する
    @param[in] size アラインメントサイズ
    @return 指定されたアラインメントにそっていないことを確認するためのマスク値
 */
#define CALC_ALIGN_MASK(size)					\
	((size_t)((size) - 1))

/** 指定されたアラインメントで切り詰める
    @param[in] addr メモリアドレス
    @param[in] size アラインメントサイズ
    @return 指定のアラインメント境界にしたがったアドレスで切り詰めた値(先頭アドレス)
 */
#define TRUNCATE_ALIGN(addr, size)		\
	( ((size_t)(addr)) & ((size_t)(~CALC_ALIGN_MASK((size)) ) ) )

/** 指定されたアラインメントでラウンドアップする
    @param[in] addr メモリアドレス
    @param[in] size アラインメントサイズ
    @return 指定のアラインメントでアドレス値をラウンドアップした値
 */
#define ROUNDUP_ALIGN(addr, size)		\
	( TRUNCATE_ALIGN((((size_t)(addr)) + ((size_t)(size))), (size) ) )

#endif  /*  _KERN_ADDR_INFO_H   */

