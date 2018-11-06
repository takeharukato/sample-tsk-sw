/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2018 Takeharu KATO                                      */
/*                                                                    */
/*  File system                                                       */
/*                                                                    */
/*  The following codes are derived from xv6.                         */
/*  https://pdos.csail.mit.edu/6.828/2016/xv6.html                    */
/*  I might replace these codes in the future.                        */
/**********************************************************************/

#include <kern/kernel.h>

inode_cache icache;

void
read_superblock(dev_id dev, superblock *sb){
    blk_buf *b;

    b = buffer_cache_blk_read(dev, SUPER_BLOCK_BLK_NO);
    memmove(sb, &b->data[0], sizeof(superblock));
    buffer_cache_blk_release(b);
}

static int 
get_free_inode(dev_id dev, uint32_t *inop) {
	int64_t  i;
	blk_no blk;
	blk_buf *b;
	int64_t offset;
	int64_t chunk_index;
	int64_t chunk_offset;
	uint64_t val;
	superblock  sb;

	read_superblock(dev, &sb);

	for(i = 0; sb.s_ninodes > i; ++i ) {

		blk = (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR) + (i / BITS_PER_BLOCK);

		b = buffer_cache_blk_read(dev, blk);

		offset = i % BITS_PER_BLOCK;
		chunk_index = offset / sizeof(uint64_t);
		chunk_offset = offset % sizeof(uint64_t);
		val = *(( (uint64_t *)(&b->data[0]) ) + chunk_index);

		if ( !( val & ( 1ULL << chunk_offset ) ) ) {

			*(( (uint64_t *)(&b->data[0]) ) + chunk_index) |=
				( 1ULL << chunk_offset );
			*inop = (uint32_t)i;
			buffer_cache_blk_write(b);
			buffer_cache_blk_release(b);
			return 0;
		}
		buffer_cache_blk_release(b);
	}

	return ENOENT;
}

static int 
find_free_data_block(dev_id dev, blk_buf **bp) {
	int64_t            i;
	superblock        sb;
	blk_buf           *b;
	blk_buf          *db;
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

			*(( (uint64_t *)(&b->data[0]) ) + chunk_index) |= 
				( 1ULL << chunk_offset );
			buffer_cache_blk_write(b);
			buffer_cache_blk_release(b);

			db = buffer_cache_blk_read(dev, sb.s_firstdata_block+i);
			memset(&db->data[0], 0, BSIZE);
			*bp = db;
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
	buffer_cache_blk_write(b);
	buffer_cache_blk_release(b);

	return 0;
}

/** Read disk i-node and returns busy buffer
 */
static int
read_disk_inode(dev_id dev, uint32_t ino, d_inode **dinodep, blk_buf  **bp) {
	superblock        sb;
	blk_no           blk;
	blk_buf           *b;
	int            index;

	read_superblock(dev, &sb);

	blk = (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR + sb.s_imap_blocks + sb.s_bmap_blocks) +
		(ino / INODES_PER_BLOCK);
	index = ino % INODES_PER_BLOCK;

	b = buffer_cache_blk_read(dev, blk);
	*dinodep = ( (d_inode *)(&b->data[0]) ) + index;
	*bp = b;

	return 0;
}

/** Find the inode by an i-node number
    and increase the i-node counter.
 */
static inode* 
inode_get (dev_id dev, uint32_t inum){
	int             i;
	inode *ip, *empty;

	mutex_hold(&icache.mtx);

	for (i = 0, empty = NULL; NINODE > i; ++i) {

		/* Search in the inode cache */
		ip = &icache.inode[i];
		if ( ( ip->ref > 0 ) && ( ip->i_dev == dev ) && ( ip->inum == inum ) ) {
			
			++ip->ref;  /* Found */
			goto out;
		}

		if ( ( empty == NULL ) && ( ip->ref == 0 ) ) 
			empty = ip;    /*  First empty slot.  */
	}
	
	kassert( empty != NULL );

	ip = empty;
	ip->i_dev = dev;
	ip->inum = inum;
	ip->ref = 1;
	ip->flags = 0;
out:
	mutex_release(&icache.mtx);
	return ip;
}

