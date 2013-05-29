CFLAGS=-DHAVE_CONFIG_H -Icommon -I$(ROOTDIR)/lib/libbluetooth/include -Wall
LDFLAGS += -L$(ROOTDIR)/lib/libbluetooth -Lcommon
LDLIBS += -lbluetooth -lhelper
LIBS = libhelper.a
EXEC = bccmd hciconfig avinfo ciptool hciattach hcisecfilter hcitool l2ping ppporc sdptool


all: $(LIBS) $(EXEC)

libhelper.a:
	$(MAKE) -C common

bccmd: csr.o  csr_hci.o csr_bcsp.o csr_h4.o csr_3wire.o ubcsp.o

hciconfig: csr.o hciconfig.o

hciattach: hciattach.o hciattach_tialt.o hciattach_st.o hciattach_ti.o 

clean:
	$(MAKE) -C common clean
	rm -fr *.o hciconfig avinfo ciptool hciattach hcisecfilter hcitool l2ping ppporc sdptool

romfs: 
	$(ROMFSINST) -e CONFIG_USER_BT_AVINFO /sbin/avinfo
	$(ROMFSINST) -e CONFIG_USER_BT_BCCMD /sbin/bccmd
	$(ROMFSINST) -e CONFIG_USER_BT_CIPTOOL /sbin/ciptool
	$(ROMFSINST) -e CONFIG_USER_BT_HCICONFIG /sbin/hciconfig
	$(ROMFSINST) -e CONFIG_USER_BT_HCIATTACH /sbin/hciattach
	$(ROMFSINST) -e CONFIG_USER_BT_HCISECFILTER /sbin/hcisecfilter
	$(ROMFSINST) -e CONFIG_USER_BT_HCITOOL /sbin/hcitool
	$(ROMFSINST) -e CONFIG_USER_BT_L2PING /sbin/l2ping
	$(ROMFSINST) -e CONFIG_USER_BT_PPPORC /sbin/ppporc
	$(ROMFSINST) -e CONFIG_USER_BT_SDPTOOL /sbin/sdptool

