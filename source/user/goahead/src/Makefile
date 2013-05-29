#
# Makefile for the GoAhead web server reference source base
#  for the uClinux OS
#
# Copyright (c) GoAhead Software, Inc. 1995-2000
#
# $Id: Makefile,v 1.69.2.5 2012-03-30 12:31:45 chhung Exp $
#

NAME	= goahead

# User Management switch
UMSW	= -DUSER_MANAGEMENT_SUPPORT

ifeq ("$(CONFIG_RT_FIRST_CARD)", "2860")
FIRST_CARD=$(CONFIG_RT_FIRST_CARD)V2
else
FIRST_CARD=$(CONFIG_RT_FIRST_CARD)
endif
FIRST_WSC=CONFIG_RT$(FIRST_CARD)_AP_WSC
SECOND_WSC=CONFIG_RT$(CONFIG_RT_SECOND_CARD)_AP_WSC
FIRST_WSC_V2=CONFIG_RT$(FIRST_CARD)_AP_WSC_V2
SECOND_WSC_V2=CONFIG_RT$(CONFIG_RT_SECOND_CARD)_AP_WSC_V2
FIRST_WDS=CONFIG_RT$(FIRST_CARD)_AP_WDS
SECOND_WDS=CONFIG_RT$(CONFIG_RT_SECOND_CARD)_AP_WDS
FIRST_MBSS=CONFIG_RT$(FIRST_CARD)_AP_MBSS
SECOND_MBSS=CONFIG_RT$(CONFIG_RT_SECOND_CARD)_AP_MBSS
FIRST_NEW_MBSS=CONFIG_RT$(FIRST_CARD)_AP_NEW_MBSS_MODE
#SECOND_NEW_MBSS=CONFIG_RT$(CONFIG_RT_SECOND_CARD)_AP_NEW_MBSS_MODE
FIRST_IGMP_SNOOP=CONFIG_RT$(FIRST_CARD)_AP_IGMP_SNOOP
SECOND_IGMP_SNOOP=CONFIG_RT$(CONFIG_RT_SECOND_CARD)_AP_IGMP_SNOOP
FIRST_DFS=CONFIG_RT$(FIRST_CARD)_AP_DFS
SECOND_DFS=CONFIG_RT$(CONFIG_RT_SECOND_CARD)_AP_DFS
FIRST_CARRIER=CONFIG_RT$(FIRST_CARD)_AP_CARRIER
SECOND_CARRIER=CONFIG_RT$(CONFIG_RT_SECOND_CARD)_AP_CARRIER
FIRST_DLS=CONFIG_RT$(FIRST_CARD)_AP_DLS
SECOND_DLS=CONFIG_RT$(CONFIG_RT_SECOND_CARD)_AP_DLS
ifeq ("$(CONFIG_RT_FIRST_CARD)", "2860")
FIRST_80211N_DRAFT3=CONFIG_RT$(FIRST_CARD)_AP_80211N_DRAFT3
else
FIRST_80211N_DRAFT3=CONFIG_RT$(FIRST_CARD)_80211N_DRAFT3
endif
SECOND_80211N_DRAFT3=CONFIG_RT$(CONFIG_RT_SECOND_CARD)_80211N_DRAFT3
FIRST_WAPI=CONFIG_RT$(FIRST_CARD)_AP_WAPI
SECOND_WAPI=CONFIG_RT$(CONFIG_RT_SECOND_CARD)_AP_WAPI
FIRST_APCLI=CONFIG_RT$(FIRST_CARD)_AP_APCLI

# Digest Access switch
# DASW	= -DDIGEST_ACCESS_SUPPORT

# SSL switches
ifeq ("$(CONFIG_USER_GOAHEAD_SSL)", "y")
SSLPATCHFILE = matrix_ssl.o sslSocket.o
MATRIXDIR = $(ROOTDIR)/user/matrixssl-1.8.3
SSLINC = $(MATRIXDIR)
SSLLIB = $(MATRIXDIR)/src/libmatrixsslstatic.a
SSLSW = -DWEBS_SSL_SUPPORT -DMATRIX_SSL -I$(SSLINC)
endif

