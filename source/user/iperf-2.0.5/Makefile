SUBDIRS = compat src


all:
	for d in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$d all; done

romfs:
	for d in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$d romfs; done

clean:
	for d in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$d clean; done

