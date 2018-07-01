/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Buddy memory allocator                                            */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

/** 空きチャンクのサイズがリストに接続可能か確認する
 */
static int
is_chunk_linkable(chunk_t *c) {
	int rc;
	int idx;

	idx = find_msr_bit_in_size(CHUNK_AREA_SIZE(c));
	
	if ( (CHUNK_AREA_SIZE(c) == 0) || ( !CHUNK_IS_ROUNDUP(c) ) )  {
		rc = EINVAL;  /* サイズが, 0または, 割り当て単位の2冪になっていない  */
		goto out;
	}

	if ( idx > FREE_CHUNK_LIST_NR ) {
		rc = E2BIG;  /* サイズが大きすぎる  */
		goto out;
	}

	rc = 0;
out:
	return rc;
}

/** 空きリストに空き領域があるか調べる
    @param[in] hp  ヒープ管理情報
    @param[in] idx 空きリストのインデックス
    @retval 真 空きリストに空き領域がある
    @retval 偽 空きリストに空き領域がない
 */
static inline int
has_free_chunk(heap_t *hp, int idx) {
	free_chunks_t *free_list = GET_FREE_CHUNK_LIST(hp);

	return !(list_is_empty(&free_list->flist[idx]));
}


/** 空きチャンクをアリーナの空きチャンクリストに追加する
    @param[in]             c チャンク情報
    @retval 0 正常にリストに追加できた
    @retval EINVAL サイズが, 0または, 割り当て単位の2冪になっていない 
    @retval E2BIG  サイズが大きすぎる
    (EINVAL, E2BIGは, is_chunk_linkableからの返却値)
 */
static int
add_free_chunk(chunk_t *c) {
	int rc;
	int idx;
	heap_t *hp;
	free_chunks_t *free_list;

	rc = is_chunk_linkable(c);
	if (rc != 0)  /* フリーリストの接続条件にあっていない  */
		goto out;

	hp = CHUNK2HEAP(c);
	free_list = GET_FREE_CHUNK_LIST(hp);
	idx = size2index(CHUNK_AREA_SIZE(c));
	
	/*
	 * 空き領域リストにつなぐ
	 */
	/* TODO:空き領域情報の排他が必要  */
	list_add(&free_list->flist[idx], &c->free);
	free_list->bitmap |=  (flist_bitmap_t)(1 << idx);

	rc = 0;
out:
	return rc;
}

/** チャンクをアリーナの空きチャンクリストから外す
    @param[in]             c チャンク情報
    @retval 0 正常にリストから外した
    @retval EINVAL サイズが, 0または, 割り当て単位の2冪になっていない 
    @retval E2BIG  サイズが大きすぎる
    (EINVAL, E2BIGは, is_chunk_linkableからの返却値)
 */
static int
remove_free_chunk(chunk_t *c) {
	int rc;
	int idx;
	heap_t *hp;
	free_chunks_t *free_list;

	rc = is_chunk_linkable(c);
	if (rc != 0)  /* フリーリストの接続条件にあっていない  */
		goto out;

	hp = CHUNK2HEAP(c);
	free_list = GET_FREE_CHUNK_LIST(hp);
	idx = size2index(CHUNK_AREA_SIZE(c));
	
	/*
	 * 空き領域リストから外す
	 */
	/* TODO:空き領域情報の排他が必要  */
	list_del(&c->free);
	if (list_is_empty(&free_list->flist[idx]))
		free_list->bitmap &= ~((flist_bitmap_t)( 1 << idx ));

	rc = 0;
out:
	return rc;
}

/** 空きリストの先頭要素を取り出す
    @param[in] hp  ヒープ管理情報
    @param[in] idx 空きリストのインデックス
    @retval 非NULL 空きリストの先頭チャンク
    @retval NULL   空きチャンクが指定されたインデクスのリストに存在しない
 */
static chunk_t *
get_first_chunk(heap_t *hp, int idx) {
	free_chunks_t *free_list = GET_FREE_CHUNK_LIST(hp);
	chunk_t *c;

	c = NULL;
	if (has_free_chunk(hp, idx)) {
		c = CONTAINER_OF(list_ref_top(&free_list->flist[idx]),
		    chunk_t, free);
		remove_free_chunk(c);
	}

	return c;
}

