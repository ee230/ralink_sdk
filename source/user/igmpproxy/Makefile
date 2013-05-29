DIRS = src 

all:
	for i in $(DIRS) ; do make -C $$i $@ || exit $?; done

romfs:
	for i in $(DIRS) ; do make -C $$i $@ ; done

clean:
	for i in $(DIRS) ; do make -C $$i clean ; done

