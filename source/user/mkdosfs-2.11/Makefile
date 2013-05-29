EXEC = mkdosfs

all: $(EXEC)

mkdosfs: mkdosfs.o
	$(CC) $(LDFLAGS) $^ -o $@

romfs: $(EXEC)
	$(ROMFSINST) /sbin/$(EXEC)

clean :
	echo *.o *.i *.s *~ \#*# core .#* .new*
	rm -f $(EXEC) *.o *.i *.s *~ \#*# core .#* .new*

