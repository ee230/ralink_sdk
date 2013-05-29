

ARPTABLES_VERSION:=0.0.3-3

#CC=mipsel-linux-gcc
KERNEL_DIR:=./
# default paths
PREFIX:=/usr/local
LIBDIR:=$(PREFIX)/lib
BINDIR:=$(PREFIX)/sbin
MANDIR:=$(PREFIX)/man
INITDIR:=/etc/rc.d/init.d
SYSCONFIGDIR:=/etc/sysconfig
DESTDIR:=

COPT_FLAGS:=-O2
CFLAGS:=$(COPT_FLAGS) -Wall -Wunused -I$(KERNEL_DIR)/include/ -Iinclude/ -DARPTABLES_VERSION=\"$(ARPTABLES_VERSION)\" #-g -DDEBUG #-pg # -DARPTC_DEBUG

ifndef ARPT_LIBDIR
ARPT_LIBDIR:=$(LIBDIR)/arptables
endif

include extensions/Makefile


all: arptables

arptables.o: arptables.c
	$(CC) $(CFLAGS) -c -o $@ $<

arptables-standalone.o: arptables-standalone.c
	$(CC) $(CFLAGS) -c -o $@ $<

libarptc/libarptc.o: libarptc/libarptc.c libarptc/libarptc_incl.c
	$(CC) $(CFLAGS) -c -o $@ $<

arptables: arptables-standalone.o arptables.o libarptc/libarptc.o $(EXT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(DESTDIR)$(MANDIR)/man8/arptables.8: arptables.8
	mkdir -p $(@D)
	install -m 0644 -o root -g root $< $@

$(DESTDIR)$(BINDIR)/arptables: arptables
	mkdir -p $(DESTDIR)$(BINDIR)
	install -m 0755 -o root -g root $< $@

tmp1:=$(shell printf $(BINDIR) | sed 's/\//\\\//g')
tmp2:=$(shell printf $(SYSCONFIGDIR) | sed 's/\//\\\//g')
.PHONY: scripts
scripts: arptables-save arptables-restore arptables.sysv
	cat arptables-save | sed 's/__EXEC_PATH__/$(tmp1)/g' > arptables-save_
	install -m 0755 -o root -g root arptables-save_ $(DESTDIR)$(BINDIR)/arptables-save
	cat arptables-restore | sed 's/__EXEC_PATH__/$(tmp1)/g' > arptables-restore_
	install -m 0755 -o root -g root arptables-restore_ $(DESTDIR)$(BINDIR)/arptables-restore
	cat arptables.sysv | sed 's/__EXEC_PATH__/$(tmp1)/g' | sed 's/__SYSCONFIG__/$(tmp2)/g' > arptables.sysv_
	install -m 0755 -o root -g root arptables.sysv_ $(DESTDIR)$(INITDIR)/arptables
	rm -f arptables-save_ arptables-restore_ arptables.sysv_

.PHONY: install
install: $(DESTDIR)$(MANDIR)/man8/arptables.8 $(DESTDIR)$(BINDIR)/arptables scripts

.PHONY: clean
clean:
	rm -f arptables
	rm -f *.o *~
	rm -f extensions/*.o extensions/*~
	rm -f libarptc/*.o libarptc/*~
	rm -f include/*~ include/libarptc/*~

DIR:=arptables-v$(ARPTABLES_VERSION)
CVSDIRS:=CVS extensions/CVS libarptc/CVS include/CVS include/libarptc/CVS
# This is used to make a new userspace release
.PHONY: release
release:
	rm -rf $(CVSDIRS)
	mkdir -p include/linux/netfilter_arp
	install -m 0644 -o root -g root \
		$(KERNEL_DIR)/include/linux/netfilter_arp.h include/linux/
	install -m 0644 -o root -g root \
		$(KERNEL_DIR)/include/linux/netfilter_arp/*.h \
		include/linux/netfilter_arp/
	install -m 0644 -o root -g root \
		include/arp_tables.h include/linux/netfilter_arp/arp_tables.h
	make clean
	cd ..;tar -c $(DIR) | gzip >$(DIR).tar.gz

romfs:
	$(ROMFSINST) arptables /bin/arptables
         