/*
 * Copyright 2002 Momentum Computer
 * Author: Matthew Dharm <mdharm@momenco.com>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <linux/config.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <asm/pci.h>
#include <asm/io.h>
//#include <asm/mv64340.h>
#include <asm/rt2880/eureka_ep430.h>

#include <linux/init.h>
#include <asm/pci_channel.h>

#ifdef CONFIG_PCI



/*
 * These functions and structures provide the BIOS scan and mapping of the PCI
 * devices.
 */

#define MAX_PCI_DEVS 10
#if 1
#define RT2880_PCI_SLOT1_BASE	0x20000000 //0x00510000 
#define RT2880_PCI_SLOT1_END	RT2880_PCI_SLOT2_BASE -1
#define RT2880_PCI_SLOT2_BASE	0x28000000 //0x00518000 
#define RT2880_PCI_SLOT2_END	0x2fffffff //0x0051ffff 

#define RT2880_PCI_SLOT1_1_BASE	RT2880_PCI_SLOT1_BASE+0x04000000
#define RT2880_PCI_SLOT1_2_BASE	RT2880_PCI_SLOT1_BASE+0x05000000
#define RT2880_PCI_SLOT1_3_BASE	RT2880_PCI_SLOT1_BASE+0x06000000
#define RT2880_PCI_SLOT1_4_BASE	RT2880_PCI_SLOT1_BASE+0x07000000
#define RT2880_PCI_SLOT1_5_BASE	RT2880_PCI_SLOT1_BASE+0x07800000

#define RT2880_PCI_SLOT2_1_BASE	RT2880_PCI_SLOT2_BASE+0x04000000
#define RT2880_PCI_SLOT2_2_BASE	RT2880_PCI_SLOT2_BASE+0x05000000
#define RT2880_PCI_SLOT2_3_BASE	RT2880_PCI_SLOT2_BASE+0x06000000
#define RT2880_PCI_SLOT2_4_BASE	RT2880_PCI_SLOT2_BASE+0x07000000
#define RT2880_PCI_SLOT2_5_BASE	RT2880_PCI_SLOT2_BASE+0x07800000
#else
#define RT2880_PCI_SLOT1_BASE	0x00510000 
#define RT2880_PCI_SLOT1_END	RT2880_PCI_SLOT2_BASE -1
#define RT2880_PCI_SLOT2_BASE	0x00518000 
#define RT2880_PCI_SLOT2_END	0x0051ffff 
#endif

unsigned long mem_pool;
unsigned long io_pool;
void __inline__ read_config(unsigned long bus, unsigned long dev, unsigned long func, unsigned long reg, unsigned long *val);
void __inline__ write_config(unsigned long bus, unsigned long dev, unsigned long func, unsigned long reg, unsigned long val);

/*  Functions to implement "pci ops"  */
static int rt2880_pcibios_read_config_word(struct pci_dev *dev,
					    int offset, u16 * val);
static int rt2880_pcibios_read_config_byte(struct pci_dev *dev,
					    int offset, u8 * val);
static int rt2880_pcibios_read_config_dword(struct pci_dev *dev,
					     int offset, u32 * val);
static int rt2880_pcibios_write_config_byte(struct pci_dev *dev,
					     int offset, u8 val);
static int rt2880_pcibios_write_config_word(struct pci_dev *dev,
					     int offset, u16 val);
static int rt2880_pcibios_write_config_dword(struct pci_dev *dev,
					      int offset, u32 val);

struct pci_fixup pcibios_fixups[] = {
	{PCI_FIXUP_HEADER, PCI_ANY_ID, PCI_ANY_ID, pcibios_fixup_resources },
	{0}
};

/*
 *  General-purpose PCI functions.
 */

struct pci_ops rt2880_pci_ops = {
	rt2880_pcibios_read_config_byte,
	rt2880_pcibios_read_config_word,
	rt2880_pcibios_read_config_dword,
	rt2880_pcibios_write_config_byte,
	rt2880_pcibios_write_config_word,
	rt2880_pcibios_write_config_dword
};


/*
 * pci_range_ck -
 *
 * Check if the pci device that are trying to access does really exists
 * on the evaluation board.  
 *
 * Inputs :
 * bus - bus number (0 for PCI 0 ; 1 for PCI 1)
 * dev - number of device on the specific pci bus
 *
 * Outpus :
 * 0 - if OK , 1 - if failure
 */
