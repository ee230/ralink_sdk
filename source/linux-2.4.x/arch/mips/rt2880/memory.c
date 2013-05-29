/*
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999,2000 MIPS Technologies, Inc.  All rights reserved.
 *
 * ########################################################################
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * ########################################################################
 *
 * PROM library functions for acquiring/using memory descriptors given to
 * us from the YAMON.
 *
 */
#include <linux/config.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/bootmem.h>

#include <asm/bootinfo.h>
#include <asm/page.h>

#include <asm/rt2880/prom.h>

#define DEBUG

enum surfboard_memtypes {
	surfboard_dontuse,
	surfboard_rom,
	surfboard_ram,
};
struct prom_pmemblock mdesc[PROM_MAX_PMEMBLOCKS];

#ifdef DEBUG
static char *mtypes[3] = {
	"Dont use memory",
	"Used ROM memory",
	"Free RAM memory",
};
#endif

/* References to section boundaries */
extern char _end;

#define PFN_ALIGN(x)    (((unsigned long)(x) + (PAGE_SIZE - 1)) & PAGE_MASK)


struct prom_pmemblock * __init prom_getmdesc(void)
{
	/*
	*  Configures memory map based on Environment Variables set up by
	*  surfmilo (linux loader):
	*
	*    rambase = base of useable SDRAM
	*    ramsize = size of useable SDRAM
	*
	*  Assumptions made by linux:
	*    The kernel variables (.data) immediately follow the kernel code
	*      (.text) in memory.  The end of .data section, symbol _end,
	*      is used to denote the end of the kernel.
	*    The kernel code resides in ram, and all available ram exists
	*      after the kernel.
	*
	*  Note: Cpu jump vectors are in remapped ISRAM/VECMEM,
	*        which begins at 0000_0000
	*
	*        Linux memory map does not contain ISRAM since ISRAM
	*        does not support DMA transfers.
	*/

	char *env_str;
	unsigned int ramsize, rambase;

	env_str = prom_getenv("ramsize");
	if (!env_str) {
		ramsize = CONFIG_RT2880_RAM_SIZE * 1024 * 1024;
		prom_printf("ramsize = %d MBytes\n", CONFIG_RT2880_RAM_SIZE );
/* by bruce */
#if 0
#define SDRAM_32bit
#ifdef SDRAM_32bit
		prom_printf("ramsize not set, set to default (32Mb)\n");
		ramsize = 0x02000000;
#else
		prom_printf("ramsize not set, set to default (16Mb)\n");
		ramsize = 0x01000000;
#endif
#endif
	} else {
#ifdef DEBUG
		prom_printf("ramsize = %s\n", env_str);
#endif
		ramsize = simple_strtol(env_str, NULL, 0);
	}

	env_str = prom_getenv("rambase");
	if (!env_str) {
		prom_printf("rambase not set, set to default (0x08000000)\n");
		rambase = 0x08000000;
	} else {
#ifdef DEBUG
		prom_printf("rambase = %s\n", env_str);
#endif
		rambase = simple_strtol(env_str, NULL, 0);
	}

	memset(mdesc, 0, sizeof(mdesc));

	mdesc[0].type = surfboard_ram;
	mdesc[0].base = rambase;
	mdesc[0].size = ramsize;

	return &mdesc[0];
}

static int __init prom_memtype_classify (unsigned int type)
{
	switch (type) {
	case surfboard_ram:
		return BOOT_MEM_RAM;
	case surfboard_rom:
		return BOOT_MEM_ROM_DATA;
	default:
		return BOOT_MEM_RESERVED;
	}
}

void __init prom_meminit(void)
{
	struct prom_pmemblock *p;
#ifdef DEBUG
	struct prom_pmemblock *psave;
#endif

	p = prom_getmdesc();

#ifdef DEBUG
	prom_printf("MEMORY DESCRIPTOR dump:\n");
	psave = p;	/* Save p */
	while (p->size) {
		int i = 0;
		prom_printf("[%d,%p]: base<%08lx> size<%08lx> type<%s>\n",
			    i, p, p->base, p->size, mtypes[p->type]);
		p++;
		i++;
	}
	p = psave;	/* Restore p */

#endif

	while (p->size) {
		long type;
		unsigned long base, size;

		type = prom_memtype_classify (p->type);
		base = p->base;
		size = p->size;
		//prom_printf(" base = %x, size = %x, type = %x\n", base, size, type);
		add_memory_region(base, size, type);
                p++;
	}
}

void __init
prom_free_prom_memory (void)
{
	int i;
	unsigned long freed = 0;
	unsigned long addr;

	for (i = 0; i < boot_mem_map.nr_map; i++) {
		if (boot_mem_map.map[i].type != BOOT_MEM_ROM_DATA)
			continue;

		addr = boot_mem_map.map[i].addr;
		while (addr < boot_mem_map.map[i].addr
			      + boot_mem_map.map[i].size) {
			ClearPageReserved(virt_to_page(__va(addr)));
			set_page_count(virt_to_page(__va(addr)), 1);
			free_page((unsigned long)__va(addr));
			addr += PAGE_SIZE;
			freed += PAGE_SIZE;
		}
	}
	printk("Freeing prom memory: %ldkb freed\n", freed >> 10);
}
