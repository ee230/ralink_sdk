TOPDIR=../../
include $(TOPDIR)Rules.mak

CFLAGS+=-DHAVE_CONFIG_H -Iinc -I$(ROOTDIR)/lib/include

LIBTHREADUTIL=libthreadutil.a
LIBTHREADUTIL_VERS=1.3.1
LIBTHREADUTIL_SHARED=libthreadutil.so.$(LIBTHREADUTIL_VERS)

OBJS    := src/FreeList.o  src/iasnprintf.o  src/LinkedList.o  src/ThreadPool.o  src/TimerThread.o

all: $(LIBTHREADUTIL)

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

shared: all
	$(LD) $(LDFLAGS) -soname=$(LIBTHREADUTIL_SHARED) \
		-o $(LIBTHREADUTIL_SHARED) --whole-archive $(LIBTHREADUTIL) \
		--no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
		-L$(TOPDIR)lib -lc -lpthread $(LDADD_LIBFLOAT) $(LIBGCC);
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBTHREADUTIL_SHARED)
	$(INSTALL) -m 644 $(LIBTHREADUTIL_SHARED) $(TOPDIR)lib


$(LIBTHREADUTIL) ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBTHREADUTIL) $(OBJS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBTHREADUTIL)
	$(INSTALL) -m 644 $(LIBTHREADUTIL) $(TOPDIR)lib

romfs:
	$(INSTALL) -m 644 $(LIBTHREADUTIL_SHARED) $(TOPDIR)lib

$(OBJS): Makefile

clean:
	$(RM) *.[oa] $(LIBTHREADUTIL_SHARED)* 