static __inline__ int pci_range_ck(unsigned char bus, unsigned char dev)
{
	/* Accessing device 31 crashes the MV-64340. */
	//kaiker
	return 0;
	if (dev < 5)
		return 0;
	return -1;
}

void __inline__ read_config(unsigned long bus, unsigned long dev, unsigned long func, unsigned long reg, unsigned long *val)
{
	unsigned long address_reg, data_reg, address;

 	address_reg = EUREKA_EP430_PCI_CONFIG_ADDR;
        data_reg = EUREKA_EP430_PCI_CONFIG_DATA_VIRTUAL_REG;

        /* set addr */
        address = (bus << 16) | (dev << 11) | (func << 8) | (reg & 0xfc) | 0x80000000 ;

        /* start the configuration cycle */
        MV_WRITE(address_reg, address);
        /* read the data */
        MV_READ(data_reg, val);
	return;
}

void __inline__ write_config(unsigned long bus, unsigned long dev, unsigned long func, unsigned long reg, unsigned long val)
{
	unsigned long address_reg, data_reg, address;

 	address_reg = EUREKA_EP430_PCI_CONFIG_ADDR;
        data_reg = EUREKA_EP430_PCI_CONFIG_DATA_VIRTUAL_REG;

        /* set addr */
        address = (bus << 16) | (dev << 11) | (func << 8) | (reg & 0xfc) | 0x80000000;

        /* start the configuration cycle */
        MV_WRITE(address_reg, address);
        /* read the data */
        MV_WRITE(data_reg, val);
	return;
}

/*
 * marvell_pcibios_(read/write)_config_(dword/word/byte) -
 *
 * reads/write a dword/word/byte register from the configuration space
 * of a device.
 *
 * Note that bus 0 and bus 1 are local, and we assume all other busses are
 * bridged from bus 1.  This is a safe assumption, since any other
 * configuration will require major modifications to the CP7000G
 *
 * Inputs :
 * bus - bus number
 * dev - device number
 * offset - register offset in the configuration space
 * val - value to be written / read
 *
 * Outputs :
 * PCIBIOS_SUCCESSFUL when operation was succesfull
 * PCIBIOS_DEVICE_NOT_FOUND when the bus or dev is errorneous
 * PCIBIOS_BAD_REGISTER_NUMBER when accessing non aligned
 */

static int rt2880_pcibios_read_config_dword(struct pci_dev *device,
					      int offset, u32* val)
{
	int dev, bus, func;
	uint32_t address_reg, data_reg;
	uint32_t address;

	bus = device->bus->number;
	dev = PCI_SLOT(device->devfn);
	func = PCI_FUNC(device->devfn);

	/* verify the range */
	if (pci_range_ck(bus, dev))
		return PCIBIOS_DEVICE_NOT_FOUND;

	/* select the MV-64340 registers to communicate with the PCI bus */
	
		address_reg = EUREKA_EP430_PCI_CONFIG_ADDR;
		data_reg = EUREKA_EP430_PCI_CONFIG_DATA_VIRTUAL_REG;
	

	address = (bus << 16) | (dev << 11) | (func << 8) |
		(offset & 0xfc) | 0x80000000;

	/* start the configuration cycle */
	MV_WRITE(address_reg, address);

	/* read the data */
	MV_READ(data_reg, val);

	return PCIBIOS_SUCCESSFUL;
}


static int rt2880_pcibios_read_config_word(struct pci_dev *device,
					     int offset, u16* val)
{
	int dev, bus, func;
	uint32_t address_reg, data_reg;
	uint32_t address,temp;

	bus = device->bus->number;
	dev = PCI_SLOT(device->devfn);
	func = PCI_FUNC(device->devfn);

	/* verify the range */
	if (pci_range_ck(bus, dev))
		return PCIBIOS_DEVICE_NOT_FOUND;

	/* select the MV-64340 registers to communicate with the PCI bus */
	
		address_reg = EUREKA_EP430_PCI_CONFIG_ADDR;
		data_reg = EUREKA_EP430_PCI_CONFIG_DATA_VIRTUAL_REG;
	

	address = (bus << 16) | (dev << 11) | (func << 8) |
		(offset & 0xfc) | 0x80000000;

	/* start the configuration cycle */
	MV_WRITE(address_reg, address);

	/* read the data */
	//MV_READ_16(data_reg + (offset & 0x3), val);
	//kaiker
	/* read the data */
	MV_READ(data_reg, &temp);

	switch(offset % 4)
    {
        case 0:
	case 1:
	    temp &= 0x0000FFFF;
	    break;
	case 2:
	case 3:
	    temp &= 0xFFFF0000;
	    temp = temp >> 16;
	    break;
    }
	*val = (u16)temp;

	return PCIBIOS_SUCCESSFUL;
}

