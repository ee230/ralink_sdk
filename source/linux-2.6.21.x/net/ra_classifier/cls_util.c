#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <asm/mipsregs.h>
#include <asm/div64.h>

#include "cls_util.h"


#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define GCC_IMM_ASM "n"
#define GCC_REG_ACCUM "$0"
#else
#define GCC_IMM_ASM "rn"
#define GCC_REG_ACCUM "accum"
#endif

/*
* No traps on overflows for any of these...
*/

uint32  div64_32(uint64 num, uint32 denum)
{
	uint32 res = 0;
	
	do_div64_32(res, num>>32, num, denum);
	
	return res;
}


unsigned long GetDeltaTime(unsigned long cur_cyc, unsigned long prev_cyc)
{
	unsigned long delta;
	
#if defined(CONFIG_RALINK_EXTERNAL_TIMER)
	if(prev_cyc <= cur_cyc)
		delta = (cur_cyc - prev_cyc)*20;
	else
		delta = (0x010000 - (prev_cyc - cur_cyc))*20;
#else
	if(prev_cyc <= cur_cyc)
		delta = (cur_cyc - prev_cyc)/((mips_cpu_feq>>1)/(1000));
	else
		delta = (0xFFFFFFFF - (prev_cyc - cur_cyc))/((mips_cpu_feq>>1)/(1000));
#endif
	return delta;	
	
}

unsigned long GetMicroDeltaTime(unsigned long cur_cyc, unsigned long prev_cyc)
{
	unsigned long delta;
	
#if defined(CONFIG_RALINK_EXTERNAL_TIMER)
#else
	if(prev_cyc <= cur_cyc)
		delta = (cur_cyc - prev_cyc)/((mips_cpu_feq>>1)/(1000*1000));
	else
		delta = (0xFFFFFFFF - (prev_cyc - cur_cyc))/((mips_cpu_feq>>1)/(1000*1000));
#endif
	return delta;	
	
}

EXPORT_SYMBOL(GetDeltaTime);
