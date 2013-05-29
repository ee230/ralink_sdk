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
    util.c

    Abstract:

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
    Name        Date            Modification logs
    Steven Liu  2006-10-06      Initial version
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
#include <linux/ctype.h>
#include <asm/uaccess.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <net/tcp.h>
#include <linux/udp.h>
#include <linux/in.h>
#include <linux/if_vlan.h>
#include <asm/checksum.h>

#include "util.h"

void MacReverse(uint8_t *Mac)
{
    uint8_t tmp;
    uint8_t i;

    for(i=5;i>2;i--)
    {
	tmp=Mac[i];
	Mac[i]=Mac[5-i];
	Mac[5-i]=tmp;
    }
}

int GetNext(char *src, int separator, char *  dest)
{
        char *  c;
        int     len;

        if ( (src == NULL) || (dest == NULL) ) {
                return -1;
        }

        c = strchr(src, separator);
        if (c == NULL) {
                strcpy(dest, src);
                return -1;
        }
        len = c - src;
        strncpy(dest, src, len);
        dest[len] = '\0';
        return len + 1;
}

static inline int atoi(char *s)
{
        int i=0;
        while (isdigit(*s)) {
                i = i*10 + *(s++) - '0';
        }
        return i;
}

/* Convert IP address from Hex to string */                                   
uint8_t *Ip2Str(IN uint32_t Ip) 
{      
    static uint8_t Buf[32];
    uint8_t *ptr = (char *)&Ip;
    uint8_t  c[4];
    
    c[0] = *(ptr);
    c[1] = *(ptr+1);
    c[2] = *(ptr+2);
    c[3] = *(ptr+3);
    sprintf(Buf, "%d.%d.%d.%d", c[0], c[1], c[2], c[3]);
    return Buf;
}

unsigned int Str2Ip(IN char *str)
{
    int             len;
    char *          ptr = str;
    char            buf[128];
    unsigned char   c[4];
    int             i;                                                                                            
    for (i = 0; i < 3; ++i) {
	if ((len = GetNext(ptr, '.', buf)) == -1) {
	    return 1; /* parsing error */
	}
	c[i] = atoi(buf);
	ptr += len;
    }
    c[3] = atoi(ptr);
    return ((c[0]<<24) + (c[1]<<16) + (c[2]<<8) + c[3]);
}

/* calculate ip address range */
/* start_ip <= x < end_ip */
void CalIpRange(uint32_t StartIp, uint32_t EndIp, uint8_t *M, uint8_t *E)
{
  uint32_t Range=(EndIp+1)-StartIp;
  uint32_t i;

  for(i=0;i<32;i++){
      if( (Range>>i) & 0x01) {
	  break;
      }
  }

  if(i!=32) {
      *M= Range>>i;
      *E= i;
  } else {
      *M= 0;
      *E= 0;
  }

}

/* update mss field in tcp header to avoid packet fragmentation */
void ChangeTcpMssOpt(INOUT struct sk_buff *skb,
	IN uint16_t mss, INOUT struct tcphdr *th)
{
    uint8_t *ptr;
    uint16_t org_mss;
    uint32_t sum;

    uint32_t length=(th->doff*4)-sizeof(struct tcphdr);
    ptr = (uint8_t *)(th + 1);

    while(length>0) {
	int opcode=*ptr++;
	int opsize;

	switch (opcode) {
	case TCPOPT_EOL:
	    return;
	case TCPOPT_NOP:        /* Ref: RFC 793 section 3.1 */
	    length--;
	    continue;
	default:
	    opsize=*ptr++;
	    if (opsize < 2) /* "silly options" */
		return;
	    if (opsize > length)
		return; /* don't parse partial options */
	    switch(opcode) {
	    case TCPOPT_MSS:
		if(opsize==TCPOLEN_MSS) {
		    org_mss = ntohs(*(__u16 *)ptr);
		    if (org_mss) {
			// if original mss is bigger than new mss, overwrite it.
			if(org_mss > mss){
			    *(__u16 *)ptr = htons(mss);

			    // Recalculate tcp checksum
			    // Ref RFC1141(Incremental Updating of the Internet Checksum)
			    sum = org_mss + (~mss & 0xffff);
			    sum += ntohs(th->check);
			    sum = (sum & 0xffff) + (sum>>16);
			    th->check = htons(sum + (sum>>16));
			}
		    }
		}
		break;
	    case TCPOPT_WINDOW:
		break;
	    case TCPOPT_TIMESTAMP:
		break;
	    case TCPOPT_SACK_PERM:
		break;
	    case TCPOPT_SACK:
		break;
	    };
	    ptr+=opsize-2;
	    length-=opsize;
	};
    }

}

void RegModifyBits ( uint32_t Addr, uint32_t Data, 
		     uint32_t  Offset, uint32_t Len)
{
    uint32_t Mask=0;
    uint32_t Value;
    uint32_t i;

    for (i = 0; i < Len; i++) {
	Mask |= 1 << (Offset + i);
    }

    Value = RegRead(Addr);
    Value &= ~Mask;
    Value |= (Data << Offset) & Mask;;

    RegWrite(Addr, Value);
}

