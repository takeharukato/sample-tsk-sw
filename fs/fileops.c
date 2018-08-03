/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  File operations                                                   */
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
	off_t off;
	inode *ip, *dp;
	char name[FNAME_MAX];
	dirent ent;

	dp = inode_nameiparent(path, name);
	if ( dp == NULL )
		return NULL;

	inode_lock(dp);
	ip = inode_dirlookup(dp, name, &ent);
	if ( ip != NULL ) {

		inode_unlock(ip);
		inode_put(ip);

		inode_lock(ip);
		if ( (type == FS_IMODE_FILE) && (ip->i_mode == FS_IMODE_FILE) )
			return ip;
		inode_unlock(ip);
		inode_put(ip);

		return 0;
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
sys_dup(int fd){
	int                newfd;
	file_descriptor *f, *new;

	if ( ( fd < 0 ) || ( THR_FDS_NR <= fd ) )
		return -EINVAL;
	
	f = current->fds[fd];
	if ( f == NULL )
		return -EBADF;

	
	fd = alloc_new_fd(f);
	if( fd < 0 )
		return fd;

	fdtable_duplicate_fd(f, &new);

	return fd;
}

int
sys_read(int fd, char *dst, size_t count){
	int             rc;
	file_descriptor *f;
	size_t counts;
	io_cnt_t rdcnt;
	char *p;

	if ( ( fd < 0 ) || ( THR_FDS_NR <= fd ) )
		return -EINVAL;
	
	f = current->fds[fd];
	if ( f == NULL )
		return -EBADF;

	rc = fd_file_read(f, dst, counts, &rdcnt);
	return (rc == 0) ?  (rdcnt) : (rc);
}

int
sys_write(int fd, char *src, size_t count){
	int             rc;
	file_descriptor *f;
	size_t counts;
	io_cnt_t wrcnt;
	char *p;

	if ( ( fd < 0 ) || ( THR_FDS_NR <= fd ) )
		return -EINVAL;
	
	f = current->fds[fd];
	if ( f == NULL )
		return -EBADF;

	rc = fd_file_write(f, src, counts, &wrcnt);
	return (rc == 0) ?  (wrcnt) : (rc);
}

int
sys_close(int fd){
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
sys_open(char *path, omode_t omode){
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
