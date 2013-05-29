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
  ra_nat.c

  Abstract:

  Revision History:
  Who         When            What
  --------    ----------      ----------------------------------------------
  Name        Date            Modification logs
  Steven Liu  2007-05-30      Support RT2880G
  Steven Liu  2008-03-19      Support RT3052/RT2883 
  Steven Liu  2007-09-25      Support RT2880E
  Steven Liu  2006-10-06      Initial version
 *
 */

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netfilter.h>
#include <linux/netdevice.h>
#include <linux/types.h>
#include <asm/uaccess.h>
#include <asm/string.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <net/tcp.h>
#include <linux/udp.h>
#include <linux/in.h>
#include <linux/if_vlan.h>
#include <linux/if_ether.h>
#include <linux/if_pppox.h>
#include <linux/ppp_defs.h>
#include <asm/checksum.h>
#include <linux/pci.h>
#include <linux/etherdevice.h>

#include "ra_nat.h"
#include "foe_fdb.h"
#include "frame_engine.h"
#include "hwnat_ioctl.h"
#include "acl_ioctl.h"
#include "ac_ioctl.h"
#include "acl_policy.h"
#include "mtr_policy.h"
#include "ac_policy.h"
#include "util.h"


extern int (*ra_sw_nat_hook_rx) (struct sk_buff * skb);
extern int (*ra_sw_nat_hook_tx) (struct sk_buff * skb, int gmac_no);

struct FoeEntry    *PpeFoeBase;
dma_addr_t	    PpePhyFoeBase;
struct net_device  *DstPort[MAX_IF_NUM];
uint32_t	    DebugLevel=0;
uint32_t	    ChipVer=0;
uint32_t	    ChipId=0;


static void FoeAllocTbl(uint32_t NumOfEntry)
{
    uint32_t FoeTblSize;

    FoeTblSize = NumOfEntry * sizeof(struct FoeEntry);
    PpeFoeBase = pci_alloc_consistent(NULL, FoeTblSize, &PpePhyFoeBase);

    RegWrite(PPE_FOE_BASE, PpePhyFoeBase);
    memset(PpeFoeBase, 0, FoeTblSize);
}

static uint8_t *ShowCpuReason(struct sk_buff *skb)
{
    switch(FOE_AI(skb))
    {
    case TTL_0: /* 0x80 */
	return("TTL=0\n");
    case FOE_EBL_NOT_IPV4_HLEN5: /* 0x90 */
	return("FOE enable & not IPv4h5nf\n");
    case FOE_EBL_NOT_TCP_UDP_L4_READY: /* 0x91 */
	return("FOE enable & not TCP/UDP/L4_read\n");
    case TCP_SYN_FIN_RST: /* 0x92 */
	return("TCP SYN/FIN/RST\n");
    case UN_HIT: /* 0x93 */
	return("Un-hit\n");
    case HIT_UNBIND: /* 0x94 */
	return("Hit unbind\n");
    case HIT_UNBIND_RATE_REACH: /* 0x95 */
	return("Hit unbind & rate reach\n");
    case HIT_FIN:  /* 0x96 */
	return("Hit fin\n");
    case HIT_BIND_TTL_1: /* 0x97 */
	return("Hit bind & ttl=1 & ttl-1\n");
    case HIT_BIND_KEEPALIVE:  /* 0x98 */
	return("Hit bind & keep alive\n");
    case HIT_BIND_FORCE_TO_CPU: /* 0x99 */
	return("Hit bind & force to CPU\n");
    case ACL_FOE_TBL_ERR: /* 0x9A */
	return("acl link foe table error (!static & !unbind)\n");
    case ACL_TBL_TTL_1: /* 0x9B */
	return("acl link FOE table & TTL=1 & TTL-1\n");
    case ACL_ALERT_CPU: /* 0x9C */
	return("acl alert cpu\n");
    case NO_FORCE_DEST_PORT: /* 0xB0 */
	return("No force destination port\n");
    case EXCEED_MTU: /* 0xB1 */
	return("Exceed mtu\n");
    }

    return("CPU Reason Error \n");
}


static int32_t PpeGetIfNum(struct sk_buff *skb)
{
    uint32_t i;

    for(i=0;i<MAX_IF_NUM;i++) {
	if(DstPort[i]!=NULL) {
	    if(skb->dev==DstPort[i]){
		return i;
	    }
	}
    }

    return -1;
}


