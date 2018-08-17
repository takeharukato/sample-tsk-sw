/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2018 Takeharu KATO                                      */
/*                                                                    */
/*  File interfaces                                                   */
/*                                                                    */
/*  The following codes are derived from xv6.                         */
/*  https://pdos.csail.mit.edu/6.828/2016/xv6.html                    */
/*  I might replace these codes in the future.                        */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

/** Allocate a file descriptor for the given file.
 */
static int
alloc_new_fd(file_descriptor *f){
	int fd;

	for(fd = 0; THR_FDS_NR > fd; ++fd){

		if ( current->fds[fd] == NULL ) {

			current->fds[fd] = f;
			return fd;
		}
	}
	return -ENOENT;
}
/** Is the directory dp empty except for "." and ".." ?
 */
static int
isdirempty(inode *dp){
	off_t   off;
	d_dirent de;
	off_t   rd_bytes;

	for (off=0; dp->i_size > off; off+=sizeof(de) ) {

		rd_bytes = inode_read(dp, (void *)&de, off, sizeof(d_dirent));
		kassert ( rd_bytes != sizeof(de));

		if ( ( strncmp(".", de.d_name, FNAME_MAX) == 0 ) ||
		     ( strncmp("..", de.d_name, FNAME_MAX) == 0 ) )
			continue;

		if (de.d_ino != 0)
			return 0;
	}

	return 1;
}

static inode*
create(char *path, imode_t type, dev_id dev){
	inode *ip, *dp;
	char name[FNAME_MAX];
	dirent ent;

	dp = inode_nameiparent(path, name);
	if ( dp == NULL )
		return NULL;

	inode_lock(dp);
	ip = inode_dirlookup(dp, name, &ent);
	if ( ip != NULL ) {

		inode_unlock(dp);
		inode_put(dp);

		inode_lock(ip);

		if ( (type == FS_IMODE_FILE) && (ip->i_mode == FS_IMODE_FILE) )
			return ip;  /* Return locked inode */

		inode_unlock(ip);
		inode_put(ip);

		return NULL;
	}

	ip = inode_allocate(dp->i_dev, type);
	kassert( ip != NULL );

	inode_lock(ip);
	ip->i_dev = dev;
	ip->i_nlink = 1;
	inode_update(ip);

	if (type == FS_IMODE_DIR){  // Create . and .. entries.
		++dp->i_nlink;  // for ".."
		inode_update(dp);

		// No ip->i_nlink++ for ".": avoid cyclic ref count.
		if ( (inode_add_directory_entry(ip, ".", ip->inum) < 0 ) || 
		     (inode_add_directory_entry(ip, "..", dp->inum) < 0 ) )
			panic("create dots");
	}

	if (inode_add_directory_entry(dp, name, ip->inum) < 0)
		panic("create: dirlink");

	inode_unlock(dp);
	inode_put(dp);

	return ip;
}


int
fs_dup(int fd){
	file_descriptor *f, *new;
	int new_fd;

	if ( ( fd < 0 ) || ( THR_FDS_NR <= fd ) )
		return -EINVAL;
	
	f = current->fds[fd];
	if ( f == NULL )
		return -EBADF;

	
	new_fd = alloc_new_fd(f);
	if( new_fd < 0 )
		return new_fd;

	fdtable_duplicate_fd(f, &new);

	return new_fd;
}

int
fs_read(int fd, char *dst, size_t counts){
	int             rc;
	file_descriptor *f;
	io_cnt_t rdcnt;

	if ( ( fd < 0 ) || ( THR_FDS_NR <= fd ) )
		return -EINVAL;
	
	f = current->fds[fd];
	if ( f == NULL )
		return -EBADF;

	rc = fd_file_read(f, dst, counts, &rdcnt);
	return (rc == 0) ?  (rdcnt) : (rc);
}

int
fs_write(int fd, char *src, size_t counts){
	int             rc;
	file_descriptor *f;
	io_cnt_t wrcnt;

	if ( ( fd < 0 ) || ( THR_FDS_NR <= fd ) )
		return -EINVAL;
	
	f = current->fds[fd];
	if ( f == NULL )
		return -EBADF;

	rc = fd_file_write(f, src, counts, &wrcnt);
	return (rc == 0) ?  (wrcnt) : (rc);
}

int
fs_close(int fd){
	file_descriptor *f;
  
	if ( ( fd < 0 ) || ( THR_FDS_NR <= fd ) )
		return -EINVAL;
	
	f = current->fds[fd];
	if ( f == NULL )
		return -EBADF;

	current->fds[fd] = NULL;

	fdtable_close_fd(f);

	return 0;
}

