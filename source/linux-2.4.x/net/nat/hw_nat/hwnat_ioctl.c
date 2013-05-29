/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2006, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attempt
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

    Module Name:
    hwnat_ioctl.c

    Abstract:

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
    Name        Date            Modification logs
    Steven Liu  2006-10-06      Initial version
*/

#include <linux/init.h>
#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>   
#include <linux/fs.h>       
#include <linux/errno.h>    
#include <linux/types.h>    
#include <linux/proc_fs.h>
#include <linux/fcntl.h>    
#include <asm/system.h>     
#include <linux/wireless.h>
#include <asm/uaccess.h>

#include "frame_engine.h"
#include "hwnat_ioctl.h"
#include "foe_fdb.h"
#include "util.h"

#ifdef  CONFIG_DEVFS_FS
#include <linux/devfs_fs_kernel.h>
static	devfs_handle_t devfs_handle;
#endif

int	hw_nat_major =  HW_NAT_MAJOR;
extern int DebugLevel;

int HwNatIoctl (struct inode *inode, struct file *filp,
                  unsigned int cmd, unsigned long arg)
{
    struct hwnat_args *opt=(struct hwnat_args *)arg;
    struct hwnat_tuple *opt2=(struct hwnat_tuple *)arg;

    switch(cmd) 
    {
    case HW_NAT_ADD_ENTRY:
	opt2->result = FoeAddEntry(opt2);
	break;
    case HW_NAT_DEL_ENTRY:
	opt2->result = FoeDelEntry(opt2);
	break;
    case HW_NAT_GET_ALL_ENTRIES:
	 opt->result = FoeGetAllEntries(opt);
	break;
    case HW_NAT_BIND_ENTRY:
	opt->result = FoeBindEntry(opt);
	break;
    case HW_NAT_UNBIND_ENTRY:
	opt->result = FoeUnBindEntry(opt);
	break;
    case HW_NAT_INVALID_ENTRY:
	opt->result = FoeDelEntryByNum(opt->entry_num);
	break;
    case HW_NAT_DUMP_ENTRY: /* For Debug */
	FoeDumpEntry(opt->entry_num);
	break;
    case HW_NAT_DEBUG: /* For Debug */
	DebugLevel=opt->debug;
	break;
    default:
	break;
    }

    return 0;
}

struct file_operations hw_nat_fops = {
    ioctl:      HwNatIoctl,
};


int PpeRegIoctlHandler(void)
{

#ifdef  CONFIG_DEVFS_FS
    if(devfs_register_chrdev(hw_nat_major, HW_NAT_DEVNAME , &hw_nat_fops)) {
	NAT_PRINT(KERN_WARNING " hw_nat: can't create device node - %s\n",HW_NAT_DEVNAME);
	return -EIO;
    }

    devfs_handle = devfs_register(NULL, HW_NAT_DEVNAME, DEVFS_FL_DEFAULT, hw_nat_major, 0, 
	    S_IFCHR | S_IRUGO | S_IWUGO, &hw_nat_fops, NULL);
#else
    int result=0;
    result = register_chrdev(hw_nat_major, HW_NAT_DEVNAME, &hw_nat_fops);
    if (result < 0) {
	NAT_PRINT(KERN_WARNING "hw_nat: can't get major %d\n",hw_nat_major);
        return result;
    }

    if (hw_nat_major == 0) {
	hw_nat_major = result; /* dynamic */
    }
#endif

    return 0;
}



void PpeUnRegIoctlHandler(void)
{

#ifdef  CONFIG_DEVFS_FS
    devfs_unregister_chrdev(hw_nat_major, HW_NAT_DEVNAME);
    devfs_unregister(devfs_handle);
#else
    unregister_chrdev(hw_nat_major, HW_NAT_DEVNAME);
#endif

}