uint32_t FoeDumpPkt(struct sk_buff *skb)
{
    struct ethhdr *eth = NULL;
    struct vlan_hdr *vh1 = NULL;
    struct vlan_hdr *vh2 = NULL;
    struct iphdr *iph = NULL;
    struct tcphdr *th = NULL;
    struct udphdr *uh = NULL;

    uint32_t vlan1_gap = 0;
    uint32_t vlan2_gap = 0;
    uint32_t pppoe_gap=0;
    uint16_t pppoe_sid = 0;
    uint16_t eth_type=0;
    

    NAT_PRINT("\nRx===<FOE_Entry=%d>=====\n",FOE_ENTRY_NUM(skb)); 
    NAT_PRINT("RcvIF=%s\n", skb->dev->name);
    NAT_PRINT("FOE_Entry=%d\n",FOE_ENTRY_NUM(skb));
    NAT_PRINT("FVLD=%d\n",FOE_FVLD(skb));
    NAT_PRINT("CPU Reason=%s",ShowCpuReason(skb));
    NAT_PRINT("ALG_IF=%d\n",FOE_ALG_RXIF(skb));
    NAT_PRINT("SP=%d\n",FOE_SP(skb));
    NAT_PRINT("AIS=%d\n",FOE_AIS(skb));
    NAT_PRINT("L4F=%d\n", FOE_L4F(skb));
    NAT_PRINT("IPF=%d\n", FOE_IPF(skb));
    NAT_PRINT("L4FVLD=%d\n",FOE_L4FVLD(skb));
    NAT_PRINT("IPFVLD=%d\n",FOE_IPFVLD(skb));


    eth_type=ntohs(skb->protocol);

    // Layer 2
    if(eth_type==ETH_P_8021Q) {
	vlan1_gap = VLAN_HLEN;
	vh1 = (struct vlan_hdr *)(skb->data);

	/* VLAN + PPPoE */
	if(ntohs(vh1->h_vlan_encapsulated_proto)==ETH_P_PPP_SES){
	    pppoe_gap = 8;
	    if (GetPppoeSid(skb, vlan1_gap, &pppoe_sid, 0)) {
		return 0;
	    }
	    /* Double VLAN = VLAN + VLAN */
	}else if(ntohs(vh1->h_vlan_encapsulated_proto)==ETH_P_8021Q) {
	    vlan2_gap = VLAN_HLEN;
	    vh2 = (struct vlan_hdr *)(skb->data + VLAN_HLEN);

	    /* VLAN + VLAN + PPPoE */
	    if(ntohs(vh2->h_vlan_encapsulated_proto)==ETH_P_PPP_SES){
		pppoe_gap = 8;
		if (GetPppoeSid(skb, vlan1_gap, &pppoe_sid, 0)) {
		    printk("222\n");
		    return 0;
		}
		/* VLAN + VLAN + IP */
	    }else if(ntohs(vh2->h_vlan_encapsulated_proto)!=ETH_P_IP) {
		return 0;
	    }
	    /* VLAN + IP */
	}else if(ntohs(vh1->h_vlan_encapsulated_proto)!=ETH_P_IP) {
	    return 0;
	}
    }else if(eth_type != ETH_P_IP) {
	return 0;
    }
    
    eth = (struct ethhdr *)(skb->data-14) ; /* DA + SA + ETH_TYPE */

    // Layer 3
    iph = (struct iphdr *) (skb->data + vlan1_gap + vlan2_gap + pppoe_gap);


    // Layer 4
    if(iph->protocol==IPPROTO_TCP) {
	th = (struct tcphdr *) ((uint8_t *) iph + iph->ihl * 4);
    }else if(iph->protocol==IPPROTO_UDP) {
	uh = (struct udphdr *) ((uint8_t *) iph + iph->ihl * 4);
    }else { //Not TCP or UDP
	return 0;
    }

    if(vlan1_gap) {
	NAT_PRINT("VLAN1: %d\n",ntohs(vh1->h_vlan_TCI));
    }
    if(vlan2_gap) {
	NAT_PRINT("VLAN2: %d\n",ntohs(vh2->h_vlan_TCI));
    }
    if(pppoe_gap) {
	NAT_PRINT("PPPoE Session ID: %d\n", ntohs(pppoe_sid));
    }
    
    NAT_PRINT("----------------------------------\n");
    NAT_PRINT("SrcMac=%0X:%0X:%0X:%0X:%0X:%0X\n",MAC_ARG(eth->h_source));
    NAT_PRINT("DstMac=%0X:%0X:%0X:%0X:%0X:%0X\n",MAC_ARG(eth->h_dest));
    NAT_PRINT("SrcIp:%s\n",Ip2Str(iph->saddr));
    NAT_PRINT("DstIp:%s\n",Ip2Str(iph->daddr));
    NAT_PRINT("SrcPort:%d Dstport:%d\n",ntohs(th->source),ntohs(th->dest));
    NAT_PRINT("==================================\n");


    return 1;

}

