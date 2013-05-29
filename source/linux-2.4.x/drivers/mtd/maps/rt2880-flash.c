/*
 * $Id: rt2880-flash.c,v 1.21 2009-06-04 11:50:12 winfred Exp $
 *
 * Normal mappings of chips in physical memory
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>

#include <linux/mtd/partitions.h>

#define WINDOW_ADDR 		CONFIG_MTD_PHYSMAP_START
#define WINDOW_SIZE 		CONFIG_MTD_PHYSMAP_LEN
#define BUSWIDTH 		CONFIG_MTD_PHYSMAP_BUSWIDTH

#define MTD_BOOT_PART_SIZE	0x30000
#define	MTD_CONFIG_PART_SIZE	0x10000
#define	MTD_FACTORY_PART_SIZE	0x10000

#ifdef CONFIG_DUAL_IMAGE
#if defined (CONFIG_RT2880_FLASH_2M)
#define IMAGE1_SIZE		0x100000
#elif defined (CONFIG_RT2880_FLASH_4M)
#define IMAGE1_SIZE		0x200000
#elif defined (CONFIG_RT2880_FLASH_8M)
#define IMAGE1_SIZE		0x400000
#elif defined (CONFIG_RT2880_FLASH_16M)
#define IMAGE1_SIZE		0x800000
#elif defined (CONFIG_RT2880_FLASH_32M)
#define IMAGE1_SIZE		0x1000000
#endif

#define MTD_KERN2_PART_SIZE	MTD_KERN_PART_SIZE
#define MTD_KERN2_PART_OFFSET	IMAGE1_SIZE + (MTD_BOOT_PART_SIZE + MTD_CONFIG_PART_SIZE \
					+ MTD_FACTORY_PART_SIZE)
#else
#if defined (CONFIG_RT2880_FLASH_2M)
#define IMAGE1_SIZE		0x200000
#elif defined (CONFIG_RT2880_FLASH_4M)
#define IMAGE1_SIZE		0x400000
#elif defined (CONFIG_RT2880_FLASH_8M)
#define IMAGE1_SIZE		0x800000
#elif defined (CONFIG_RT2880_FLASH_16M)
#define IMAGE1_SIZE		0x1000000
#elif defined (CONFIG_RT2880_FLASH_32M)
#define IMAGE1_SIZE		0x2000000
#endif
#endif

#ifdef CONFIG_RT2880_ROOTFS_IN_FLASH
#define MTD_ROOTFS_PART_SIZE	IMAGE1_SIZE - (MTD_BOOT_PART_SIZE + MTD_CONFIG_PART_SIZE \
					+ MTD_FACTORY_PART_SIZE + CONFIG_MTD_KERNEL_PART_SIZ)
#else
#define MTD_KERN_PART_SIZE		IMAGE1_SIZE - (MTD_BOOT_PART_SIZE + MTD_CONFIG_PART_SIZE \
					+ MTD_FACTORY_PART_SIZE)
#endif

static struct mtd_info *mymtd;

__u8 physmap_read8(struct map_info *map, unsigned long ofs)
{
	return __raw_readb(map->map_priv_1 + ofs);
}

__u16 physmap_read16(struct map_info *map, unsigned long ofs)
{
	return __raw_readw(map->map_priv_1 + ofs);
}

__u32 physmap_read32(struct map_info *map, unsigned long ofs)
{
	return __raw_readl(map->map_priv_1 + ofs);
}

void physmap_copy_from(struct map_info *map, void *to, unsigned long from, ssize_t len)
{
	memcpy_fromio(to, map->map_priv_1 + from, len);
}

void physmap_write8(struct map_info *map, __u8 d, unsigned long adr)
{
	__raw_writeb(d, map->map_priv_1 + adr);
	mb();
}

void physmap_write16(struct map_info *map, __u16 d, unsigned long adr)
{
	__raw_writew(d, map->map_priv_1 + adr);
	mb();
}

void physmap_write32(struct map_info *map, __u32 d, unsigned long adr)
{
	__raw_writel(d, map->map_priv_1 + adr);
	mb();
}

void physmap_copy_to(struct map_info *map, unsigned long to, const void *from, ssize_t len)
{
	memcpy_toio(map->map_priv_1 + to, from, len);
}

struct map_info physmap_map = {
	name: "RT2880 SOC Physically mapped flash",
	size: WINDOW_SIZE,
	buswidth: BUSWIDTH,
	read8: physmap_read8,
	read16: physmap_read16,
	read32: physmap_read32,
	copy_from: physmap_copy_from,
	write8: physmap_write8,
	write16: physmap_write16,
	write32: physmap_write32,
	copy_to: physmap_copy_to
};

#ifdef CONFIG_MTD_PARTITIONS
#ifdef CONFIG_MTD_CMDLINE_PARTS
static struct mtd_partition *mtd_parts = 0;
static int                   mtd_parts_nb = 0;
#else
static struct mtd_partition physmap_partitions[] = {
/* Put your own partition definitions here */
	{
		name:		"Bootloader",  /* mtdblock0 */
		size:		MTD_BOOT_PART_SIZE,  /* 192K */
		offset:		0,
	}, {
		name:		"Config", /* mtdblock1 */
		size:		MTD_CONFIG_PART_SIZE,  /* 64K */
		offset:		MTDPART_OFS_APPEND
	}, {
		name:		"Factory", /* mtdblock2 */
		size:		MTD_FACTORY_PART_SIZE,  /* 64K */
		offset:		MTDPART_OFS_APPEND
	}, {
#ifdef CONFIG_RT2880_ROOTFS_IN_FLASH
		name:		"Kernel", /* mtdblock3 */
		size:		CONFIG_MTD_KERNEL_PART_SIZ, 
		offset:		MTDPART_OFS_APPEND,
	}, {
		name:		"RootFS", /* mtdblock4 */
		size:		MTD_ROOTFS_PART_SIZE,
		offset:		MTDPART_OFS_APPEND,
#else //CONFIG_RT2880_ROOTFS_IN_RAM
		name:		"Kernel", /* mtdblock3 */
		size:		MTD_KERN_PART_SIZE, 
		offset:		MTDPART_OFS_APPEND,
#endif //CONFIG_RT2880_ROOTFS_IN_FLASH
#ifdef CONFIG_DUAL_IMAGE
	}, {
		name:		"Kernel2", /* mtdblock4 */
		size:		MTD_KERN2_PART_SIZE, 
		offset:		MTD_KERN2_PART_OFFSET,
	}
