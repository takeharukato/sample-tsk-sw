/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  ヒープ管理                                                        */
/*                                                                    */
/**********************************************************************/
#include "kern/kernel.h"

static heap_t global_heap = HEAP_INITIALIZER(global_heap, HEAP_SIZE);  /*< 大域ヒープ  */

/** フリーリストを初期化する
    @param[in] fc フリーリスト情報のアドレス
 */
static void
init_free_chunk_bitmap(free_chunks_t *fc) {
	int i;
	psw_t psw;

	psw_disable_interrupt(&psw);
	memset(&fc->bitmap,0, sizeof(flist_bitmap_t));
	for(i = 0;i < FREE_CHUNK_LIST_NR; ++i) {
		init_list_head(&fc->flist[i]);
	}
	psw_restore_interrupt(&psw);
}

/** ヒープ管理情報を初期化する
    @param[in] hp   ヒープ管理情報のアドレス
    @param[in] size ヒープ領域長
 */
static void
init_heap_node(heap_t *hp, size_t size) {
	chunk_t *c;
	psw_t psw;

	psw_disable_interrupt(&psw);

	init_list_node(&hp->list);
	memset(&hp->heap, 0, size);
	hp->size  = size;
	hp->chunk = (chunk_t *)&(hp->heap[0]);
	init_free_chunk_bitmap(&hp->fchunks);
	setup_heap_chunks(hp, size);

	psw_restore_interrupt(&psw);
}

/** メモリサイズからビットマップのインデクスを算出する
    @param[in] size 獲得メモリサイズ
    @return ビットマップのインデクス
 */
int
size2index(size_t size) {
	int           idx;
	size_t alloc_size;
	size_t       mask;

	alloc_size = roundup_size(size); /* 要求メモリサイズをメモリ割り当て単位で切り上げる  */

	idx = find_msr_bit_in_size(alloc_size) - INDEX_SHIFT; /* 最上位ビットを元にインデクス位置を算出  */

        /*
	 * 最上位ビット以外が立っていたら１つ大きいサイズを割り当てる  
	 */
	mask = index2size(idx) - 1;  
	if (alloc_size & mask)
		++idx;
	
	return idx;
}

/** ヒープ内のチャンクの状態を表示する
    @param[in] hp ヒープ管理情報
 */
void
print_chunks(heap_t *hp) {
	chunk_t *c;
	psw_t psw;

	psw_disable_interrupt(&psw);
	kprintf("[HEAP:%p]\n", hp->chunk);
	for(c = hp->chunk; ;c = CHUNK_NEXT(c)) {
		kprintf("(chunk, size, attr, prev, next, side, buddy) = "
		       "(0x%lx, %lu, 0x%x, 0x%lx, 0x%lx, %s, 0x%lx)\n",
		       c, CHUNK_AREA_SIZE(c), CHUNK_ATTR(c), CHUNK_PREV(c), CHUNK_NEXT(c), 
		       ( (CHUNK_BUDDY_SIDE(c) == CHUNK_BUDDY_ODD) ? ("ODD") : ("EVEN") ),
		       CHUNK_GET_BUDDY(c));

		kprintf("(heap-size, chunk-pos, chunk-size, chunk-last)=(%lu, %lu, %lu, %lu)\n",
		       GET_HEAP_SIZE(CHUNK2HEAP(c)), CHUNK_OFFSET_POS(c), CHUNK_AREA_SIZE(c), 
		       CHUNK_OFFSET_POS(c) + CHUNK_AREA_SIZE(c));

		if (CHUNK_IS_LAST(c))
			break;
	}
	psw_restore_interrupt(&psw);
}

/** メインヒープの管理情報を返却する
    @return メインヒープの管理情報のアドレス
 */
heap_t *
refer_main_heap(void) {

	return &global_heap;
}

/** メインヒープの管理情報を初期化する
 */
void
init_heap(void) {

	init_heap_node(&global_heap, HEAP_SIZE);
	return;
}
