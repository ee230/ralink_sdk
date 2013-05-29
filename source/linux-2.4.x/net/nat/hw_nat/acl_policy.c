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
    
    acl_policy.c

    Abstract:

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
    Name        Date            Modification logs
    Steven Liu  2007-01-24      Initial version
*/

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/list.h>
#include <linux/if_ether.h>

#include "util.h"
#include "acl_policy.h"
#include "frame_engine.h"

AclPlcyNode AclPlcyList= { .List = LIST_HEAD_INIT(AclPlcyList.List)};

uint32_t SyncAclTbl(void)
{
	struct list_head *pos = NULL, *tmp;
	AclPlcyNode *node = NULL;

	PpeSetPreAclEbl(0);
	list_for_each_safe(pos, tmp, &AclPlcyList.List) {
		node = list_entry(pos, AclPlcyNode, List);

		switch(node->RuleType)
		{
		case ACL_ADD_SMAC_DIP_ANY:
		case ACL_ADD_SMAC_DIP_TCP:
		case ACL_ADD_SMAC_DIP_UDP:
			AclInsSmacDipDp(node);
			break;
		case ACL_ADD_SIP_DIP_ANY:
		case ACL_ADD_SIP_DIP_TCP:
		case ACL_ADD_SIP_DIP_UDP:
			AclInsSipDipDp(node);
			break;
		}
	}

	/* Empty Rule */
	if(node==NULL) {
		printk("ACL Table All Empty!\n");
		return ACL_SUCCESS;
	}

	if(node->Method==ACL_ALLOW_RULE) {
		AclInsDflAllow(); /* insert my/broadcast mac */
		AclInsDflDeny(); /* if there is no entry matched, drop the packet */
	}

	PpeSetPreAclEbl(1);
	return ACL_SUCCESS;
}

AclPlcyNode *AclExistNode(AclPlcyNode *NewNode)
{
	struct list_head *pos = NULL, *tmp;
	AclPlcyNode *node = NULL;

	list_for_each_safe(pos, tmp, &AclPlcyList.List) {
		node = list_entry(pos, AclPlcyNode, List);

		switch(NewNode->RuleType)
		{
		case ACL_ADD_SMAC_DIP_ANY: /* primary key = smac + dip + protocol */
			if(strcmp(node->Mac,NewNode->Mac)==0 &&
					node->DipS==NewNode->DipS && node->DipE==NewNode->DipE &&
					node->Proto==NewNode->Proto) {
				return node;
			}
			break;
		case ACL_ADD_SMAC_DIP_TCP:
		case ACL_ADD_SMAC_DIP_UDP: /* primary key = smac + dip +dp + protocol */
			if(strcmp(node->Mac,NewNode->Mac)==0 &&
					(node->DipS==NewNode->DipS) && (node->DipE==NewNode->DipE) &&
					(node->DpS==NewNode->DpS) && (node->Proto==NewNode->Proto)) {
				return node;
			}
			break;
		case ACL_ADD_SIP_DIP_ANY:  /* primary key = sip + dip + protocol */
			if(node->SipS==NewNode->SipS && node->SipE==NewNode->SipE &&
					node->DipS==NewNode->DipS && node->DipE==NewNode->DipE &&
					node->Proto==NewNode->Proto) {
				return node;
			}
			break;
		case ACL_ADD_SIP_DIP_TCP:
		case ACL_ADD_SIP_DIP_UDP: /* primary key = sip + dip + dp + protocol */
			if(node->SipS==NewNode->SipS && node->SipE==NewNode->SipE &&
					node->DipS==NewNode->DipS && node->DipE==NewNode->DipE &&
					node->DpS==NewNode->DpS && node->Proto==NewNode->Proto) {
				return node;
			}
			break;
		}
	}

	return NULL;
}

uint32_t AclAddNode(AclPlcyNode *NewNode)
{
	AclPlcyNode *node = NULL;

	if((node=AclExistNode(NewNode))) {
		return ACL_SUCCESS;
	}

	node = (AclPlcyNode *)kmalloc(sizeof(AclPlcyNode), GFP_ATOMIC);

	if(node == NULL) {
		return ACL_FAIL;
	}

	memcpy(node, NewNode, sizeof(AclPlcyNode));
	list_add_tail( &node->List, &AclPlcyList.List);

	return SyncAclTbl();
}

