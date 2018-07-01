/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  memory chunk relevant routines                                    */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_CHUNK_H)
#define  _KERN_CHUNK_H 

#include <kern/freestanding.h>

#include <kern/list.h>
#include <kern/heap.h>

#include <hal/addrinfo.h>

/** メモリ獲得単位/メモリ管理単位
 */
#define ALLOC_UNIT_SHIFT                    (4)  /*<  メモリの割り当て単位を16( = 2^4 )バイト とする  */
#define INDEX_SHIFT      (ALLOC_UNIT_SHIFT + 1)  /*<  サイズから空きリストビットマップのインデクスを出すシフト値  */
#define ALLOC_UNIT        (1<<ALLOC_UNIT_SHIFT)  /*<  メモリ管理単位  */

/** チャンクの状態ビット(サイズの末尾2ビットを使用する)
 */
#define CHUNK_USE_BIT    (0x0)  /*< 使用中であることを示す  */
#define CHUNK_FREE_BIT   (0x1)  /*< 空き領域であることを示す  */
#define CHUNK_EXTRA_BIT  (0x2)  /*< エキストラアリーナのチャンクであることを表す  */
#define CHUNK_ATTR_MASK (CHUNK_FREE_BIT|CHUNK_EXTRA_BIT)

/** バディの位置関係
 */
#define CHUNK_BUDDY_EVEN (0x0)  /*< 偶数側バディ  */
#define CHUNK_BUDDY_ODD  (0x1)  /*< 偶数側バディ  */

/** チャンクデータ構造
 */
typedef struct _chunk{
	size_t  prev_size;   /*< 前のチャンクまでのサイズ  */
	size_t       size;   /*< ヘッダを含む領域長(=次のチャンクまでのサイズ)  */
	list_t       free;   /*< 空きリスト管理および割り当て時には, freeメンバの位置から割り当てる  */
}chunk_t;

/** チャンク中のユーザメモリ領域までのオフセット
 */
#define CHUNK_MEM_OFFSET ((size_t)(&(((chunk_t *)(0))->free)))

/** 空きリストのビットマップのインデックスからチャンクサイズを出す
    @param[in] idx インデクス
    @return チャンクサイズ
 */
#define index2size(idx) (ALLOC_UNIT * ( 1UL << (idx) ) )  

/** 要求メモリ量をメモリ割り当て単位で切り上げる
    @param[in] s 要求サイズ
    @return 獲得するメモリサイズ
 */
#define roundup_size(s)				   \
	( ((s) == 0) ? (MALIGN_SIZE) :		   \
	    (ADDR_NOT_ALIGNED((s), MALIGN_SIZE)) ? \
	    ( ROUNDUP_ALIGN((s), MALIGN_SIZE) ) :  \
		((size_t)(s)) )

/** 割当てメモリサイズを算出する
    @param[in] s チャンクのエリアサイズ
    @return 割り当てるメモリサイズ
    @note sには, chunkヘッダ中のユーザ領域開始位置までの長さ + ユーザ領域長をアドレスサイズでラウンドアップした長さ
    を与える(CALC_AREA_SIZEの算出値). 本マクロは, sの長さを格納可能なメモリを割当て単位の2冪で算出する
 */
#define size2alloc(s)					\
	(index2size(size2index(s)))

/** チャンク管理情報からユーザ領域のアドレスを得る
   @param[in] c チャンク管理情報のアドレス
   @return ユーザ領域のアドレス
 */
#define CHUNK2PTR(c)						\
	((void *)( (void *)(c) + CHUNK_MEM_OFFSET ))

/** ユーザ領域先頭アドレスからチャンク管理情報のアドレスを得る
   @param[in] p ユーザ領域先頭アドレス
   @return チャンク管理情報のアドレス
 */
#define PTR2CHUNK(p)					\
	((chunk_t *)((void *)(p) - CHUNK_MEM_OFFSET))

/** 獲得メモリサイズからチャンクのエリアサイズを算出する
    @param[in] s 獲得メモリサイズ
    @return チャンクのエリアサイズ
    @note 本マクロは, サイズをアドレス長でラウンドアップした上で, ヘッダ長を加算する.実際に割り当てるメモリ量は,
    本マクロの返却値をsize2allocマクロに与えたサイズとなる.
 */
#define CALC_AREA_SIZE(s)						\
	( ((size_t)roundup_size((s))) + CHUNK_MEM_OFFSET )

/** チャンクのエリアサイズを獲得する
    @param[in] c チャンク管理情報
    @return チャンクのエリアサイズ
 */
#define CHUNK_AREA_SIZE(c)			\
	((size_t)( ( (c)->size ) & ( ~( (size_t) (CHUNK_ATTR_MASK) ) ) ) )

/** チャンクの属性情報を取り出す
    @param[in] c チャンク管理情報
    @return チャンクの属性情報
 */
