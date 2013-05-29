/*
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999,2000 MIPS Technologies, Inc.  All rights reserved.
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
 * PROM library initialisation code.
 */
#include <linux/config.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/serialP.h>

#include <asm/bootinfo.h>
#include <asm/io.h>
#include <asm/serial.h>
#include <asm/rt2880/prom.h>
#include <asm/rt2880/generic.h>
#include <asm/rt2880/surfboard.h>
#include <asm/rt2880/surfboardint.h>
extern unsigned long surfboard_sysclk;
extern unsigned long mips_machgroup;

/* Environment variable */
typedef struct {
	char *name;
	char *val;
} t_env_var;

int prom_argc;
int *_prom_argv, *_prom_envp;

/* PROM version of rs_table - needed for Serial Console */
struct serial_state prom_rs_table[] = {
       SERIAL_PORT_DFNS        /* Defined in serial.h */
};

/*
 * YAMON (32-bit PROM) pass arguments and environment as 32-bit pointer.
 * This macro take care of sign extension, if running in 64-bit mode.
 */
#define prom_envp(index) ((char *)(((int *)(int)_prom_envp)[(index)]))


int init_debug = 0;
u32 mips_cpu_feq;

char *prom_getenv(char *envname)
{
	/*
	 * Return a pointer to the given environment variable.
	 * In 64-bit mode: we're using 64-bit pointers, but all pointers
	 * in the PROM structures are only 32-bit, so we need some
	 * workarounds, if we are running in 64-bit mode.
	 */
	int i, index=0;
	// Dennis Lee +
	return NULL;
	// 
	i = strlen(envname);

	while (prom_envp(index)) {
		if(strncmp(envname, prom_envp(index), i) == 0) {
			return(prom_envp(index+1));
		}
		index += 2;
	}

	return NULL;
}

static inline unsigned char str2hexnum(unsigned char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return 0; /* foo */
}

static inline void str2eaddr(unsigned char *ea, unsigned char *str)
{
	int i;

	for (i = 0; i < 6; i++) {
		unsigned char num;

		if((*str == '.') || (*str == ':'))
			str++;
		num = str2hexnum(*str++) << 4;
		num |= (str2hexnum(*str++));
		ea[i] = num;
	}
}

int get_ethernet_addr(char *ethernet_addr)
{
        char *ethaddr_str;

        ethaddr_str = prom_getenv("ethaddr");
	if (!ethaddr_str) {
	        printk("ethaddr not set in boot prom\n");
		return -1;
	}
	str2eaddr(ethernet_addr, ethaddr_str);

	if (init_debug > 1) {
	        int i;
		printk("get_ethernet_addr: ");
	        for (i=0; i<5; i++)
		        printk("%02x:", (unsigned char)*(ethernet_addr+i));
		printk("%02x\n", *(ethernet_addr+i));
	}

	return 0;
}

void prom_init_sysclk(void)
{
 /* New add by kaiker */
	u32 reg;
	u8	clk_sel;
	
#if defined(CONFIG_RT2880_FPGA)
	mips_cpu_feq = 25000000; /* FPGA run in 25MHZ */
#else
	reg = (*((volatile u32 *)(0xA0300010)));	
	/* 
	 * CPU_CLK_SEL (bit 21:20)
	 *	11: 300Mhz
	 *      10: 280Mhz
	 *      01: 250Mhz
	 *      00: 233Mhz
	 */

	clk_sel = (reg>>20) & 0x03;
	switch(clk_sel) {
#if defined (CONFIG_RALINK_RT2880_SHUTTLE)
		case 0:
			mips_cpu_feq = (233333333);
			break;
		case 1:
			mips_cpu_feq = (250000000);
			break;
		case 2:
			mips_cpu_feq = (266666666);
			break;
		case 3:
			mips_cpu_feq = (280000000);
			break;
#elif defined (CONFIG_RALINK_RT2880_MP) 
		case 0:
			mips_cpu_feq = (250000000);
			break;
		case 1:
			mips_cpu_feq = (266666666);
			break;
		case 2:
			mips_cpu_feq = (280000000);
			break;
		case 3:
			mips_cpu_feq = (300000000);
			break;
#else
#error Please Choice Chip Version (Shuttle/MP)
#endif
	}

#endif

	surfboard_sysclk = mips_cpu_feq/2;
	printk("\n The CPU feqenuce set to %d MHz\n",mips_cpu_feq / 1000 / 1000);


/* remove by bruce */
#if 0
	unsigned long tempClk = 0;
	char *argptr;
	/* config system clock */
	argptr = prom_getcmdline();

	if ((argptr = strstr(argptr, "sysclk=")) != NULL) 
	{
		argptr += strlen("sysclk=");

		while ((*argptr >= '0') && (*argptr <= '9'))
		{
			tempClk = (tempClk * 10) + ((*argptr) - '0');
			argptr++;
		}


		if (tempClk > 0)
			surfboard_sysclk = tempClk;
		else
			surfboard_sysclk = SURFBOARD_SYSTEM_CLOCK;
	}
#endif

}

