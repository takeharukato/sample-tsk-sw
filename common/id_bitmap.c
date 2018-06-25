/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  ID bitmap operations                                              */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/** IDビットマップ中の空きIDを得る
    @param[in] idmap IDマップのアドレス
    @param[in] idp 獲得したIDを返却する領域
    @retval 0  正常終了
    @retval ENOENT 利用可能なIDがない
 */
int
get_new_id(id_bitmap_t *idmap, obj_id_t *idp) {
	int i,j;
	psw_t psw;

	psw_disable_interrupt(&psw);
	for(i = 0; i < BITMAP_ARRAY_NR; ++i) {
		for(j = 0; j < sizeof(bitmap_ent_t); ++j) {
			if (!(idmap->bitmap[i] & ( 1UL << j ) )) {
				idmap->bitmap[i] |= ((bitmap_ent_t)( 1 << j ));
				*idp = j + i * sizeof(bitmap_ent_t);
				return 0;
			}
		}
	}
	psw_restore_interrupt(&psw);

	return ENOENT;
}

/** IDビットマップにIDを返却する
    @param[in] idmap IDマップのアドレス
    @param[in] id    返却するID
 */
void
put_id(id_bitmap_t *idmap, obj_id_t id) {
	int idx, offset;
	psw_t psw;

	psw_disable_interrupt(&psw);
	idx = id / BITMAP_ARRAY_NR;
	offset = id % sizeof(bitmap_ent_t);
	idmap->bitmap[idx] &= ~((bitmap_ent_t)( 1UL << offset ));
	psw_restore_interrupt(&psw);

	return;
}

/** IDビットマップのIDを予約する
    @param[in] idmap IDマップのアドレス
    @param[in] id    予約するID
    @retval 0     正常に予約した
    @retval EBUSY 既に使用されている
 */
int
reserve_id(id_bitmap_t *idmap, obj_id_t id) {
	int rc;
	int idx, offset;
	psw_t psw;

	psw_disable_interrupt(&psw);
	idx = id / BITMAP_ARRAY_NR;
	offset = id % sizeof(bitmap_ent_t);
	if (idmap->bitmap[idx] & ((bitmap_ent_t)( 1UL << offset ))) {
		rc = EBUSY;
		goto out;
	}
	idmap->bitmap[idx] |= ((bitmap_ent_t)( 1UL << offset ));
	rc = 0;
out:
	psw_restore_interrupt(&psw);

	return rc;
}

/** IDビットマップの初期化
    @param[in] idmap IDビットマップのアドレス
 */
void
init_idmap(id_bitmap_t *idmap){
	psw_t psw;

	psw_disable_interrupt(&psw);
	memset(idmap, 0, sizeof(id_bitmap_t));
	psw_restore_interrupt(&psw);

	return;
}
