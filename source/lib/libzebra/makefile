TOPDIR=../
include $(TOPDIR)Rules.mak


CFLAGS= -c -DHAVE_CONFIG_H -DSYSCONFDIR=\"/var/zebra/\" -I. -I.. -I../lib -Wall
LDFLAGS=

LIB_LDFLAGS = -shared

SOURCES=buffer.c distribute.c hash.c log.c pid_output.c print_version.c sockopt.c table.c vty.c \
		checksum.c filter.c if.c md5.c plist.c sockunion.c thread.c zclient.c \
		command.c getopt1.c keychain.c memory.c pqueue.c routemap.c str.c vector.c \
		daemon.c getopt.c linklist.c network.c prefix.c smux.c stream.c version.c 


OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=libzebra.a

# zebra library
LIBZEBRA_A= libzebra.a
LIBZEBRA_SHARED=libzebra.so
LIBZEBRA_SHARED_FULLNAME=libzebra-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(AR) cru $@ $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@


shared: all
#zebra
	$(LD) $(LIB_LDFLAGS) -soname=$(LIBZEBRA_SHARED).$(MAJOR_VERSION) \
           -o $(LIBZEBRA_SHARED_FULLNAME) --whole-archive $(LIBZEBRA_A) \
           --no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
           -L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);

	$(INSTALL) -d $(TOPDIR)lib
	$(RM) -f $(TOPDIR)lib/$(LIBZEBRA_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBZEBRA_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBZEBRA_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBZEBRA_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBZEBRA_SHARED)
	$(LN) -sf $(LIBZEBRA_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBZEBRA_SHARED).$(MAJOR_VERSION)
	$(LN) -sf $(LIBZEBRA_SHARED_FULLNAME) $(LIBZEBRA_SHARED)
	$(LN) -sf $(LIBZEBRA_SHARED_FULLNAME) $(LIBZEBRA_SHARED).$(MAJOR_VERSION)

clean:
	rm -f *.o *.a *.so $(LIBZEBRA_SHARED).$(MAJOR_VERSION) $(LIBZEBRA_SHARED)
	find ./ -name Makefile -exec rm -f {} \;



