/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Device switcher                                                   */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

static devsw devices;

int
register_device_driver(dev_id dev, device_driver *drv, void *private){
	int                         rc;
	devsw          *dsp = &devices;
	device_driver            *drvp;

	if ( dev >= DEVSW_DEVICES_NR )
		return EINVAL;

	drvp = &dsp->drv[dev];

	mutex_hold(&dsp->mtx);

	if ( drvp->dev > 0 ) {

		rc = EBUSY;
		goto error_out;
	}
	
	drvp->dev = drv->dev;
	drvp->private = drv->private;

	drvp->open = drv->open;
	drvp->read = drv->read;
	drvp->write = drv->write;
	drvp->blkrw = drv->blkrw;
	drvp->close = drv->close;

	rc = 0;

error_out:
	mutex_release(&dsp->mtx);

	return rc;
}

int
unregister_device_driver(dev_id dev){
	int                         rc;
	devsw          *dsp = &devices;
	device_driver            *drvp;

	if ( dev >= DEVSW_DEVICES_NR )
		return EINVAL;

	drvp = &dsp->drv[dev];

	mutex_hold(&dsp->mtx);

	if ( drvp->dev == 0 ) {

		rc = ENOENT;
		goto error_out;
	}

	drvp->dev = 0;
	drvp->private = NULL;
	drvp->open = NULL;
	drvp->read = NULL;
	drvp->write = NULL;
	drvp->blkrw = NULL;
	drvp->close = NULL;

	rc = 0;

error_out:	
	mutex_release(&dsp->mtx);

	return rc;
}

void
devsw_init(void) {
	int                          i;
	devsw          *dsp = &devices;
	device_driver            *drvp;

	for(i = 0; DEVSW_DEVICES_NR > i; ++i) {

		mutex_init(&dsp->mtx);

		drvp = &dsp->drv[i];

		drvp->dev = 0;
		drvp->private = NULL;
		drvp->open = NULL;
		drvp->read = NULL;
		drvp->write = NULL;
		drvp->blkrw = NULL;
		drvp->close = NULL;
	}
}