int32_t PpeRxHandler(struct sk_buff * skb)
{
    struct ethhdr *eth=NULL;
    struct vlan_hdr *vh = NULL;
    struct iphdr *iph = NULL;
    struct tcphdr *th = NULL;
    struct udphdr *uh = NULL;

    uint32_t vlan_gap = 0;
    uint32_t pppoe_gap=0;
    uint16_t eth_type=0;

    struct FoeEntry *foe_entry;

    foe_entry=&PpeFoeBase[FOE_ENTRY_NUM(skb)];
    eth_type=ntohs(skb->protocol);

    if(DebugLevel==1) {
       FoeDumpPkt(skb);
    }

    if( ((FOE_MAGIC_TAG(skb) == FOE_MAGIC_PCI) ||
			    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_WLAN))){ 

#ifdef CONFIG_RA_SHW_NAT_EBL 
	    /* 
	     * HW_NAT:
	     * PCI/WLAN<->CPU<->PPE<->CPU<->PCI/WLAN
	     *
	     * SHW_NAT:
	     * PCI/WLAN<->CPU<->PCI/WLAN
	     *
	     * Software based HW NAT can speed up PCI/WLAN to  PCI/WLAN traffic.
	     * If SHW_NAT is enabled, transfer to non-GE port directly and do not
	     * need to transmit to PPE first.
	     *
	     */

	    struct FoeEntry *foe_tx_entry;
	    int32_t hash_index;
	    uint32_t current_time;
	    struct FoePriKey key;
	    
	    if(eth_type==ETH_P_8021Q) {
		    vlan_gap = VLAN_HLEN;
		    vh = (struct vlan_hdr *) skb->data;
		    if(ntohs(vh->h_vlan_encapsulated_proto)==ETH_P_PPP_SES) {
			    pppoe_gap = 8;
		    }else if(ntohs(vh->h_vlan_encapsulated_proto)!=ETH_P_IP) {
			    return 1;
		    }
	    }else {
		    if(eth_type==ETH_P_PPP_SES) {
			    pppoe_gap = 8;
		    }else if (eth_type!=ETH_P_IP){
			    return 1;
		    }
	    }

	    iph = (struct iphdr *) (skb->data + vlan_gap + pppoe_gap);
	    key.sip=ntohl(iph->saddr);
	    key.dip=ntohl(iph->daddr);

	    if(iph->protocol==IPPROTO_TCP) {
		    th = (struct tcphdr *) ((uint8_t *) iph + iph->ihl * 4);
		    key.sport=ntohs(th->source);
		    key.dport=ntohs(th->dest);
	    }else if(iph->protocol==IPPROTO_UDP) {
		    uh = (struct udphdr *) ((uint8_t *) iph + iph->ihl * 4);
		    key.sport=ntohs(uh->source);
		    key.dport=ntohs(uh->dest);
	    }else{ // only handle TCP/UDP flow
		    return 1;
	    }

	    hash_index=FoeHashFun(&key,BIND);
	    if(hash_index != -1) {//binding entry found 
		    foe_tx_entry=&PpeFoeBase[hash_index]; 
		    current_time =RegRead(FOE_TS_T)&0xFF;
		    foe_tx_entry->bfib1.time_stamp=(uint16_t)current_time;//update timestamp
		    skb->dev = DstPort[foe_tx_entry->iblk2.act_dp];

		    //rebuild smac,dmac,sip,dip,sp,dp fields
		    PpePktRebuild(skb,iph, foe_tx_entry); 
		    skb->dev->hard_start_xmit(skb, skb->dev);
		    return 0;
	    }
#endif

	    if(skb->dev == DstPort[DP_WLAN]) {
		    FOE_ALG_RXIF(skb)=0; /* packet is coming from WLAN */
	    }else {
		    FOE_ALG_RXIF(skb)=1; /* packet is coming from PCI */
	    }
	    
	    //redirect to PPE
	    FOE_MAGIC_TAG(skb) = FOE_MAGIC_PPE;
	    skb->dev = DstPort[DP_GMAC];
	    skb_push(skb, ETH_HLEN); //pointer to layer2 header
	    skb->dev->hard_start_xmit(skb, skb->dev);
	    return 0;

    }

    /* 
     * If FOE_AIS=1 and FOW_SP=0, it means this is reentry packet.
     * (WLAN->CPU->PPE->CPU or PCI->CPU->PPE->CPU)
     *
     * We should set skb->dev and skb->pkt_type to notify upper layer which 
     * is the right input interface of the packet.
     *
     * (Notes: In RT2880 A/B/C/D, only wireless packet will be injected to PPE,
     *          so we set skb->dev=wireless interface.)
     */
    if((FOE_AIS(skb) == 1) && ( FOE_SP(skb) == 0)) {

	    if(ChipId==RT2880 && ChipVer <= RT2880D){ // RT2880A ~ RT2880D
		    skb->dev=DstPort[DP_WLAN];
	    }else{

		    if(FOE_ALG_RXIF(skb)==0) {/* WLAN */
			    skb->dev=DstPort[DP_WLAN];
		    }else { /*PCI*/
			    skb->dev=DstPort[DP_PCI];
		    }

		    /*
		     * Clear FOE_ALG_RXIF field
		     * Linux netfilter will use this bit to show ALG is interested or not.
		     */
		    FOE_ALG_RXIF(skb)=0; 
	    }


	    eth=(struct ethhdr *)(skb->data-ETH_HLEN);

	    if(eth->h_dest[0] & 1)
	    {
		    if(memcmp(eth->h_dest, skb->dev->broadcast, ETH_ALEN)==0){
                        skb->pkt_type=PACKET_BROADCAST;
		    } else {
                        skb->pkt_type=PACKET_MULTICAST;
		    }
	    }else {

		    if(memcmp(eth->h_dest, skb->dev->dev_addr, ETH_ALEN)==0){
			    skb->pkt_type=PACKET_HOST;
		    }else{
			    skb->pkt_type=PACKET_OTHERHOST;
		    }
	    }
    }

    /* It means the flow is already in binding state, just transfer to output interface */
    if((FOE_AI(skb)==HIT_BIND_FORCE_TO_CPU)) {
	    skb->dev = DstPort[foe_entry->iblk2.act_dp];
	    skb_push(skb, ETH_HLEN); //pointer to layer2 header
	    skb->dev->hard_start_xmit(skb, skb->dev);
	    return 0;
    }


    if( (FOE_AI(skb)==HIT_BIND_KEEPALIVE) && (DFL_FOE_KA_ORG==0)){

	    /* Notes: 	 
	     *
	     *	 PPE_FOE_CFG->FOE_KA_ORG(bit12)
	     *	 
	     *	 Keep alive packet with original header
	     *	 1: Original header
	     *	 0: New header
	     *
	     *	 Either original or new header mode, all of the keepalive packets from 
	     *	 PPE to cpu will carry "keep alive with original header" in cpu reason field.
	     *
	     *	 If PPE in keepalive with new header mode:
	     *
	     *	 Step1: Recover to original packet and pass to cpu to refresh
	     *	 	uppler table (We have to recover SMAC/DMAC/SIP/DIP/SP/DP 
	     *	        and recalculate IP/TCP/UDP checksum)
	     *	
	     *	 Step2: TxHandler have to drop this packet because PPE forwards
	     *	        packet to cpu and output port at the same time.
	     *
	     */

	    /* FIXME:
	     * Recover to original SMAC/DMAC, but we don't know the SMAC.
	     * At this time, we fill out DMAC=My_Mac, SMAC=new_DMAC to workaround this problem.
	     *
	     */
	    struct ethhdr *eth;
	    eth=(struct ethhdr *)(skb->data-ETH_HLEN);
	    FoeGetMacInfo(eth->h_dest, foe_entry->smac_hi);
	    FoeGetMacInfo(eth->h_source, foe_entry->dmac_hi);

	    if(eth_type==ETH_P_8021Q) {
		    vlan_gap = VLAN_HLEN;
		    vh = (struct vlan_hdr *) skb->data;

		    /* 
		     * Recover to original vlan header 
		     *
		     * LAN Ports VID=1, WAN Ports VID=2
		     * Packet from WAN to LAN: VLANID 2 --(FOE)--> VLANID 1
		     * Packet from LAN to WAN: VLANID 1 --(FOE)--> VLANID 2
		     *
		     * Keepalive in new header mode: 
		     * FOE pass packet with new header to cpu after packet modified
		     * 
		     * Packet from WAN to LAN: New VLANID = 1, change to VLANID=2 which mean 
		     *              	       packet is coming from WAN port.
		     * Packet from LAN to WAN: New VLANID = 2, change VLANID=2 which mean 
		     *              	       packet is coming from WAN port.
		     */
		    if(ntohs(vh->h_vlan_TCI)==1){
			    /* It make packet like coming from WAN port */
			    vh->h_vlan_TCI=htons(2);

		    } else {
			    /* It make packet like coming from LAN port */
			    vh->h_vlan_TCI=htons(1);
		    }

		    if(ntohs(vh->h_vlan_encapsulated_proto)==ETH_P_PPP_SES) {
			    pppoe_gap = 8;
		    }else if(ntohs(vh->h_vlan_encapsulated_proto)!= ETH_P_IP) {
			    return 1;
		    }
	    }else {
		    if(eth_type==ETH_P_PPP_SES) {
			    pppoe_gap = 8;
		    }else if(eth_type != ETH_P_IP) {
			    return 1;
		    }
	    }

	    iph = (struct iphdr *) (skb->data + vlan_gap + pppoe_gap);

	    //Recover to original layer 4 header 
	    if (iph->protocol == IPPROTO_TCP) {
		    th = (struct tcphdr *) ((uint8_t *) iph + iph->ihl * 4);
		    FoeToOrgTcpHdr(foe_entry, iph, th);

	    } else if (iph->protocol == IPPROTO_UDP) {
		    uh = (struct udphdr *) ((uint8_t *) iph + iph->ihl * 4);
		    FoeToOrgUdpHdr(foe_entry, iph, uh);
	    }

	    //Recover to original layer 3 header 
	    FoeToOrgIpHdr(foe_entry,iph);

	    /* 
	     * PCI/WLAN<->CPU<->PPE<->GMAC
	     *
	     * Ethernet driver will call eth_type_trans() to set skb->pkt_type.
	     * If(destination mac != my mac) 
	     *   skb->pkt_type=PACKET_OTHERHOST;
	     * 
	     * When Packets from PCI/WLAN be redirected into PPE, PPE process
	     * packets based on different flow state:
	     *
	     * unbind: pass to CPU and GE driver set skb->pkt_type=PACKET_OTHERHOST 
	     *         because destination mac is PCI/WLAN's mac. It's wrong because
	     *         all of those packets are PACKET_HOST not PACKET_OTHERHOST.
	     * bind: transmit to output port immediately.
	     *
	     */
	    skb->pkt_type=PACKET_HOST;	

    }

    return 1;
}

