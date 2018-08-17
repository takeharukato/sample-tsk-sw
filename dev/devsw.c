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

	if ( ( drvp->dev > 0 ) || ( drvp->drv_count > 0 ) ) {

		rc = EBUSY;
		goto error_out;
	}
	
	drvp->dev = dev;
	drvp->drv_count = 0;

	drvp->private = drv->private;

	drvp->open = drv->open;
	drvp->read = drv->read;
	drvp->write = drv->write;
	drvp->blkrw = drv->blkrw;
	drvp->close = drv->close;
	drvp->state = DEVSW_DRIVER_STATE_RUN;

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

	++drvp->drv_count;  /* To avoid the race with register_device_driver */

	drvp->state |= DEVSW_DRIVER_STATE_UNREG;
	drvp->state &= ~DEVSW_DRIVER_STATE_RUN;

	mutex_release(&dsp->mtx);
	do{
		mutex_hold(&dsp->mtx);

		if ( drvp->drv_count == 1 ) 
			break;  /* No one uses this device */

		mutex_release(&dsp->mtx);
	}while(1);

	kassert( drvp->drv_count == 1 );

	drvp->dev = 0;
	drvp->drv_count = 0;
	drvp->private = NULL;
	drvp->open = NULL;
	drvp->read = NULL;
	drvp->write = NULL;
	drvp->blkrw = NULL;
	drvp->close = NULL;

	drvp->state &= ~DEVSW_DRIVER_STATE_UNREG;
	kassert( drvp->state == DEVSW_DRIVER_STATE_NONE );

	rc = 0;

error_out:	
	mutex_release(&dsp->mtx);

	return rc;
}

device_driver *
devsw_get_handle(dev_id dev){
	devsw          *dsp = &devices;
	device_driver            *drvp;

	kassert( DEVSW_DEVICES_NR >= dev );

	mutex_hold(&dsp->mtx);

	drvp = &dsp->drv[dev];

	if ( (drvp->state & DEVSW_DRIVER_STATE_RUN) == 0 ) 
		goto error_out;
	++drvp->drv_count;

error_out:
	mutex_release(&dsp->mtx);

	return drvp;
}

void
devsw_put_handle(dev_id dev){
	devsw          *dsp = &devices;
	device_driver            *drvp;

	kassert( DEVSW_DEVICES_NR >= dev );

	mutex_hold(&dsp->mtx);

	drvp = &dsp->drv[dev];

	kassert( drvp->drv_count > 0 );
	--drvp->drv_count;

	mutex_release(&dsp->mtx);

	return;
}

void
devsw_init(void) {
	int                          i;
	devsw          *dsp = &devices;
	device_driver            *drvp;

	for(i = 0; DEVSW_DEVICES_NR > i; ++i) {

		mutex_init(&dsp->mtx);

		drvp = &dsp->drv[i];

		mutex_init(&drvp->mtx);

		drvp->dev = 0;
		drvp->private = NULL;
		drvp->open = NULL;
		drvp->read = NULL;
		drvp->write = NULL;
		drvp->blkrw = NULL;
		drvp->close = NULL;
	}
}

