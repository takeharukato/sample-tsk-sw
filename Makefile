top=.
include Makefile.inc
targets=kernel
headers=param.h list.h	heap.h chunk.h malloc.h 

subdirs=common kern hal user
cleandirs=include ${subdirs} tools configs
kernlibs = user/libuser.a kern/libkern.a common/libcommon.a hal/hal/libhal.a
mconf=tools/kconfig/mconf

all:${targets}

start_obj =
ifeq ($(CONFIG_HAL),y)
start_obj += hal/hal/boot.o
endif
start_obj += kern/main.o

${mconf}:
	${MAKE} -C tools

menuconfig:configs/Config.in ${mconf}
	${RM} include/autoconf.h
	${mconf} configs/Config.in || :


include/autoconf.h: .config
	${RM} -f $@
	tools/kconfig/conf-header.sh .config > $@

${targets}:include/autoconf.h subsystem
ifeq ($(CONFIG_HAL),y)
	${CC} ${CFLAGS} -nostdlib  -T hal/hal/kernel.lds  -o $@.elf ${start_obj} ${kernlibs}
	${CP} $@.elf $@.tmp
	${STRIP} $@.tmp
	${OBJCOPY} -O binary $@.tmp $@
	${RM} $@.tmp
else
	${CC} ${CFLAGS} -o $@ ${start_obj} ${kernlibs}
endif
hal:
	${MAKE} -C include hal
	${MAKE} -C hal hal

subsystem: hal
	for dir in ${subdirs} ; do \
	${MAKE} -C $${dir} ;\
	done

clean:
	for dir in ${cleandirs} ; do \
	${MAKE} -C $${dir} clean ;\
	done
	${RM} *.o ${targets} *.tmp *.elf

distclean:clean
	for dir in ${cleandirs} ; do \
	${MAKE} -C $${dir} distclean ;\
	done
	${RM}  *~ .config* _config