# If-Modified-Support switches (requires math library, libm.a)
# IFMODSW = -DWEBS_IF_MODIFIED_SUPPORT
# IFMODLIB = /usr/lib/libm.a

# Dependencies
DEPEND_FILES	= asp.o balloc.o base64.o cgi.o default.o  \
				  ejlex.o ejparse.o form.o \
				  h.o handler.o mime.o misc.o page.o \
				  ringq.o rom.o \
				  sock.o sockGen.o $(SSLPATCHFILE) \
				  security.o sym.o uemf.o url.o value.o \
				  md5c.o um.o umui.o websda.o emfdb.o \
				  webrom.o webs.o websuemf.o \
				  internet.o utils.o wireless.o \
				  firewall.o management.o $(NAME).o
ifeq ("$(CONFIG_UNIQUE_WPS)", "y")
DEPEND_FILES += wps_unique.o
else
DEPEND_FILES += wps.o
endif
ifneq ("$(CONFIG_RT2860V2_STA)", "")
DEPEND_FILES += station.o
endif
ifneq ("$(CONFIG_RTDEV)", "")
DEPEND_FILES += inic.o
ifneq ("$(CONFIG_UNIQUE_WPS)", "y")
DEPEND_FILES += wps_inic.o
endif
CFLAGS += -DRTDEV_SUPPORT
endif
ifneq ("$(CONFIG_RT2561_AP)", "")
DEPEND_FILES += legacy.o
endif
ifeq ("$(CONFIG_USER_GOAHEAD_IPV6)", "y")
CFLAGS += -DWF_USE_IPV6
endif
ifeq ("$(CONFIG_USER_GOAHEAD_HOSTNAME)", "y")
CFLAGS += -DGA_HOSTNAME_SUPPORT
endif
ifeq ("$(CONFIG_USER_WPA_SUPPLICANT)", "y")
ifeq ("$(CONFIG_RT2860V2_STA_WPA_SUPPLICANT)", "y")
CFLAGS += -DWPA_SUPPLICANT_SUPPORT
endif
endif
ifneq ("$(CONFIG_USB)", "")
DEPEND_FILES += usb.o
endif
ifeq ("$(CONFIG_RALINKAPP_MPLAYER)", "y")
DEPEND_FILES += media.o
endif
ifeq ("$(CONFIG_RALINKAPP_SWQOS)", "y")
DEPEND_FILES += qos.o
endif

ifneq ("$($(FIRST_WSC))", "")
CFLAGS += -DRT2860_WSC_SUPPORT
ifneq ("$($(FIRST_WSC_V2))", "")
CFLAGS += -DRT2860_WSCV2_SUPPORT
endif
endif
ifneq ("$($(SECOND_WSC))", "")
CFLAGS += -DRTDEV_WSC_SUPPORT
ifneq ("$($(SECOND_WSC_V2))", "")
CFLAGS += -DRTDEV_WSCV2_SUPPORT
endif
endif
ifneq ($(findstring y, $(CONFIG_RTPCI_AP_WSC)$(CONFIG_RTUSB_AP_WSC)),)
CFLAGS += -DRT2860_WSC_SUPPORT
CFLAGS += -DRTDEV_WSC_SUPPORT
ifneq ($(findstring y, $(CONFIG_RTPCI_AP_WSC_V2)$(CONFIG_RTUSB_AP_WSC_V2)),)
CFLAGS += -DRT2860_WSCV2_SUPPORT
CFLAGS += -DRTDEV_WSCV2_SUPPORT
endif
endif

