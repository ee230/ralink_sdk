/*
 * Normal mappings of chips in physical memory
 *
 * Copyright (C) 2003 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 *
 * 031022 - [jsun] add run-time configure and partition setup
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/mtd/concat.h>
#include <linux/io.h>

#include <linux/err.h>
#include <asm/rt2880/rt_mmap.h>
#include "ralink-flash.h"

#define MAX_RESOURCES		4

struct physmap_flash_info {
	struct mtd_info		*mtd[MAX_RESOURCES];
	struct mtd_info		*cmtd;
	struct map_info		map[MAX_RESOURCES];
#ifdef CONFIG_MTD_PARTITIONS
	int			nr_parts;
	struct mtd_partition	*parts;
#endif
};

static int physmap_flash_remove(struct platform_device *dev)
{
	struct physmap_flash_info *info;
	struct physmap_flash_data *physmap_data;
	int i;

	info = platform_get_drvdata(dev);
	if (info == NULL)
		return 0;
	platform_set_drvdata(dev, NULL);

	physmap_data = dev->dev.platform_data;

	if (info->cmtd) {
#ifdef CONFIG_MTD_PARTITIONS
		if (info->nr_parts || physmap_data->nr_parts) {
			del_mtd_partitions(info->cmtd);

			if (info->nr_parts)
				kfree(info->parts);
		} else {
			del_mtd_device(info->cmtd);
		}
#else
		del_mtd_device(info->cmtd);
#endif
#ifdef CONFIG_MTD_CONCAT
		if (info->cmtd != info->mtd[0])
			mtd_concat_destroy(info->cmtd);
#endif
	}

	for (i = 0; i < MAX_RESOURCES; i++) {
		if (info->mtd[i] != NULL)
			map_destroy(info->mtd[i]);
	}
	return 0;
}


static struct mtd_partition ralink_partitions[] = {
    	{
                name:           "ALL",
                size:           MTDPART_SIZ_FULL,
                offset:         0,
        },

        {
                name:           "Bootloader",  /* mtdblock0 */
                size:           MTD_BOOT_PART_SIZE,  /* 192K */
                offset:         0,
        }, {
                name:           "Config", /* mtdblock1 */
                size:           MTD_CONFIG_PART_SIZE,  /* 64K */
                offset:         MTDPART_OFS_APPEND
        }, {
                name:           "Factory", /* mtdblock2 */
                size:           MTD_FACTORY_PART_SIZE,  /* 64K */
                offset:         MTDPART_OFS_APPEND
        }, {
#ifdef CONFIG_RT2880_ROOTFS_IN_FLASH
                name:           "Kernel", /* mtdblock3 */
                size:           CONFIG_MTD_KERNEL_PART_SIZ,
                offset:         MTDPART_OFS_APPEND,
        }, {
                name:           "RootFS", /* mtdblock4 */
                size:           MTD_ROOTFS_PART_SIZE,
                offset:         MTDPART_OFS_APPEND,
#ifdef CONFIG_ROOTFS_IN_FLASH_NO_PADDING
        }, {
                name:           "Kernel_RootFS",
                size:           MTD_KERN_PART_SIZE + MTD_ROOTFS_PART_SIZE,
                offset:         MTD_BOOT_PART_SIZE + MTD_CONFIG_PART_SIZE + MTD_FACTORY_PART_SIZE,
#endif
#else //CONFIG_RT2880_ROOTFS_IN_RAM
                name:           "Kernel", /* mtdblock3 */
                size:           MTD_KERN_PART_SIZE,
                offset:         MTDPART_OFS_APPEND,
#endif
#ifdef CONFIG_DUAL_IMAGE
	}, {
		name:		"Kernel2", /* mtdblock4 */
		size:		MTD_KERN2_PART_SIZE, 
		offset:		MTD_KERN2_PART_OFFSET,
#ifdef CONFIG_RT2880_ROOTFS_IN_FLASH
	}, {
                name:           "RootFS2",
                size:           MTD_ROOTFS2_PART_SIZE,
                offset:         MTD_ROOTFS2_PART_OFFSET,
#endif
        }
