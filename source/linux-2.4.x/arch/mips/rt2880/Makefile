#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#

USE_STANDARD_AS_RULE := true
all: rt2880.o

O_TARGET:= rt2880.o

obj-y				:= mipsIRQ.o reset.o display.o init.o irq.o \
				   memory.o printf.o cmdline.o setup.o time.o
obj-$(CONFIG_PCI)		+= pci.o
obj-$(CONFIG_KGDB)		+= gdb_hook.o

ifdef CONFIG_RALINK_TIMER
obj-$(CONFIG_RALINK_TIMER) +=  rt_timer.o
export-objs +=  rt_timer.o
endif

#--

include $(TOPDIR)/Rules.make