ifneq ("$($(FIRST_WDS))", "")
CFLAGS += -DRT2860_WDS_SUPPORT
endif
ifneq ("$($(SECOND_WDS))", "")
CFLAGS += -DRTDEV_WDS_SUPPORT
endif
ifneq ($(findstring y, $(CONFIG_RTPCI_AP_WDS)$(CONFIG_RTUSB_AP_WDS)),)
CFLAGS += -DRT2860_WDS_SUPPORT
CFLAGS += -DRTDEV_WDS_SUPPORT
endif

ifneq ("$($(FIRST_MBSS))", "")
CFLAGS += -DRT2860_MBSS_SUPPORT
ifneq ($(findstring y, $($(FIRST_NEW_MBSS))$(CONFIG_16MBSSID_MOD)),)
CFLAGS += -DRT2860_NEW_MBSS_SUPPORT
endif
endif
ifneq ("$($(SECOND_MBSS))", "")
CFLAGS += -DRTDEV_MBSS_SUPPORT
#ifneq ("$($(SECOND_NEW_MBSS))", "")
#CFLAGS += -DRTDEV_NEW_MBSS_SUPPORT
#endif
endif
ifneq ($(findstring y, $(CONFIG_RTPCI_AP_MBSS)$(CONFIG_RTUSB_AP_MBSS)),)
CFLAGS += -DRT2860_MBSS_SUPPORT
CFLAGS += -DRTDEV_MBSS_SUPPORT
ifneq ($(findstring y, $(CONFIG_RTPCI_AP_NEW_MBSS_MODE)$(CONFIG_RTUSB_AP_NEW_MBSS_MODE)),)
CFLAGS += -DRT2860_NEW_MBSS_SUPPORT
#CFLAGS += -DRTDEV_NEW_MBSS_SUPPORT
endif
endif

ifneq ("$($(FIRST_IGMP_SNOOP))", "")
CFLAGS += -DRT2860_IGMPSNOOP_SUPPORT
endif
ifneq ("$($(SECOND_IGMP_SNOOP))", "")
CFLAGS += -DRTDEV_IGMPSNOOP_SUPPORT
endif
ifneq ($(findstring y, $(CONFIG_RTPCI_AP_IGMP_SNOOP)$(CONFIG_RTUSB_AP_IGMP_SNOOP)),)
CFLAGS += -DRT2860_IGMPSNOOP_SUPPORT
CFLAGS += -DRTDEV_IGMPSNOOP_SUPPORT
endif

ifneq ("$($(FIRST_DFS))", "")
CFLAGS += -DRT2860_DFS_SUPPORT
endif
ifneq ("$($(SECOND_DFS))", "")
CFLAGS += -DRTDEV_DFS_SUPPORT
endif
ifneq ($(findstring y, $(CONFIG_RTPCI_AP_DFS)$(CONFIG_RTUSB_AP_DFS)),)
CFLAGS += -DRT2860_DFS_SUPPORT
CFLAGS += -DRTDEV_DFS_SUPPORT
endif

ifneq ("$($(FIRST_CARRIER))", "")
CFLAGS += -DRT2860_CARRIER_SUPPORT
endif
ifneq ("$($(SECOND_CARRIER))", "")
CFLAGS += -DRTDEV_CARRIER_SUPPORT
endif
ifneq ($(findstring y, $(CONFIG_RTPCI_AP_CARRIER)$(CONFIG_RTUSB_AP_CARRIER)),)
CFLAGS += -DRT2860_CARRIER_SUPPORT
CFLAGS += -DRTDEV_CARRIER_SUPPORT
endif

ifneq ("$($(FIRST_DLS))", "")
CFLAGS += -DRT2860_DLS_SUPPORT
endif
ifneq ("$($(SECOND_DLS))", "")
CFLAGS += -DRTDEV_DLS_SUPPORT
endif
ifneq ($(findstring y, $(CONFIG_RTPCI_AP_DLS)$(CONFIG_RTUSB_AP_DLS)),)
CFLAGS += -DRT2860_DLS_SUPPORT
CFLAGS += -DRTDEV_DLS_SUPPORT
endif

