TARGET = libsupp.a
CFLAGS += -DHAVE_CONFIG_H -DLINUX -I../include

OBJS = pr_fnmatch.o sstrncpy.o strsep.o vsnprintf.o glibc-glob.o glibc-hstrerror.o glibc-mkstemp.o pr-syslog.o pwgrent.o
       
all: cap $(OBJS)
	$(AR) cr $(TARGET) $(OBJS)

cap:
	$(MAKE) -C libcap

romfs:

clean:
	$(MAKE) -C libcap clean
	rm -f $(TARGET) *.o

