/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  type definitions                                                  */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_KERN_TYPES_H)
#define _KERN_KERN_TYPES_H
#include <stdint.h>
#include <stddef.h>

#define BIT_PER_BYTE       (8)  /*<  バイトマシンを想定              */

typedef uint64_t         obj_id_t;  /*< ID情報                       */
typedef uint64_t      exit_code_t;  /*< スレッド終了コード           */
typedef int           thr_state_t;  /*< スレッド状態                 */
typedef char *            caddr_t;  /*< コアアドレスを示す型         */
typedef uint64_t        msg_ent_t;  /*< メッセージ1エントリのサイズ  */
typedef obj_id_t            tid_t;  /*< スレッドID    */
#endif  /*  _KERN_KERN_TYPES_H  */