static int rt2880_pcibios_read_config_byte(struct pci_dev *device,
					     int offset, u8* val)
{
	int dev, bus, func;
	uint32_t address_reg, data_reg;
	uint32_t address;
	uint32_t temp;

	bus = device->bus->number;
	dev = PCI_SLOT(device->devfn);
	func = PCI_FUNC(device->devfn);

	/* verify the range */
	if (pci_range_ck(bus, dev))
		return PCIBIOS_DEVICE_NOT_FOUND;

	/* select the MV-64340 registers to communicate with the PCI bus */
	
		address_reg = EUREKA_EP430_PCI_CONFIG_ADDR;
		data_reg = EUREKA_EP430_PCI_CONFIG_DATA_VIRTUAL_REG;
	

	address = (bus << 16) | (dev << 11) | (func << 8) |
		(offset & 0xfc) | 0x80000000;

	/* start the configuration cycle */
	MV_WRITE(address_reg, address);

//kaiker
	/* read the data */
	MV_READ(data_reg, &temp);


	/* read the data */
	//MV_READ(data_reg, val);
	
	/* write the data */
	//MV_READ_8(data_reg + (offset & 0x3), val);

	switch(offset % 4)
    {
        case 0:
	    temp &= 0x000000FF;
	    break;
	case 1:
	    temp &= 0x0000FF00;
   	    temp = temp >> 8;
	    break;
	case 2:
            temp &= 0x00FF0000;
            temp = temp >> 16;
            break;
 	case 3:
            temp &= 0xFF000000;
            temp = temp >> 24;
 	break;
    }

	*val = (u8)temp;

	return PCIBIOS_SUCCESSFUL;
}

static int rt2880_pcibios_write_config_dword(struct pci_dev *device,
					      int offset, u32 val)
{
	int dev, bus, func;
	uint32_t address_reg, data_reg;
	uint32_t address;

	bus = device->bus->number;
	dev = PCI_SLOT(device->devfn);
	func = PCI_FUNC(device->devfn);

	/* verify the range */
	if (pci_range_ck(bus, dev))
		return PCIBIOS_DEVICE_NOT_FOUND;

	/* select the MV-64340 registers to communicate with the PCI bus */
	
		address_reg = EUREKA_EP430_PCI_CONFIG_ADDR;
		data_reg = EUREKA_EP430_PCI_CONFIG_DATA_VIRTUAL_REG;
	

	address = (bus << 16) | (dev << 11) | (func << 8) |
		(offset & 0xfc) | 0x80000000;

	/* start the configuration cycle */
	MV_WRITE(address_reg, address);

	/* write the data */
	MV_WRITE(data_reg, val);

	return PCIBIOS_SUCCESSFUL;
}


static int rt2880_pcibios_write_config_word(struct pci_dev *device,
					     int offset, u16 val)
{
	int dev, bus, func;
	uint32_t address_reg, data_reg;
	uint32_t address,temp;

	bus = device->bus->number;
	dev = PCI_SLOT(device->devfn);
	func = PCI_FUNC(device->devfn);

	/* verify the range */
	if (pci_range_ck(bus, dev))
		return PCIBIOS_DEVICE_NOT_FOUND;

	/* select the MV-64340 registers to communicate with the PCI bus */
	
		address_reg = EUREKA_EP430_PCI_CONFIG_ADDR;
		data_reg = EUREKA_EP430_PCI_CONFIG_DATA_VIRTUAL_REG;
	
	address = (bus << 16) | (dev << 11) | (func << 8) |
		(offset & 0xfc) | 0x80000000;

	/* start the configuration cycle */
	MV_WRITE(address_reg, address);
	
	//kaiker

	/* read the data */
	MV_READ(data_reg, &temp);

	switch(offset % 4)
    { 
        case 0:
	case 1:
	    temp &= 0xFFFF0000;
            temp += val;
	    break;
	case 2:
	case 3:
            temp &= 0x0000FFFF;
            temp += (u32)(((u32)val) << 16);
	    break;
    }

	
//kaiker
	/* write the data */
	MV_WRITE(data_reg, temp);
	

	/* write the data */
	//MV_WRITE_16(data_reg + (offset & 0x3), val);

	return PCIBIOS_SUCCESSFUL;
}