/** chunkの領域長を縮小する
    @param[in]             c チャンク情報
    @param[in] new_area_size チャンクのエリアサイズ
    @retval 0      正常終了
    @retval ENOSPC 領域の縮小に失敗した
    @note chunkのユーザメモリサイズを変更する場合は, resize_chunkを使用し,
    本関数を直接使用しないこと.
 */
static int
chunk_shrink_size(chunk_t *c, size_t new_area_size, chunk_t **new_chunk_p) {
	int                rc;
	chunk_t          *new;
	chunk_t         *next;
	size_t free_area_size;

	next = CHUNK_NEXT(c);
	free_area_size = CHUNK_AREA_SIZE(c) - new_area_size;  /*  縮小によってできた空き領域のサイズ */

	if ( free_area_size < ALLOC_UNIT_SHIFT ) {  /*  領域の縮小ができない場合  */
		rc = ENOSPC;
		goto out;
	}

	if (CHUNK_IS_FREE(c))  /*  空きチャンクの場合は, サイズが変更されるので, リストから外す  */
		remove_free_chunk(c);

	/*
	 領域縮小前
	 |             |                |
	 |<-----c----->|<-次のチャンク->|
	 |             |                |
	 領域縮小後
	 |     |       |                |
	 |<-c->|<-new->|<-次のチャンク->|
	 |     |       |                |
	 [チャンクのリスト情報]
	 new->prev = 縮小後のcのエリアサイズ
	 new->next = 新規にできたチャンクのエリアサイズ
	 */
	new = (chunk_t *)(((void *)c) + new_area_size);  /*  縮小によってできた空き領域  */
	CHUNK_INIT(new, free_area_size);                 /*  縮小によってできた空き領域の情報をセットする  */
	new->prev_size = new_area_size;                  /*  縮小後のサイズ前の領域にcの先頭アドレスが存在する  */
	CHUNK_MAKE_FREE(new);                            /* 空き領域に設定する  */		

	next->prev_size = free_area_size;                /* 前方ポインタを更新する  */

	CHUNK_SET_AREA_SIZE(new, free_area_size);        /* 新規空き領域のエリアサイズを設定する  */
	CHUNK_SET_AREA_SIZE(c, new_area_size);           /* 縮小後のサイズ後ろに新規空き領域が存在する  */
	*new_chunk_p = new;                              /* 新規空き領域を返却する  */

	if (CHUNK_IS_FREE(c))  /*  サイズが変更されたので, リストにつなぎ直す  */
		add_free_chunk(c);

	rc = 0;

out:
	return rc;
}

/** 空きチャンクを16の2のべき乗に分割する
    @param[in] c チャンク管理情報のアドレス
 */
static void
fixup_chunk(chunk_t *c) {
	int             rc;
	size_t        size;
	chunk_t *new_chunk;

	/* 現在のサイズを割当て単位の2冪に切り詰めた領域(CHUNK_TRANC_SIZE(c))を
	 * 現在のチャンクの後ろにつくることで, 割当て単位の2冪でチャンクを結合できるようにする.
	 * このために, 現在のエリアサイズを後ろに作成する領域分だけ縮小する.
	 */
	for(rc = 0, size = CHUNK_AREA_SIZE(c);
	    (!CHUNK_IS_ROUNDUP(c)) && (size > ALLOC_UNIT_SHIFT);
	    size = CHUNK_AREA_SIZE(c)) {
		rc = chunk_shrink_size(c, CHUNK_AREA_SIZE(c) - CHUNK_TRANC_SIZE(c), &new_chunk);
		if (rc == 0)
			add_free_chunk(new_chunk);
	}
	
}
/** チャンクの領域長を変更する
    @param[in] c チャンク管理情報のアドレス
    @param[in] s 変更後のチャンクのエリアサイズ
    @retval 0      正常終了
    @retval ENOSPC 領域の縮小に失敗した(chunk_shrink_sizeから返却)
 */
