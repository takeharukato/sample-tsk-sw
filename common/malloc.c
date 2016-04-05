/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  OS用mallocインターフェース                                        */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/** メモリ獲得関数
    @param[in] size メモリ獲得サイズ
    @return    獲得したメモリのユーザ領域
    @note 0バイトでも最小メモリ割当て単位を割り当てる
 */
void *
kmalloc(size_t size) {
	int     rc;
	heap_t *hp = refer_main_heap();
	size_t   s = (size == 0) ? (MALIGN_SIZE) : (size);  
	chunk_t *c;
	
	rc = get_free_chunk(hp, s, &c);
	if (rc != 0)
		goto error_out;

	return CHUNK2PTR(c);

error_out:
	return NULL;
}
/** 獲得したメモリ領域を開放する
    @param[in] m 開放するメモリ領域
 */
void
kfree(void *m) {

	free_chunk(PTR2CHUNK(m));
}

