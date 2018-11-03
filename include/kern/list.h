/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  List Data Structure                                               */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_LIST_H)
#define _KERN_LIST_H

#include <kern/freestanding.h>
#include <kern/kern_types.h>

/** List entry
 */
typedef struct _list{
	struct _list *prev;       /*<  Previous pointer  */
	struct _list *next;       /*<  Next pointer      */
}list_t;

/** List head
 */
typedef struct _list_head{
	struct _list *prev;       /*<  Previous pointer  */
	struct _list *next;       /*<  Next pointer      */
}list_head_t;

#define __LIST_ENT_INITIALIZER(le)  \
	{(struct _list *)&le, (struct _list *)&le}

#define __LIST_HEAD_INITIALIZER(lh)  \
	{(struct _list *)&lh, (struct _list *)&lh}

void list_rotate(struct _list_head *);
int list_is_empty(struct _list_head *);
list_t *list_ref_top(struct _list_head *);
list_t *list_get_top(struct _list_head *);
list_t *list_ref_tail(struct _list_head *);
list_t *list_get_tail(struct _list_head *);
void list_add_top(struct _list_head *, list_t *);
void list_add(struct _list_head *, list_t *);
void list_del(list_t *);

void init_list_node(list_t *);
void init_list_head(list_head_t *);

static inline int
list_is_linked(list_t *node) {
	return !( (node->prev == node) && (node->next == node) );
}


/** Obtain an address of the structure from list member
    @param[in] p An address of the list member
    @param[in] t A type of the structure
    @param[in] m The member name of the list entry in the structure 't'
 */
#define CONTAINER_OF(p, t, m)			\
	((t *)(((void *)(p)) - ((void *)(&(((t *)(0))->m)))))

/** Search in a list
    @param[in] itr    Iterator (the name of list_t type pointer)
    @param[in] refp   Pointer to a struct which contains list head
    @param[in] member List head member name in refp
 */
#define list_for_each(itr, refp, member)					\
  for((itr) = list_ref_top(&((refp)->member)); (itr) != (list_t *)&((refp)->member); (itr) = (itr)->next) 

/** Reverse search in a list
    @param[in] itr    Iterator (the name of list_t type pointer)
    @param[in] refp   Pointer to a struct which contains list head
    @param[in] member List head member name in refp
 */
#define reverse_for_each(itr, refp, member)					\
  for((itr) = list_ref_tail(&((refp)->member)); (itr) != (list_t *)&((refp)->member); (itr) = (itr)->prev) 

#endif  /* _KERN_LIST_H  */