#else
	}
#endif 
};


int ra_check_flash_type(void)
{
#if defined (CONFIG_MTD_NOR_RALINK)
    return BOOT_FROM_NOR;
#elif defined (CONFIG_MTD_SPI_RALINK)
    return BOOT_FROM_SPI;
#elif defined (CONFIG_MTD_NAND_RALINK)
    return BOOT_FROM_NAND;
#else // CONFIG_MTD_ANY_RALINK //

    uint8_t Id[10];
    int syscfg=0;
    int boot_from=0;
    int chip_mode=0;

#if defined(CONFIG_RALINK_RT6855A)
	syscfg = (*((int *)(RALINK_SYSCTL_BASE + 0x8c)));
	chip_mode = ((syscfg>>28) & 0x3)|(((syscfg>>22) & 0x3)<<2);

	switch(chip_mode)
	{
	case 0:
	case 2:
	case 3:
		boot_from = BOOT_FROM_SPI;
		break;
	case 1:
	case 12:
		boot_from = BOOT_FROM_NAND;
		break;	
	}
	return boot_from;	        
#endif
    memset(Id, 0, sizeof(Id));
    strncpy(Id, (char *)RALINK_SYSCTL_BASE, 6);
    syscfg = (*((int *)(RALINK_SYSCTL_BASE + 0x10)));

    if((strcmp(Id,"RT3052")==0) || (strcmp(Id, "RT3350")==0)) {
        boot_from = (syscfg >> 16) & 0x3;
        switch(boot_from)
        {
        case 0:
        case 1:
            boot_from=BOOT_FROM_NOR;
            break;
        case 2:
            boot_from=BOOT_FROM_NAND;
            break;
        case 3:
            boot_from=BOOT_FROM_SPI;
            break;
        }
    }else if(strcmp(Id,"RT3883")==0) {
        boot_from = (syscfg >> 4) & 0x3;
        switch(boot_from)
        {
        case 0:
        case 1:
            boot_from=BOOT_FROM_NOR;
            break;
        case 2:
        case 3:
            chip_mode = syscfg & 0xF;
            if((chip_mode==0) || (chip_mode==7)) {
                boot_from = BOOT_FROM_SPI;
            }else if(chip_mode==8) {
                boot_from = BOOT_FROM_NAND;
            }else {
                printk("unknow chip_mode=%d\n",chip_mode);
            }
            break;
        }
    }else if(strcmp(Id,"RT3352")==0) {
	boot_from = BOOT_FROM_SPI;
    }else if(strcmp(Id,"RT5350")==0) {
        boot_from = BOOT_FROM_SPI;
    }else if(strcmp(Id,"RT2880")==0) {
        boot_from = BOOT_FROM_NOR;
    }else if(strcmp(Id,"RT6855")==0) {
        boot_from = BOOT_FROM_SPI;
    } else {
        printk("%s: %s is not supported\n",__FUNCTION__, Id);
    }

    return boot_from;
#endif
}


static const char *rom_probe_types[] = {
					"cfi_probe",
					"jedec_probe",
					"qinfo_probe",
					"map_rom",
					NULL };
#ifdef CONFIG_MTD_PARTITIONS
static const char *part_probe_types[] = { "cmdlinepart", "RedBoot", NULL };
#endif

