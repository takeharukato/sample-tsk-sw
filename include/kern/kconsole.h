/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Kernel console                                                    */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_KCONSOLE_H)
#define  _HAL_KCONSOLE_H 
#include "kern/list.h"

typedef struct _kconsole{
	list_t           link;      /*< コンソールリスト       */
	int                 x;      /*< 出力カーソルX座標位置  */
	int                 y;      /*< 出力カーソルY座標位置  */
	void (*putchar)(int );      /*< 一文字出力             */
	void (*cls)(void);          /*< 画面クリア             */
	void (*locate)(int , int);  /*< カーソル移動           */
}kconsole_t;

typedef struct _kconsole_list{
	list_head_t head;
}kconsole_list_t;

#define KCONSOLE_INITILIZER(name)  {			\
	.link = {&((name).link), &((name).link)},	\
	.x = 0,				\
	.y = 0,				\
	.putchar = NULL,		\
	.cls = NULL,			\
	.locate = NULL			\
	}

#define KCONSOLE_LIST_INITILIZER(name) {	\
	.head = {(list_t *)&(name.head), (list_t *)&(name.head)} \
	 }

void kputchar(int );
void kcls(void);
void klocate(int, int);
int register_kconsole(kconsole_t *);
void unregister_kconsole(kconsole_t *);
void init_pseudo_console(void);
#endif  /*  _HAL_KCONSOLE_H   */
