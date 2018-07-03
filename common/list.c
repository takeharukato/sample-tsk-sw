/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  List operations                                                   */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

/** リスト削除
 */
void
list_del(list_t *node) {

	node->next->prev = node->prev;
	node->prev->next = node->next;
	node->next = node->prev = node;
}

/** リストを末尾に追加
 */
void
list_add(struct _list_head *head, list_t *node) {

	node->next = (list_t *)head;
	node->prev = head->prev;
	head->prev->next = node;
	head->prev = node;
}

/** リストを先頭に追加
 */
void
list_add_top(struct _list_head *head, list_t *node) {

	node->next = head->next;
	node->prev = head->prev;
	head->next->prev = node;
	head->next = node;
}

/** 先頭ノード参照
 */
list_t *
list_ref_top(struct _list_head *head) {
	
	return head->next;
}

/** 先頭ノード獲得
 */
list_t *
list_get_top(struct _list_head *head) {
	list_t *top;

	top = list_ref_top(head);
	list_del(top);

	return top;
}

/** 最終ノード参照
 */
list_t *
list_ref_tail(struct _list_head *head) {
	
	return head->prev;
}

/** 最終ノード獲得
 */
list_t *
list_get_tail(struct _list_head *head) {
	list_t *tail;

	tail = list_ref_tail(head);
	list_del(tail);

	return tail;
}

int
list_is_empty(struct _list_head *head) {
	
	return  (head->prev == (list_t *)head) && (head->next == (list_t *)head);
}

void 
list_rotate(struct _list_head *head) {

	list_add(head, list_get_top(head));
}

void 
init_list_node(list_t *node){

	node->prev = node->next = node;	
}

void 
init_list_head(list_head_t *head) {

	head->prev = head->next = (list_t *)head;	
}
