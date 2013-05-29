
MAKEFILE = 

all:
	cd zebra && make $(MAKEFILE)
	cd ripd && make $(MAKEFILE)
#	cd ospfd && make $(MAKEFILE)

clean:
	cd zebra && make $(MAKEFILE) clean
	cd ripd && make $(MAKEFILE) clean
#	cd ospfd && make $(MAKEFILE) clean
	rm -f Makefile

romfs:
	cd zebra && make $(MAKEFILE) romfs
	cd ripd && make $(MAKEFILE) romfs
#	cd ospfd && make $(MAKEFILE) romfs
