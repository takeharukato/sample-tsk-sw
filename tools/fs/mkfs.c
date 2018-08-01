/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  mkfs                                                              */
/*                                                                    */
/**********************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#include <kern/kernel.h>
#include <kern/fs.h>

#define DEFAULT_FS_NAME "fsimg.img"
#define DEFAULT_FS_SIZE (4UL*1024*1024) 
#define DEFAULT_INODE_NR (1024)

typedef uint64_t fs_bitmap;

static void *fsimage_area;
static superblock *sb = NULL;
void
map_file(const char *fname, size_t size){
	int fd;
	int rc;

	fd = open(fname, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
	if ( fd < 0 ) {

		fprintf(stderr, "Can not open file:%s\n", fname);
		exit(EXIT_FAILURE);
	}

	rc = ftruncate(fd, size);
	if ( rc < 0 ) {

		fprintf(stderr, "Can not truncate file:%s\n", fname);
		exit(EXIT_FAILURE);
	}
	
	fsimage_area = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if ( fsimage_area == MAP_FAILED ) {

		fprintf(stderr, "Can not map file:%s\n", fname);
		exit(EXIT_FAILURE);
	}
}

static void*
refer_sector(blk_no blk){

	return fsimage_area + BSIZE*blk;
}

static void*
read_sector(blk_no blk, void *dest, size_t size){
	void *mb;

	mb = refer_sector(blk);
	memmove(dest, mb, size);
}

static void*
write_sector(blk_no blk, void *src, size_t size){
	void *mb;

	mb = refer_sector(blk);
	memmove(mb, src, size);
}

static void
mark_inode_used(uint32_t ino){
	blk_no  blk;
	uint8_t buf[BSIZE];
	int64_t offset;
	int64_t chunk_index;
	int64_t chunk_offset;
	uint64_t *valp;
	
	blk = (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR) + (ino / BITS_PER_BLOCK);

	offset = ino % BITS_PER_BLOCK;
	chunk_index = offset / sizeof(uint64_t);
	chunk_offset = offset % sizeof(uint64_t);

	read_sector(blk, &buf[0], BSIZE);

	valp = ( (uint64_t *)(&buf[0]) ) + chunk_index;
	*valp |= ( 1ULL << chunk_offset );

	write_sector(blk, &buf[0], BSIZE);
}

static void
mark_inode_free(uint32_t ino){
	blk_no  blk;
	uint8_t buf[BSIZE];
	int64_t offset;
	int64_t chunk_index;
	int64_t chunk_offset;
	uint64_t *valp;
	
	blk = (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR) + (ino / BITS_PER_BLOCK);

	offset = ino % BITS_PER_BLOCK;
	chunk_index = offset / sizeof(uint64_t);
	chunk_offset = offset % sizeof(uint64_t);

	read_sector(blk, &buf[0], BSIZE);

	valp = ( (uint64_t *)(&buf[0]) ) + chunk_index;
	*valp &= ~( 1ULL << chunk_offset );

	write_sector(blk, &buf[0], BSIZE);
}

static int 
find_free_inode(uint32_t *inop) {
	int     rc;
	int64_t  i;
	blk_no blk;
	uint8_t buf[BSIZE];
	int64_t offset;
	int64_t chunk_index;
	int64_t chunk_offset;
	uint64_t val;

	for(i = 0; sb->s_ninodes > i; ++i ) {

		blk = (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR) + (i / BITS_PER_BLOCK);

		read_sector(blk, &buf[0], BSIZE);

		offset = i % BITS_PER_BLOCK;
		chunk_index = offset / sizeof(uint64_t);
		chunk_offset = offset % sizeof(uint64_t);
		val = *(( (uint64_t *)(&buf[0]) ) + chunk_index);
		if ( !( val & ( 1ULL << chunk_offset ) ) ) {

			*inop = (uint32_t)i;
			return 0;
		}
	}

	return ENOENT;
}

static void
mark_data_block_used(blk_no blk){
	blk_no  bitmap_blk;
	uint8_t buf[BSIZE];
	int64_t offset;
	int64_t chunk_index;
	int64_t chunk_offset;
	uint64_t *valp;
	
	bitmap_blk = (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR + sb->s_imap_blocks) +
		(blk / BITS_PER_BLOCK);

	offset = blk % BITS_PER_BLOCK;
	chunk_index = offset / sizeof(uint64_t);
	chunk_offset = offset % sizeof(uint64_t);

	read_sector(bitmap_blk, &buf[0], BSIZE);

	valp = ( (uint64_t *)(&buf[0]) ) + chunk_index;
	*valp |= ( 1ULL << chunk_offset );

	write_sector(bitmap_blk, &buf[0], BSIZE);
}

static void
mark_data_block_free(blk_no blk){
	blk_no  bitmap_blk;
	uint8_t buf[BSIZE];
	int64_t offset;
	int64_t chunk_index;
	int64_t chunk_offset;
	uint64_t *valp;
	
	bitmap_blk = (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR + sb->s_imap_blocks) +
		(blk / BITS_PER_BLOCK);

	offset = blk % BITS_PER_BLOCK;
	chunk_index = offset / sizeof(uint64_t);
	chunk_offset = offset % sizeof(uint64_t);

	read_sector(bitmap_blk, &buf[0], BSIZE);

	valp = ( (uint64_t *)(&buf[0]) ) + chunk_index;
	*valp &= ~( 1ULL << chunk_offset );

	write_sector(bitmap_blk, &buf[0], BSIZE);
}

static int
read_disk_data_block(blk_no dblk, uint8_t *buf) {
	blk_no blk;

	blk = sb->s_firstdata_block + dblk;
	read_sector(blk, &buf[0], BSIZE);

	return 0;
}

static int
write_disk_data_block(blk_no dblk, uint8_t *buf) {
	blk_no blk;

	blk = sb->s_firstdata_block + dblk;
	write_sector(blk, &buf[0], BSIZE);

	return 0;
}

static int 
find_free_data_block(blk_no *blkp) {
	int     rc;
	int64_t  i;
	blk_no blk;
	blk_no  bitmap_blk;
	uint8_t buf[BSIZE];
	int64_t offset;
	int64_t chunk_index;
	int64_t chunk_offset;
	uint64_t val;

	for(i = 0; sb->s_nblocks > i; ++i ) {

		bitmap_blk = (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR + sb->s_imap_blocks) +
			(i / BITS_PER_BLOCK);

		read_sector(bitmap_blk, &buf[0], BSIZE);

		offset = i % BITS_PER_BLOCK;
		chunk_index = offset / sizeof(uint64_t);
		chunk_offset = offset % sizeof(uint64_t);
		val = *(( (uint64_t *)(&buf[0]) ) + chunk_index);
		if ( !( val & ( 1ULL << chunk_offset ) ) ) {

			*blkp = (blk_no)i;
			return 0;
		}
	}

	return ENOENT;
}

static int
read_disk_inode(uint32_t ino, d_inode *dinodep) {
	int     rc;
	int64_t  i;
	blk_no blk;
	uint8_t buf[BSIZE];
	int index;

	blk = (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR + sb->s_imap_blocks + sb->s_bmap_blocks) +
		(ino / INODES_PER_BLOCK);

	index = ino % INODES_PER_BLOCK;

	read_sector(blk, &buf[0], BSIZE);

	memmove(dinodep, ( (d_inode *)&buf[0] ) + index , sizeof(d_inode));

	return 0;
}

static int
write_disk_inode(uint32_t ino, d_inode *dinodep) {
	int     rc;
	int64_t  i;
	blk_no blk;
	uint8_t buf[BSIZE];
	int index;

	blk = (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR + sb->s_imap_blocks + sb->s_bmap_blocks) +
		(ino / INODES_PER_BLOCK);

	index = ino % INODES_PER_BLOCK;

	read_sector(blk, &buf[0], BSIZE);

	memmove( ( (d_inode *)&buf[0] ) + index, dinodep, sizeof(d_inode));

	write_sector(blk, &buf[0], BSIZE);

	return 0;
}

static int
get_free_inode(uint32_t *inop){
	int       rc;
	uint32_t ino;

	rc = find_free_inode(&ino);
	if ( rc != 0 )
		goto error_out;

	mark_inode_used(ino);

	*inop = ino;
	rc = 0;
error_out:
	return rc;
}

static void
write_root_dirent(void){
	blk_no blk;
	d_inode root_inode;
	uint8_t buf[BSIZE];
	d_dirent *dentp;

	mark_inode_used(ROOT_DENT_INO);
	find_free_data_block(&blk);

	memset(&root_inode, 0, sizeof(d_inode));

	root_inode.i_mode = FS_IMODE_DIR;
	root_inode.i_nlink = 1;
	root_inode.i_size = BSIZE;
	root_inode.i_addr[0]=blk;

	memset(&buf[0], 0, BSIZE);

	dentp = (d_dirent *)&buf[0];
	dentp->d_ino = ROOT_DENT_INO;
	memmove(&dentp->d_name[0], ".", 2);
	++dentp;

	dentp->d_ino = ROOT_DENT_INO;
	memmove(&dentp->d_name[0], "..", 3);
	
	write_disk_data_block(blk, &buf[0]);
	write_disk_inode(ROOT_DENT_INO, &root_inode);
}

static void
init_super_block(size_t size, obj_size_t nr_files) {
	int64_t    inode_blocks;
	int64_t    remains;
	int64_t    data_blocks;

	inode_blocks = ( nr_files + (INODES_PER_BLOCK - 1) )/ INODES_PER_BLOCK;

	sb->s_max_size = size/BSIZE;
	sb->s_ninodes = nr_files;
	sb->s_imap_blocks = ( nr_files + ( BITS_PER_BLOCK - 1 ) ) / BITS_PER_BLOCK; 


	remains = sb->s_max_size
		- (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR)
		- sb->s_imap_blocks
		- inode_blocks;

	/* Calculate the number of data blocks
	 * Note: Remaining blocks = data blocks + the number of blocks for data block bitmap
	 * the number of blocks for data block bitmap 
	    = ( 'data blocks' + (BITS_PER_BLOCK - 1) ) / BITS_PER_BLOCK
	 * Remaining blocks = 
	 *                    s_max_size( blocks in the device) - sector0 - super block 
	 *                    - inode bit map blocks - inodes
	 * remains =  [data blocks] + 
	              ( ( [data blocks] + (BITS_PER_BLOCK - 1) ) / BITS_PER_BLOCK )
	 */ 
	data_blocks = ( remains * BITS_PER_BLOCK - (BITS_PER_BLOCK - 1) ) / 
		(BITS_PER_BLOCK + 1) ;
	sb->s_bmap_blocks = ( data_blocks + (BITS_PER_BLOCK - 1) ) / BITS_PER_BLOCK;
	sb->s_nblocks = (uint32_t)data_blocks;
	sb->s_firstdata_block = (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR)
		+ sb->s_imap_blocks + inode_blocks + sb->s_bmap_blocks;

	sb->s_magic = FS_MAGIC_NR;

	mark_inode_used(0);
	mark_inode_used(1);

	printf("File system size(unit:KiB): %u\n", size / 1024);
	printf("The number of files: %u\n", nr_files);
	printf("File system size(unit:blk): %u\n", sb->s_max_size);
	printf("The number of inodes: %u\n", sb->s_ninodes);
	printf("Inode bitmap blocks(unit:blk): %u\n", sb->s_imap_blocks);
	printf("Block bitmap blocks(unit:blk): %u\n", sb->s_bmap_blocks);
	printf("Inode blocks(unit:blk): %u\n", inode_blocks);
	printf("First blocks(unit:blk): %u\n", sb->s_firstdata_block);
	printf("Number of data blocks(unit:blk): %u\n", sb->s_nblocks);

	return;
}

