VPATH = src
CFLAGS += -O3 -pipe -mips32 -mtune=mips32 -Wall -Wno-unused -s -fpack-struct -Wpadded  -D__LITTLE_ENDIAN
LD= $(CC)
OS_LAYER = osl-linux.c

all: lld2d lld2test

include src/common.mk

depend:
	$(CC) $(CFLAGS) -M $(DCFILES) >.depend

clear:
	rm -f *.o lld2d lld2test

-include .depend