uint32_t AclDelNode(AclPlcyNode *DelNode)
{
	struct list_head *pos = NULL, *tmp;
	AclPlcyNode *node;

	list_for_each_safe(pos, tmp, &AclPlcyList.List) {
		node = list_entry(pos, AclPlcyNode, List);

		switch(DelNode->RuleType)
		{
		case ACL_DEL_SMAC_DIP_ANY:
			if( (memcmp(node->Mac, DelNode->Mac, ETH_ALEN)==0) &&
					node->DipS== DelNode->DipS && node->DipE== DelNode->DipE ){
				goto found;
			}
			break;
		case ACL_DEL_SMAC_DIP_TCP:
		case ACL_DEL_SMAC_DIP_UDP:
			if( (memcmp(node->Mac, DelNode->Mac, ETH_ALEN)==0) &&
					node->DipS== DelNode->DipS && node->DipE== DelNode->DipE &&
					node->DpS == DelNode->DpS && node->DpE == DelNode->DpE &&
					node->Proto == DelNode->Proto){
				goto found;
			}
			break;
		case ACL_DEL_SIP_DIP_ANY:
			if(node->SipS== DelNode->SipS && node->SipE== DelNode->SipE && 
					node->DipS== DelNode->DipS && node->DipE== DelNode->DipE ){
				goto found;
			}
			break;
		case ACL_DEL_SIP_DIP_TCP:
		case ACL_DEL_SIP_DIP_UDP:
			if(node->SipS== DelNode->SipS && node->SipE== DelNode->SipE && 
					node->DipS== DelNode->DipS && node->DipE== DelNode->DipE &&
					node->DpS == DelNode->DpS && node->DpE == DelNode->DpE &&
					node->Proto == DelNode->Proto){
				goto found;
			}
			break;
		}
	}

	return ACL_FAIL;

found:
	list_del(pos);
	kfree(node);
	return SyncAclTbl();
}

/*
 * Pre ACL Function
 */
uint32_t PpeGetPreAclEbl(void)
{
	uint32_t PpeFlowSet=0;

	PpeFlowSet = RegRead(PPE_FLOW_SET);

	if( (PpeFlowSet & ~BIT_FUC_ACL) ||
			(PpeFlowSet & ~BIT_FMC_ACL) ||
			(PpeFlowSet & ~BIT_FBC_ACL)){
		return 1;
	}else {
		return 0;
	}
}

void  PpeSetPreAclEbl(uint32_t AclEbl)
{
	uint32_t PpeFlowSet=0;

	PpeFlowSet = RegRead(PPE_FLOW_SET);

	/* ACL engine for unicast/multicast/broadcast flow */
	if(AclEbl==1) {
		PpeFlowSet |= BIT_FUC_ACL | BIT_FMC_ACL | BIT_FBC_ACL;
	} else {
		/* Set Pre ACL Table */
		PpeFlowSet &= ~(BIT_FUC_ACL | BIT_FMC_ACL | BIT_FBC_ACL);
		RegModifyBits(PPE_PRE_ACL, DFL_PRE_ACL_STR, 0, 9);
		RegModifyBits(PPE_PRE_ACL, DFL_PRE_ACL_END, 16, 9);
	}

	RegWrite( PPE_FLOW_SET, PpeFlowSet);

}


uint16_t PpeGetPreAclStr(void)
{
	uint32_t PpePreAcl=0;

	PpePreAcl = RegRead(PPE_PRE_ACL);
	return PpePreAcl & 0x1FF;
}

void PpeSetPreAclStr(uint16_t PreAclStr)
{
	RegModifyBits(PPE_PRE_ACL, PreAclStr, 0, 9);
}

uint16_t PpeGetPreAclEnd(void)
{
	uint32_t PpePreAcl=0;

	PpePreAcl = RegRead(PPE_PRE_ACL);
	return (PpePreAcl>>16) & 0x1FF;
}

void PpeSetPreAclEnd(uint16_t PreAclEnd)
{
	RegModifyBits(PPE_PRE_ACL, PreAclEnd, 16, 9);
}


void inline PpeInsAclEntry(void *Rule)
{
	uint32_t Index=0;
	uint32_t *p=(uint32_t *)Rule;

	Index = PpeGetPreAclEnd();

	printk("Policy Table Base=%08X Offset=%d\n",POLICY_TBL_BASE, Index*8);
	printk("%08X: %08X\n",POLICY_TBL_BASE + Index*8, *p);
	printk("%08X: %08X\n",POLICY_TBL_BASE + Index*8+4, *(p+1));

	RegWrite(POLICY_TBL_BASE + Index*8, *p); /* Low bytes */
	RegWrite(POLICY_TBL_BASE + Index*8 + 4, *(p+1)); /* High bytes */

	/* Update PRE_ACL_END */    
	RegModifyBits(PPE_PRE_ACL, Index+1, 16, 9);
}