/* is_in = 1 --> in  */
/* is_in = 0 --> out */
int32_t GetPppoeSid(struct sk_buff *skb, uint32_t vlan_gap, 
		uint16_t *sid, uint32_t is_in)
{
	struct pppoe_hdr *peh = NULL;
	uint32_t offset = 0;

	if(!is_in) {
		offset = ETH_HLEN;
	}

	peh = (struct pppoe_hdr *) (skb->data + offset + vlan_gap);

	if(DebugLevel==1) { 
		NAT_PRINT("\n==============\n");
		NAT_PRINT(" Ver=%d\n",peh->ver);
		NAT_PRINT(" Type=%d\n",peh->type);
		NAT_PRINT(" Code=%d\n",peh->code);
		NAT_PRINT(" sid=%x\n",ntohs(peh->sid));
		NAT_PRINT(" Len=%d\n",ntohs(peh->length));
		NAT_PRINT(" tag_type=%x\n",ntohs(peh->tag[0].tag_type));
		NAT_PRINT(" tag_len=%d\n",ntohs(peh->tag[0].tag_len));
		NAT_PRINT("=================\n");
	}

	if (peh->ver != 1 || peh->type != 1
			|| (ntohs(peh->tag[0].tag_type) != PPP_IP)){
		return 1;
	}

	*sid = peh->sid;
	return 0;
}

int32_t PpeTxHandler(struct sk_buff *skb, int gmac_no)
{
	struct vlan_hdr *vh = NULL;
	struct iphdr *iph = NULL;
	struct tcphdr *th = NULL;
	struct udphdr *uh = NULL;
	struct ethhdr *eth = NULL;
	uint32_t vlan1_gap = 0;
	uint32_t vlan2_gap = 0;
	uint32_t pppoe_gap = 0;
	uint16_t pppoe_sid = 0;
	struct FoeEntry *foe_entry;
	uint32_t current_time;
	struct FoeEntry entry;
	uint16_t eth_type=0;

    
	/* 
	 * Packet is interested by ALG?
	 * Yes: Don't enter binind state
	 * No: If flow rate exceed binding threshold, enter binding state.
	 */
	if((FOE_AI(skb)==HIT_UNBIND_RATE_REACH) && (FOE_ALG_RXIF(skb)==0)) 
	{
		eth = (struct ethhdr *) skb->data;
		eth_type=ntohs(eth->h_proto);
		foe_entry=&PpeFoeBase[FOE_ENTRY_NUM(skb)];

#if defined (CONFIG_RA_HW_NAT_SEMIAUTO_BIND)
		// It's ready for becoming binding state in semi-auto bind mode, so
		// no need to save any information
		if(foe_entry->tmp_buf.time_stamp != 0 ) {
		    return 1;
		}
#endif
		//if this entry is already in binding state, skip it 
		if(foe_entry->bfib1.state == BIND) {
			return 1;
		}

		/* Get original setting */
		memcpy(&entry, foe_entry, sizeof(entry));


		/* Set Layer2 Info - DMAC, SMAC */
		FoeSetMacInfo(entry.dmac_hi,eth->h_dest);
		FoeSetMacInfo(entry.smac_hi,eth->h_source);

		/* Set VLAN Info - VLAN1/VLAN2 */
		if(eth_type==ETH_P_8021Q) {
			vlan1_gap = VLAN_HLEN;
			vh = (struct vlan_hdr *)(skb->data + ETH_HLEN);
			entry.vlan1 = ntohs(vh->h_vlan_TCI);

			/* VLAN + PPPoE */
			if(ntohs(vh->h_vlan_encapsulated_proto)==ETH_P_PPP_SES){
				pppoe_gap = 8;
				if (GetPppoeSid(skb, vlan1_gap, &pppoe_sid, 0)) {
					return 1;
				}
				entry.pppoe_id = ntohs(pppoe_sid);
			/* Double VLAN = VLAN + VLAN */
			}else if(ntohs(vh->h_vlan_encapsulated_proto)==ETH_P_8021Q) {
			    vlan2_gap = VLAN_HLEN;
			    vh = (struct vlan_hdr *)(skb->data + ETH_HLEN + VLAN_HLEN);
			    entry.vlan2 = ntohs(vh->h_vlan_TCI);

			    /* VLAN + VLAN + PPPoE */
			    if(ntohs(vh->h_vlan_encapsulated_proto)==ETH_P_PPP_SES){
				pppoe_gap = 8;
				if (GetPppoeSid(skb, vlan1_gap, &pppoe_sid, 0)) {
					return 1;
				}
				entry.pppoe_id = ntohs(pppoe_sid);
			    /* VLAN + VLAN + IP */
			    }else if(ntohs(vh->h_vlan_encapsulated_proto)!=ETH_P_IP) {
				return 1;
			    }
			/* VLAN + IP */
			}else if(ntohs(vh->h_vlan_encapsulated_proto)!=ETH_P_IP) {
			    return 1;
			}
		}else if(eth_type != ETH_P_IP) {
		    return 1;
		}

		/* 
		 * PPE support SMART VLAN/PPPoE Tag Push/PoP feature
		 *
		 * INSERT:
		 * 	Original packet has vlan/pppoe tag -> modify tag
		 *	Original packet don't have vlan/pppoe tag -> insert tag
		 * DELETE:
		 *	Original packet has vlan/pppoe tag -> delete tag
		 *	Original packet don't have vlan/pppoe tag -> no action
		 *
		 */
		if(vlan1_gap) { 
			entry.bfib1.v1=INSERT;
		} else { 
			entry.bfib1.v1 = DELETE ;
		}
		
		if(vlan2_gap) { 
			entry.bfib1.v2=INSERT;
		} else { 
			entry.bfib1.v2 = DELETE ;
		}

		if(pppoe_gap) { 
			entry.bfib1.pppoe = INSERT ;
		} else { 
			entry.bfib1.pppoe = DELETE ;
		}

		/* Set Layer3 Info - NEW_SIP, NEW_DIP*/
		iph = (struct iphdr *) (skb->data + ETH_HLEN + vlan1_gap + vlan2_gap + pppoe_gap);
		entry.new_sip = ntohl(iph->saddr);
		entry.new_dip = ntohl(iph->daddr);

		/* Set Layer4 Info - NEW_SPORT, NEW_DPORT */
		if (iph->protocol == IPPROTO_TCP) {
			th = (struct tcphdr *) ((uint8_t *) iph + iph->ihl * 4);
			entry.new_sport = ntohs(th->source);
			entry.new_dport = ntohs(th->dest);
			entry.bfib1.t_u = TCP;
		} else if (iph->protocol == IPPROTO_UDP) {
			uh = (struct udphdr *) ((uint8_t *) iph + iph->ihl * 4);
			entry.new_sport = ntohs(uh->source);
			entry.new_dport = ntohs(uh->dest);
			entry.bfib1.t_u = UDP;
		}else {
		    NAT_PRINT("PpeTxHandler: Wrong IP Protocol=%x in IpHdr\n",iph->protocol);
		    return 1;
		}

		/* Set Current time to time_stamp field in information block 1 */
		current_time =RegRead(FOE_TS_T)&0xFFFF;
		entry.bfib1.time_stamp=(uint16_t)current_time;

		/* Set Information block 2 */
		entry.iblk2.fd=1;

		/* CPU need to handle traffic between WLAN/PCI and GMAC port */	
		if( (skb->dev == DstPort[DP_WLAN]) ||
		    (skb->dev == DstPort[DP_PCI])) {
			entry.iblk2.dp=0; /* cpu */
		}else {
#if defined (CONFIG_RALINK_RT2880)
			entry.iblk2.dp=1; /* GMAC1 */
#elif defined (CONFIG_RALINK_RT3052)
			if(entry.vlan1==1) {
			    entry.iblk2.dp=1; /* LAN traffic use VirtualPort1 in GMAC1*/
			}else if(entry.vlan1==2) {
			    entry.iblk2.dp=2; /* WAN traffic use VirtualPort2 in GMAC1*/
			}else {
			    /* for one arm NAT test -> no vlan tag */
			    entry.iblk2.dp=1; 
			}
#elif defined (CONFIG_RALINK_RT2883)
			entry.iblk2.dp=gmac_no; //GMAC1 or GMAC2
#endif
		}

		/* Set actual output port info */	
		entry.iblk2.act_dp=PpeGetIfNum(skb);

		/* Change Foe Entry State to Binding State*/
#if defined (CONFIG_RA_HW_NAT_AUTO_BIND)
		entry.bfib1.state = BIND;
#elif defined (CONFIG_RA_HW_NAT_SEMIAUTO_BIND)
		/* Don't bind this flow until user wants to bind it. */
		memcpy(&entry.tmp_buf, &entry.bfib1 , sizeof(entry.bfib1));
#endif
		memcpy(foe_entry, &entry, sizeof(entry));
		FoeDumpEntry(FOE_ENTRY_NUM(skb));	

	}else if( (FOE_AI(skb)==HIT_BIND_KEEPALIVE) && (DFL_FOE_KA_ORG==0)){
		/* this is duplicate packet in keepalive new header mode, 
		 * just drop it */
		return 0;
	}

	return 1;
}

