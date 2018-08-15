/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Timer definitions                                                 */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_TIMER_H)
#define  _KERN_TIMER_H 

#include <kern/freestanding.h>
#include <kern/kern_types.h>
#include <kern/list.h>

typedef struct _uptime_counter{
	uptime_cnt  uptime;
}uptime_counter;

typedef struct _call_out_que{
	list_head_t head;
}call_out_que;

typedef struct _call_out_ent{
	list_t        link;
	uptime_cnt  expire;
	void (*callout)(void *_private);
	void      *private;
}call_out_ent;

void timer_update_uptime(void);
void timer_update_thread_time(void);
void callout_ent_init(call_out_ent *_entp, void (*_callout)(void *_private), void *_private);
int callout_add(call_out_ent *_entp, uptime_cnt _rel_expire);
void callout_init(void);
#endif  /*  _KERN_TIMER_H   */
