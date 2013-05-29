TOPDIR=../../
include $(TOPDIR)Rules.mak

CFLAGS+=-DHAVE_CONFIG_H -Iinc -Isrc/inc -I$(ROOTDIR)/lib/include

LIBIXML=libixml.a
LIBIXML_VERS=1.3.1
LIBIXML_SHARED=libixml.so.$(LIBIXML_VERS)

OBJS    := src/attr.o src/element.o  src/ixmlmembuf.o src/namedNodeMap.o src/nodeList.o \
	   src/document.o  src/ixml.o  src/ixmlparser.o  src/node.o


all: $(LIBIXML)

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

shared: all
	$(LD) $(LDFLAGS) -soname=$(LIBIXML_SHARED) \
		-o $(LIBIXML_SHARED) --whole-archive $(LIBIXML) \
		--no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
		-L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBIXML_SHARED)
	$(INSTALL) -m 644 $(LIBIXML_SHARED) $(TOPDIR)lib


$(LIBIXML) ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBIXML) $(OBJS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBIXML)
	$(INSTALL) -m 644 $(LIBIXML) $(TOPDIR)lib

romfs:
	$(INSTALL) -m 644 $(LIBIXML_SHARED) $(TOPDIR)lib

$(OBJS): Makefile

clean:
	$(RM) *.[oa] $(LIBIXML_SHARED)* 