void  PpeSetFoeEbl(uint32_t FoeEbl)
{
	uint32_t PpeFlowSet=0;

	PpeFlowSet = RegRead(PPE_FLOW_SET);

	/* FOE engine need to handle unicast/multicast/broadcast flow */
	if(FoeEbl==1) {
		PpeFlowSet = BIT_FUC_FOE | BIT_FMC_FOE | BIT_FBC_FOE;
	} else {
		PpeFlowSet &= ~(BIT_FUC_FOE | BIT_FMC_FOE | BIT_FBC_FOE);
	}

	RegWrite( PPE_FLOW_SET, PpeFlowSet);
}


static void PpeSetFoeHashMode(uint32_t HashMode)
{

	/* Allocate FOE table base */ 
	FoeAllocTbl(FOE_4TB_SIZ);

	switch(FOE_4TB_SIZ){
	case 1024:
		RegModifyBits(PPE_FOE_CFG, FoeTblSize_1K, 0, 3);
		break;
	case 2048:
		RegModifyBits(PPE_FOE_CFG, FoeTblSize_2K, 0, 3);
		break;
	case 4096:
		RegModifyBits(PPE_FOE_CFG, FoeTblSize_4K, 0, 3);
		break;
	case 8192:
		RegModifyBits(PPE_FOE_CFG, FoeTblSize_8K, 0, 3);
		break;
	case 16384:
		RegModifyBits(PPE_FOE_CFG, FoeTblSize_16K, 0, 3);
		break;
	}

	/*
	 * RT2880 A/B/C/D Chip Bug
	 *
	 * HashMode=0/1 in 1K table size -> set HashMode =0/1
	 * HashMode=0/1 in 2K,4K,8K,16K table size -> set HashMode =1/0
	 *
	 */
	if(ChipId==RT2880 && ChipVer <= RT2880D) {
		if(FOE_4TB_SIZ!=1024){
			HashMode=~HashMode;
		}
	}

	/* Set Hash Mode */
	RegModifyBits(PPE_FOE_CFG, HashMode , 3, 1);

	/* Set action for FOE search miss */
#if defined (CONFIG_RA_HW_NAT_AUTO_BIND) || defined (CONFIG_RA_HW_NAT_SEMIAUTO_BIND)
	RegModifyBits(PPE_FOE_CFG, FWD_CPU_BUILD_ENTRY, 4, 2);
#elif defined (CONFIG_RA_HW_NAT_MANUAL_BIND)
	RegModifyBits(PPE_FOE_CFG, ONLY_FWD_CPU, 4, 2);
#else
	#error "Please Choice Action for FoE search miss"
#endif
}

static void PpeSetAgeOut(void)
{
	/* set Unbind State Age Enable */
	RegModifyBits(PPE_FOE_CFG, DFL_FOE_UNB_AGE, 8, 1);

	/* set Bind TCP Age Enable */
	RegModifyBits(PPE_FOE_CFG, DFL_FOE_TCP_AGE, 9, 1);

	/* set Bind UDP Age Enable */
	RegModifyBits(PPE_FOE_CFG, DFL_FOE_UDP_AGE, 10, 1);

	/* set Bind TCP FIN Age Enable */
	RegModifyBits(PPE_FOE_CFG, DFL_FOE_FIN_AGE, 11, 1);

	/* set min threshold of packet count for aging out at unbind state */
	RegModifyBits(PPE_FOE_UNB_AGE, DFL_FOE_UNB_MNP, 16, 16);

	/* Delta time for aging out an ACL link to FOE entry */
	RegModifyBits(PPE_FOE_UNB_AGE, DFL_FOE_ACL_DLTA, 8, 8);
	
	/* set Delta time for aging out an unbind FOE entry */
	RegModifyBits(PPE_FOE_UNB_AGE, DFL_FOE_UNB_DLTA, 0, 8);

	/* set Delta time for aging out an bind UDP FOE entry */
	RegModifyBits(PPE_FOE_BND_AGE1, DFL_FOE_UDP_DLTA, 0, 16);

	/* set Delta time for aging out an bind TCP FIN FOE entry */
	RegModifyBits(PPE_FOE_BND_AGE2, DFL_FOE_FIN_DLTA, 16, 16);

	/* set Delta time for aging out an bind TCP FOE entry */
	RegModifyBits(PPE_FOE_BND_AGE2, DFL_FOE_TCP_DLTA, 0, 16);
}

