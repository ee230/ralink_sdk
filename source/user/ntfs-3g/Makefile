DIRS = libfuse-lite src

all:
	for i in $(DIRS) ; do $(MAKE) -C $$i $@ || exit $?; done

romfs:
	for i in $(DIRS) ; do $(MAKE) -C $$i $@ || exit $?; done

clean:
	for i in $(DIRS) ; do $(MAKE) -C $$i $@ || exit $?; done


