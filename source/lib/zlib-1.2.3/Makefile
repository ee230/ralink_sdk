TOPDIR=../
include $(TOPDIR)Rules.mak

MAJOR_VERSION=1
MINOR_VERSION=2
SUBLEVEL=3
LIBZ=libz.a
LIBZ_SHARED=libz.so
LIBZ_SHARED_FULLNAME=libz-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

CFLAGS+=$(SSP_ALL_CFLAGS)

OBJS = adler32.o crc32.o infback.o inflate.o minigzip.o uncompr.o \
	compress.o  deflate.o  gzio.o inffast.o inftrees.o trees.o zutil.o

OBJA =

all: $(LIBZ)

$(LIBZ) ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBZ) $(OBJS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBZ)
	$(INSTALL) -m 644 $(LIBZ) $(TOPDIR)lib/

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

shared: all
	$(LD) $(LDFLAGS) -soname=$(LIBZ_SHARED).$(MAJOR_VERSION) \
                -o $(LIBZ_SHARED_FULLNAME) --whole-archive $(LIBZ) \
                --no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
                -L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBZ_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBZ_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBZ_SHARED_FULLNAME) $(TOPDIR)lib
	$(INSTALL) -m 644 *.h $(TOPDIR)include
	$(LN) -sf $(LIBZ_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBZ_SHARED)
	$(LN) -sf $(LIBZ_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBZ_SHARED).$(MAJOR_VERSION)

clean:
	 $(RM) *.[oa] *~ core $(LIBZ_SHARED)* $(LIBZ_SHARED_FULLNAME)*