static int rt2880_pcibios_write_config_byte(struct pci_dev *device,
					     int offset, u8 val)
{
	int dev, bus, func;
	uint32_t address_reg, data_reg;
	uint32_t address,temp;

	bus = device->bus->number;
	dev = PCI_SLOT(device->devfn);
	func = PCI_FUNC(device->devfn);

	/* verify the range */
	if (pci_range_ck(bus, dev))
		return PCIBIOS_DEVICE_NOT_FOUND;

	/* select the MV-64340 registers to communicate with the PCI bus */
	
		address_reg = EUREKA_EP430_PCI_CONFIG_ADDR;
		data_reg = EUREKA_EP430_PCI_CONFIG_DATA_VIRTUAL_REG;
	

	address = (bus << 16) | (dev << 11) | (func << 8) |
		(offset & 0xfc) | 0x80000000;

	/* start the configuration cycle */
	MV_WRITE(address_reg, address);

//kaiker--------------------
	/* read the data */
	MV_READ(data_reg, &temp);

 	switch (offset % 4)
    {
        case 0:
	    temp &= 0xFFFFFF00;
	    temp += val;
	     break;
	case 1:
		temp &= 0xFFFF00FF;
	    temp += (u32)(((u32)val) << 8);
	    break;
	case 2:
		temp &= 0xFF00FFFF;
	    temp += (u32)(((u32)val) << 16);
	     break;
	case 3:
   	    temp &= 0x00FFFFFF;
	    temp += (u32)(((u32)val) << 24);
	    break;
    }

//--------------------------	

	/* write the data */
//	MV_WRITE_8(data_reg + (offset & 0x3), val);

	/* write the data */
	MV_WRITE(data_reg, temp);


	return PCIBIOS_SUCCESSFUL;
}