static void PpeSetFoeKa(void)
{
	/* set Keep alive packet with new/org header */
	RegModifyBits(PPE_FOE_CFG, DFL_FOE_KA_ORG, 12, 1);

	/* set Keep alive enable */
	RegModifyBits(PPE_FOE_CFG, DFL_FOE_KA_EN, 13, 1);

	/* ACL link to FOE age enable */
	RegModifyBits(PPE_FOE_CFG, DFL_FOE_ACL_AGE, 14, 1);
	
	/* Keep alive timer value */
	RegModifyBits(PPE_FOE_KA, DFL_FOE_KA_T, 0, 16);

	/* Keep alive time for bind FOE TCP entry */
	RegModifyBits(PPE_FOE_KA, DFL_FOE_TCP_KA, 16, 8);

	/* Keep alive timer for bind FOE UDP entry */
	RegModifyBits(PPE_FOE_KA, DFL_FOE_UDP_KA, 24, 8);

}

static void PpeSetFoeBindRate(uint32_t FoeBindRate)
{
	/* Allowed max entries to be build during a time stamp unit */

	/* smaller than 1/4 of total entries */
	RegModifyBits(PPE_FOE_LMT1, DFL_FOE_QURT_LMT, 0, 14);

	/* between 1/2 and 1/4 of total entries */
	RegModifyBits(PPE_FOE_LMT1, DFL_FOE_HALF_LMT, 16, 14);

	/* between full and 1/2 of total entries */
	RegModifyBits(PPE_FOE_LMT2, DFL_FOE_FULL_LMT, 0, 14);

	/* Set reach bind rate for unbind state */
	RegWrite(PPE_FOE_BNDR, FoeBindRate);
}


static void PpeSetFoeGloCfgEbl(uint32_t Ebl)
{
	if(Ebl==1) {
		/* PPE Engine Enable */ 
		RegModifyBits(PPE_GLO_CFG, 1, 0, 1);

		/* PPE Packet with TTL=0 */ 
		RegModifyBits(PPE_GLO_CFG, DFL_TTL0_DRP, 4, 1);

		/* Use VLAN priority tag as priority decision */
		RegModifyBits(PPE_GLO_CFG, DFL_VPRI_EN, 8, 1);

		/* Use DSCP as priority decision */
		RegModifyBits(PPE_GLO_CFG, DFL_DPRI_EN, 9, 1);

		/* Re-generate VLAN priority tag */
		RegModifyBits(PPE_GLO_CFG, DFL_REG_VPRI, 10, 1);

		/* Re-generate DSCP */
		RegModifyBits(PPE_GLO_CFG, DFL_REG_DSCP, 11, 1);
		
		/* Random early drop mode */
		RegModifyBits(PPE_GLO_CFG, DFL_RED_MODE, 12, 2);

	} else {
		/* PPE Engine Disable */ 
		RegModifyBits(PPE_GLO_CFG, 0, 0, 1);
	}

}

#ifndef CONFIG_RALINK_RT3052_MP
/*
 * - VLAN->UP: Incoming VLAN Priority to User Priority (Fixed)
 * - DSCP->UP: Incoming DSCP to User Priority
 * - UP->xxx : User Priority to VLAN/InDSCP/OutDSCP/AC Priority Mapping
 *
 * VLAN | DSCP |  UP |VLAN Pri|In-DSCP |Out-DSCP| AC | WMM_AC
 * -----+------+-----+--------+--------+--------+----+-------
 *   0	| 00-07|  0  |   0    |  0x00  |  0x00	|  0 |  BE
 *   3	| 08-15|  3  |   3    |  0x08  |  0x10	|  0 |  BE
 *   1	| 16-23|  1  |   1    |  0x08  |  0x00	|  0 |  BG
 *   2  | 24-31|  2  |   2    |  0x10  |  0x08	|  0 |  BG
 *   4	| 32-39|  4  |   4    |  0x20  |  0x18	|  1 |  VI
 *   5  | 40-47|  5  |   5    |  0x28  |  0x20	|  1 |  VI
 *   6  | 48-55|  6  |   6    |  0x30  |  0x28	|  2 |  VO
 *   7	| 56-32|  7  |   7    |  0x38  |  0x30	|  2 |  VO
 * -----+------+-----+--------+--------+--------+----+--------
 *
 */