ifneq ("$($(FIRST_80211N_DRAFT3))", "")
CFLAGS += -DRT2860_11NDRAFT3_SUPPORT
endif
ifneq ("$($(SECOND_80211N_DRAFT3))", "")
CFLAGS += -DRTDEV_11NDRAFT3_SUPPORT
endif
ifneq ($(findstring y, $(CONFIG_RTPCI_80211N_DRAFT3)$(CONFIG_RTUSB_80211N_DRAFT3)),)
CFLAGS += -DRT2860_11NDRAFT3_SUPPORT
CFLAGS += -DRTDEV_11NDRAFT3_SUPPORT
endif

ifeq ("$(CONFIG_RALINKAPP_WAPI)", "y")
ifneq ("$($(FIRST_WAPI))", "")
CFLAGS += -DRT2860_WAPI_SUPPORT
endif
ifneq ("$($(SECOND_WAPI))", "")
CFLAGS += -DRTDEV_WAPI_SUPPORT
endif
endif
ifneq ($(findstring y, $(CONFIG_RTPCI_AP_WAPI)$(CONFIG_RTUSB_AP_WAPI)),)
CFLAGS += -DRT2860_WAPI_SUPPORT
CFLAGS += -DRTDEV_WAPI_SUPPORT
endif

ifneq ("$($(FIRST_APCLI))", "")
CFLAGS += -DRT2860_APCLI_SUPPORT
endif
ifneq ($(findstring y, $(CONFIG_RTPCI_AP_APCLI)$(CONFIG_RTUSB_AP_APCLI)),)
CFLAGS += -DRT2860_APCLI_SUPPORT
endif

ifneq ($(findstring y, $(CONFIG_RALINK_RT2883)$(CONFIG_RALINK_RT3883)),)
CFLAGS += -DRT2860_ABAND_SUPPORT
endif
ifneq ($(findstring $(CONFIG_RT_FIRST_CARD), 5592 3593 3572 5572),)
CFLAGS += -DRT2860_ABAND_SUPPORT
endif
ifneq ($(findstring $(CONFIG_RT_SECOND_CARD), 5592 3593 3572 5572),)
CFLAGS += -DRTDEV_ABAND_SUPPORT
endif

ifneq ($(findstring y, $(CONFIG_RALINK_RT2883)$(CONFIG_RALINK_RT3883)),)
CFLAGS += -DRT2860_3T3R_SUPPORT
endif
ifneq ($(findstring $(CONFIG_RT_FIRST_CARD), 3593),)
CFLAGS += -DRT2860_3T3R_SUPPORT
endif
ifneq ($(findstring $(CONFIG_RT_SECOND_CARD), 3593),)
CFLAGS += -DRTDEV_3T3R_SUPPORT
endif
ifneq ($(findstring y, $(CONFIG_RALINK_RT3050_1T1R)$(CONFIG_RALINK_RT5350)),)
CFLAGS += -DRT2860_1T1R_SUPPORT
endif

CFLAGS	+= -DWEBS -DUEMF -DOS="LINUX" -DLINUX $(UMSW) $(DASW) $(SSLSW) $(IFMODSW)
CFLAGS  += -Wall -fno-strict-aliasing
CFLAGS	+= -I$(ROOTDIR)/lib/libnvram -I$(ROOTDIR)/$(LINUXDIR)/drivers/char -I$(ROOTDIR)/$(LINUXDIR)/include
CFLAGS  += -I$(ROOTDIR)/$(LINUXDIR)/drivers/flash 
OTHERS	= -DB_STATS -DB_FILL -DDEBUG
LDFLAGS	+= $(SSLLIB) $(IFMODLIB)
LDLIBS	+= -lnvram

CONF_H	= $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h
UCONF_H	= $(ROOTDIR)/config/autoconf.h
BUSYCONF_H = $(ROOTDIR)/user/busybox/include/autoconf.h

all:	$(NAME)