static int physmap_flash_probe(struct platform_device *dev)
{
	struct physmap_flash_data *physmap_data;
	struct physmap_flash_info *info;
	const char **probe_type;
	int err = 0;
	int i;
	int devices_found = 0;

	physmap_data = dev->dev.platform_data;
	if (physmap_data == NULL)
		return -ENODEV;

	info = devm_kzalloc(&dev->dev, sizeof(struct physmap_flash_info),
			    GFP_KERNEL);
	if (info == NULL) {
		err = -ENOMEM;
		goto err_out;
	}

	platform_set_drvdata(dev, info);

	for (i = 0; i < dev->num_resources; i++) {
		printk(KERN_NOTICE "physmap platform flash device: %.8llx at %.8llx\n",
		       (unsigned long long)resource_size(&dev->resource[i]),
		       (unsigned long long)dev->resource[i].start);

		if (!devm_request_mem_region(&dev->dev,
			dev->resource[i].start,
			resource_size(&dev->resource[i]),
			dev_name(&dev->dev))) {
			dev_err(&dev->dev, "Could not reserve memory region\n");
			err = -ENOMEM;
			goto err_out;
		}

		info->map[i].name = dev_name(&dev->dev);
		info->map[i].phys = dev->resource[i].start;
		info->map[i].size = resource_size(&dev->resource[i]);
		info->map[i].bankwidth = physmap_data->width;
		info->map[i].set_vpp = physmap_data->set_vpp;
		info->map[i].pfow_base = physmap_data->pfow_base;

		info->map[i].virt = devm_ioremap(&dev->dev, info->map[i].phys,
						 info->map[i].size);
		if (info->map[i].virt == NULL) {
			dev_err(&dev->dev, "Failed to ioremap flash region\n");
			err = -EIO;
			goto err_out;
		}

		simple_map_init(&info->map[i]);

		probe_type = rom_probe_types;
		if (physmap_data->probe_type == NULL) {
			for (; info->mtd[i] == NULL && *probe_type != NULL; probe_type++)
				info->mtd[i] = do_map_probe(*probe_type, &info->map[i]);
		} else
			info->mtd[i] = do_map_probe(physmap_data->probe_type, &info->map[i]);

		if (info->mtd[i] == NULL) {
			dev_err(&dev->dev, "map_probe failed\n");
			err = -ENXIO;
			goto err_out;
		} else {
			devices_found++;
		}
		info->mtd[i]->owner = THIS_MODULE;
		info->mtd[i]->dev.parent = &dev->dev;
	}

	if (devices_found == 1) {
		info->cmtd = info->mtd[0];
	} else if (devices_found > 1) {
		/*
		 * We detected multiple devices. Concatenate them together.
		 */
#ifdef CONFIG_MTD_CONCAT
		info->cmtd = mtd_concat_create(info->mtd, devices_found, dev_name(&dev->dev));
		if (info->cmtd == NULL)
			err = -ENXIO;
#else
		printk(KERN_ERR "physmap-flash: multiple devices "
		       "found but MTD concat support disabled.\n");
		err = -ENXIO;
#endif
	}
	if (err)
		goto err_out;

#ifdef CONFIG_MTD_PARTITIONS
	err = parse_mtd_partitions(info->cmtd, part_probe_types,
				&info->parts, 0);
	if (err > 0) {
		add_mtd_partitions(info->cmtd, info->parts, err);
		info->nr_parts = err;
		return 0;
	}

	if (physmap_data->nr_parts) {
		printk(KERN_NOTICE "Using physmap partition information\n");
		add_mtd_partitions(info->cmtd, physmap_data->parts,
				   physmap_data->nr_parts);
		return 0;
	}
#endif

	add_mtd_device(info->cmtd);
	return 0;

err_out:
	physmap_flash_remove(dev);
	return err;
}

#ifdef CONFIG_PM
static void physmap_flash_shutdown(struct platform_device *dev)
{
	struct physmap_flash_info *info = platform_get_drvdata(dev);
	int i;

	for (i = 0; i < MAX_RESOURCES && info->mtd[i]; i++)
		if (info->mtd[i]->suspend && info->mtd[i]->resume)
			if (info->mtd[i]->suspend(info->mtd[i]) == 0)
				info->mtd[i]->resume(info->mtd[i]);
}
#else
#define physmap_flash_shutdown NULL
#endif

