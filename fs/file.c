/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  file descriptor table                                             */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>
#include <kern/fdtable.h>

static fd_table fdtbl;

void
file_descriptor_table_init(void){

	mutex_init(&fdtbl.mtx);
}

// Allocate a file structure.
file_descriptor *
filealloc(void){
	int               i;
	file_descriptor  *f;

	mutex_hold(&fdtbl.mtx);
	for( i = 0; MAX_FD_TABLE_SIZE > i; ++i) {

		f = &fdtbl.file[i];
		if ( f->ref == 0 ) {

			f->ref = 1;
			mutex_release(&fdtbl.mtx);
			return f;
		}
	}

	mutex_release(&fdtbl.mtx);

	return 0;
}

// Increment ref count for file f.
file_descriptor *
filedup(file_descriptor  *f){

	mutex_hold(&fdtbl.mtx);

	if ( f->ref < 1 )
		panic("filedup");

	f->ref++;

	mutex_release(&fdtbl.mtx);

	return f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(file_descriptor  *f) {
	file_descriptor  ff;

	mutex_hold(&fdtbl.mtx);

	if(f->ref < 1)
		panic("fileclose");

	--f->ref;

	if ( f->ref > 0 ){

		mutex_release(&fdtbl.mtx);
		return;
	}

	ff = *f;

	f->ref = 0;

	mutex_release(&fdtbl.mtx);

	iput(ff.ip);
	
	return;
}

// Read from file f.
int
fileread(file_descriptor  *f, void *addr, size_t n){
	int rc;

	if ( !( f->mode & FD_MODE_READ ) )
		return -1;

	ilock(f->inode);

	rc = readi(f->inode, addr, f->off, n);
	if ( rc > 0 ) 
		f->off += rc;
	iunlock(f->inode);

	return rc;
}

// Write to file f.
int
filewrite(file_descriptor  *f, void *addr, size_t n){
	int rc;
	int max = ((MAXOPBLOCKS-1-1-2) / 2) * 512;
	int i = 0;

	if ( !( f->mode & FD_MODE_WRITE ) )
		return -1;

	// write a few blocks at a time to avoid exceeding
	// the maximum log transaction size, including
	// i-node, indirect block, allocation blocks,
	// and 2 blocks of slop for non-aligned writes.
	// this really belongs lower down, since writei()
	// might be writing a device like the console.
	while( i < n ) {
		int n1 = n - i;

		if (n1 > max)
			n1 = max;

		ilock(f->inode);
		rc = writei(f->inode, addr + i, f->off, n1);
		if ( rc > 0 )
			f->off += rc;
		iunlock(f->inode);

		if (rc < 0)
			break;
		if (rc != n1)
			panic("short filewrite");
		i += rc;
	}

	return i == n ? n : -1;
}