static void  PpeSetUserPriority(void)
{
    /* Set weight of decision in resolution */
    RegWrite(UP_RES, DFL_UP_RES);
   
    /* Set DSCP to User priority mapping table */ 
    RegWrite(DSCP0_7_MAP_UP, DFL_DSCP0_7_UP);
    RegWrite(DSCP8_15_MAP_UP, DFL_DSCP8_15_UP);
    RegWrite(DSCP16_23_MAP_UP, DFL_DSCP16_23_UP);
    RegWrite(DSCP24_31_MAP_UP, DFL_DSCP24_31_UP);
    RegWrite(DSCP32_39_MAP_UP, DFL_DSCP32_39_UP);
    RegWrite(DSCP40_47_MAP_UP, DFL_DSCP40_47_UP);
    RegWrite(DSCP48_55_MAP_UP, DFL_DSCP48_55_UP);
    RegWrite(DSCP56_63_MAP_UP, DFL_DSCP56_63_UP);
   
#if 0 
    /* Set boundary and range of auto user priority */ 
    RegModifyBits(AUTO_UP_CFG1, DFL_ATUP_BND1, 16, 14);
    RegModifyBits(AUTO_UP_CFG2, DFL_ATUP_BND2, 0, 14);
    RegModifyBits(AUTO_UP_CFG2, DFL_ATUP_BND3, 16, 14);
    RegModifyBits(AUTO_UP_CFG1, DFL_ATUP_R1_UP, 0, 3);
    RegModifyBits(AUTO_UP_CFG1, DFL_ATUP_R2_UP, 4, 3);
    RegModifyBits(AUTO_UP_CFG1, DFL_ATUP_R3_UP, 8, 3);
    RegModifyBits(AUTO_UP_CFG1, DFL_ATUP_R4_UP, 12, 3);
#endif

    /* Set mapping table of user priority to vlan priority */
    RegModifyBits(UP_MAP_VPRI, DFL_UP0_VPRI, 0, 3);
    RegModifyBits(UP_MAP_VPRI, DFL_UP1_VPRI, 4, 3);
    RegModifyBits(UP_MAP_VPRI, DFL_UP2_VPRI, 8, 3);
    RegModifyBits(UP_MAP_VPRI, DFL_UP3_VPRI, 12, 3);
    RegModifyBits(UP_MAP_VPRI, DFL_UP4_VPRI, 16, 3);
    RegModifyBits(UP_MAP_VPRI, DFL_UP5_VPRI, 20, 3);
    RegModifyBits(UP_MAP_VPRI, DFL_UP6_VPRI, 24, 3);
    RegModifyBits(UP_MAP_VPRI, DFL_UP7_VPRI, 28, 3);
   
    /* Set mapping table of user priority to in-profile DSCP */
    RegModifyBits(UP0_3_MAP_IDSCP, DFL_UP0_IDSCP, 0, 6);
    RegModifyBits(UP0_3_MAP_IDSCP, DFL_UP1_IDSCP, 8, 6);
    RegModifyBits(UP0_3_MAP_IDSCP, DFL_UP2_IDSCP, 16, 6);
    RegModifyBits(UP0_3_MAP_IDSCP, DFL_UP3_IDSCP, 24, 6);
    RegModifyBits(UP4_7_MAP_IDSCP, DFL_UP4_IDSCP, 0, 6);
    RegModifyBits(UP4_7_MAP_IDSCP, DFL_UP5_IDSCP, 8, 6);
    RegModifyBits(UP4_7_MAP_IDSCP, DFL_UP6_IDSCP, 16, 6);
    RegModifyBits(UP4_7_MAP_IDSCP, DFL_UP7_IDSCP, 24, 6);
     
    /* Set mapping table of user priority to out-profile DSCP */
    RegModifyBits(UP0_3_MAP_ODSCP, DFL_UP0_ODSCP, 0, 6);
    RegModifyBits(UP0_3_MAP_ODSCP, DFL_UP1_ODSCP, 8, 6);
    RegModifyBits(UP0_3_MAP_ODSCP, DFL_UP2_ODSCP, 16, 6);
    RegModifyBits(UP0_3_MAP_ODSCP, DFL_UP3_ODSCP, 24, 6);
    RegModifyBits(UP4_7_MAP_ODSCP, DFL_UP4_ODSCP, 0, 6);
    RegModifyBits(UP4_7_MAP_ODSCP, DFL_UP5_ODSCP, 8, 6);
    RegModifyBits(UP4_7_MAP_ODSCP, DFL_UP6_ODSCP, 16, 6);
    RegModifyBits(UP4_7_MAP_ODSCP, DFL_UP7_ODSCP, 24, 6);

    /* Set mapping table of user priority to access category */
    RegModifyBits(UP_MAP_AC, DFL_UP0_AC, 0, 2);
    RegModifyBits(UP_MAP_AC, DFL_UP1_AC, 2, 2);
    RegModifyBits(UP_MAP_AC, DFL_UP2_AC, 4, 2);
    RegModifyBits(UP_MAP_AC, DFL_UP3_AC, 6, 2);
    RegModifyBits(UP_MAP_AC, DFL_UP4_AC, 8, 2);
    RegModifyBits(UP_MAP_AC, DFL_UP5_AC, 10, 2);
    RegModifyBits(UP_MAP_AC, DFL_UP6_AC, 12, 2);
    RegModifyBits(UP_MAP_AC, DFL_UP7_AC, 14, 2);
}
#endif

static void FoeFreeTbl(uint32_t NumOfEntry)
{
	uint32_t FoeTblSize;

	FoeTblSize = NumOfEntry * sizeof(struct FoeEntry);
	pci_free_consistent(NULL, FoeTblSize, PpeFoeBase, PpePhyFoeBase);
	RegWrite( PPE_FOE_BASE, 0);
}

static int32_t PpeEngStart(void)
{
	/* Set PPE Flow Set */
	PpeSetFoeEbl(1);

	/* Set PPE FOE Hash Mode */
	PpeSetFoeHashMode(DFL_FOE_HASH_MODE);

	/* Set Policy Table Index */
	PpeSetPreAclEbl(0);
	PpeSetPreMtrEbl(0);
	PpeSetPreAcEbl(0);
	PpeSetPostMtrEbl(0);
	PpeSetPostAcEbl(0);

	/* Set Auto Age-Out Function */
	PpeSetAgeOut();

	/* Set PPE FOE KEEPALIVE TIMER */
	PpeSetFoeKa(); 

	/* Set PPE FOE Bind Rate */
	PpeSetFoeBindRate(DFL_FOE_BNDR); 

	/* Set PPE Global Configuration */
	PpeSetFoeGloCfgEbl(1);

#ifndef CONFIG_RALINK_RT3052_MP
	/* Set User Priority related register */
	PpeSetUserPriority();
#endif
	return 0;
}

static int32_t PpeEngStop(void)
{
	/* Set PPE FOE ENABLE */
	PpeSetFoeGloCfgEbl(0);

	/* Set PPE Flow Set */
	PpeSetFoeEbl(0);

	/* Free FOE table */ 
	FoeFreeTbl(FOE_4TB_SIZ);

	return 0;
}

static void PpeSetDstPort(void)
{
	DstPort[DP_PCI]=dev_get_by_name(CONFIG_RA_HW_NAT_PCI_NAME); /* PCI Slot */
	DstPort[DP_GMAC]=dev_get_by_name(CONFIG_RA_HW_NAT_GMAC_NAME); /* GMAC */
	DstPort[DP_WLAN]=dev_get_by_name(CONFIG_RA_HW_NAT_WLAN_NAME);   /* WLAN */
}

