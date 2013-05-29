EXE = ntfs-3g 
OBJS = ntfs-3g.o ../libfuse-lite/libfuse-lite.a

CFLAGS += -DHAVE_CONFIG_H -I../include/
CFLAGS += -I$(ROOTDIR)/lib/libntfs-3g/include
LDFLAGS += -L$(ROOTDIR)/lib/libntfs-3g
LDLIBS += -lntfs-3g -lpthread


all: $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(LDFLAGS) $(LDLIBS)

romfs:
	$(ROMFSINST) ntfs-3g /bin/ntfs-3g
	ln -sf $(EXE) $(ROOTDIR)/romfs/bin/mount.ntfs-3g

clean:
	rm -f *.o

