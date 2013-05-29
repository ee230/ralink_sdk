/*
 * Empty function.
  * If your want to support this chip for ethtool, just overwrite this file with original .c file.
   * ex. cp amd8111e.c.orig amd8111e.c
    */
    
#include <stdio.h>
#include "ethtool-util.h"

int
ixgb_dump_regs(struct ethtool_drvinfo *info, struct ethtool_regs *regs)
{
	return 0;
}