static void
bmap(inode *ip, blk_no i_addr, blk_no *blkp){
	int                rc;
	blk_buf           *bp;	
	blk_buf          *nbp;	
	blk_no      blk, *idx;

	/*
	 * Direct blocks
	 */
	if ( i_addr < FS_IADDR_DIRECT_NR ) {

		blk = ip->i_addr[i_addr];

		if ( blk == 0 ) {
			
			rc = find_free_data_block(ip->i_dev, &bp);
			kassert( rc == 0 );
			ip->i_addr[i_addr] = bp->blockno;
			*blkp = bp->blockno;
			buffer_cache_blk_release(bp);
		} else
			*blkp = blk;
		goto out;
	} 

	/*
	 * Indirect blocks
	 */
	i_addr -= FS_IADDR_DIRECT_NR;
	if ( i_addr < ( FS_IADDR_IN_DIRECT_NR * IADDRS_PER_BLOCK ) ) {

		blk = ip->i_addr[(i_addr/IADDRS_PER_BLOCK) + FS_IADDR_IN_DIRECT_MIN];
		if ( blk == 0 ) {

			/*
			 * Allocate a new index block
			 */
			rc = find_free_data_block(ip->i_dev, &nbp);
			kassert( rc == 0 );
			ip->i_addr[i_addr] = nbp->blockno;
			blk = nbp->blockno;
			buffer_cache_blk_release(nbp);
		}

		/* We do not hold the index block.
		 * We should hold it here.
		 */
		bp = buffer_cache_blk_read(ip->i_dev, blk);
		idx = (blk_no *)(&bp->data[0]);
		if ( idx[i_addr] == 0 ) {  

			/*
			 * Allocate new data block.
			 */
			rc = find_free_data_block(ip->i_dev, &nbp);
			kassert( rc == 0 );
			idx[i_addr] = nbp->blockno;
			*blkp = nbp->blockno;
			buffer_cache_blk_release(nbp);
		} else 
			*blkp = idx[i_addr];
		buffer_cache_blk_release(bp);
		goto out;
	}
	
out:
    return;
}

/** Free file data blocks
 *
 */
static void
inode_truncate(inode *ip){
	int              i, j;
	blk_buf           *bp;
	blk_no      blk, *idx;

	/* 
	 * Free direct blocks
	 */
	for (i = 0; FS_IADDR_DIRECT_NR > i; ++i) {

		if ( ip->i_addr[i] != 0 ) {
			
			release_data_block(ip->i_dev, ip->i_addr[i]);
			ip->i_addr[i] = 0;
		}
	}

	/* 
	 * Free indirect blocks
	 */
	for( i = 0; FS_IADDR_IN_DIRECT_NR > i; ++i) {
		
		blk = ip->i_addr[ FS_IADDR_IN_DIRECT_MIN + i ];
		if ( blk != 0 ) {

			bp = buffer_cache_blk_read(ip->i_dev, blk);
			idx = (blk_no *)(&bp->data[0]);

			for(j = 0; IADDRS_PER_BLOCK > j; ++j) {

				if ( idx[j] != 0 ) {

					release_data_block(ip->i_dev, idx[j]);
					idx[j] = 0;
				}
			}
			buffer_cache_blk_release(bp);
		}
		release_data_block(ip->i_dev, blk);
		ip->i_addr[ FS_IADDR_IN_DIRECT_MIN + i ] = 0;
	}

    ip->i_size = 0;
    inode_update(ip);

    return;
}

static char *
skip_element(char *path, char *name){
    char *s;
    int len;

    /*
     * Skip delimiters
     */
    while ( *path == '/' ) 
	    ++path;

    if ( *path == '\0' ) 
        return NULL;

    s = path;  /* s indicates the start of an element of path */

    while ( ( *path != '/' ) && ( *path != '\0' ) ) 
	    ++path;
    
    len = path - s; /* path indicates the end ('/' or '\0') of an element of path */

    if (len >= FNAME_MAX) 
	    len = FNAME_MAX;
    memmove(name, s, len);
    name[len] = '\0';

    while ( *path == '/' ) 
	    ++path;

    return path;
}

static inode *
inode_path2inode(char *path, int nameiparent, char *name) {
	inode *ip, *next;

	kassert( *path == '/' );

	if ( *path == '/' ) {
		ip = inode_get(ROOT_DEV, ROOT_DENT_INO);
	} else {
	
		//ip = inode_duplicate(current->cwd);
		panic("cwd not supported.");
	}

	path = skip_element(path, name);
	while ( path != NULL ) {
		
		inode_lock(ip);
		
		if ( ip->i_mode != FS_IMODE_DIR ) {
			
			inode_unlock(ip);
			inode_put(ip);
			return NULL;
		}

		if ( (nameiparent != 0) && (*path == '\0') ) {

			// Stop one level early.
			inode_unlock(ip);
			return ip;
		}

		next = inode_dirlookup(ip, name, 0);
		if ( next == NULL ) {
			
			inode_unlock(ip);
			inode_put(ip);
			return NULL;
		}
		
		inode_unlock(ip);
		inode_put(ip);
		
		ip = next;
		path = skip_element(path, name);
	}
	
	if ( nameiparent != 0 ) {
		
		inode_put(ip);
		return NULL;
	}

	return ip;
}

