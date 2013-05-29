# If you want to debug daemon, add following line
CFLAGS +=  -DDBG 

OBJS =	dhry21a.o dhry21b.o timers.o

all: dhrystone

dhrystone: $(OBJS)
	$(CC) -o dhrystone $(OBJS)

romfs:
	$(ROMFSINST) /bin/dhrystone

clean:
	rm -f dhrystone *.elf *.gdb *.o
