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

	/* write magic into block 0 */
	memmove(fsimage_area, "This is a sample", strlen("This is a sample")+1);
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
write_fs_image(size_t size, obj_size_t nr_files) {
	superblock *sb;

	printf("Size in blocks:%d blk\n", size/BSIZE);
	printf("max file size: %lu\n", MAXFILE_IN_BLOCKS);

	sb = refer_sector(SUPER_BLOCK_BLK_NO);

	sb->size_in_blks = size/BSIZE;
	sb->ninodes = nr_files;

	sb->ibitmap_blk = RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR;
	sb->nr_ibitmap_blks = (sb->ninodes / OBJS_PER_BITMAP ) / BITMAP_PER_BLOCK+1;

	sb->inode_blk = RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR + sb->nr_ibitmap_blks ;
	sb->nr_inode_blks = sb->ninodes/INODES_PER_BLOCK+1;

	sb->blk_bitmap_blk = sb->inode_blk + sb->nr_inode_blks; 
	sb->nr_blk_bitmap_blks = 
		( (sb->size_in_blks -
		    ( sb->nr_inode_blks + sb->nr_ibitmap_blks + 
			RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR)) / 
		    OBJS_PER_BITMAP ) / BITMAP_PER_BLOCK+1;

	sb->nblocks = sb->size_in_blks - 
		( sb->nr_blk_bitmap_blks + sb->nr_inode_blks + sb->nr_ibitmap_blks + 
		    RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR);

	sb->data_blk= sb->blk_bitmap_blk+sb->nr_blk_bitmap_blks;
		
	
	printf("sb->size= %ld\n", sb->size_in_blks);
	printf("sb->nblocks= %ld\n", sb->nblocks);
	printf("sb->ninodes= %ld\n", sb->ninodes);
	printf("sb->ibitmap_blk= %ld\n", sb->ibitmap_blk);
	printf("sb->nr_ibitmap_blks= %ld\n", sb->nr_ibitmap_blks);
	printf("sb->inode_blk = %ld\n", sb->inode_blk);
	printf("sb->nr_inode_blks = %ld\n", sb->nr_inode_blks);
	printf("sb->blk_bitmap_blk = %ld\n",sb->blk_bitmap_blk);
	printf("sb->nr_blk_bitmap_blk = %ld\n",sb->nr_blk_bitmap_blks);
	printf("sb->data_blocks = %ld\n", sb->data_blk);

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

	   write_fs_image(size, nr_files);

	   munmap(imgfile, size);

	   return 0;
}