void __init pcibios_fixup_bus(struct pci_bus *bus)
{
	struct pci_bus *current_bus = bus;
	struct pci_dev *devices;
	struct list_head *devices_link;
	u16 cmd;
	return;
#if 0
	/* loop over all known devices on this bus */
	printk("****%s %s():%d\n",__FILE__,__FUNCTION__,__LINE__);
	list_for_each(devices_link, &(current_bus->devices)) {

		devices = pci_dev_b(devices_link);
		printk("devices = %x\n",devices);
		if (devices == NULL)
			continue;
		#if 0
		if ((current_bus->number == 0) &&
			(PCI_SLOT(devices->devfn) == 1) &&
			(PCI_FUNC(devices->devfn) == 0)) {
			/* LSI 53C10101R SCSI (A) */
			devices->irq = 2;
		} else if ((current_bus->number == 0) &&
			(PCI_SLOT(devices->devfn) == 1) &&
			(PCI_FUNC(devices->devfn) == 1)) {
			/* LSI 53C10101R SCSI (B) */
			devices->irq = 2;
		} else if ((current_bus->number == 1) &&
			(PCI_SLOT(devices->devfn) == 1)) {
			/* Intel 21555 bridge */
			devices->irq = 12;
		} else if ((current_bus->number == 1) &&
			(PCI_SLOT(devices->devfn) == 2)) {
			/* PMC Slot */
			devices->irq = 4;
		} else {
			/* We don't have assign interrupts for other devices. */
			devices->irq = 0xff;
		}
		#endif
		/* Assign an interrupt number for the device */
		//bus->ops->write_byte(devices, PCI_INTERRUPT_LINE, devices->irq);

		/* enable master for everything but the MV-64340 */
		//if (((current_bus->number != 0) && (current_bus->number != 1))
		//		|| (PCI_SLOT(devices->devfn) != 0)) {
			bus->ops->read_word(devices, PCI_COMMAND, &cmd);
			cmd |= PCI_COMMAND_MASTER;
			bus->ops->write_word(devices, PCI_COMMAND, cmd);
		//}
	}
	//mv64340_board_pcibios_fixup_bus(c);
#endif
}
#if 0
int pci_scan(unsigned long slot)
{
	unsigned long val, i, BaseAddr, data = 0;
	//int io_set=0, mem_set=0;
	
	//printk("%s %s():%d\n",__FILE__,__FUNCTION__,__LINE__);
	BaseAddr = PCI_BASE_ADDRESS_0;

	read_config(0, slot, 0, 0, &val);
	if(val !=0){
	  for (i=0;i<2;i++, BaseAddr+=4) {  //detect resource usage
	
	  write_config(0, slot, 0, BaseAddr, 0xffffffff);
	  read_config(0, slot, 0, BaseAddr, &data);
	  //printk("write %d 0xffffffff and read back %x\n", i, data);
          if (data!=0) {  //resource request exist
              int j;
              if (data&1) {  //IO space
		  //if(io_set == 1)
		//	  continue;
                  //pci_config->BAR[i].Type = IO_SPACE;
                  //scan resource size
                  for (j=2;j<32;j++)
                      if (data&(1<<j)) break;
		  if(j>16){
			printk("slot 0x%x, request memory over 32k, not support\n", slot);
			return -1;
		  }
		  if(slot == 0x11)
		    write_config(0, slot, 0, BaseAddr, 0x00520000);
		  else if(slot == 0x12)
		    write_config(0, slot, 0, BaseAddr, 0x00528000);

		    read_config(0, slot, 0, BaseAddr, &data);
		    //printk(" ********* %x\n", data);
		  //io_set = 1;
                  //if (j<32) pci_config->BAR[i].Size = 1<<j;
                  //else  pci_config->BAR[i].Size = 0;
              } else {  //Memory space
		  //if(mem_set == 1)
		//	  continue;
                  //pci_config->BAR[i].Type = MEM_SPACE;
                  //bus width
                  if ((data&0x0006)==4) {
		    //pci_config->BAR[i].Width = WIDTH64; //bus width 64
		    printk("slot 0x%x, 64bit, not support\n", slot);
		    return -1;
		  }
                  //else 
		     //pci_config->BAR[i].Width = WIDTH32;  //bus width 32

                  //prefetchable
                  //if (data&0x0008) pci_config->BAR[i].Prefetch = 1; //prefetchable
                  //else pci_config->BAR[i].Prefetch = 0;  //no prefetch
                  //scan resource size
                  //if (pci_config->BAR[i].Width==WIDTH32) {
                    for (j=4;j<32;j++)
                      if (data&(1<<j)) break;
		    if(j>16){
			printk("slot 0x%x, request memory over 32k, not support\n", slot);
			return -1;
		    }
		  if(slot == 0x11)
		    write_config(0, slot, 0, BaseAddr, RT2880_PCI_SLOT1_BASE);
		  else if(slot == 0x12)
		    write_config(0, slot, 0, BaseAddr, RT2880_PCI_SLOT2_BASE);

		    //read_config(0, slot, 0, BaseAddr, &data);
		    //printk(" ********* %x\n", data);
		    //mem_set = 1;
                    //if (j<32) pci_config->BAR[i].Size = 1<<j;
                    //else  pci_config->BAR[i].Size = 0;
                  //} else //width64 is not support
                  //  pci_config->BAR[i].Size = 0;
              };
          } else {  //no resource
              //memset(&(pci_config->BAR[i]), 0, sizeof(base_address_s));
		printk("slot 0x%x, error access\n", slot);
		return -1;
          };
    	  };//for
	} else {
	  printk("slot 0x%x empty\n", slot);
	}
}
#endif
void __init pcibios_fixup_resources(struct pci_dev *dev)
{

	u16 cmd;
	u32 bus, devid, func;

		printk("%s %s():%d\n",__FILE__,__FUNCTION__,__LINE__);
		pci_write_config_byte(dev, PCI_CACHE_LINE_SIZE, 0x14);  //configure cache line size 0x14
  		pci_write_config_byte(dev, PCI_LATENCY_TIMER, 0xFF);  //configure latency timer 0x10
		//Set device
		pci_read_config_word(dev, PCI_COMMAND, &cmd);		
#if 0
		cmd = cmd | PCI_COMMAND_MASTER | PCI_COMMAND_IO | PCI_COMMAND_MEMORY;
		cmd |= (PCI_COMMAND_PARITY | PCI_COMMAND_SERR);			 
#else
		cmd = cmd | PCI_COMMAND_MASTER | PCI_COMMAND_IO | PCI_COMMAND_MEMORY |
			PCI_COMMAND_INVALIDATE | PCI_COMMAND_FAST_BACK | PCI_COMMAND_SERR |
			PCI_COMMAND_WAIT | PCI_COMMAND_PARITY;
#endif
		pci_write_config_word(dev, PCI_COMMAND, cmd);
		
        	bus = dev->bus->number;
        	devid = PCI_SLOT(dev->devfn);
        	func = PCI_FUNC(dev->devfn);
		printk("bus = %d, dev = %d, fun = %d\n", bus,devid,func);
		
		if (bus == 0 && (devid == 0x11 ||devid==0x12) ) {

		unsigned long val, i, j, BaseAddr, data = 0;
		int mem_cnt=0;
		struct resource *res;
		BaseAddr = PCI_BASE_ADDRESS_0;

		//pci_write_config_dword(dev, BaseAddr, ~0);
		//pci_read_config_dword(dev, BaseAddr, &val);
		//if(val !=0){	//find the card
	  	  for (i=0;i<6;i++, BaseAddr+=4) {  //detect resource usage
			pci_write_config_dword(dev, BaseAddr, ~0);
			pci_read_config_dword(dev, BaseAddr, &data);
			res = &dev->resource[i];
          		if (data!=0) {  //resource request exist
              		  if (data&1) {  //IO space

				for (j=2;j<32;j++)
				  if (data&(1<<j)) break;	
				

	  			pci_write_config_dword(dev, BaseAddr, 0x460000 + io_pool);
				res->start = 0x460000 + io_pool;
				io_pool += 1<<j;
				res->end = 0x460000+ io_pool - 1;
				if(io_pool> 0x10000) 
				  printk("\n******** PCI IO Init ERROR *********\n");
#if 0
		  		if(devid == 0x11){
	  			  pci_write_config_dword(dev, BaseAddr, 0x00520000+ 0x10000*func);
				  res->start = 0x00520000;
				  res->end = 0x00520000 + 0x10000*func -1;
				}
		  		else if(devid == 0x12){
	  			  pci_write_config_dword(dev, BaseAddr, 0x00528000+ 0x10000*func);
				  res->start = 0x00528000;
				  res->end = 0x00528000 + 0x10000*func -1;
				}
#endif

				//pci_read_config_dword(dev, BaseAddr, &data);
		    		//printk(" ********* %x\n", data);
              		  } else { //memory space 
				for (j=4;j<32;j++)
                      		  if (data&(1<<j)) break;

	  			pci_write_config_dword(dev, BaseAddr, 0x20000000 + mem_pool);
				res->start = 0x20000000 + mem_pool;
				mem_pool += 1<<j;
				res->end = 0x20000000+ mem_pool - 1;
				if(mem_pool> 0x10000000) 
				  printk("\n******** PCI MEM Init ERROR *********\n");

#if 0	
		  		if(devid == 0x11){
				 if(mem_cnt==0) {
	  			  pci_write_config_dword(dev, BaseAddr, RT2880_PCI_SLOT1_BASE);
				  res->start = RT2880_PCI_SLOT1_BASE;
				  //res->size = 1<<j;
				  res->end = RT2880_PCI_SLOT1_BASE + (1<<j) -1;
				 }else if(mem_cnt==1){
	  			  pci_write_config_dword(dev, BaseAddr, RT2880_PCI_SLOT1_1_BASE);
				  res->start = RT2880_PCI_SLOT1_1_BASE;
				  //res->size = 1<<j;
				  res->end = RT2880_PCI_SLOT1_1_BASE + (1<<j) -1;
				 }
				}
		  		else if(devid== 0x12){
				 if(mem_cnt==0) {
	  			  pci_write_config_dword(dev, BaseAddr, RT2880_PCI_SLOT2_BASE);
				  res->start = RT2880_PCI_SLOT2_BASE;
				  //res->size = 1<<j;
				  res->end = RT2880_PCI_SLOT2_BASE + (1<<j) -1;
				 }else if(mem_cnt==1){
	  			  pci_write_config_dword(dev, BaseAddr, RT2880_PCI_SLOT2_1_BASE);
				  res->start = RT2880_PCI_SLOT2_1_BASE;
				  //res->size = 1<<j;
				  res->end = RT2880_PCI_SLOT2_1_BASE + (1<<j) -1;
				 }
				}
#endif

				//pci_read_config_dword(dev, BaseAddr, &data);
				//printk("start = 0x%08x, end = 0x%08x\n", res->start, res->end);
		    		//printk(" ********* %x\n", data);
				//mem_cnt++;
              		  }//if(data&1)
          		}else{
				printk("%s %s():%d  ",__FILE__,__FUNCTION__,__LINE__ );
				printk("devid=0x%x, baseaddr = 0x%x\n", devid, BaseAddr);
			}//if(data!=0)
		  }//for
		//}else{
		//	printk(" slot =%d configuration space access error\n", devid);
		//}
		if(devid == 0x11) {
			dev->irq = 2;
			pci_write_config_byte(dev, PCI_INTERRUPT_LINE, 2);
			//pci_write_config_byte(dev, PCI_INTERRUPT_PIN, 2);
		}else if(devid == 0x12) {
			dev->irq = 15;
			pci_write_config_byte(dev, PCI_INTERRUPT_LINE, 15);
			//pci_write_config_byte(dev, PCI_INTERRUPT_PIN, 15);
		}

		}

		return;

}