#define CHUNK_ATTR(c)					\
	( (c)->size  &  (size_t)(CHUNK_ATTR_MASK)  )

/** チャンクの属性情報をクリアする
    @param[in] c チャンク管理情報
 */
#define CHUNK_CLR_ATTR(c)					\
	( ( (c)->size ) & ~( (size_t)(CHUNK_ATTR_MASK) )  )

/** 空きチャンクに設定する
    @param[in] c チャンク管理情報
 */
#define CHUNK_MAKE_FREE(c)	do{					\
		(c)->size |= ((size_t)(CHUNK_FREE_BIT));		\
	}while(0)

/** 使用済チャンクに設定する
    @param[in] c チャンク管理情報
 */
#define CHUNK_MAKE_USED(c)	do{					\
		(c)->size &= ~((size_t)(CHUNK_FREE_BIT));		\
	}while(0)

/** エキストラアリーナのチャンクであることを確認する
    @param[in] c チャンク管理情報
    @retval 真 エキストラアリーナのチャンクである
    @retval 偽 エキストラアリーナのチャンクである
 */
#define CHUNK_IN_EXTRA_ARENA(c)						\
	( CHUNK_ATTR(c) & CHUNK_EXTRA_BIT )

/** メインアリーナのチャンクであることを確認する
    @param[in] c チャンク管理情報
    @retval 真 ヒープ内の最終チャンクである
    @retval 偽 ヒープ内の最終チャンクでない
    @note 現在未使用(将来のために実装)
 */
#define CHUNK_IN_MAIN_HEAP(c)						\
	( !CHUNK_IN_EXTRA_ARENA(c) )

/** チャンクからヒープ領域管理情報を取り出す
    @param[in] c チャンク管理情報
    @return チャンクの所属するヒープのヒープ管理情報のアドレス
 */
#define CHUNK2HEAP(c)				    \
	( refer_main_heap() )

/** ヒープの先頭からのオフセット位置を返却する
    @param[in] c チャンク管理情報
    @return チャンクのオフセット位置
 */
#define CHUNK_OFFSET_POS(c)			\
	( (size_t)( ( (size_t)(c) ) - ( (size_t)((CHUNK2HEAP((c)))->chunk)) ) )

/** バディのどちら側に位置するチャンクであるか返却する
    @param[in] c チャンク管理情報
    @retval CHUNK_BUDDY_ODD  奇数側
    @retval CHUNK_BUDDY_EVEN 偶数側
 */
#define CHUNK_BUDDY_SIDE(c)			\
	( ( CHUNK_OFFSET_POS((c)) & CHUNK_AREA_SIZE((c)) ) ? (CHUNK_BUDDY_ODD) : (CHUNK_BUDDY_EVEN) )

/** バディチャンクのチャンク情報を返却する
    @param[in] c チャンク管理情報
    @return バディチャンクのチャンク情報へのポインタ
 */
#define CHUNK_GET_BUDDY(c)				    \
	((chunk_t *)( (CHUNK_BUDDY_SIDE((c)) == CHUNK_BUDDY_ODD) ?	\
	     ( ((void *)(c)) - CHUNK_AREA_SIZE((c)) ) :			\
	     ( ((void *)(c)) + CHUNK_AREA_SIZE((c)) ) ) )

/** バディチャンクの偶数側を返却する
    @param[in] c チャンク管理情報
    @return 偶数側チャンクのチャンク情報へのポインタ
 */
#define CHUNK_GET_EVEN_SIDE(c)				    \
	((chunk_t *)( (CHUNK_BUDDY_SIDE((c)) == CHUNK_BUDDY_ODD) ?	\
	     ( ((void *)(c)) - CHUNK_AREA_SIZE((c)) ) :			\
	    ((void *)(c)) ) )

/** 双方のバディの大きさが一致すること(互いが互いのバディになっていること)を確認する
    @param[in] c チャンク管理情報
    @retval 真 双方のバディのオーダが一致する
    @retval 偽 双方のバディのオーダが一致しない
 */
#define CHUNK_BUDDY_MATCHED(c)				    \
	( CHUNK_AREA_SIZE(self) & CHUNK_AREA_SIZE(CHUNK_GET_BUDDY((c))) )

/** ヒープ内の最初のチャンクであることを確認する
    @param[in] c チャンク管理情報
    @retval 真 ヒープ内の最終チャンクである
    @retval 偽 ヒープ内の最終チャンクでない
 */
#define CHUNK_IS_FIRST(c)						\
	(CHUNK_OFFSET_POS(c) == 0)

/** ヒープ内の最終チャンクであることを確認する
    @param[in] c チャンク管理情報
    @retval 真 ヒープ内の最終チャンクである
    @retval 偽 ヒープ内の最終チャンクでない
 */