static void 
write_fs_image(size_t size, obj_size_t nr_files) {
	uint8_t buf[BSIZE];


	init_super_block(size, nr_files);
	write_root_dirent();

	memset(&buf[0], 0, BSIZE);
	memmove(&buf[0], sb, sizeof(superblock));
	write_sector(SUPER_BLOCK_BLK_NO, &buf[0], BSIZE);

	return;
}

int
main(int argc, char *argv[]) {
           int flags, opt;
	   char *imgfile=DEFAULT_FS_NAME;
	   size_t size=DEFAULT_FS_SIZE;
	   obj_size_t nr_files=DEFAULT_INODE_NR;

           while ((opt = getopt(argc, argv, "hs:n:")) != -1) {
               switch (opt) {
               case 's':
		       size = atoi(optarg);
		       break;
               case 'n':
		       nr_files = atoi(optarg);
		       break;
               case 'h':
               default: /* '?' */
                   fprintf(stderr, "Usage: %s [-h] [-s imagesize] output-file\n",
                           argv[0]);
                   exit(EXIT_FAILURE);
               }
           }

	   if ( optind < argc ) 
		   imgfile=argv[optind];

           printf("output file = %s\n", imgfile);
           printf("output size = %d\n", size);

	   map_file(imgfile, size);

	   sb = refer_sector(SUPER_BLOCK_BLK_NO);

	   write_fs_image(size, nr_files);

	   munmap(imgfile, size);

	   return 0;
}
