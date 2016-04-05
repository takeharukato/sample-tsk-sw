CPU ?= `uname -m|sed -e "s/amd64/x86-64/" -e "s/i?86/ia32/"`

CC=${CROSS_COMPILE}gcc
AR=${CROSS_COMPILE}ar
RANLIB=${CROSS_COMPILE}ranlib
STRIP=${CROSS_COMPILE}strip
OBJCOPY=${CROSS_COMPILE}objcopy
CP=cp
RM=rm -f
LN=ln
ARFLAGS=rvc
OPT_FLAGS=-O0 -g
CFLAGS=${OPT_FLAGS}

.PHONY: clean distclean hal tools menuconfig

.SUFFIXES: .o .c .S

.c.o:
	${CC} ${CFLAGS} -c -o $@ $<

.S.o:
	${CC} ${CFLAGS} -c -o $@ $<

-include ${top}/.config

