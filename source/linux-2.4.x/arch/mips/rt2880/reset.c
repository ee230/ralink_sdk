/*
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
 */
#include <linux/config.h>

#include <asm/reboot.h>
#include <asm/rt2880/generic.h>

static void mips_machine_restart(char *command);
static void mips_machine_halt(void);
static void mips_machine_power_off(void);

static void mips_machine_restart(char *command)
{
	*(volatile unsigned int*)(SOFTRES_REG) = GORESET;
}

static void mips_machine_halt(void)
{
	*(volatile unsigned int*)(SOFTRES_REG) = GORESET;
}

static void mips_machine_power_off(void)
{
	*(volatile unsigned int*)(POWER_DIR_REG) = POWER_DIR_OUTPUT;
	*(volatile unsigned int*)(POWER_POL_REG) = 0;
	*(volatile unsigned int*)(POWEROFF_REG) = POWEROFF;
}


void mips_reboot_setup(void)
{
	_machine_restart = mips_machine_restart;
	_machine_halt = mips_machine_halt;
	_machine_power_off = mips_machine_power_off;
}
