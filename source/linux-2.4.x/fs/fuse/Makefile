O_TARGET := fuse.o
VERSION = 2.5.3
EXTRA_CFLAGS += -DFUSE_VERSION=\"$(VERSION)\"

obj-y  := dev.o dir.o file.o inode.o compat/parser.o
obj-m := $(O_TARGET)

include $(TOPDIR)/Rules.make
