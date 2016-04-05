/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  Inter thread message                                              */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_MESSAGE_H)
#define  _KERN_MESSAGE_H 
#include "kern/kern_types.h"
#include "kern/wqueue.h"

#define MSG_ENT_NR      (7)  /*< メッセージバッファのエントリ数  */

#define MSG_RECV_ANY    (0)  /*< 任意のスレッドからのメッセージを受け付ける  */

typedef struct _message{
	msg_ent_t    buf[MSG_ENT_NR]; /*< メッセージ本体                 */
}message_t;

typedef struct _message_buffer{
	tid_t         sender;        /*< 送信スレッドのスレッドID       */
	tid_t       receiver;        /*< 受信スレッドのスレッドID       */
	message_t        msg;        /*< メッセージ本体                 */
	wait_queue_t      wq;        /*< スレッド待ちキュー             */
	list_t          link;        /*< 受信メッセージキューへのリンク */
}message_buffer_t;

void msg_init_message_buffer(message_buffer_t *);
int  msg_send(tid_t dst, message_t *);
int  msg_recv(tid_t src, message_t *);
#endif  /*  _KERN_MESSAGE_H   */
