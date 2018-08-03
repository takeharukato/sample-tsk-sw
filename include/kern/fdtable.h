/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  File descriptor table                                             */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_FDTABLE_H)
#define  _KERN_FDTABLE_H

#include <kern/kern_types.h>

#define MAX_FD_TABLE_SIZE (1024)
#define FREAD             (0x1)
#define FWRITE            (0x2)

struct _inode;

typedef struct _file_descriptor{
	rw_flags       f_flags;
	ref_cnt        f_count;
	struct _inode *f_inode;
	off_t         f_offset;
}file_descriptor;

typedef struct _fd_table{
	mutex                               mtx;
	file_descriptor file[MAX_FD_TABLE_SIZE];
}fd_table;

void fdtable_init(void);
int fdtable_alloc_new_fd(file_descriptor **_fdp);
int fdtable_duplicate_fd(file_descriptor  *_f, file_descriptor **_fdp);
int fd_file_read(file_descriptor  *_f, void *_addr, size_t _n, io_cnt_t *_rd_cnt);
int fd_file_write(file_descriptor  *_f, void *_addr, size_t _n, io_cnt_t *_wr_cnt);
void fdtable_close_fd(file_descriptor  *_f);
#endif  /*  _KERN_FDTABLE_H   */