/** Sync i-node between a memory inode and a disk inode.
 */
void
inode_update(inode *ip){
	int        i;
	blk_buf  *bp;
	d_inode *dip;

	read_disk_inode(ip->i_dev, ip->inum, &dip, &bp);

	dip->i_mode = ip->i_mode;
	dip->i_dev = ip->i_dev;
	dip->i_rdev = ip->i_rdev;
	dip->i_nlink = ip->i_nlink;
	dip->i_size = ip->i_size;

	for(i = 0; FS_IADDR_NR > i; ++i) 
		dip->i_addr[i] = ip->i_addr[i];

	buffer_cache_blk_write(bp);
	buffer_cache_blk_release(bp);
}

/** Decrement inode reference by an i-node number
 */
void
inode_put(inode *ip){

	mutex_hold(&icache.mtx);
	if ( ( ip->ref == 1 ) && ( ip->flags & I_VALID ) && ( ip->i_nlink == 0 ) ){

		kassert( !(ip->flags & I_BUSY) );

		/*
		 * i-node has no links.
		 * Free file data blocks and then free inode.
		 */
		ip->flags |= I_BUSY;
		mutex_release(&icache.mtx);

		inode_truncate(ip);  /* Free data blocks */
		inode_update(ip);

		mutex_hold(&icache.mtx);
		ip->flags = 0;
		wque_wakeup(&ip->waiters, WQUE_REASON_WAKEUP);
	}

	--ip->ref;
	mutex_release(&icache.mtx);
}

inode *
inode_allocate(dev_id dev, imode_t mode){
	uint32_t  inum;
	blk_buf    *bp;
	d_inode   *dip;
	int         rc;
	superblock  sb;

	read_superblock(dev, &sb);

	rc = get_free_inode(dev, &inum);
	if ( rc != 0 )
		return NULL;
	
	read_disk_inode(dev, inum, &dip, &bp);
	memset(dip, 0, sizeof(d_inode));
	dip->i_mode = mode;
	buffer_cache_blk_write(bp);
	buffer_cache_blk_release(bp);

	return inode_get(dev, inum);
}
/** Duplicate i-node 
 */
inode *
inode_duplicate(inode *ip) {

	mutex_hold(&icache.mtx);
	++ip->ref;
	mutex_release(&icache.mtx);

	return ip;
}

/** lock inode 
 */
void
inode_lock(inode *ip){
	blk_no           i;
	superblock      sb;
	blk_buf        *bp;
	d_inode       *dip;
	wq_reason   reason;

	kassert( ip->ref > 0 );

	mutex_hold(&icache.mtx);

	/*
	 * Wait on i-node and lock it
	 */
	while( ip->flags & I_BUSY ) {

		reason = wque_wait_on_event_with_mutex(&ip->waiters, &icache.mtx);
		kassert( reason == WQUE_REASON_WAKEUP );
	}

	ip->flags |= I_BUSY;  /* Note: We should hold icache mutex here */

	/*
	 * Re-read inode from disk
	 */
	if ( !( ip->flags & I_VALID ) ) {

		read_superblock(ip->i_dev, &sb);	
		read_disk_inode(ip->i_dev, ip->inum, &dip, &bp);

		/*
		 * Note: Do not read i_dev from disk inode.
		 * i_dev is updated by inode_get.
		 */
		ip->i_mode = dip->i_mode;
		ip->i_rdev = dip->i_rdev;
		ip->i_nlink = dip->i_nlink;
		ip->i_size = dip->i_size;

		for(i = 0; FS_IADDR_NR > i; ++i) 
			ip->i_addr[i] = dip->i_addr[i];

		buffer_cache_blk_release(bp);
		ip->flags |= I_VALID;
	}
	
	mutex_release(&icache.mtx);
}

/** Unlock inode
 */ 
void
inode_unlock(inode *ip){

	mutex_hold(&icache.mtx);

	kassert( ip->flags & I_BUSY );

	ip->flags &= ~I_BUSY;
	wque_wakeup(&ip->waiters, WQUE_REASON_WAKEUP);
	
	mutex_release(&icache.mtx);
}