#
#	Primary link
#
$(NAME): clean_inet $(DEPEND_FILES)
	$(CC) -o $@ $(DEPEND_FILES) $(LDFLAGS) $(EXTRALIBS) $(LDLIBS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

romfs:
	$(ROMFSINST) /bin/$(NAME)
ifeq ("$(CONFIG_USER_GOAHEAD_SSL)", "y")
	$(ROMFSINST) /etc_ro/serverkey.pem
	$(ROMFSINST) /etc_ro/servercert.pem
endif

clean:
	rm -f $(NAME) *.o

clean_inet:
	rm -f internet.o

#
#	Dependencies
#
asp.o:  webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

balloc.o: balloc.c uemf.h

base64.o:  base64.c webs.h wsIntrn.h  ej.h ejIntrn.h uemf.h

cgi.o:  webs.h wsIntrn.h uemf.h

default.o:  default.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h $(CONF_H)

ejlex.o:  ejlex.c ej.h ejIntrn.h uemf.h

ejparse.o:  ejparse.c ej.h ejIntrn.h uemf.h

emfdb.o:  emfdb.h wsIntrn.h uemf.h

firewall.o: firewall.c webs.h firewall.h $(CONF_H) $(UCONF_H)

form.o:  form.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

goahead.o: goahead.c uemf.h wsIntrn.h internet.h utils.h wireless.h firewall.h management.h qos.h $(CONF_H) $(UCONF_H) $(BUSYBOXCONF_H)

h.o:  h.c uemf.h

handler.o:  handler.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

inic.o: inic.c inic.h internet.h utils.h webs.h $(CONF_H)

legacy.o: legacy.c legacy.h internet.h utils.h webs.h $(CONF_H)

internet.o: internet.c internet.h utils.h webs.h  $(CONF_H) $(UCONF_H) $(BUSYBOXCONF_H)

management.o: management.c management.h webs.h $(CONF_H) $(UCONF_H) 

matrix_ssl.o: matrix_ssl.c wsIntrn.h webs.h websSSL.h sslSocket.h

md5c.o:  md5.h wsIntrn.h uemf.h

media.o: media.c media.h webs.h

mime.o:  mime.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

misc.o:  misc.c uemf.h

page.o:  page.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

ringq.o:  ringq.c uemf.h

rom.o:  rom.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

security.o:  security.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h $(CONF_H)

sock.o:  sock.c uemf.h

sockGen.o:  sockGen.c uemf.h $(CONF_H)

sslSocket.o: sslSocket.c sslSocket.h

station.o: station.c station.h oid.h stapriv.h webs.h $(CONF_H) $(UCONF_H)

usb.o: usb.c usb.h webs.h $(UCONF_H)

sym.o:  sym.c uemf.h

uemf.o:  uemf.c uemf.h

um.o:  webs.h wsIntrn.h um.h uemf.h

umui.o:  webs.h wsIntrn.h um.h uemf.h

url.o:  url.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

utils.o: utils.c utils.h webs.h $(CONF_H) $(UCONF_H)

value.o:  value.c uemf.h

webrom.o:  webrom.c webs.h wsIntrn.h uemf.h

webs.o:  webs.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h $(CONF_H)

websda.o:  webs.h wsIntrn.h websda.h uemf.h

websuemf.o:  websuemf.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

websSSL.o:  websSSL.c websSSL.h wsIntrn.h ej.h ejIntrn.h uemf.h

wireless.o: wireless.c wireless.h internet.h utils.h webs.h $(CONF_H) $(UCONF_H)

wps.o: wps.c wps.h utils.h webs.h internet.h wireless.h station.h oid.h $(CONF_H)

wps_inic.o: wps_inic.c wps.h utils.h webs.h internet.h oid.h $(CONF_H)

wps_unique.o: wps_unique.c wps.h utils.h webs.h internet.h oid.h $(CONF_H)

qos.o: qos.h utils.h $(CONF_H)
