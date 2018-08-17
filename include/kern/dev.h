/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Device switch                                                     */
/*                                                                    */
/**********************************************************************/

#if !defined(_KERN_DEV_H)
#define  _KERN_DEV_H 

#include <kern/freestanding.h>
#include <kern/kern_types.h>

#define DEVSW_DEVICES_NR          (8)

#define DEVSW_DRIVER_STATE_NONE   (0)
#define DEVSW_DRIVER_STATE_RUN    (1)
#define DEVSW_DRIVER_STATE_UNREG  (2)

typedef struct _device_driver{
	mutex                    mtx;
	dev_id                   dev;
	uint32_t               state;
	ref_cnt            drv_count;
	void                *private;
	int (*open)(struct _file_descriptor *_f);
	off_t (*read)(struct _inode *_ip, struct _file_descriptor *_f, void *_dst, off_t _off, size_t _counts);
	off_t (*write)(struct _inode *_ip, struct _file_descriptor *_f, void *_src, off_t _off, size_t _counts);
	int (*blkrw)(struct _device_driver *drv, struct _blk_buf *_b);
	void (*close)(struct _inode *_ip, struct _file_descriptor *_f);
}device_driver;

typedef struct _devsw{
	mutex                           mtx;
	device_driver drv[DEVSW_DEVICES_NR];
}devsw;

int register_device_driver(dev_id _dev, struct _device_driver *_drv, void *_private);
int unregister_device_driver(dev_id _dev);
struct _device_driver *devsw_get_handle(dev_id _dev);
void devsw_put_handle(dev_id _dev);
void devsw_init(void);
#endif  /*  _KERN_DEV_H   */
