TARGET = libfuse-lite.a
CFLAGS += -I../include

OBJS = fuse_loop.o fuse_lowlevel.o fuse_mt.o fuse_session.o fusermount.o \
       mount.o fuse.o fuse_kern_chan.o fuse_loop_mt.o fuse_opt.o \
       fuse_signals.o helper.o mount_util.o

all: $(OBJS)
	$(AR) cr $(TARGET) $(OBJS)

romfs:


clean:
	rm -f $(TARGET) *.o

