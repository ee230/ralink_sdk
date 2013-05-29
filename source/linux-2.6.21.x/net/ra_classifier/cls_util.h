#include <linux/types.h>

#define uint32 unsigned long
#define uint64 unsigned long long

/*
* No traps on overflows for any of these...
*/

uint32  div64_32(uint64 num, uint32 denum);
extern unsigned long GetDeltaTime(unsigned long cur_cyc, unsigned long prev_cyc);
extern unsigned long GetMicroDeltaTime(unsigned long cur_cyc, unsigned long prev_cyc);
extern int mips_cpu_feq;  