#define CHUNK_IS_LAST(c)						\
	( ( CHUNK_OFFSET_POS((c)) + CHUNK_AREA_SIZE((c)) ) == GET_HEAP_SIZE(CHUNK2HEAP((c))))

/** チャンクが空きチャンクか調べる
    @param[in] c チャンク管理情報
    @retval 真 空きチャンクである
    @retval 偽 空きチャンクでない
 */
#define CHUNK_IS_FREE(c)						\
	( CHUNK_ATTR(c) & CHUNK_FREE_BIT )

/** チャンクが使用中か調べる
    @param[in] c チャンク管理情報
    @retval 真 使用中チャンクである
    @retval 偽 使用中チャンクでない
 */
#define CHUNK_IS_USED(c)						\
	(!CHUNK_IS_FREE(c))

/** チャンクのエリアサイズを設定する
    @param[in] c チャンク管理情報
    @param[in] s チャンクのエリアサイズ
 */
#define CHUNK_SET_AREA_SIZE(c, s)	do{		\
		(c)->size = (s) | CHUNK_ATTR(c);	\
	}while(0)

/** チャンクのユーザ領域サイズを獲得する
    @param[in] c チャンク管理情報
    @retval  チャンクのユーザ領域サイズ
 */
#define CHUNK_MEM_SIZE(c)				\
	( CHUNK_AREA_SIZE(c)  - CHUNK_MEM_OFFSET )

/** 次のチャンクを参照する
    @param[in] c チャンク管理情報
    @retval  次のチャンクがある場合 次のチャンクの管理情報のアドレス
    @retval  最終チャンクの場合     c のアドレス
 */
#define CHUNK_NEXT(c)				\
	( CHUNK_IS_LAST(c) ?			\
	    (c) :							\
	    ( (chunk_t *) ( ( (void *)(c) ) + ( (size_t)(CHUNK_AREA_SIZE(c)) ) ) ) )

/** 前のチャンクを参照する
    @param[in] c チャンク管理情報
    @retval  次のチャンクがある場合 次のチャンクの管理情報のアドレス
    @retval  最終チャンクの場合     c のアドレス
 */
#define CHUNK_PREV(c)				\
	( CHUNK_IS_FIRST(c) ?						\
	    (c) :							\
	    ( (chunk_t *) ( ( (void *)(c) ) - (c)->prev_size ) ) )

/** チャンク情報を初期化する
    @param[in] c チャンク管理情報
    @param[in] s チャンクのエリアサイズ
    @note　前方チャンクまでのサイズを0に設定, エリアサイズを設定の上, 空きリスト管理ノードを初期化する
 */
#define CHUNK_INIT(c, s) do {			\
		(c)->prev_size = 0;		\
		CHUNK_CLR_ATTR(c);		\
		CHUNK_MAKE_FREE(c);		\
		CHUNK_SET_AREA_SIZE(c, s);	\
		init_list_node(&((c)->free));	\
	} while(0)

/** チャンクが割り当てサイズの2冪に揃えられていることを確認するためのマスク
    @param[in] c チャンク管理情報
    @return 最上位ビット以外のビットだけを取り出したマスク値
 */
#define CHUNK_ROUNDUP_MASK(c)			\
	(((size_t)( ( 1 << (find_msr_bit_in_size(CHUNK_AREA_SIZE(c)) - 1) ) - 1 ) ) )	

/** チャンクサイズが2冪に揃えられていることを確認する
    @param[in] c チャンク管理情報
    @retval 真 チャンクが割り当てサイズの2冪に揃えられている
    @retval 偽 チャンクが割り当てサイズの2冪に揃えられていない
 */
#define CHUNK_IS_ROUNDUP(c)				    \
	( (find_msr_bit_in_size(CHUNK_AREA_SIZE(c)) < 2) ||	    \
	( !(CHUNK_AREA_SIZE(c) & (((size_t)(ALLOC_UNIT - 1)))) && \
	    !( CHUNK_AREA_SIZE(c) & CHUNK_ROUNDUP_MASK(c) ) ) )

/** 領域長を割当て単位の2冪に切り詰めた大きさを算出する
    @param[in] c チャンク管理情報
    @return チャンク内を割当て単位の2冪単位で割当てた場合の最大領域長
 */
#define CHUNK_TRANC_SIZE(c)			   \
	(find_msr_bit_in_size(CHUNK_AREA_SIZE((c)) < 2) ?	\
	(ALLOC_UNIT) :						\
	    (1 << (find_msr_bit_in_size(CHUNK_AREA_SIZE((c))) - 1)) )


int get_free_chunk(heap_t *, size_t , chunk_t **);
int free_chunk(chunk_t *);
void setup_heap_chunks(heap_t *, size_t );
#endif  /*   _KERN_CHUNK_H   */