uint32_t AclInsDflAllow(void)
{
	/* Allow ARP Packet */
	struct l2_rule L2Rule;

	memset(&L2Rule,0,sizeof(L2Rule));
	L2Rule.com.fpp.ee=1;
	L2Rule.com.fpp.fpp=0;
	L2Rule.com.fpp.fpn=FPN_ALLOW;
	L2Rule.com.dir= OTHERS;
	L2Rule.com.rt=L2_RULE;
	L2Rule.com.pn = PN_DONT_CARE;
	L2Rule.com.match=1;

	L2Rule.others.e=1; /* eth type */
	L2Rule.others.etyp_pprot=ETH_P_ARP;

	PpeInsAclEntry(&L2Rule);
	return ACL_SUCCESS;

}

uint32_t AclInsDflDeny(void)
{
	struct l2_rule L2Rule;

	memset(&L2Rule,0,sizeof(L2Rule));
	L2Rule.com.dir= OTHERS;
	L2Rule.com.rt=L2_RULE;
	L2Rule.com.pn = PN_DONT_CARE;
	L2Rule.com.match=1;

	L2Rule.com.fpp.ee=1;
	L2Rule.com.fpp.fpp=0;
	L2Rule.com.fpp.fpn=FPN_DROP;

	L2Rule.others.v=0;
	L2Rule.others.vid=0;

	PpeInsAclEntry(&L2Rule);
	return ACL_SUCCESS;

}


/* Insert Layer2 Rule */
uint32_t AclSetMacEntry(AclPlcyNode *node, enum L2RuleDir Dir, enum FoeTblEE End)
{
	struct l2_rule L2Rule;

	memset(&L2Rule,0,sizeof(L2Rule));

	memcpy(&L2Rule.mac,node->Mac, ETH_ALEN);
	L2Rule.com.rt=L2_RULE;
	L2Rule.com.dir= Dir;
	L2Rule.com.pn = PN_DONT_CARE;
	L2Rule.com.match = 1;

	switch(End) 
	{
	case ENTRY_END_FOE:
		L2Rule.com.foe.ee=1;
		L2Rule.com.foe.foe=1;
		L2Rule.com.foe.foe_tb=node->FoeTb;
		break;
	case ENTRY_END_FP:
		L2Rule.com.fpp.ee=1;
		L2Rule.com.fpp.fpp=0;

		if(node->Method==ACL_ALLOW_RULE) {
			L2Rule.com.fpp.fpn=FPN_ALLOW;
		} else {
			L2Rule.com.fpp.fpn=FPN_DROP;
		}
		break;
	case NOT_ENTRY_END:
		L2Rule.com.ee_0.ee=0;
		L2Rule.com.ee_0.logic=AND;
		break;
	default:
		return ACL_FAIL;
	}

	PpeInsAclEntry(&L2Rule);

	return ACL_SUCCESS;
} 

uint32_t AclSetIpFragEntry(AclPlcyNode *node, enum FoeTblEE End)
{
	struct l3_rule L3Rule;

	memset(&L3Rule,0,sizeof(L3Rule));

	/* 
	 * MFV  MF  FOV  FOZ
	 * ---+---+----+----
	 *  1   1    1    1   MoreFrag=1 & Offset=0 ->First Frag Pkt
	 *  1   1    1    0   MoreFrag=1 & Offset!=0 ->Middle Frag Pkts
	 *  1   0    1    0   MoreFrag=0 & Offset!=0 ->Last Frag Pkts
	 *  1   0    1    1   MoreFrag=0 & Offset=0 -> Normal Pkt
	 *
	 *  HINT: Fragment Pkt = "NOT Normal Pkt"
	 */
	L3Rule.com.dir= IP_QOS;
	L3Rule.com.match=0; /* NOT Equal */
	L3Rule.com.pn = PN_DONT_CARE;
	L3Rule.com.rt=L3_RULE;
	L3Rule.qos.tos_s=0;
	L3Rule.qos.tos_e=255;
	L3Rule.qos.mfv=1;
	L3Rule.qos.mf=0;
	L3Rule.qos.fov=1;
	L3Rule.qos.foz=1;
	L3Rule.qos.v4=1;

	switch(End) 
	{
	case ENTRY_END_FOE:
		L3Rule.com.foe.ee=1;
		L3Rule.com.foe.foe=1;
		L3Rule.com.foe.foe_tb=node->FoeTb;
		break;
	case ENTRY_END_FP:
		L3Rule.com.fpp.ee=1;
		L3Rule.com.fpp.fpp=0; 

		if(node->Method==ACL_ALLOW_RULE) {
			L3Rule.com.fpp.fpn=FPN_ALLOW;
		} else {
			L3Rule.com.fpp.fpn=FPN_DROP;
		}
		break;
	case NOT_ENTRY_END:
		L3Rule.com.ee_0.ee=0;
		L3Rule.com.ee_0.logic=AND;
		break;
	default:
		return ACL_FAIL;
	}

	PpeInsAclEntry(&L3Rule);

	return ACL_SUCCESS;
}

