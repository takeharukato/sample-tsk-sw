top=..
include ${top}/Makefile.inc
CFLAGS += -I${top}/include
objects=thread.o threadmgr.o ready_queue.o sched.o idle.o wqueue.o message.o reaper.o \
	pconsole.o


lib=libkern.a

all:${lib} main.o

${lib}:${objects}
	${AR} ${ARFLAGS} $@ $^
	${RANLIB} $@
clean:
	${RM} *.o ${lib}

distclean:clean
	${RM} *~