int
fs_open(char *path, omode_t omode){
	int fd, rc;
	file_descriptor *f;
	inode *ip;

	if (omode & O_CREATE) {

		/* Note: create locks this inode */
		ip = create(path, FS_IMODE_FILE, ROOT_DEV);

		if ( ip == NULL ) 
			return -1;
	} else {

		ip = inode_namei(path);
		if( ip == NULL ) 
			return -1;

		inode_lock(ip);

		if ( ( ip->i_mode == FS_IMODE_DIR ) && ( omode != O_RDONLY ) ) 
			goto inode_put_out;
	}	
	
	rc = fdtable_alloc_new_fd(&f);
	if ( rc != 0 ) 
		goto inode_put_out;
	
	fd = alloc_new_fd(f);
	
	if ( (fd < 0) ) {
		
		fdtable_close_fd(f);
		goto inode_put_out;
	}
		
	inode_unlock(ip);
	
	f->f_inode = ip;
	f->f_offset = 0;
	f->f_flags = omode;

	return fd;
	
inode_put_out:
	inode_unlock(ip);
	inode_put(ip);

	return -1;    
}

int
fs_link(char *old, char *new){
	int               rc;
	char name[FNAME_MAX];
	inode       *dp, *ip;

	ip = inode_namei(old);
	if ( ip == NULL ) 
		return -1;

	inode_lock(ip);
    
	if( ip->i_mode == FS_IMODE_DIR ){

		inode_unlock(ip);
		inode_put(ip);
		return -1;
	}

	++ip->i_nlink;

	inode_unlock(ip);
	inode_put(ip);

	dp = inode_nameiparent(new, name);
	if( dp == NULL ) 
		goto error_out;


	inode_lock(dp);

	if ( dp->i_dev != ip->i_dev ) {

		inode_unlock(dp);
		inode_put(dp);
		goto error_out;
	}

	rc = inode_add_directory_entry(dp, name, ip->inum);
	if ( rc < 0 ) {

		inode_unlock(dp);
		inode_put(dp);
		goto error_out;
	}

	inode_unlock(dp);
	inode_put(dp);
    
	inode_put(ip);

	return 0;

error_out:
	inode_lock(ip);

	--ip->i_nlink;
	inode_update(ip);

	inode_unlock(ip);
	inode_put(ip);
	
	return -1;
}

int
fs_unlink(char *path){
	inode       *ip, *dp;
	d_dirent          de;
	char name[FNAME_MAX];
	dirent           ent;
	off_t         wr_len;

	dp = inode_nameiparent(path, name);
	if ( dp == NULL ) 
		return -1;

	inode_lock(dp);

	if ( ( strncmp(".", name, FNAME_MAX) == 0 ) ||
	    ( strncmp("..", name, FNAME_MAX) == 0 ) )
		goto error_out;

	ip = inode_dirlookup(dp, name, &ent);

	if ( ip == NULL )
		goto error_out;

	inode_lock(ip);

	kassert( ip->i_nlink > 0 );

	if ( ( ip->i_mode == FS_IMODE_DIR ) && ( !isdirempty(ip) ) ) {

		inode_unlock(ip);
		inode_put(ip);
		goto error_out;
	}

	memset(&de, 0, sizeof(de));

	wr_len = inode_write(dp, (char*)&de, ent.d_off, sizeof(de));
	kassert( wr_len == sizeof(d_dirent) );

	if ( ip->i_mode == FS_IMODE_DIR ) {

		--dp->i_nlink;  /* for ".." */
		inode_update(dp);
	}

	inode_unlock(dp);
	inode_put(dp);

	ip->i_nlink--;
	inode_update(ip);

	inode_unlock(ip);
	inode_put(ip);

	return 0;

error_out:
	inode_unlock(dp);
	inode_put(dp);

	return -1;
}

off_t
fs_lseek(int fd, off_t off, int where) {
	file_descriptor *f;
	off_t          new;

	if ( ( fd < 0 ) || ( THR_FDS_NR <= fd ) )
		return -EINVAL;
	
	f = current->fds[fd];
	if ( f == NULL )
		return -EBADF;

	if ( where == SEEK_SET )
		new = off;
	if ( where == SEEK_CUR )
		new = f->f_offset + off;

	kassert(f->f_inode != NULL );

	inode_lock(f->f_inode);

	if ( where == SEEK_END )
		new = f->f_inode->i_size + off;

	if ( new < 0 )
		new = 0;

	inode_unlock(f->f_inode);
	f->f_offset = new;

	return new;
}

/** Get file status
 */
int
fs_fstat(int fd, struct _stat *st){
	file_descriptor *f;

	if ( ( fd < 0 ) || ( THR_FDS_NR <= fd ) )
		return -EINVAL;
	
	f = current->fds[fd];
	if ( f == NULL )
		return -EBADF;

	kassert(f->f_inode != NULL );
	
	fd_file_stat(f, st);

	return 0;
}
