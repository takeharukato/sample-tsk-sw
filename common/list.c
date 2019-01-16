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
list_del(struct _list *node) {

	node->next->prev = node->prev;
	node->prev->next = node->next;
	node->next = node->prev = node;
}

/** リストを末尾に追加
 */
void
list_add(struct _list_head *head, struct _list *node) {

	node->next = (struct _list *)head;
	node->prev = head->prev;
	head->prev->next = node;
	head->prev = node;
}

/** リストを先頭に追加
 */
void
list_add_top(struct _list_head *head, struct _list *node) {

	node->next = head->next;
	node->prev = (struct _list *)head;
	head->next->prev = node;
	head->next = node;
}

/** リストを指定位置(pos)の前に挿入
 */
void
list_insert_at(struct _list *pos, struct _list *node) {

	node->next = pos;
	node->prev = pos->prev;
	pos->prev->next = node;
	pos->prev = node;
}

/** 先頭ノード参照
 */
struct _list *
list_ref_top(struct _list_head *head) {
	
	return head->next;
}

/** 先頭ノード獲得
 */
struct _list *
list_get_top(struct _list_head *head) {
	struct _list *top;

	top = list_ref_top(head);
	list_del(top);

	return top;
}

/** 最終ノード参照
 */
struct _list *
list_ref_tail(struct _list_head *head) {
	
	return head->prev;
}

/** 最終ノード獲得
 */
struct _list *
list_get_tail(struct _list_head *head) {
	struct _list *tail;

	tail = list_ref_tail(head);
	list_del(tail);

	return tail;
}

/** リスト(キュー)が空であることを確認する
 */
int
list_is_empty(struct _list_head *head) {
	
	return  (head->prev == (struct _list *)head) && (head->next == (struct _list *)head);
}

/** リスト(キュー)を回転する
 */
void 
list_rotate(struct _list_head *head) {

	list_add(head, list_get_top(head));
}

/** リストノードを初期化する
 */
void 
init_list_node(struct _list *node){

	node->prev = node->next = node;	
}

/** リストヘッドを初期化する
 */
void 
init_list_head(list_head_t *head) {

	head->prev = head->next = (struct _list *)head;	
}
