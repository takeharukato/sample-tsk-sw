/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  list data structure                                               */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_LIST_H)
#define _KERN_LIST_H

#include <kern/freestanding.h>

#include <kern/kern_types.h>

/** リストエントリ
 */
typedef struct _list{
	struct _list *prev;       /*<  前の領域へのポインタ  */
	struct _list *next;       /*<  次の領域へのポインタ  */
}list_t;

/** リストヘッド
 */
typedef struct _list_head{
	struct _list *prev;       /*<  前の領域へのポインタ  */
	struct _list *next;       /*<  次の領域へのポインタ  */
}list_head_t;

/** リストから構造体へのポインタを得る
    @param[in] p リストメンバのアドレス
    @param[in] t 構造体の型
    @param[in] m tの構造体中のリストメンバのメンバ名
 */
#define CONTAINER_OF(p, t, m)			\
	((t *)(((void *)(p)) - ((void *)(&(((t *)(0))->m)))))

void list_rotate(struct _list_head *);
int list_is_empty(struct _list_head *);
list_t *list_ref_top(struct _list_head *);
list_t *list_get_top(struct _list_head *);
void list_add_top(struct _list_head *, list_t *);
void list_add(struct _list_head *, list_t *);
void list_del(list_t *);

void init_list_node(list_t *);
void init_list_head(list_head_t *);

static inline int
list_is_linked(list_t *node) {
	return !( (node->prev == node) && (node->next == node) );
}


/** リスト探査
    @param[in] itr    イテレータに使用するlist_t型のポインタ変数名
    @param[in] refp   リストヘッダを含む構造体へのポインタ変数名
    @param[in] member refp中のリストヘッドメンバ名
 */
#define list_for_each(itr, refp, member)					\
  for((itr) = list_ref_top(&((refp)->member)); (itr) != (list_t *)&((refp)->member); (itr) = (itr)->next) 

#endif  /* _KERN_LIST_H  */
