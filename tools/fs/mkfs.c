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

#define DEFAULT_FS_NAME "fsimg.img"
#define DEFAULT_FS_SIZE (4UL*1024*1024) 

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
	memmove(fsimage_area, "This is a sample", strlen("This is a sample")+1);
}

int
main(int argc, char *argv[]) {
           int flags, opt;
	   char *imgfile=DEFAULT_FS_NAME;
	   size_t size=DEFAULT_FS_SIZE;

           while ((opt = getopt(argc, argv, "hs:")) != -1) {
               switch (opt) {
               case 's':
		       size = atoi(optarg);
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

	   munmap(imgfile, size);

	   return 0;
}
