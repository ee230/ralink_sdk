TOPDIR=../
include $(TOPDIR)Rules.mak

CFLAGS+=$(SSP_ALL_CFLAGS) -DHAVE_CONFIG_H -Iinclude/

LIBNTFS3G=libntfs-3g.a
LIBNTFS3G_VERS=26
LIBNTFS3G_SHARED=libntfs-3g.so.$(LIBNTFS3G_VERS)

OBJS    := attrib.o bitmap.o collate.o compress.o device.o dir.o inode.o \
	   logfile.o mft.o mst.o security.o unix_io.o volume.o attrlist.o \
	   bootsect.o compat.o debug.o index.o lcnalloc.o logging.o misc.o \
	   runlist.o unistr.o version.o

all:	$(LIBNTFS3G)

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

shared: all
	$(LD) $(LDFLAGS) -soname=$(LIBNTFS3G_SHARED) \
		-o $(LIBNTFS3G_SHARED) --whole-archive $(LIBNTFS3G) \
		--no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
		-L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBNTFS3G_SHARED)
	$(INSTALL) -m 644 $(LIBNTFS3G_SHARED) $(TOPDIR)lib

$(LIBNTFS3G) ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBNTFS3G) $(OBJS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBNTFS3G)
	$(INSTALL) -m 644 $(LIBNTFS3G) $(TOPDIR)lib

romfs:
	$(INSTALL) -m 644 $(LIBNTFS3G_SHARED) $(TOPDIR)lib

$(OBJS): Makefile

clean:
	$(RM) *.[oa] $(LIBNTFS3G_SHARED)*

