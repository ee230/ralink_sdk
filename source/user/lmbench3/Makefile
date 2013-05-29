# Makefile for top level of lmbench
# $Id: Makefile,v 1.2 2006-11-24 04:15:50 steven Exp $

# Possible things to $(MAKE):
#
# build		(default) go to the source directory and build the benchmark
# results	go to the source directory and build and run the benchmark
# rerun		run the benchmark again
# see		see the results that came with this release
#		Go to the results directory and read the Makefile.
# doc.lpr	print the documentation
# doc.x		preview the documentation (needs X, groff, pic, etc)
# clean		go to the subdirs and $(MAKE) clean
# get		$(MAKE) sure all files are checked out
# shar		build a shippable shar archive

SHELL=/bin/sh

build: 
	cd src && $(MAKE)

results: FRC
	cd src && $(MAKE) results

rerun: 
	cd src && $(MAKE) rerun

see:
	cd results && $(MAKE) summary percent 2>/dev/null | more

doc.lpr:
	cd doc && $(MAKE) PS && lpr *.PS

doc.x:
	cd doc && $(MAKE) x

clobber clean: 
	for i in doc src results scripts; do \
		echo ===== $$i =====; \
		(cd $$i && $(MAKE) clean); \
	done
	/bin/rm -rf bin/*
#	-bk clean 

get: 
	for i in doc src results scripts; do \
		echo ===== $$i =====; \
		(cd $$i && bk get -q); \
	done
	@co -q

info: 
	for i in doc src results scripts; do \
		echo ===== $$i =====; \
		(cd $$i && info); \
	done

release: scripts/mkrelease
	scripts/mkrelease

scripts/mkrelease:
	cd scripts && co mkrelease

# XXX - . must be named lmbench for this to work
shar:
	$(MAKE) clean
	co -q Makefile
	$(MAKE) get
	cd .. && \
	find lmbench -type f -print  | egrep -v 'noship|RCS' > /tmp/FILES
	cd .. && shar -S -a -n lmbench1.0 -L 50K < /tmp/FILES 

romfs:
#	$(ROMFSINST) bin/mipsel-linux/lmbench /bin/lmbench
	$(ROMFSINST) scripts/net_test /bin/net_test
	$(ROMFSINST) scripts/mem_test /bin/mem_test

#	$(ROMFSINST) bin/mipsel-linux/mhz /bin/mhz
#	$(ROMFSINST) bin/mipsel-linux/enough /bin/enough
#	$(ROMFSINST) bin/mipsel-linux/msleep /bin/msleep
#	$(ROMFSINST) bin/mipsel-linux/par_mem /bin/parmem

	$(ROMFSINST) bin/mipsel-linux/bw_mem /bin/bw_mem
	$(ROMFSINST) bin/mipsel-linux/bw_tcp /bin/bw_tcp
#	$(ROMFSINST) bin/mipsel-linux/bw_file_rd /bin/bw_file_rd
#	$(ROMFSINST) bin/mipsel-linux/bw_mmap_rd /bin/bw_mmap_rd
#	$(ROMFSINST) bin/mipsel-linux/bw_pipe /bin/bw_pipe
#	$(ROMFSINST) bin/mipsel-linux/bw_unix /bin/bw_unix
	
	$(ROMFSINST) bin/mipsel-linux/lat_tcp /bin/lat_tcp
	$(ROMFSINST) bin/mipsel-linux/lat_udp /bin/lat_udp
#	$(ROMFSINST) bin/mipsel-linux/lat_fs /bin/lat_fs
#	$(ROMFSINST) bin/mipsel-linux/lat_proc /bin/lat_proc
#	$(ROMFSINST) bin/mipsel-linux/lat_http /bin/lat_http
#	$(ROMFSINST) bin/mipsel-linux/lat_rpc /bin/lat_rpc
#	$(ROMFSINST) bin/mipsel-linux/lat_select /bin/lat_select
#	$(ROMFSINST) bin/mipsel-linux/lat_connect /bin/lat_connect
#	$(ROMFSINST) bin/mipsel-linux/lat_mmap /bin/lat_mmap
#	$(ROMFSINST) bin/mipsel-linux/lat_sem /bin/lat_sem
#	$(ROMFSINST) bin/mipsel-linux/lat_unix_connect /bin/lat_unix_connect
#	$(ROMFSINST) bin/mipsel-linux/lat_ctx /bin/lat_ctx
#	$(ROMFSINST) bin/mipsel-linux/lat_fcntl /bin/lat_fcntl
#	$(ROMFSINST) bin/mipsel-linux/lat_pagefault /bin/lat_pagefault
#	$(ROMFSINST) bin/mipsel-linux/lat_syscall /bin/lat_syscall
#	$(ROMFSINST) bin/mipsel-linux/lat_fifo /bin/lat_fifo
#	$(ROMFSINST) bin/mipsel-linux/lat_ops /bin/lat_ops
#	$(ROMFSINST) bin/mipsel-linux/lat_sig /bin/lat_sig
#	$(ROMFSINST) bin/mipsel-linux/lat_mem_rd /bin/lat_mem_rd

FRC:
