TARGET = libcap.a
CFLAGS += -DHAVE_CONFIG_H -DLINUX -I../include
OBJS = cap_alloc.o cap_proc.o cap_extint.o cap_flag.o cap_text.o cap_sys.o

all: $(OBJS)
	$(AR) cr $(TARGET) $(OBJS)
romfs:

clean:
	rm -f $(TARGET) *.o

