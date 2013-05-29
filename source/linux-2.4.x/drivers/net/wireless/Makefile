#
# drivers/net/wireless/Makefile
#
# Makefile for the Linux Wireless network device drivers.
#

O_TARGET := wireless_net.o

obj-y		:=
obj-m		:=
obj-n		:=
obj-		:=

# Things that need to export symbols
export-objs	:= airo.o orinoco.o hermes.o atmel.o

obj-$(CONFIG_HERMES)		+= orinoco.o hermes.o
obj-$(CONFIG_PCMCIA_HERMES)	+= orinoco_cs.o
obj-$(CONFIG_APPLE_AIRPORT)	+= airport.o
obj-$(CONFIG_PLX_HERMES)	+= orinoco_plx.o
obj-$(CONFIG_TMD_HERMES)	+= orinoco_tmd.o
obj-$(CONFIG_PCI_HERMES)	+= orinoco_pci.o

obj-$(CONFIG_AIRO)		+= airo.o
obj-$(CONFIG_AIRO_CS)		+= airo_cs.o airo.o
obj-$(CONFIG_PCMCIA_ATMEL)	+= atmel_cs.o atmel.o

ifeq ($(CONFIG_PRISM54),y)
obj-$(CONFIG_PRISM54)		+= prism54/prism54.o
endif
subdir-$(CONFIG_PRISM54)	+= prism54

ifeq ($(CONFIG_RT2880v2_INIC_MII),y)
obj-$(CONFIG_RT2880v2_INIC_MII)    += iNIC/mii/iNIC_mii.o
endif
subdir-$(CONFIG_RT2880v2_INIC_MII) += iNIC/mii

ifeq ($(CONFIG_RT2880v2_INIC_PCI),y)
obj-$(CONFIG_RT2880v2_INIC_PCI)    += iNIC/pci/iNIC_pci.o
endif
subdir-$(CONFIG_RT2880v2_INIC_PCI) += iNIC/pci


ifeq ($(CONFIG_RT2860V2_AP),y)
obj-$(CONFIG_RT2860V2_AP)	+= rt2860v2_ap/rt2860v2_ap.o
endif
subdir-$(CONFIG_RT2860V2_AP)	+= rt2860v2_ap

ifeq ($(CONFIG_RT2860V2_STA),y)
obj-$(CONFIG_RT2860V2_STA)	+= rt2860v2_sta/rt2860v2_sta.o
endif
subdir-$(CONFIG_RT2860V2_STA)	+= rt2860v2_sta

ifeq ($(CONFIG_RT2561_AP),y)
obj-$(CONFIG_RT2561_AP)		+= rt2561_ap/rt2561ap.o
endif
subdir-$(CONFIG_RT2561_AP)	+= rt2561_ap

#ifeq ($(CONFIG_RT2680_AP),y)
#obj-$(CONFIG_RT2680_AP)	+= rt2680_ap/rt2680ap.o
#endif
#subdir-$(CONFIG_RT2680_AP)	+= rt2680_ap

include $(TOPDIR)/Rules.make