static inline uint16_t
CsumPart(uint32_t o, uint32_t n, uint16_t old)
{
        uint32_t d[] = { o, n };
	    return csum_fold(csum_partial((char *) d, sizeof(d), old ^ 0xFFFF));
}

/*
 * KeepAlive with new header mode will pass the modified packet to cpu.
 * We must change to original packet to refresh NAT table.
 */

/*
 * Recover TCP Src/Dst Port and recalculate tcp checksum
 */
void FoeToOrgTcpHdr(IN struct FoeEntry *foe_entry, IN struct iphdr *iph, OUT struct tcphdr *th)
{
    th->check = CsumPart((th->source) ^ 0xffff, htons(foe_entry->sport), th->check);
    th->check = CsumPart((th->dest) ^ 0xffff, htons(foe_entry->dport), th->check);
    th->check = CsumPart(~(iph->saddr), htonl(foe_entry->sip), th->check);
    th->check = CsumPart(~(iph->daddr), htonl(foe_entry->dip), th->check);
    th->source = htons(foe_entry->sport);
    th->dest = htons(foe_entry->dport);
}

/*
 * Recover UDP Src/Dst Port and recalculate udp checksum
 */
void FoeToOrgUdpHdr(IN struct FoeEntry *foe_entry,IN struct iphdr *iph, OUT struct udphdr *uh)
{
    uh->check = CsumPart((uh->source)^0xffff, htons(foe_entry->sport), uh->check);
    uh->check = CsumPart((uh->dest)^0xffff, htons(foe_entry->dport), uh->check);
    uh->check = CsumPart(~(iph->saddr), htonl(foe_entry->sip), uh->check);
    uh->check = CsumPart(~(iph->daddr), htonl(foe_entry->dip), uh->check);
    uh->source = htons(foe_entry->sport);
    uh->dest = htons(foe_entry->dport);
}

/*
 * Recover Src/Dst IP and recalculate ip checksum
 */
void FoeToOrgIpHdr(IN struct FoeEntry *foe_entry, OUT struct iphdr *iph)
{ 
    iph->saddr = htonl(foe_entry->sip);
    iph->daddr = htonl(foe_entry->dip);
    iph->check = 0;
    iph->check = ip_fast_csum((unsigned char *) (iph), iph->ihl);

}

/*
 * Recover SIP/DIP, SP/DP, SMAC/DMAC, VLANID and recalculate ip checksum
 */
void PpePktRebuild(struct sk_buff *skb, struct iphdr *iph, struct FoeEntry *foe_entry)
{
    struct udphdr *uh;
    struct tcphdr *th;
    struct ethhdr *eth;
    struct FoeEntry entry;
    
    memcpy(&entry, foe_entry, sizeof(entry));
   
    //overwrite layer4 header
    if(iph->protocol==IPPROTO_TCP) {
	th = (struct tcphdr *) ((uint8_t *) iph + iph->ihl * 4);
	th->check = CsumPart((th->source) ^ 0xffff, htons(entry.new_sport), th->check);
	th->check = CsumPart((th->dest) ^ 0xffff, htons(entry.new_dport), th->check);
	th->check = CsumPart(~(iph->saddr), htonl(entry.new_sip), th->check);
	th->check = CsumPart(~(iph->daddr), htonl(entry.new_dip), th->check);
	th->source = htons(entry.new_sport);
	th->dest = htons(entry.new_dport);
    }else if(iph->protocol==IPPROTO_UDP) {
	uh = (struct udphdr *) ((uint8_t *) iph + iph->ihl * 4);
	uh->check = CsumPart((uh->source)^0xffff, htons(entry.new_sport), uh->check);
	uh->check = CsumPart((uh->dest)^0xffff, htons(entry.new_dport), uh->check);
	uh->check = CsumPart(~(iph->saddr), htonl(entry.new_sip), uh->check);
	uh->check = CsumPart(~(iph->daddr), htonl(entry.new_dip), uh->check);
	uh->source = htons(entry.new_sport);
	uh->dest = htons(entry.new_dport);
    } 

    //overwrite layer3 header
    iph->saddr = htonl(entry.new_sip);
    iph->daddr = htonl(entry.new_dip);
    iph->check = 0;   
    iph->check = ip_fast_csum((unsigned char *) (iph), iph->ihl);

    //overwrite vlan header
    if (entry.bfib1.v1==INSERT) { 
	skb_push(skb, VLAN_HLEN);
	/* vlan_type(0x8100) + vlan id + eth_type(0x0800) */
	*(uint16_t *) ((skb)->data) = htons(entry.vlan1);
	*(uint16_t *) ((skb)->data - 2) = htons(ETH_P_8021Q);
    }

    //overwrite layer2 header
    skb_push(skb, ETH_HLEN);
    eth=(struct ethhdr *)skb->data;
    FoeGetMacInfo(eth->h_dest, entry.dmac_hi);
    FoeGetMacInfo(eth->h_source, entry.smac_hi);
}
