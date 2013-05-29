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
    foe_fdb.h

    Abstract:

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
    Name        Date            Modification logs
    Steven Liu  2006-10-06      Initial version
*/

#include "foe_fdb.h"

#ifndef _RA_NAT_WANTED
#define _RA_NAT_WANTED

/*
 * TYPEDEFS AND STRUCTURES
 */
enum DstPort {
    DP_PCI,
    DP_GMAC,
    DP_WLAN,
    MAX_IF_NUM
};

typedef struct 
{
    uint16_t  MAGIC_TAG;
    uint32_t  FOE_Entry: 14;
    uint32_t  FVLD:1;
    uint32_t  ALG_RXIF:1;     /* ALG / RxIf= WLAN or PCI */
    uint32_t  AI:8;
    uint32_t  SP:3;
    uint32_t  AIS:1;
    uint32_t  L4F:1;
    uint32_t  IPF:1;
    uint32_t  L4FVLD:1;
    uint32_t  IPFVLD:1;
} PdmaRxDescInfo4;

/*
 * DEFINITIONS AND MACROS
 */

/*
 *    2bytes	    4bytes 
 * +-----------+-------------------+
 * | Magic Tag | RX/TX Desc info4  |
 * +-----------+-------------------+
 * |<------FOE Flow Info---------->|
 */
#define FOE_MAGIC_PCI		    0x7273
#define FOE_MAGIC_WLAN		    0x7274
#define FOE_MAGIC_GE		    0x7275
#define FOE_MAGIC_PPE		    0x7276

#define FOE_MAGIC_TAG(skb)	    ((PdmaRxDescInfo4 *)((skb)->head))->MAGIC_TAG
#define FOE_ENTRY_NUM(skb)	    ((PdmaRxDescInfo4 *)((skb)->head))->FOE_Entry
#define FOE_FVLD(skb)		    ((PdmaRxDescInfo4 *)((skb)->head))->FVLD
				    //RxHandler: RxIf(WLAN/PCI), TxHandler:ALG Interested?
#define FOE_ALG_RXIF(skb)	    ((PdmaRxDescInfo4 *)((skb)->head))->ALG_RXIF 
#define FOE_AI(skb)		    ((PdmaRxDescInfo4 *)((skb)->head))->AI
#define FOE_SP(skb)		    ((PdmaRxDescInfo4 *)((skb)->head))->SP
#define FOE_AIS(skb)		    ((PdmaRxDescInfo4 *)((skb)->head))->AIS
#define FOE_L4F(skb)		    ((PdmaRxDescInfo4 *)((skb)->head))->L4F
#define FOE_IPF(skb)		    ((PdmaRxDescInfo4 *)((skb)->head))->IPF
#define FOE_L4FVLD(skb)		    ((PdmaRxDescInfo4 *)((skb)->head))->L4FVLD
#define FOE_IPFVLD(skb)		    ((PdmaRxDescInfo4 *)((skb)->head))->IPFVLD

/* 
 * HW_NAT module will modify mss field in tcp option header 
 * to avoid fragmentation in WAN port.
 */
#define MAX_TCP_MSS		    1448
	

/*
 * EXPORT FUNCTION
 */
int32_t GetPppoeSid(struct sk_buff *skb, uint32_t vlan_gap, uint16_t *sid, uint32_t is_in);
void AclInsTestEntry(void);
void AcInsTestEntry(void);
void MtrInsTestEntry(void);
void FoeInsTestEntry(void);

#endif
