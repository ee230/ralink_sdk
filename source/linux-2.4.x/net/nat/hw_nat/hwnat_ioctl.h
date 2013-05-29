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
    hwnat_ioctl.h

    Abstract:

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
    Name        Date            Modification logs
    Steven Liu  2006-10-06      Initial version
*/

#ifndef	__HW_NAT_IOCTL_H__
#define	__HW_NAT_IOCTL_H__

#define HW_NAT_ADD_ENTRY    		(0x01)
#define HW_NAT_DEL_ENTRY    		(0x02)
#define HW_NAT_DUMP_ENTRY    		(0x03)
#define HW_NAT_GET_ALL_ENTRIES 		(0x04)
#define HW_NAT_BIND_ENTRY		(0x05)
#define HW_NAT_UNBIND_ENTRY		(0x06)
#define HW_NAT_INVALID_ENTRY		(0x07)
#define HW_NAT_DEBUG	   		(0x08)

#define HW_NAT_DEVNAME			"hwnat0"
#define HW_NAT_MAJOR			(220)

enum hwnat_status {
	HWNAT_SUCCESS=0,
	HWNAT_FAIL=1,
	HWNAT_ENTRY_NOT_FOUND=2
};

struct hwnat_tuple {
	unsigned short  hash_index;
	unsigned short  is_udp;
	unsigned int    sip;
	unsigned int    dip;
	unsigned short  sport;
	unsigned short  dport;
	unsigned int    new_sip;
	unsigned int    new_dip;
	unsigned short  new_sport;
	unsigned short  new_dport;
	unsigned short  vlan1;
	unsigned short  pppoe_id;
	unsigned char	dmac[6];
	unsigned char	smac[6];
	unsigned char   vlan1_act:2;
	unsigned char   snap_act:2;
	unsigned char   pppoe_act:2;
	unsigned char	dst_port:2;
    	enum hwnat_status	result;
};

struct hwnat_args {
    unsigned int    	debug:1;
    unsigned int    	entry_state:2; /* invalid=0, unbind=1, bind=2, fin=3 */
    enum hwnat_status	result;
    unsigned int    	entry_num:16;
    unsigned int    	num_of_entries:16;
    struct hwnat_tuple  entries[0];
};

int PpeRegIoctlHandler(void);
void PpeUnRegIoctlHandler(void);

#endif
