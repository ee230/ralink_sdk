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
    acl_ioctl.h

    Abstract:

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
    Name        Date            Modification logs
    Steven Liu  2007-02-15      Initial version
*/

#ifndef	__ACL_IOCTL_H__
#define	__ACL_IOCTL_H__

#include "common.h"

#define ACL_ADD_SMAC_DIP_ANY   		(0)
#define ACL_ADD_SMAC_DIP_TCP   		(1)
#define ACL_ADD_SMAC_DIP_UDP   		(2)
#define ACL_DEL_SMAC_DIP_ANY   		(3)
#define ACL_DEL_SMAC_DIP_TCP   		(4)
#define ACL_DEL_SMAC_DIP_UDP   		(5)

#define ACL_ADD_SIP_DIP_ANY    		(6)
#define ACL_ADD_SIP_DIP_TCP    		(7)
#define ACL_ADD_SIP_DIP_UDP    		(8)
#define ACL_DEL_SIP_DIP_ANY    		(9)
#define ACL_DEL_SIP_DIP_TCP    		(10)
#define ACL_DEL_SIP_DIP_UDP    		(11)
#define ACL_CLEAN_TBL    		(12)

#define ACL_DEVNAME			"acl0"
#define ACL_MAJOR			(230)

enum AclRuleMethod {
	ACL_ALLOW_RULE=0,
	ACL_DENY_RULE=1
};

enum AclRuleOpt {
	ACL_RULE_ADD=0,
	ACL_RULE_DEL=1
};

enum AclProtoType {
	ACL_PROTO_ANY=0,
	ACL_PROTO_TCP=1,
	ACL_PROTO_UDP=2
};

enum AclResult {
	ACL_SUCCESS=0,
	ACL_FAIL=1,
	ACL_TBL_FULL=2
};

struct acl_args {
	unsigned char  mac[6];
	enum AclResult result; /* ioctl result */
	enum AclRuleMethod  method; /* Deny, Allow */
	unsigned long  sip_s; /* start of sip */
	unsigned long  sip_e; /* end of sip */
	unsigned long  dip_s; /* start of dip */
	unsigned long  dip_e; /* end of dip */
	unsigned short dp_s; /* start of dp */
	unsigned short dp_e; /* end of dp */
};

int AclRegIoctlHandler(void);
void AclUnRegIoctlHandler(void);

#endif
