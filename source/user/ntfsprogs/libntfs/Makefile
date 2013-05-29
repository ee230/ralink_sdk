OBJS = attrib.o bootsect.o compress.o debug.o dir.o index.o logfile.o misc.o \
       security.o version.o attrlist.o collate.o device.o inode.o logging.o \
       mst.o unistr.o volume.o bitmap.o compat.o crypto.o device_io.o \
       lcnalloc.o mft.o runlist.o

CFLAGS = -DHAVE_CONFIG_H -I../include

LTVERSION_LIBNTFS = 10:0:0

all: $(OBJS)
	$(AR) cru libntfs.a $^
	$(CC) -shared -o libntfs.so -Wl,-soname,libntfs.so -Wl,-s $^

romfs:
	$(ROMFSINST) /lib/libntfs.so

clean:
	rm -f $(OBJS) libntfs.so libntfs.a

