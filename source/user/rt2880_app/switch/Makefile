EXEC = switch  
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth
CONF_H  = $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h

ifeq ($(CONFIG_RALINK_RT6855),y)
SRC=switch_gsw.c
else
ifeq ($(CONFIG_RALINK_RT6352),y)
SRC=switch_gsw.c
else
ifeq ($(CONFIG_RALINK_RT71100),y)
SRC=switch_gsw.c
else
ifeq ($(CONFIG_RALINK_RT6855A),y)
SRC=switch_gsw.c
else
SRC=switch.c
endif
endif
endif
endif

all: $(EXEC)
switch: $(SRC) $(CONF_H)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC) $(LDLIBS)

romfs:
	$(ROMFSINST) /bin/switch

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