static struct platform_driver physmap_flash_driver = {
	.probe		= physmap_flash_probe,
	.remove		= physmap_flash_remove,
	.shutdown	= physmap_flash_shutdown,
	.driver		= {
		.name	= "physmap-flash",
		.owner	= THIS_MODULE,
	},
};


#ifdef CONFIG_MTD_PHYSMAP_COMPAT
static struct physmap_flash_data physmap_flash_data = {
	.width		= CONFIG_MTD_PHYSMAP_BANKWIDTH,
	.nr_parts       = ARRAY_SIZE(ralink_partitions),
	.parts		= ralink_partitions,
};

static struct resource physmap_flash_resource = {
	.start		= CONFIG_MTD_PHYSMAP_START,
	.end		= CONFIG_MTD_PHYSMAP_START + CONFIG_MTD_PHYSMAP_LEN - 1,
	.flags		= IORESOURCE_MEM,
};

static struct platform_device physmap_flash = {
	.name		= "physmap-flash",
	.id		= 0,
	.dev		= {
	.platform_data	= &physmap_flash_data,
	},
	.num_resources	= 1,
	.resource	= &physmap_flash_resource,
};

void physmap_configure(unsigned long addr, unsigned long size,
		int bankwidth, void (*set_vpp)(struct map_info *, int))
{
	physmap_flash_resource.start = addr;
	physmap_flash_resource.end = addr + size - 1;
	physmap_flash_data.width = bankwidth;
	physmap_flash_data.set_vpp = set_vpp;
}

#ifdef CONFIG_MTD_PARTITIONS
void physmap_set_partitions(struct mtd_partition *parts, int num_parts)
{
	physmap_flash_data.nr_parts = num_parts;
	physmap_flash_data.parts = parts;
}
#endif
#endif

static int __init physmap_init(void)
{
#if defined (CONFIG_MTD_NOR_RALINK) || defined (CONFIG_MTD_ANY_RALINK)
	int err;

	if(ra_check_flash_type()!=BOOT_FROM_NOR) { /* NOR */
            return 0;
        }

	err = platform_driver_register(&physmap_flash_driver);
#ifdef CONFIG_MTD_PHYSMAP_COMPAT
	if (err == 0) {
		err = platform_device_register(&physmap_flash);
		if (err)
		    platform_driver_unregister(&physmap_flash_driver);
	}
#endif


	return err;
#endif
}

static void __exit physmap_exit(void)
{
#if defined (CONFIG_MTD_NOR_RALINK) || defined (CONFIG_MTD_ANY_RALINK)

#ifdef CONFIG_MTD_PHYSMAP_COMPAT
	platform_device_unregister(&physmap_flash);
#endif
	platform_driver_unregister(&physmap_flash_driver);
#endif
}

