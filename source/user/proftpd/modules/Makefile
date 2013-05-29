OBJS = mod_core.o mod_xfer.o mod_auth_unix.o mod_auth_file.o mod_auth.o mod_ls.o mod_log.o mod_site.o \
       mod_delay.o  mod_cap.o  module_glue.o 

CFLAGS += -DHAVE_CONFIG_H -DLINUX -I../include

all: $(OBJS)

romfs:

clean:
	rm -f *.o

