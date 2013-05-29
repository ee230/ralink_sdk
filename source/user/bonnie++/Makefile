EXES = bonnie++ zcav
CXXFLAGS = -O2 -DNDEBUG -Wall -W -Wshadow -Wpointer-arith -Wwrite-strings -pedantic -ffor-scope

all: bonnie zcav

bonnie: bonnie++.o bon_io.o bon_file.o bon_time.o semaphore.o forkit.o bon_suid.o
	$(CXX) -o bonnie++ $^

zcav: zcav.o bon_suid.o
	$(CXX) -o $@ $^

romfs:
	for i in $(EXES) ; do $(ROMFSINST) /sbin/$$i ; done

clean:
	rm -f *.o $(EXES)



