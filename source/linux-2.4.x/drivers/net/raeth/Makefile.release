O_TARGET := raeth.o

ifeq ($(CONFIG_RT2880_GMAC),y)
RAETH_OBJ = raeth_k.obj
endif

ifeq ($(CONFIG_RT2880_GMAC),m)
RAETH_OBJ = raeth_m.obj
endif

obj-y := $(RAETH_OBJ)
obj-m := $(O_TARGET)
include $(TOPDIR)/Rules.make
