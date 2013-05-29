EXEC = cpu
OBJS = cpu.o

all: $(EXEC)

.c.o:
	$(CC) -c $(CFLAGS) $<

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@

romfs:
	$(ROMFSINST) /bin/cpu

clean:
	-rm -f $(EXEC) *.elf *.o