/*
** This function sets up the local prom_rs_table used only for the fake console
** console (mainly prom_printf for debug display and no input processing)
** and also sets up the global rs_table used for the actual serial console.
** To get the correct baud_base value, prom_init_sysclk() must be called before
** this function is called.
*/
void prom_init_serial_port(void)
{
	struct serial_struct serial_info;


	/* config serial port -- info common to both uarts */
	serial_info.type = 0;
	serial_info.xmit_fifo_size = 0;
	serial_info.custom_divisor = 0;
	serial_info.close_delay = 0;
	serial_info.io_type = 0;
	serial_info.hub6 = 0;
	serial_info.closing_wait = 0;
	serial_info.closing_wait2 = 0;
	serial_info.iomem_base = 0;
	serial_info.iomem_reg_shift = 0;
	serial_info.port_high = 0;
	serial_info.flags = STD_COM_FLAGS;
	serial_info.baud_base = (surfboard_sysclk / SURFBOARD_BAUD_DIV);


	/* uart0 info */
	serial_info.line = 0;	/* line 0 of rs_table */
	serial_info.port = SURFBOARD_UART_REGS_BASE;
	serial_info.irq = SURFBOARDINT_UART;

	/* Initialize global serial port table */
	/* Very important if surfboard_sysclk is differnt than default defined
	** in surfboard.h.
	*/
	if (early_serial_setup((struct serial_struct *) &serial_info) != 0)
		printk("Serial Port 0 Config failed\n"); 

	/* Initialize local serial port table */
	prom_rs_table[serial_info.line].magic = 0;
	prom_rs_table[serial_info.line].baud_base = serial_info.baud_base;
	prom_rs_table[serial_info.line].port = serial_info.port;
	prom_rs_table[serial_info.line].irq = serial_info.irq;
	prom_rs_table[serial_info.line].flags = serial_info.flags;


	/* uart1 info */
	serial_info.line = 1;	/* line 1 of rs_table */
	serial_info.port = SURFBOARD_UART1_REGS_BASE;
	serial_info.irq = SURFBOARDINT_UART1;

	/* Initialize global serial port table */
	/* Very important if surfboard_sysclk is differnt than default defined
	** in surfboard.h.
	*/
	if (early_serial_setup((struct serial_struct *) &serial_info) != 0)
		printk("Serial Port 1 Config failed\n"); 

	/* Initialize local serial port table */
	prom_rs_table[serial_info.line].magic = 0;
	prom_rs_table[serial_info.line].baud_base = serial_info.baud_base;
	prom_rs_table[serial_info.line].port = serial_info.port;
	prom_rs_table[serial_info.line].irq = serial_info.irq;
	prom_rs_table[serial_info.line].flags = serial_info.flags;
}

int prom_get_ttysnum(void)
{
	char *argptr;
	int ttys_num = 0;       /* default */

	/* get ttys_num to use with the fake console/prom_printf */
	argptr = prom_getcmdline();

	if ((argptr = strstr(argptr, "console=ttyS")) != NULL)
	{
                argptr += strlen("console=ttyS");

                if (argptr[0] == '0')           /* ttyS0 */
                        ttys_num = 0;           /* happens to be rs_table[0] */
                else if (argptr[0] == '1')      /* ttyS1 */
                        ttys_num = 1;           /* happens to be rs_table[1] */
	}

	return (ttys_num);
}

void prom_init_mac(void)
{
	//default value
	unsigned int high=0x00AB;
	unsigned int low=0x12349999;

#ifdef SURFBOARD_1
	high=0x00AB; 	low=0x12345601;
#endif
#ifdef SURFBOARD_2
	high=0x00AB;	low=0x12345602;
#endif
#ifdef SURFBOARD_3
	high=0x00AB;	low=0x12345603;
#endif

#ifdef DIAMONDBOARD_1
	high=0x00AB;	low=0x12345611;
#endif
#ifdef DIAMONDBOARD_2
	high=0x00AB;	low=0x12345612;
#endif
#ifdef DIAMONDBOARD_3
	high=0x00AB;	low=0x12345613;
#endif

#ifdef BUCKYBOARD_1
	high=0x00AB;	low=0x12345621;
#endif
#ifdef BUCKYBOARD_2
	high=0x00AB;	low=0x12345622;
#endif
#ifdef BUCKYBOARD_3
	high=0x00AB;	low=0x12345623;
#endif

	do 
	{
		*(volatile unsigned int *) (mips_io_port_base + 0x301C00 + 0x4 ) = high;
	} while (0);
	do 
	{
		*(volatile unsigned int *) (mips_io_port_base + 0x301C00 + 0x8 ) = low;
	} while (0);
	low++;
	do 
	{
		*(volatile unsigned int *) (mips_io_port_base + 0x302000 + 0x4 ) = high;
	} while (0);
	do 
	{
		*(volatile unsigned int *) (mips_io_port_base + 0x302000 + 0x8 ) = low;
	} while (0);
}

/* change prototype by bobtseng, 2006.1.3. */
#if 0
int __init prom_init(int argc, char **argv, char **envp)
#endif
int __init prom_init()
{

/* change parameter by bobtseng, 2006.1.3. */
#if 0
	prom_argc = argc;
	_prom_argv = (int *)argv;
	_prom_envp = (int *)envp;
#endif

	mips_machgroup = MACH_GROUP_RT2880;

	set_io_port_base(KSEG1);

	prom_init_cmdline();
	prom_init_sysclk();
	prom_init_serial_port();  /* Needed for Serial Console */
	// prom_init_mac(); // remove for test, bobtseng 2006.1.3.
	prom_setup_printf(prom_get_ttysnum());
#if defined(CONFIG_RT2880_FPGA) 
	prom_printf("\nTHIS IS FPGA\n");
#elif defined(CONFIG_RT2880_ASIC)
	prom_printf("\nTHIS IS ASIC\n");
#else
#error Please Choice Chip Type
#endif

	prom_meminit();
	return 0;
}

