/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  heap management relevant routines                                 */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_HEAP_H)
#define  _KERN_HEAP_H 
#include <stdint.h>

#include "kern/kern_types.h"
#include "kern/param.h"
#include "kern/list.h"

typedef uint32_t flist_bitmap_t;                                    /*< ビットマップのデータ型  */

#define FREE_CHUNK_LIST_NR (sizeof(flist_bitmap_t) * BIT_PER_BYTE)  /*< 空きチャンクリストの数  */

struct _chunk;

/** 空きチャンク管理リスト
 */
typedef struct _free_chunks{
	flist_bitmap_t                 bitmap;   /*< 空きリストのビットマップ  */
	list_head_t flist[FREE_CHUNK_LIST_NR];   /*< 空きチャンクのリスト      */
}free_chunks_t;

/** ヒープ管理情報
 */
typedef struct _heap{
	list_t             list;            /*< ヒープのリンク情報    */
	free_chunks_t   fchunks;            /*< 空きチャンク管理情報  */
	struct _chunk    *chunk;            /*< 先頭チャンク          */
	size_t             size;            /*< ヒープサイズ          */
	uint8_t heap[HEAP_SIZE];            /*< ヒープ                */
}heap_t;

typedef struct _heap_info{
	list_head_t list;                   /*< ヒープのリスト        */
}heap_info_t;

/** ヒープ管理情報の初期化指示子
    @param[in] var ヒープ管理情報の変数名
 */
#define HEAP_INITIALIZER(var, sz) {		\
	.list = {&((var).list), &((var).list)},	\
	.fchunks = {0,{0,} },	 	        \
	.chunk = (chunk_t *)(&((var).heap[0])),	\
        .size = (sz),			        \
	.heap = {0, },			        \
 }

/** 空きチャンク管理情報を取得する
    @param[in] h ヒープ管理情報
    @retval 空きチャンク管理情報のアドレス
 */
#define GET_FREE_CHUNK_LIST(h)						\
	((free_chunks_t *)(&((h)->fchunks)))

/** ヒープの大きさを参照する
    @param[in] h ヒープ管理情報
    @retval ヒープの大きさ
 */
#define GET_HEAP_SIZE(h)						\
	((h)->size)

void init_heap(void);
int  size2index(size_t );
heap_t *refer_main_heap(void);
void print_chunks(heap_t *);
#endif  /*   _KERN_HEAP_H   */