module_init(physmap_init);
module_exit(physmap_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Woodhouse <dwmw2@infradead.org>");
MODULE_DESCRIPTION("Generic configurable MTD map driver");

/* legacy platform drivers can't hotplug or coldplg */
#ifndef CONFIG_MTD_PHYSMAP_COMPAT
/* work with hotplug and coldplug */
MODULE_ALIAS("platform:physmap-flash");
#endif

/*
 * Flash API: ra_mtd_read, ra_mtd_write
 * Arguments:
 *   - num: specific the mtd number
 *   - to/from: the offset to read from or written to
 *   - len: length
 *   - buf: data to be read/written
 * Returns:
 *   - return -errno if failed
 *   - return the number of bytes read/written if successed
 */
#ifdef RA_MTD_RW_BY_NUM
int ra_mtd_write(int num, loff_t to, size_t len, const u_char *buf)
{
	int ret = -1;
	size_t rdlen, wrlen;
	struct mtd_info *mtd;
	struct erase_info ei;
	u_char *bak = NULL;

	mtd = get_mtd_device(NULL, num);
	if (IS_ERR(mtd))
		return (int)mtd;
	if (len > mtd->erasesize) {
		put_mtd_device(mtd);
		return -E2BIG;
	}

	bak = kmalloc(mtd->erasesize, GFP_KERNEL);
	if (bak == NULL) {
		put_mtd_device(mtd);
		return -ENOMEM;
	}

	ret = mtd->read(mtd, 0, mtd->erasesize, &rdlen, bak);
	if (ret != 0) {
		put_mtd_device(mtd);
		kfree(bak);
		return ret;
	}
	if (rdlen != mtd->erasesize)
		printk("warning: ra_mtd_write: rdlen is not equal to erasesize\n");

	memcpy(bak + to, buf, len);

	ei.mtd = mtd;
	ei.callback = NULL;
	ei.addr = 0;
	ei.len = mtd->erasesize;
	ei.priv = 0;
	ret = mtd->erase(mtd, &ei);
	if (ret != 0) {
		put_mtd_device(mtd);
		kfree(bak);
		return ret;
	}

	ret = mtd->write(mtd, 0, mtd->erasesize, &wrlen, bak);

	put_mtd_device(mtd);
	kfree(bak);
	return ret;
}
#endif

int ra_mtd_write_nm(char *name, loff_t to, size_t len, const u_char *buf)
{
	int ret = -1;
	size_t rdlen, wrlen;
	struct mtd_info *mtd;
	struct erase_info ei;
	u_char *bak = NULL;

	mtd = get_mtd_device_nm(name);
	if (IS_ERR(mtd))
		return (int)mtd;
	if (len > mtd->erasesize) {
		put_mtd_device(mtd);
		return -E2BIG;
	}

	bak = kmalloc(mtd->erasesize, GFP_KERNEL);
	if (bak == NULL) {
		put_mtd_device(mtd);
		return -ENOMEM;
	}

	ret = mtd->read(mtd, 0, mtd->erasesize, &rdlen, bak);
	if (ret != 0) {
		put_mtd_device(mtd);
		kfree(bak);
		return ret;
	}
	if (rdlen != mtd->erasesize)
		printk("warning: ra_mtd_write: rdlen is not equal to erasesize\n");

	memcpy(bak + to, buf, len);

	ei.mtd = mtd;
	ei.callback = NULL;
	ei.addr = 0;
	ei.len = mtd->erasesize;
	ei.priv = 0;
	ret = mtd->erase(mtd, &ei);
	if (ret != 0) {
		put_mtd_device(mtd);
		kfree(bak);
		return ret;
	}

	ret = mtd->write(mtd, 0, mtd->erasesize, &wrlen, bak);

	put_mtd_device(mtd);
	kfree(bak);
	return ret;
}

#ifdef RA_MTD_RW_BY_NUM
int ra_mtd_read(int num, loff_t from, size_t len, u_char *buf)
{
	int ret;
	size_t rdlen;
	struct mtd_info *mtd;

	mtd = get_mtd_device(NULL, num);
	if (IS_ERR(mtd))
		return (int)mtd;

	ret = mtd->read(mtd, from, len, &rdlen, buf);
	if (rdlen != len)
		printk("warning: ra_mtd_read: rdlen is not equal to len\n");

	put_mtd_device(mtd);
	return ret;
}
#endif

int ra_mtd_read_nm(char *name, loff_t from, size_t len, u_char *buf)
{
	int ret;
	size_t rdlen;
	struct mtd_info *mtd;

	mtd = get_mtd_device_nm(name);
	if (IS_ERR(mtd))
		return (int)mtd;

	ret = mtd->read(mtd, from, len, &rdlen, buf);
	if (rdlen != len)
		printk("warning: ra_mtd_read_nm: rdlen is not equal to len\n");

	put_mtd_device(mtd);
	return ret;
}

#ifdef RA_MTD_RW_BY_NUM
EXPORT_SYMBOL(ra_mtd_write);
EXPORT_SYMBOL(ra_mtd_read);
#endif
EXPORT_SYMBOL(ra_mtd_write_nm);
EXPORT_SYMBOL(ra_mtd_read_nm);