static void PpeInsTestEntry(void) 
{
/* Test Case Lists*/
//#define IP_ACL_TO_FOE
//#define IPFRAG_ACL_TO_FOE

/* ACL Link to Foe Entry Number */
#define FOE_ENTRY_ID	30

#if defined IP_ACL_TO_FOE
	struct FoeEntry *entry;
	AclPlcyNode node;

	entry=&PpeFoeBase[FOE_ENTRY_ID*2+1]; // entry num=30

	entry->usib1.sta=1; //static entry 
	entry->usib1.state=UNBIND; // unbind state
	entry->usib1.minf=1; // more information
	entry->usib1.ttl=0; // !TTL-1 
	entry->usib1.pppoe=0; //No Act
	entry->usib1.snap=0;
	entry->usib1.v1=0; //No Act
	entry->usib1.v2=0;
	entry->usib1.da=0; //No Act
	entry->usib1.sa=0; //No Act
	entry->usib1.dport=0; //No Act
	entry->usib1.sport=0; //No Act
	entry->usib1.sip=1; //Modify sip
	entry->usib1.dip=1; //Modify dip

	entry->sip=Str2Ip("10.10.10.3"); //sip
	entry->dip=Str2Ip("10.10.20.3"); //dip

	entry->iblk2.fd=1; //force destination
	entry->iblk2.dp=0; /* 0:cpu, 1:GE1, 2:GE2 */

	entry->new_sip=Str2Ip("10.10.10.3"); //new sip
	entry->new_dip=Str2Ip("10.10.20.3"); //new dip

	node.SipS=Str2Ip("10.10.10.3");
	node.SipE=Str2Ip("10.10.10.5");
	node.DipS=Str2Ip("10.10.20.3");
	node.DipE=Str2Ip("10.10.20.3");
	node.FoeTb=FOE_ENTRY_ID;

	PpeSetPreAclEbl(0);
	AclSetIpEntry(&node, SIP, ENTRY_END_FOE);
	PpeSetPreAclEbl(1);

#elif defined IPFRAG_ACL_TO_FOE
	struct FoeEntry *entry;
	AclPlcyNode node;
	unsigned char smac[6]={0x0, 0x0C, 0x43, 0x40, 0x90, 0x68};
	unsigned char dmac[6]={0x0, 0x16, 0x36, 0x1F, 0x91, 0xCC};

	/* Set Foe Table */
	entry=&PpeFoeBase[FOE_ENTRY_ID*2+1]; 

	entry->usib1.sta=1; //static entry 
	entry->usib1.state=UNBIND; // unbind state
	entry->usib1.minf=1; // more information
	entry->usib1.ttl=0; // !TTL-1 
	entry->usib1.pppoe=0; //No Act
	entry->usib1.snap=0;
	entry->usib1.v1=MODIFY; 
	entry->usib1.v2=0;
	entry->usib1.da=1;
	entry->usib1.sa=1;
	entry->usib1.dport=0;
	entry->usib1.sport=0;
	entry->usib1.sip=1; //Modify sip
	entry->usib1.dip=1; //Modify dip
	entry->vlan1 = 1;

	entry->sip=Str2Ip("10.10.10.3"); //sip
	entry->dip=Str2Ip("10.10.20.3"); //dip

	entry->iblk2.fd=1; //force destination
	entry->iblk2.dp=0; /* 0:cpu, 1:GE1, 2:GE2*/

	entry->new_sip=Str2Ip("10.10.10.254"); //new sip
	entry->new_dip=Str2Ip("10.10.20.3"); //new dip
	
	/* Set Layer2 Info - DMAC, SMAC */
	FoeSetMacInfo(entry->dmac_hi, dmac);
	FoeSetMacInfo(entry->smac_hi, smac);

	PpeSetPreAclEbl(0);

	/* SIP */
	node.SipS=Str2Ip("10.10.10.3");
	node.SipE=Str2Ip("10.10.10.3");
	AclSetIpEntry(&node, SIP,  NOT_ENTRY_END);

	/* DIP */
	node.DipS=Str2Ip("10.10.20.3");
	node.DipE=Str2Ip("10.10.20.3");
	AclSetIpEntry(&node, DIP,  NOT_ENTRY_END);
	
	/* IP Fragment */
	node.FoeTb=FOE_ENTRY_ID;
	AclSetIpFragEntry(&node, ENTRY_END_FOE);
	PpeSetPreAclEbl(1);
#endif	  
}

static uint32_t SetGdmaFwd(uint32_t Ebl) 
{
	uint32_t data=0;

	data=RegRead(GDMA1_FWD_CFG);

	if(Ebl) {	
	    //Uni-cast frames forward to PPE
	    data |= GDM1_UFRC_P_PPE;
	    //Broad-cast MAC address frames forward to PPE
	    data |= GDM1_BFRC_P_PPE;
	    //Multi-cast MAC address frames forward to PPE
	    data |= GDM1_MFRC_P_PPE;
	    //Other MAC address frames forward to PPE
	    data |= GDM1_OFRC_P_PPE;

	}else {
	    //Uni-cast frames forward to CPU
	    data &= ~GDM1_UFRC_P_PPE;
	    //Broad-cast MAC address frames forward to CPU
	    data &= ~GDM1_BFRC_P_PPE;
	    //Multi-cast MAC address frames forward to CPU
	    data &= ~GDM1_MFRC_P_PPE;
	    //Other MAC address frames forward to CPU
	    data &= ~GDM1_OFRC_P_PPE;
	
	}

	RegWrite(GDMA1_FWD_CFG, data);

#ifdef CONFIG_PSEUDO_SUPPORT
	RegWrite(GDMA2_FWD_CFG, data);
#endif

	return 0;  
}

static int32_t GetChipInfo(void)
{
    uint8_t Id[10];

    memset(Id, 0, sizeof(Id));
    strncpy(Id, (char *)CHIPID, 8);
    ChipVer = RegRead(REVID);

    NAT_PRINT("CHIPID=%s\n", Id);
    NAT_PRINT("REVID=%0X\n",ChipVer);

    if(strcmp(Id,"RT2880  ")==0) {
	ChipId=RT2880;
    }else if(strcmp(Id,"RT3052  ")==0) {
	ChipId=RT3052;
    }else if(strcmp(Id,"RT2883  ")==0) {
	ChipId=RT2883;
    }else {
	return 0;
    }

    return 1;
}


/*
 * PPE Enabled: GMAC<->PPE<->CPU
 * PPE Disabled: GMAC<->CPU
 */
static int32_t PpeInitMod(void)
{

    /* Get Chip Information */
    if(!GetChipInfo()) {
	NAT_PRINT("This Chip is not supported.\n");
	return 1;
    }

    NAT_PRINT("Ralink HW NAT Module Enabled\n");

    //Get net_device structure of Dest Port 
    PpeSetDstPort();

    /* Register ioctl handler */
    PpeRegIoctlHandler();
    AclRegIoctlHandler();
    AcRegIoctlHandler();
    MtrRegIoctlHandler();

    /* Initialize PPE related register */
    PpeEngStart();

    /* Insert test entry */
    PpeInsTestEntry();

    /* Register RX/TX hook point */
    ra_sw_nat_hook_tx = PpeTxHandler;
    ra_sw_nat_hook_rx = PpeRxHandler;
    
    /* Set GMAC fowrards packet to PPE */
    SetGdmaFwd(1);

    return 0;
}

static void PpeCleanupMod(void)
{
    NAT_PRINT("Ralink HW NAT Module Disabled\n");

    /* Set GMAC fowrards packet to CPU */
    SetGdmaFwd(0);

    /* Unregister RX/TX hook point */
    ra_sw_nat_hook_rx = NULL;
    ra_sw_nat_hook_tx = NULL;

    /* Unregister ioctl handler */
    PpeUnRegIoctlHandler();
    AclUnRegIoctlHandler();
    AcUnRegIoctlHandler();
    MtrUnRegIoctlHandler();

    /* Restore PPE related register */
    PpeEngStop();

}

module_init(PpeInitMod);
module_exit(PpeCleanupMod);

MODULE_AUTHOR("Steven Liu");
MODULE_LICENSE("Proprietary");
MODULE_DESCRIPTION("Ralink Hardware NAT v0.3\n");

