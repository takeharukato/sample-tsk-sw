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
fdtable_init(void){
	int               i;
	file_descriptor  *f;

	mutex_init(&fdtbl.mtx);
	for( i = 0; MAX_FD_TABLE_SIZE > i; ++i) {

		f = &fdtbl.file[i];

		f->f_flags = 0;
		f->f_count = 0;
		f->f_inode = NULL;
		f->f_offset = 0;
	}

}

/** Allocate new file descriptor
 */
int
fdtable_alloc_new_fd(file_descriptor **fdp){
	int               i;
	int              rc;
	file_descriptor  *f;

	mutex_hold(&fdtbl.mtx);

	for( i = 0; MAX_FD_TABLE_SIZE > i; ++i) {

		f = &fdtbl.file[i];
		if ( f->f_count == 0 ) {

			f->f_count = 1;
			*fdp = f;
			rc = 0;
			goto unlock_out;
		}
	}

	rc = ENOENT;

unlock_out:	
	mutex_release(&fdtbl.mtx);

	return rc;
}


/** Duplicate a file descriptor
 */
int
fdtable_duplicate_fd(file_descriptor  *f, file_descriptor **fdp){

	mutex_hold(&fdtbl.mtx);

	kassert(f->f_count > 0);

	f->f_count++;

	*fdp = f;
	mutex_release(&fdtbl.mtx);

	return 0;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fdtable_close_fd(file_descriptor  *f) {

	mutex_hold(&fdtbl.mtx);

	kassert( f->f_count > 0 );

	f->f_count--;

	if ( f->f_count > 0 )
		goto unlock_out;

	kassert( f->f_count == 0 );
	inode_put(f->f_inode);

unlock_out:
	mutex_release(&fdtbl.mtx);

	return;
}

/** Read from file
 */
int
fd_file_read(file_descriptor  *f, void *addr, size_t n, io_cnt_t *rd_cnt){
	io_cnt_t  rd_count;
	int             rc;

	if ( !( f->f_flags & FREAD ) )
		return EPERM;

	inode_lock(f->f_inode);

	rd_count = inode_read(f->f_inode, addr, f->f_offset, n);
	if ( rd_count > 0 ) {

		f->f_offset += rd_count;
		*rd_cnt = rd_count;
		rc = 0;
	} else 
		rc = (int)rd_count;

	inode_unlock(f->f_inode);

	return rc;
}

/** Write to file
 */
int
fd_file_write(file_descriptor  *f, void *addr, size_t n, io_cnt_t *wr_cnt){
	int              i;
	int             rc;
	io_cnt_t  wr_count;

	if ( !( f->f_flags & FWRITE ) )
		return EPERM;

	inode_lock(f->f_inode);

	wr_count = inode_write(f->f_inode, addr, f->f_offset, n);

	if ( wr_count > 0 ) {

		f->f_offset += wr_count;
		*wr_cnt = wr_count;
		rc = 0;
	} else 
		rc = (int)wr_count;

	inode_unlock(f->f_inode);

	return rc;
}
