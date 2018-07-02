/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  type definitions                                                  */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_KERN_TYPES_H)
#define _KERN_KERN_TYPES_H

#include <kern/freestanding.h>

#define BIT_PER_BYTE       (8)  /*<  バイトマシンを想定              */

typedef uint64_t         obj_id_t;  /*< ID情報                       */
typedef uint64_t      exit_code_t;  /*< スレッド終了コード           */
typedef int           thr_state_t;  /*< スレッド状態                 */
typedef char *            caddr_t;  /*< コアアドレスを示す型         */
typedef uint64_t        msg_ent_t;  /*< メッセージ1エントリのサイズ  */
typedef obj_id_t            tid_t;  /*< スレッドID                   */
typedef int32_t            irq_no;  /*< 割込み番号                   */
typedef int32_t          irq_prio;  /*< 割込み優先度                 */
typedef uint32_t         irq_attr;  /*< 割込み属性                   */
typedef uint8_t          precnt_t;  /*< プリエンプション/IRQ/例外カウンタ   */
typedef uint64_t       uptime_cnt;  /*< 起動後の経過時間(タイマ割込発生回数)   */
#endif  /*  _KERN_KERN_TYPES_H  */
