TARGET = disktype

OBJS   = main.o lib.o \
         buffer.o file.o cdaccess.o cdimage.o vpc.o compressed.o \
         detect.o apple.o amiga.o atari.o dos.o cdrom.o \
         linux.o unix.o beos.o archives.o \
         udf.o blank.o cloop.o

CPPFLAGS = -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -DUSE_IOCTL_LINUX
CFLAGS   = -Wall

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

$(OBJS): %.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	$(RM) *.o *~ *% $(TARGET)
	
romfs:
	$(ROMFSINST) /bin/$(TARGET)