static int 
resize_chunk(chunk_t *c, size_t s) {
	int               rc;
	chunk_t   *new_chunk;
	size_t new_area_size;
	size_t old_area_size;

	new_area_size = size2alloc(s);
	if ( CHUNK_AREA_SIZE(c) > new_area_size ) {  /*  領域を縮小する場合  */
		rc = chunk_shrink_size(c, new_area_size, &new_chunk);
		if (rc != 0)
			goto out;
		if (!CHUNK_IS_ROUNDUP(new_chunk)) {
			fixup_chunk(new_chunk); /*  空きチャンクを割当て単位の2冪で整理する  */
		}
		if ( CHUNK_IS_FREE(new_chunk) && (!list_is_linked(&new_chunk->free)) )
			add_free_chunk(new_chunk);
	}

	rc = 0;

out:
	return rc;
}
/** 空き領域を獲得する
    @param[in] hp 空き領域を獲得するヒープ
    @param[in] s  ユーザ領域獲得サイズ
    @param[in] cp 獲得したチャンクを指し示すポインタ変数のアドレス
    @retval      0 正常終了
    @retval ENOMEM 空きメモリがない
    @retval ENOSPC 空きメモリの縮小化に失敗した
 */
int
get_free_chunk(heap_t *hp, size_t s, chunk_t **cp) {
	int            rc;
	int           idx;
	size_t alloc_size;
	chunk_t        *c;
	psw_t psw;

	psw_disable_interrupt(&psw);
	/*
	 * 空きチャンクリストからの獲得を試みる
	 */
	alloc_size = size2alloc(CALC_AREA_SIZE(s));
	for(idx = size2index(alloc_size);
	    !has_free_chunk(hp, idx); 
	    ++idx) {
		if (idx >= FREE_CHUNK_LIST_NR) {
			rc = ENOMEM;  /*  空き領域が存在しない  */
			goto error_out;
		}
	}
	c = get_first_chunk(hp, idx);	

	rc = 0;
	CHUNK_MAKE_USED(c);     /* 使用中にする        */
	if ( CHUNK_AREA_SIZE(c) ==  alloc_size ) {
		goto out;  /*  要求サイズ分の領域を切り出した  */
	}

	rc = resize_chunk(c, alloc_size);  /* サイズを縮小化する  */

out:
	psw_restore_interrupt(&psw);
	*cp = c;
	return rc;

error_out:
	psw_restore_interrupt(&psw);
	return rc;
}
/** チャンクを開放する
    @param[in] c 開放するチャンク
    @retval 0 正常終了
    @retval EINVAL サイズが, 0または, 割り当て単位の2冪になっていない 
    @retval E2BIG  サイズが大きすぎる
    (EINVAL, E2BIGは, remove_free_chunkからの返却値)
 */
int
free_chunk(chunk_t *c) {
	int rc;
	chunk_t *buddy;
	chunk_t *self = c;
	psw_t psw;

	psw_disable_interrupt(&psw);

	CHUNK_MAKE_FREE(self);

	for (buddy = CHUNK_GET_BUDDY(self); 
	       CHUNK_IS_FREE(buddy) && CHUNK_BUDDY_MATCHED(self) ;
	     buddy = CHUNK_GET_BUDDY(self) ) {

		rc = remove_free_chunk(buddy);  /*  バディを空きリストから削除  */
		if (rc != 0)  /*  削除失敗につき途中まで結合したチャンクをフリーリストに接続  */
			goto free_out;

		self = CHUNK_GET_EVEN_SIDE(self);  /*  selfを偶数側のチャンクに更新  */

		/*  チャンクを結合  */
		CHUNK_SET_AREA_SIZE(self, CHUNK_AREA_SIZE(self) * 2);

		if ( CHUNK_IS_LAST(self) )
			break;  /*  結合したチャンクが最終チャンクなら抜ける  */
	}

	rc = 0;

free_out:
	add_free_chunk(self);
	psw_restore_interrupt(&psw);

	return rc;
}

/** ヒープに初期チャンクを設定する
    @param[in] hp ヒープ管理情報
    @param[in] s  初期チャンクの大きさ
 */
void
setup_heap_chunks(heap_t *hp, size_t s) {
	chunk_t *c = (chunk_t *)&(hp->heap[0]);
	psw_t psw;

	psw_disable_interrupt(&psw);
	CHUNK_INIT(c, HEAP_SIZE);

	hp->chunk = c;
	add_free_chunk(c);
	psw_restore_interrupt(&psw);

	return;
}
