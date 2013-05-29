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
    acl_ioctl.c

    Abstract:

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
    Name        Date            Modification logs
    Steven Liu  2007-02-15      Initial version
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
#include "acl_ioctl.h"
#include "acl_policy.h"
#include "mtr_policy.h"
#include "util.h"

#ifdef  CONFIG_DEVFS_FS
#include <linux/devfs_fs_kernel.h>
static	devfs_handle_t devfs_handle;
#endif

int	acl_major =  ACL_MAJOR;

/* Rule Boundary Check */
uint32_t  AclBndryCheck(AclPlcyNode *NewNode)
{

	uint32_t CurAclEnd=PpeGetPreAclEnd();
	uint32_t MaxAclEnd=PpeGetPreMtrStr();
	uint32_t RuleSize=0;

	if(AclExistNode(NewNode)!=NULL){
		return ACL_SUCCESS;
	}

	switch(NewNode->RuleType)
	{
	case ACL_ADD_SMAC_DIP_ANY:
	case ACL_ADD_SIP_DIP_ANY:
		RuleSize=2; /* SMAC + DIP */
		break;
	case ACL_ADD_SMAC_DIP_TCP:
	case ACL_ADD_SMAC_DIP_UDP:
	case ACL_ADD_SIP_DIP_TCP:
	case ACL_ADD_SIP_DIP_UDP:
		RuleSize=3;  /* SMAC/SIP + DIP +DP */
		break;
	}

	if(CurAclEnd+RuleSize >= MaxAclEnd){
		return ACL_TBL_FULL;
	}else {
		return ACL_SUCCESS;
	}

}

uint32_t RunIoctlAddHandler(AclPlcyNode *NewNode, enum AclProtoType Proto)
{
	uint32_t Result;

	NewNode->Proto=Proto;
	Result = AclBndryCheck(NewNode); 
	if(Result !=ACL_TBL_FULL) {
		Result=AclAddNode(NewNode);
	}

	return Result;
}

uint32_t RunIoctlDelHandler(AclPlcyNode *DelNode, enum AclProtoType Proto)
{
	uint32_t Result;

	DelNode->Proto=Proto;
	Result=AclDelNode(DelNode);

	return Result;
}

int AclIoctl (struct inode *inode, struct file *filp,
                  unsigned int cmd, unsigned long arg)
{
    struct acl_args *opt=(struct acl_args *)arg;
    AclPlcyNode node;

    MacReverse(opt->mac);
    memcpy(node.Mac,opt->mac,ETH_ALEN);
    node.Method=opt->method;
    node.RuleType=cmd;
    node.SipS=opt->sip_s;
    node.SipE=opt->sip_e;
    node.DipS=opt->dip_s;
    node.DipE=opt->dip_e;
    node.DpS=opt->dp_s;
    node.DpE=opt->dp_e;

    switch(cmd) 
    {
    case ACL_ADD_SMAC_DIP_ANY:
    case ACL_ADD_SIP_DIP_ANY:
	    opt->result = RunIoctlAddHandler(&node, ACL_PROTO_ANY);
	    break;
    case ACL_DEL_SMAC_DIP_ANY:
    case ACL_DEL_SIP_DIP_ANY:
	    opt->result = RunIoctlDelHandler(&node, ACL_PROTO_ANY);
	    break;
    case ACL_ADD_SMAC_DIP_TCP:
    case ACL_ADD_SIP_DIP_TCP:
	    opt->result = RunIoctlAddHandler(&node, ACL_PROTO_TCP);
	    break;
    case ACL_DEL_SMAC_DIP_TCP:
    case ACL_DEL_SIP_DIP_TCP:
	    opt->result = RunIoctlDelHandler(&node, ACL_PROTO_TCP);
	    break;
    case ACL_ADD_SMAC_DIP_UDP:
    case ACL_ADD_SIP_DIP_UDP:
	    opt->result = RunIoctlAddHandler(&node, ACL_PROTO_UDP);
    case ACL_DEL_SMAC_DIP_UDP:
    case ACL_DEL_SIP_DIP_UDP:
	    opt->result = RunIoctlDelHandler(&node, ACL_PROTO_UDP);
	    break;
    case ACL_CLEAN_TBL:
	    AclCleanTbl();
	    break;
    default:
	    break;
    }

    return 0;
}

struct file_operations acl_fops = {
    ioctl:      AclIoctl,
};


int AclRegIoctlHandler(void)
{

#ifdef  CONFIG_DEVFS_FS
    if(devfs_register_chrdev(acl_major, ACL_DEVNAME , &acl_fops)) {
	NAT_PRINT(KERN_WARNING " acl: can't create device node - %s\n",ACL_DEVNAME);
	return -EIO;
    }

    devfs_handle = devfs_register(NULL, ACL_DEVNAME, DEVFS_FL_DEFAULT, acl_major, 0, 
	    S_IFCHR | S_IRUGO | S_IWUGO, &acl_fops, NULL);
#else
    int result=0;
    result = register_chrdev(acl_major, ACL_DEVNAME, &acl_fops);
    if (result < 0) {
	NAT_PRINT(KERN_WARNING "acl: can't get major %d\n",acl_major);
        return result;
    }

    if (acl_major == 0) {
	acl_major = result; /* dynamic */
    }
#endif

    return 0;
}

void AclUnRegIoctlHandler(void)
{

#ifdef  CONFIG_DEVFS_FS
    devfs_unregister_chrdev(acl_major, ACL_DEVNAME);
    devfs_unregister(devfs_handle);
#else
    unregister_chrdev(acl_major, ACL_DEVNAME);
#endif

}

