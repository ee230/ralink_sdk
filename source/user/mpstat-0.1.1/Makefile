# makefile for mpstat
# this file was hand generated ;-)

OPTIMIZATIONS = -O3
CFLAGS =  -Wall -Wno-unused -g -Wl,-warn-common -D__2_4__
SMPFLAGS = -D__SMP__
TARGET = mpstat
FILES = functions.c mpstat.c
	
all:
	$(CC) $(OPTIMIZATIONS) $(INCLUDES) $(CFLAGS) $(FILES) -o $(TARGET)

romfs:
	$(ROMFSINST) /bin/$(TARGET)

clean:
	rm -f core mpstat *.o