#else
	}
#endif 
};

#define NUM_PARTITIONS	(sizeof(physmap_partitions)/sizeof(struct mtd_partition))

#endif
#endif

int __init init_physmap(void)
{
	static const char *rom_probe_types[] = { "cfi_probe", "jedec_probe", "map_rom", 0 };
	const char **type;

       	printk(KERN_NOTICE "physmap flash device: %x at %x\n", WINDOW_SIZE, WINDOW_ADDR);
	physmap_map.map_priv_1 = (unsigned long)ioremap(WINDOW_ADDR, WINDOW_SIZE);

	if (!physmap_map.map_priv_1) {
		printk("Failed to ioremap\n");
		return -EIO;
	}
	
	mymtd = 0;
	type = rom_probe_types;
	for(; !mymtd && *type; type++) {
		mymtd = do_map_probe(*type, &physmap_map);
	}
	if (mymtd) {
		mymtd->module = THIS_MODULE;

#ifdef CONFIG_MTD_PARTITIONS
#ifdef CONFIG_MTD_CMDLINE_PARTS
		mtd_parts_nb = parse_cmdline_partitions(mymtd, &mtd_parts, 
							"phys");
		if (mtd_parts_nb > 0)
		{
			printk(KERN_NOTICE 
			       "Using command line partition definition\n");
			add_mtd_partitions (mymtd, mtd_parts, mtd_parts_nb);
		}
#else
		if (NUM_PARTITIONS != 0) 
		{
			printk(KERN_NOTICE 
			       "Using physmap partition definition\n");
			add_mtd_partitions (mymtd, physmap_partitions, NUM_PARTITIONS);
		}

#endif
#endif
		return 0;
	}

	iounmap((void *)physmap_map.map_priv_1);
	return -ENXIO;
}

static void __exit cleanup_physmap(void)
{
	if (mymtd) {
		del_mtd_partitions(mymtd);
		map_destroy(mymtd);
	}
	if (physmap_map.map_priv_1) {
		iounmap((void *)physmap_map.map_priv_1);
		physmap_map.map_priv_1 = 0;
	}
}

module_init(init_physmap);
module_exit(cleanup_physmap);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Woodhouse <dwmw2@infradead.org>");
MODULE_DESCRIPTION("Generic configurable MTD map driver");

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
int ra_mtd_write_nm(char *name, loff_t to, size_t len, const u_char *buf)
{
	int ret = -1;
	size_t rdlen, wrlen;
	struct mtd_info *mtd;
	struct erase_info ei;
	u_char *bak = NULL;

	mtd = get_mtd_device_nm(NULL, name);
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

int ra_mtd_read_nm(char *name, loff_t from, size_t len, u_char *buf)
{
	int ret;
	size_t rdlen;
	struct mtd_info *mtd;

	mtd = get_mtd_device_nm(NULL, name);
	if (IS_ERR(mtd))
		return (int)mtd;

	ret = mtd->read(mtd, from, len, &rdlen, buf);
	if (rdlen != len)
		printk("warning: ra_mtd_read: rdlen is not equal to len\n");

	put_mtd_device(mtd);
	return ret;
}

EXPORT_SYMBOL(ra_mtd_write_nm);
EXPORT_SYMBOL(ra_mtd_read_nm);
