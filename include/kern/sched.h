/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  scheduler definition                                              */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_SCHED_H)
#define  _KERN_SCHED_H

void sched_init(void);
void sched_schedule(void);
void sched_set_ready(thread_t *);
void sched_rotate_queue(void);
#endif  /*  _KERN_SCHED_H   */
