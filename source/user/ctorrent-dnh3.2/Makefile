EXE = ctorrent
OBJS = bencode.o bitfield.o btconfig.o btcontent.o btfiles.o btrequest.o \
       btstream.o bufio.o compat.o connect_nonb.o console.o ctcs.o ctorrent.o \
       downloader.o httpencode.o iplist.o peer.o peerlist.o rate.o \
       setnonblock.o sigint.o tracker.o sha1.o

all: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJS)
	$(STRIP) --remove-section=.note --remove-section=.comment $(EXE)

.c.o:
	$(CC) -c $<

.cpp.o:
	$(CXX) -c -o $@ $<

romfs:
	$(ROMFSINST) /bin/$(EXE)

clean:
	@rm -rf $(EXE) $(OBJS)


