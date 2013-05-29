O_TARGET := raeth.o

obj-y    := ra_mac.o raether.o mcast.o
obj-m    := $(O_TARGET)

ifeq ($(CONFIG_RAETH_ROUTER),y)
obj-y += ra_ethtool.o
else
ifeq ($(CONFIG_RT_3052_ESW),y)
obj-y += ra_ethtool.o
endif
endif

ifeq ($(CONFIG_RAETH_NETLINK),y)
obj-y += ra_netlink.o
endif

ifeq ($(CONFIG_RAETH_NAPI),y)
EXTRA_CFLAGS   += -DCONFIG_RAETH_NAPI
endif

ifeq ($(CONFIG_RT2880_BRIDGING_ONLY),y)
EXTRA_CFLAGS   += -DCONFIG_RT2880_BRIDGING_ONLY
endif

ifeq ($(CONFIG_RT2880_ENHANCE),y)
EXTRA_CFLAGS += -DCONFIG_RT2880_ENHANCE
endif

ifeq ($(CONFIG_RAETH_JUMBOFRAME),y)
EXTRA_CFLAGS += -DCONFIG_RAETH_JUMBOFRAME
endif

export-objs := raether.o ra_mac.o

include $(TOPDIR)/Rules.make 
