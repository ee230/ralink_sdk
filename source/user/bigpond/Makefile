CFLAGS=-g -O2 -Wall 
LIBS=-lnsl 

OBJS = bpalogin.o protocol.o transaction.o gen_md5.o unixmain.o

bpalogin: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

clean:
	rm -fr *.o bpalogin

romfs: bpalogin
	$(ROMFSINST) /sbin/bpalogin  
	$(ROMFSINST) /sbin/bpalogin.sh
