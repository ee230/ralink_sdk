DIRS = libntfs ntfsprogs

all:
	for i in $(DIRS) ; do make -C $$i $@ || exit $?; done

romfs:
	for i in $(DIRS) ; do make -C $$i $@ || exit $?; done

clean:
	for i in $(DIRS) ; do make -C $$i $@ || exit $?; done