int init_rt2880pci(void)
{
	unsigned long val = 0;
	int i;

	//printk("%s %s():%d\n",__FILE__,__FUNCTION__,__LINE__);

	RALINK_PCI_PCICFG_ADDR = 0;
	for(i=0;i<0xfffff;i++);
	RT2880_PCI_ARBCTL = 0x79;
	//printk(" RT2880_PCI_ARBCTL = %x\n", RT2880_PCI_ARBCTL);

	ioport_resource.start = 0x0;
        ioport_resource.end   = 0xFFFFFFFF;
        iomem_resource.start  = 0x0;
        iomem_resource.end    = 0xFFFFFFFF;

	RALINK_PCI_BAR0SETUP_ADDR = 0x0FFF0001;//0x07ff0001;	//open 256M
	//RALINK_PCI_BAR1SETUP_ADDR = 0;
	RT2880_PCI_MEMBASE = RT2880_PCI_SLOT1_BASE; //0x00510000;
	RT2880_PCI_IOBASE = 0x00460000;
	RALINK_PCI_IMBASEBAR0_ADDR = 0x08000000;

	RT2880_PCI_ID = 0x08021814;
	RT2880_PCI_CLASS = 0x00800001;
	RT2880_PCI_SUBID = 0x28801814;

	RALINK_PCI_PCIMSK_ADDR = 0x000c0000;

	write_config(0, 0, 0, PCI_BASE_ADDRESS_0, 0x08000000);
	read_config(0, 0, 0, PCI_BASE_ADDRESS_0, &val);
	printk("BAR0 at slot 0 = %x\n", val);
	mem_pool =0;
	io_pool =0;
/*
	val = RALINK_PCI_PCIMSK_ADDR;
	val |= 0x000C0000;
	RALINK_PCI_PCIMSK_ADDR = val;
*/
	return 0;

}
#if 0
void __init pcibios_fixup_irqs(void)
{
    struct pci_dev *dev;

    printk("%s %s():%d\n",__FILE__,__FUNCTION__,__LINE__);
        pci_for_each_dev(dev) {
                dev->irq = 2; // fix irq
        }
}
#endif



void __init pcibios_init(void)
{
	//printk("\n pcibios_init is called ioport_resource = %08X\n",&ioport_resource);
	//printk("\n pcibios_init is called iomem_resource = %08X\n",&iomem_resource);
	
	/* Reset PCI I/O and PCI MEM values */
	/* Reset PCI I/O and PCI MEM values */
	//ioport_resource.start = 0;//0xc0000000;
	//ioport_resource.end   = 0;//0xc0000000 + 0x20000000 - 1;
	//iomem_resource.start  = 0;//0xc0000000;
	//iomem_resource.end    = 0;//0xc0000000 + 0x20000000 - 1;
	
	printk("%s %s():%d\n",__FILE__,__FUNCTION__,__LINE__);
	init_rt2880pci();
		
	pci_scan_bus(0, &rt2880_pci_ops, NULL);

	//pci_scan(0x11);
	//pci_scan(0x12);

	//pcibios_fixup_irqs();
}

unsigned __init int pcibios_assign_all_busses(void)
{
	printk("%s %s():%d\n",__FILE__,__FUNCTION__,__LINE__);
	return 1;
}

#endif	/* CONFIG_PCI */