/* Insert Layer3 Rule */
uint32_t AclSetIpEntry(AclPlcyNode *node,  enum L3RuleDir Dir, enum FoeTblEE End)
{
	struct l3_rule L3Rule;
	uint8_t E, M;

	memset(&L3Rule,0,sizeof(L3Rule));

	switch(Dir){
	case SIP:
		CalIpRange(node->SipS, node->SipE, &M, &E);
		L3Rule.ip.ip=node->SipS;
		break;
	case DIP:
		CalIpRange(node->DipS, node->DipE, &M, &E);
		L3Rule.ip.ip=node->DipS;
		break;
	default:
		return ACL_FAIL;
	}

	L3Rule.com.dir= Dir;
	L3Rule.com.match=1;
	L3Rule.com.pn = PN_DONT_CARE;
	L3Rule.com.rt=L3_RULE;
	L3Rule.ip.ip_rng_m=M;
	L3Rule.ip.ip_rng_e=E;
	L3Rule.ip.v4=1;

	switch(End) 
	{
	case ENTRY_END_FOE:
		L3Rule.com.foe.ee=1;
		L3Rule.com.foe.foe=1;
		L3Rule.com.foe.foe_tb=node->FoeTb;
		break;
	case ENTRY_END_FP:
		L3Rule.com.fpp.ee=1;
		L3Rule.com.fpp.fpp=0; 

		if(node->Method==ACL_ALLOW_RULE) {
			L3Rule.com.fpp.fpn=FPN_ALLOW;
		} else {
			L3Rule.com.fpp.fpn=FPN_DROP;
		}
		break;
	case NOT_ENTRY_END:
		L3Rule.com.ee_0.ee=0;
		L3Rule.com.ee_0.logic=AND;
		break;
	default:
		return ACL_FAIL;
	}

	PpeInsAclEntry(&L3Rule);

	return ACL_SUCCESS;

}

uint32_t AclSetProtoEntry(AclPlcyNode *node, enum FoeTblTcpUdp Proto, enum FoeTblEE End)
{
	struct l4_rule L4Rule;

	memset(&L4Rule,0,sizeof(L4Rule));

	L4Rule.com.match=1;
	L4Rule.com.pn = PN_DONT_CARE;
	L4Rule.com.rt=L4_RULE;
	L4Rule.ip.prot = FLT_IP_PROT;

	if(Proto==TCP) {
		L4Rule.ip.prot=6; //TCP
	}else {
		L4Rule.ip.prot=17; //UDP
	}

	switch(End) 
	{
	case ENTRY_END_FOE:
		L4Rule.com.foe.ee=1;
		L4Rule.com.foe.foe=1;
		L4Rule.com.foe.foe_tb=node->FoeTb;
		break;
	case ENTRY_END_FP:
		L4Rule.com.fpp.ee=1;
		L4Rule.com.fpp.fpp=0;
		if(node->Method==ACL_ALLOW_RULE) {
			L4Rule.com.fpp.fpn=FPN_ALLOW;
		} else {
			L4Rule.com.fpp.fpn=FPN_DROP;
		}
		break;
	case NOT_ENTRY_END:
		L4Rule.com.ee_0.ee=0;
		L4Rule.com.ee_0.logic=AND;
		break;
	}

	PpeInsAclEntry(&L4Rule);

	return ACL_SUCCESS;

}


/* Insert Layer4 Rule */
uint32_t AclSetPortEntry(AclPlcyNode *node, enum L4RuleDir Dir, 
		enum FoeTblTcpUdp Proto, enum FoeTblEE End)
{
	struct l4_rule L4Rule;

