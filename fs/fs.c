/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  File system                                                       */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

void
read_superblock(dev_id dev, superblock *sb){
    blk_buf *b;

    b = buffer_cache_blk_read(dev, SUPER_BLOCK_BLK_NO);
    memmove(sb, &b->data[0], sizeof(superblock));
    buffer_cache_blk_release(b);
}

static int 
find_free_data_block(dev_id dev, blk_buf **bp) {
	int64_t            i;
	superblock        sb;
	blk_buf           *b;
	blk_no           blk;
	blk_no    bitmap_blk;
	int64_t       offset;
	int64_t  chunk_index;
	int64_t chunk_offset;
	uint64_t         val;

	read_superblock(dev, &sb);
	for(i = 0; sb.s_nblocks > i; ++i ) {

		bitmap_blk = (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR + sb.s_imap_blocks) +
			(i / BITS_PER_BLOCK);

		b = buffer_cache_blk_read(dev, bitmap_blk);

		offset = i % BITS_PER_BLOCK;
		chunk_index = offset / sizeof(uint64_t);
		chunk_offset = offset % sizeof(uint64_t);

		val = *(( (uint64_t *)(&b->data[0]) ) + chunk_index);
		if ( !( val & ( 1ULL << chunk_offset ) ) ) {

			*bp = b;
			memset(&b->data[0], 0, BSIZE);
			buffer_cache_blk_release(b); /* TODO: Check correctness */
			return 0;
		}

		buffer_cache_blk_release(b);

	}

	return ENOENT;
}

static int 
release_data_block(dev_id dev, blk_no blk) {
	superblock        sb;
	blk_buf           *b;
	blk_no    bitmap_blk;
	int64_t       offset;
	int64_t  chunk_index;
	int64_t chunk_offset;
	uint64_t         val;

	read_superblock(dev, &sb);

	bitmap_blk = (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR + sb.s_imap_blocks) +
		(blk / BITS_PER_BLOCK);

	b = buffer_cache_blk_read(dev, bitmap_blk);

	offset = blk % BITS_PER_BLOCK;
	chunk_index = offset / sizeof(uint64_t);
	chunk_offset = offset % sizeof(uint64_t);
	val = *(( (uint64_t *)(&b->data[0]) ) + chunk_index);

	kassert( ( val & ( 1ULL << chunk_offset ) ) );

	*(( (uint64_t *)(&b->data[0]) ) + chunk_index) &= ~( 1ULL << chunk_offset );

	buffer_cache_blk_release(b);

	return 0;
}

