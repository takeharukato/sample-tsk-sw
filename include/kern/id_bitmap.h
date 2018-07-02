/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  id bitmap operations                                              */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_ID_BITMAP_H)
#define _KERN_ID_BITMAP_H

#include <kern/freestanding.h>

#include <kern/errno.h>
#include <kern/kern_types.h>

typedef uint64_t bitmap_ent_t;  /*<  Bitmap entry  */
#define BITMAP_ARRAY_NR (CONFIG_ID_BITMAP_UNIT/(sizeof(bitmap_ent_t)*BIT_PER_BYTE)) /*< The number of entries in a bitmap  */

/** ID Bitmap
 */
typedef struct _bitmap{
	bitmap_ent_t bitmap[BITMAP_ARRAY_NR];  /*< ID bitmap  */
}id_bitmap_t;

/** A macro to initialize an ID bitmap 
 */
#define ID_BITMAP_INITIALIZER {			\
	.bitmap = {0,}				\
	 }

void init_idmap(id_bitmap_t *_bmap);
int get_new_id(id_bitmap_t *_bmap, obj_id_t *_id);
void put_id(id_bitmap_t *_bmap, obj_id_t _id);
int reserve_id(id_bitmap_t *_bmap, obj_id_t _id);
#endif  /*  _KERN_ID_BITMAP_H  */
