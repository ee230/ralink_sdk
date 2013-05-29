EXES = ntfsfix mkntfs ntfslabel ntfsinfo ntfsundelete ntfsresize ntfsclone \
       ntfscluster ntfsls ntfscat ntfscp ntfsck ntfscmp

CFLAGS += -DHAVE_CONFIG_H -I../include
LDFLAGS += -L../libntfs
LDLIBS += -lntfs

ntfsfix: ntfsfix.o utils.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

mkntfs: attrdef.o boot.o sd.o mkntfs.o utils.o
	$(CC) -o $@ $(LDFLAGS) $(LDLIBS) $^

ntfslabel: ntfslabel.o utils.o
	$(CC) -o $@ $(LDFLAGS) $(LDLIBS) $^

ntfsinfo: ntfsinfo.o utils.o
	$(CC) -o $@ $(LDFLAGS) $(LDLIBS) $^

ntfsundelete: ntfsundelete.o utils.o
	$(CC) -o $@ $(LDFLAGS) $(LDLIBS) $^

ntfsresize: ntfsresize.o utils.o
	$(CC) -o $@ $(LDFLAGS) $(LDLIBS) $^

ntfsclone: ntfsclone.o utils.o
	$(CC) -o $@ $(LDFLAGS) $(LDLIBS) $^

ntfscluster: ntfscluster.o cluster.o utils.o
	$(CC) -o $@ $(LDFLAGS) $(LDLIBS) $^

ntfsls: ntfsls.o utils.o
	$(CC) -o $@ $(LDFLAGS) $(LDLIBS) $^

ntfscat: ntfscat.o utils.o
	$(CC) -o $@ $(LDFLAGS) $(LDLIBS) $^

ntfscp: ntfscp.o utils.o
	$(CC) -o $@ $(LDFLAGS) $(LDLIBS) $^

ntfsck: ntfsck.o utils.o
	$(CC) -o $@ $(LDFLAGS) $(LDLIBS) $^

# libfuse is needed
# ntfsmount: ntfsmount.o utils.o
#         $(CC) -o $@ $(LDFLAGS) $(LDLIBS) $^

ntfscmp: ntfscmp.o utils.o
	$(CC) -o $@ $(LDFLAGS) $(LDLIBS) $^

all: $(EXES)

romfs:
	for i in $(EXES) ; do $(ROMFSINST) /usr/bin/$$i ; done

clean:
	rm -rf $(EXES) *.o

