ifeq ($(CONFIG_USER_PPPD),y)
    SUBDIRS += pppd pppd/plugins
endif
 
ifeq ($(CONFIG_USER_PPPD_PPPCHAT),y)
   SUBDIRS += chat
endif

ifeq ($(CONFIG_USER_PPPD_PPPSTATS),y)
    SUBDIRS += pppstats
endif

ifeq ($(CONFIG_USER_PPPD_PPPDUMP),y)
    SUBDIRS += pppdump
endif


all: 
	for d in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$d all; done

romfs:
	for d in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$d romfs; done

clean:
	rm -f `find . -name '*.[oas]' -print`
	rm -f `find . -name 'core' -print`
	rm -f `find . -name '*~' -print`
	for d in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$d clean; done