	memset(&L4Rule,0,sizeof(L4Rule));
	L4Rule.com.dir= Dir;
	L4Rule.com.match=1;
	L4Rule.com.pn = PN_DONT_CARE;
	L4Rule.com.rt=L4_RULE;

	switch(Dir) {
	case SPORT:
		L4Rule.p_start=node->SpS;
		L4Rule.p_end=node->SpE;
		break;
	case DPORT:
		L4Rule.p_start=node->DpS;
		L4Rule.p_end=node->DpE;
		break;
	default: //invalid Dir for PortEntry
		return ACL_FAIL;
	}


	if(Proto==TCP) {
		L4Rule.tcp.tu=FLT_TCP;
		L4Rule.tcp.tcp_fop=FLAG_EQUAL;
		L4Rule.tcp.tcp_fm=0x3F; 
	}else {
		L4Rule.udp.tu=FLT_UDP;
	}

	switch(End) 
	{
	case ENTRY_END_FOE:
		L4Rule.com.foe.ee=1;
		L4Rule.com.foe.foe=1;
		L4Rule.com.foe.foe_tb=node->FoeTb;
		break;
	case ENTRY_END_FP:
		L4Rule.com.fpp.ee=1;
		L4Rule.com.fpp.fpp=0;

		if(node->Method==ACL_ALLOW_RULE) {
			L4Rule.com.fpp.fpn=FPN_ALLOW;
		} else {
			L4Rule.com.fpp.fpn=FPN_DROP;
		}
		break;
	case NOT_ENTRY_END:
		L4Rule.com.ee_0.ee=0;
		L4Rule.com.ee_0.logic=AND;
		break;
	default:
		return ACL_FAIL;
	}

	PpeInsAclEntry(&L4Rule);

	return ACL_SUCCESS;

} 


uint32_t AclInsSmacDipDp(AclPlcyNode *node)
{

	//Insert SMAC Entry 
	AclSetMacEntry(node, SMAC, NOT_ENTRY_END);

	//Insert DIP Entry 
	if(node->RuleType==ACL_ADD_SMAC_DIP_ANY && node->RuleType==ACL_ADD_SIP_DIP_ANY) {
		//Insert DP Entry
		AclSetIpEntry(node, DIP, ENTRY_END_FP);
	}else{
		//Insert DP Entry
		AclSetIpEntry(node, DIP, NOT_ENTRY_END);

		if(node->RuleType==ACL_ADD_SMAC_DIP_TCP || node->RuleType==ACL_ADD_SIP_DIP_TCP) {
			AclSetPortEntry(node, DPORT, TCP, ENTRY_END_FP); 
		}else if(node->RuleType==ACL_ADD_SMAC_DIP_UDP || node->RuleType==ACL_ADD_SIP_DIP_UDP) {
			AclSetPortEntry(node, DPORT, UDP, ENTRY_END_FP);
		}
	}

	return ACL_SUCCESS;
}


uint32_t AclInsSipDipDp(AclPlcyNode *node)
{

	//Insert SIP Entry 
	AclSetIpEntry(node, SIP, NOT_ENTRY_END);

	if(node->RuleType==ACL_ADD_SMAC_DIP_ANY || node->RuleType==ACL_ADD_SIP_DIP_ANY) {
		//Insert DIP Entry 
		AclSetIpEntry(node, DIP, ENTRY_END_FP);
	}else{
		//Insert DIP Entry 
		AclSetIpEntry(node, DIP, NOT_ENTRY_END);

		//Insert DP Entry
		if(node->RuleType==ACL_ADD_SMAC_DIP_TCP || node->RuleType==ACL_ADD_SIP_DIP_TCP) {
			AclSetPortEntry(node, DPORT, TCP, ENTRY_END_FP); 
		}else if(node->RuleType==ACL_ADD_SMAC_DIP_UDP || node->RuleType==ACL_ADD_SIP_DIP_UDP) {
			AclSetPortEntry(node, DPORT, UDP, ENTRY_END_FP);
		}
	}

	return ACL_SUCCESS;
}


/* Remove all ACL entries */
uint32_t AclCleanTbl(void)
{
	struct list_head *pos = NULL, *tmp;
	AclPlcyNode *node;

	list_for_each_safe(pos, tmp, &AclPlcyList.List) {
		node = list_entry(pos, AclPlcyNode, List);
		list_del(pos);
		kfree(node);
	}

	PpeSetPreAclEbl(0); // Disable PreAcl Table

	return ACL_SUCCESS;

}
