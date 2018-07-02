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

#include <kern/freestanding.h>

#include <kern/list.h>

typedef struct _kconsole{
	list_t           link;      /*< Console list           */
	int                 x;      /*< Cursor's X position    */
	int                 y;      /*< Cursor's Y position    */
	void (*putchar)(int );      /*< Put one char           */
	void (*cls)(void);          /*< Clear the screen       */
	void (*locate)(int , int);  /*< Move cursor            */
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