off_t
inode_read(inode *ip, void *dst, off_t off, size_t counts) {
	blk_buf           *bp;
	size_t        remains;
	size_t       rd_bytes;
	blk_no            blk;

	kassert( ip->i_mode != FS_IMODE_DEV );
	
	if ( ( off > ip->i_size ) || ( off + counts < off) ) 
		return -ENXIO;

	if ( ( off + counts ) > ip->i_size ) {

		counts = ip->i_size - off;
	}

	for(remains = counts; remains > 0; ) {

		blk = 0;
		bmap(ip, off / BSIZE, &blk);
		bp = buffer_cache_blk_read(ip->i_dev, blk);
	    
		rd_bytes = remains;
		if ( remains > ( BSIZE - ( off % BSIZE ) ) )
			rd_bytes = ( BSIZE - ( off % BSIZE ) );

		kassert( remains >= rd_bytes );

		memmove(dst, (void *)&bp->data[0] + ( off % BSIZE ), rd_bytes);
		buffer_cache_blk_release(bp);
		
		remains -= rd_bytes;
		off += rd_bytes;
		dst += rd_bytes;
	}

	return counts;
}

off_t
inode_write(inode *ip, void *src, off_t off, size_t counts) {
	blk_buf           *bp;
	size_t        remains;
	size_t       wr_bytes;
	blk_no            blk;

	kassert( ip->i_mode != FS_IMODE_DEV );

	if ( ( off > ip->i_size ) || ( off + counts < off) ) 
		return -ENXIO;

	if ( ( off + counts ) > FS_MAXFILE_SIZE ) 
		return -ENXIO;

	for(remains = counts; remains > 0; ) {
		
		blk = 0;
		bmap(ip, off / BSIZE, &blk);
		bp = buffer_cache_blk_read(ip->i_dev, blk);
	    
		wr_bytes = remains;
		if ( remains > ( BSIZE - ( off % BSIZE ) ) )
			wr_bytes = ( BSIZE - ( off % BSIZE ) );

		kassert( remains >= wr_bytes );

		memmove((void *)&bp->data[0] + ( off % BSIZE ), src, wr_bytes);

		buffer_cache_blk_write(bp);
		buffer_cache_blk_release(bp);
		
		remains -= wr_bytes;
		off += wr_bytes;
		src += wr_bytes;
	}

	if ( (counts > remains ) && ( off > ip->i_size) ) {

		ip->i_size = off;
		inode_update(ip);
	}

	return counts;
}

void
inode_get_stat(struct _inode *ip, struct _stat *st){

	st->mode = ip->i_mode;
	st->dev = ip->i_dev;
	st->rdev = ip->i_rdev;
	st->ino = ip->inum;
	st->nlink = ip->i_nlink;
	st->size = ip->i_size;
}

/** Look up directory entry
 */
inode *
inode_dirlookup(inode *dp, char *name, dirent *ent){
	off_t       off;
	d_dirent     de;
	off_t  rd_bytes;

	kassert( dp->i_mode == FS_IMODE_DIR );

	for(off = 0; dp->i_size > off; off += sizeof(d_dirent)){

		rd_bytes = inode_read(dp, (void *)&de, off, sizeof(de));
		kassert( rd_bytes == sizeof(de) );

		if( de.d_ino == 0 )
			continue;

		if ( strncmp(name, de.d_name, FNAME_MAX) == 0 ) {

			/* Found the name element */
			if ( ent != NULL )
				ent->d_off = off;

			return inode_get(dp->i_dev, de.d_ino);
		}
	}

	return NULL;
}

int
inode_add_directory_entry(inode *dp, char *name, uint32_t inum){
	d_dirent     de;
	inode       *ip;
	off_t       off;
	off_t  rd_bytes;

	/*
	 *  Check that name is not present.
	 */
	ip = inode_dirlookup(dp, name, NULL);
	if ( ip != NULL ) {

		inode_put(ip);
		return -EEXIST;
	}
	
	/*
	 *  Look up an empty dirent.
	 */
	for (off = 0; dp->i_size > off; off += sizeof(d_dirent) ) {

		rd_bytes = inode_read(dp, (void *)&de, off, sizeof(d_dirent));
		kassert( rd_bytes == sizeof(d_dirent ) );

		if ( de.d_ino == 0 ) 
			break;
	}

	de.d_ino = inum;
	strncpy(de.d_name, name, FNAME_MAX);

	/*
	 * Write back this directory entry
	 */
	rd_bytes = inode_write(dp, (void *) &de, off, sizeof(d_dirent));
	kassert( rd_bytes == sizeof(d_dirent) );

	return 0;
}

inode *
inode_namei(char *path) {
	char name[FNAME_MAX+1];

	return inode_path2inode(path, 0, name);
}

inode *
inode_nameiparent(char *path, char *name) {

	return inode_path2inode(path, 1, name);
}

void
inode_cache_init(void) {
	int i;

	mutex_init(&icache.mtx);
	for(i = 0; NINODE > i; ++i) {

		memset(&icache.inode[i], 0, sizeof(inode));
		wque_init_wait_queue(&(icache.inode[i].waiters));
	}
}
