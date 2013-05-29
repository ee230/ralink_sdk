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
 * Setting up the clock on the MIPS boards.
 */

#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/timex.h>

#include <asm/mipsregs.h>
#include <asm/mipsmtregs.h>
#include <asm/hardirq.h>
#include <asm/irq.h>
#include <asm/div64.h>
#include <asm/cpu.h>
#include <asm/time.h>

#include <asm/tc3162/tc3162.h>

unsigned long cpu_khz;

static int mips_cpu_timer_irq;
extern int cp0_perfcount_irq;
extern void smtc_timer_broadcast(int);

static void delay1ms(int ms)
{
	volatile uint32 timer_now, timer_last;
	volatile uint32 tick_acc;
	uint32 one_tick_unit = 1 * SYS_HCLK * 1000 / 2;
	volatile uint32 tick_wait = ms * one_tick_unit; 
	volatile uint32 timer1_ldv = VPint(CR_TIMER1_LDV);

	tick_acc = 0;
	timer_last = VPint(CR_TIMER1_VLR);
	do {
		timer_now = VPint(CR_TIMER1_VLR);
	  	if (timer_last >= timer_now) 
	  		tick_acc += timer_last - timer_now;
		else
			tick_acc += timer1_ldv - timer_now + timer_last;
		timer_last = timer_now;
	} while (tick_acc < tick_wait);
}

void
timer_Configure(
	uint8  timer_no, 
	uint8 timer_enable, 
	uint8 timer_mode, 
	uint8 timer_halt
)
{
    uint32 word,word1;
    word = VPint(CR_TIMER_CTL);
    word1 = (timer_enable << timer_no)|(timer_mode << (timer_no + 8))|(timer_halt << (timer_no + 26));
    word |= word1;
    VPint (CR_TIMER_CTL)=word;    
} 

void 
timerSet(
	uint32 timer_no,
	uint32 timerTime, 
	uint32 enable,
	uint32 mode, 
	uint32 halt
)
{   
    uint32 word;

    word = (timerTime * SYS_HCLK) * 1000 / 2; 
    timerLdvSet(timer_no,word);
    timerCtlSet(timer_no,enable,mode,halt);
}

void
timer_WatchDogConfigure (
	uint8 tick_enable, 
	uint8 watchdog_enable
)
{
    uint32 word;
    word = VPint(CR_TIMER_CTL);
	word &= 0xfdffffdf;
    word |= ( tick_enable << 5)|(watchdog_enable<<25);
    VPint (CR_TIMER_CTL)=word;
}

#define get_current_vpe()   \
	((read_c0_tcbind() >> TCBIND_CURVPE_SHIFT) & TCBIND_CURVPE)

extern void tc3162_enable_irq(unsigned int irq);

static void mips_timer_dispatch(void)
{
	do_IRQ(SI_TIMER_INT);
}

static void mips_perf_dispatch(void)
{
	do_IRQ(cp0_perfcount_irq);
}

extern int (*perf_irq)(void);

/*
 * Estimate CPU frequency.  Sets mips_hpt_frequency as a side-effect
 */
static unsigned int __init estimate_cpu_frequency(void)
{
	unsigned int count;
	unsigned long flags;
	unsigned int start;

	local_irq_save(flags);

	/* Start r4k counter. */
	start = read_c0_count();

	/* delay 1 second */
	delay1ms(1000);

	count = read_c0_count() - start;

	/* restore interrupts */
	local_irq_restore(flags);

	count += 5000;    /* round */
	count -= count%10000;

	mips_hpt_frequency = count;

	/* on 34K, 2 cycles per count */
	count *= 2;

	return count;
}

irqreturn_t mips_perf_interrupt(int irq, void *dev_id)
{
	return perf_irq();
}

static struct irqaction perf_irqaction = {
	.handler = mips_perf_interrupt,
	.flags = IRQF_DISABLED | IRQF_PERCPU,
	.name = "performance",
};

static int vpe1_timer_installed = 0;

unsigned int __cpuinit get_c0_compare_int(void)
{
	if ((get_current_vpe()) && !vpe1_timer_installed) {
		tc3162_enable_irq(SI_TIMER1_INT);
		vpe1_timer_installed++;
	}

	if (vpe1_timer_installed == 0) {
		if (cpu_has_veic) 
			set_vi_handler(SI_TIMER_INT, mips_timer_dispatch);
	}
	mips_cpu_timer_irq = SI_TIMER_INT;

	return mips_cpu_timer_irq;
}

void __init plat_time_init(void)
{
	unsigned int est_freq;

	timerSet(1, TIMERTICKS_10MS, ENABLE, TIMER_TOGGLEMODE, TIMER_HALTDISABLE);

	est_freq = estimate_cpu_frequency ();

	printk("CPU frequency %d.%02d MHz\n", est_freq/1000000,
	       (est_freq%1000000)*100/1000000);

    cpu_khz = est_freq / 1000;
}

