/*
 * 	ralink_esw.c -- Ralink RT3052 Embedded Fast-Ethernet Switch MIB Support
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
*/

#include	<sys/param.h>
#include	<sys/types.h>
#include	<sys/time.h>
#include	<stdio.h>
#include	<netdb.h>
#include	<unistd.h>
#include 	<sys/file.h>

#include	"ctypes.h"
#include	"local.h"
#include	"debug.h"
#include	"miv.h"
#include	"asn.h"
#include	"mix.h"
#include	"ralink_esw.h"


static	CUnslType	eswSnmp;

struct ralink_esw_mib
{
	/* flow control */
	unsigned long 	cdma_fc_cfg;	/* ralink esw mib - ralinksnmp 1 */
	unsigned long 	gdma1_fc_cfg;	/* ralink esw mib - ralinksnmp 2 */
	unsigned long 	pdma_fc_cfg;	/* ralink esw mib - ralinksnmp 3 */

	/* switch scheduler */
	unsigned long	gdma1_sch_cfg;	/* ralink esw mib - ralinksnmp 4 */
	unsigned long	gdma2_sch_cfg;	/* ralink esw mib - ralinksnmp 5 */
	unsigned long 	pdma_sch_cfg;	/* ralink esw mib - ralinksnmp 6 */

	/* rx counters */
	unsigned long	gdma_rx_gbcnt0;	/* ralink esw mib - ralinksnmp 7 */
	unsigned long	gdma_rx_gpcnt0;	/* ralink esw mib - ralinksnmp 8 */
	unsigned long	gdma_rx_oercnt0; /* ralink esw mib - ralinksnmp 9 */
	unsigned long	gdma_rx_fercnt0; /* ralink esw mib - ralinksnmp 10 */
	unsigned long	gdma_rx_sercnt0; /* ralink esw mib - ralinksnmp 11 */
	unsigned long	gdma_rx_lercnt0; /* ralink esw mib - ralinksnmp 12 */
	unsigned long	gdma_rx_cercnt0; /* ralink esw mib - ralinksnmp 13 */

	/* ports stat */
	unsigned long	port0_counter;	/* ralink esw mib - ralinksnmp 14 */
	unsigned long	port1_counter;	/* ralink esw mib - ralinksnmp 15 */
	unsigned long	port2_counter;	/* ralink esw mib - ralinksnmp 16 */
	unsigned long	port3_counter;	/* ralink esw mib - ralinksnmp 17 */
	unsigned long	port4_counter;	/* ralink esw mib - ralinksnmp 18 */
	unsigned long	port5_counter;	/* ralink esw mib - ralinksnmp 19 */
};

static AsnIdType eswRetrieve(CIntfType item)
{
	AsnIdType 	asnresult;
	unsigned long 	result;
	struct ralink_esw_mib esw_stat;
	FILE 		*in;
	char		line[1024];

	in = fopen("/proc/rt2880/snmp", "r");

	if (!in)
	{
		printf("eswRetrieve() error opening proc file, please insmod reath module!\n");
		return 0;
	}

	while ( line == fgets(line, 1024, in))
	{
		if ( 7 == sscanf(line, "rx counters: %lu %lu %lu %lu %lu %lu %lu\n", &esw_stat.gdma_rx_gbcnt0, &esw_stat.gdma_rx_gpcnt0, &esw_stat.gdma_rx_oercnt0, &esw_stat.gdma_rx_fercnt0, &esw_stat.gdma_rx_sercnt0, &esw_stat.gdma_rx_lercnt0, &esw_stat.gdma_rx_cercnt0)) {
		//	printf("rx counters - %d\n", esw_stat.gdma_rx_gbcnt0);
			continue;
		}

		if ( 3 == sscanf(line, "fc config: %lu %lu %lu\n", &esw_stat.cdma_fc_cfg, &esw_stat.gdma1_fc_cfg, &esw_stat.pdma_fc_cfg)) {
			continue;
		} 

		if ( 3 == sscanf(line, "scheduler: %lu %lu %lu\n", &esw_stat.gdma1_sch_cfg, &esw_stat.gdma2_sch_cfg, &esw_stat.pdma_sch_cfg)) {
			continue;
		}
		if ( 6 == sscanf(line, "ports: %lu %lu %lu %lu %lu %lu\n", &esw_stat.port0_counter, &esw_stat.port0_counter, &esw_stat.port1_counter, &esw_stat.port2_counter, &esw_stat.port3_counter, &esw_stat.port4_counter, &esw_stat.port5_counter))
			break;
	}	/* while */

	fclose(in);

	switch ( item-1 ) {
		case RALINKESW_CDMAFCCFG:
			result = esw_stat.cdma_fc_cfg;
			break;
		case RALINKESW_GDMA1FCCFG:
			result = esw_stat.gdma1_fc_cfg;
			break;
		case RALINKESW_PDMAFCCFG:
			result = esw_stat.pdma_fc_cfg;
			break;
		case RALINKESW_GDMA1SCHCFG:
			result = esw_stat.gdma1_sch_cfg;
			break;
		case RALINKESW_GDMA2SCHCFG:
			result = esw_stat.gdma2_sch_cfg;
			break;
		case RALINKESW_PDMASCHCFG:
			result = esw_stat.pdma_sch_cfg;
			break;
		case RALINKESW_GDMAGBCNT0:
			result = esw_stat.gdma_rx_gbcnt0;
			break;
		case RALINKESW_GDMAGPCNT0:
			result = esw_stat.gdma_rx_gpcnt0;
			break;
		case RALINKESW_GDMAOERCNT0:
			result = esw_stat.gdma_rx_oercnt0;
			break;
		case RALINKESW_GDMAFERCNT0:
			result = esw_stat.gdma_rx_fercnt0;
			break;
		case RALINKESW_GDMASERCNT0:
			result = esw_stat.gdma_rx_sercnt0;
			break;
		case RALINKESW_GDMALERCNT0:
			result = esw_stat.gdma_rx_lercnt0;
			break;
		case RALINKESW_GDMACERCNT0:
			result = esw_stat.gdma_rx_cercnt0;
			break;
		case RALINKESW_PORT0CNT:
			result = esw_stat.port0_counter;
			break;
		case RALINKESW_PORT1CNT:
			result = esw_stat.port1_counter;
			break;
		case RALINKESW_PORT2CNT:
			result = esw_stat.port2_counter;
			break;
		case RALINKESW_PORT3CNT:
			result = esw_stat.port3_counter;
			break;
		case RALINKESW_PORT4CNT:
			result = esw_stat.port4_counter;
			break;
		case RALINKESW_PORT5CNT:
			result = esw_stat.port5_counter;
			break;
		default:
			break;
	}	/* switch-case */


	asnresult = asnUnsl (asnClassApplication, (AsnTagType) 1, result);
	return (asnresult);
}

static	MixStatusType	eswRelease(MixCookieType cookie)
{
	cookie = cookie;
	return (smpErrorGeneric);
}

static	MixStatusType	eswCreate(MixCookieType cookie, MixNamePtrType name, MixLengthType namelen, AsnIdType asn)
{
	cookie = cookie;
	name = name;
	namelen = namelen;
	asn = asn;
	return (smpErrorGeneric);
}

static	MixStatusType	eswDestroy(MixCookieType cookie, MixNamePtrType name, MixLengthType namelen)
{
	cookie = cookie;
	name = name;
	namelen = namelen;
	return (smpErrorGeneric);
}

static	AsnIdType	eswNext(MixCookieType cookie, MixNamePtrType name, MixLengthPtrType namelenp)
{
	CIntfType		item;

	cookie = cookie;
	if (*namelenp == (MixLengthType) 0) {
		*namelenp = (MixLengthType) 2;
		*name++ = (MixNameType) 1;
		*name = (MixNameType) 0;
		return (eswRetrieve ((CIntfType) 1));
	} else if (*namelenp == (MixLengthType) 1) {
		if ((item = (CIntfType) *name) <= (CIntfType) (ESW_MAXTYPE+1)) {
			*namelenp = (MixLengthType) 2;
			*(++name) = (MixNameType) 0;
			return (eswRetrieve (item));
		} else {
			return ((AsnIdType) 0);
		}	/* if - else  */
	}	/* if- else - if */
	else if ((item = (CIntfType) *name) < (CIntfType) (ESW_MAXTYPE+1)) {
		*namelenp = (MixLengthType) 2;
		*name++ = (MixNameType) (++item);

		return (eswRetrieve (item));
	} else {
		return ((AsnIdType) 0);
	}	/* if -else if -else */
}

static	AsnIdType  eswGet(MixCookieType cookie, MixNamePtrType name, MixLengthType namelen)
{
	CIntfType		item;

	cookie = cookie;
	if ((namelen != (MixLengthType) 2) || ((item = (CIntfType) *name) < (CIntfType) 1) || (item > (CIntfType) (ESW_MAXTYPE+1)) || (*(name + 1) != (MixNameType) 0)) {
			return ((AsnIdType) 0);
	} else {
		return (eswRetrieve (item));
	}
}

static	MixStatusType	eswSet(MixCookieType cookie, MixNamePtrType name, MixLengthType namelen, AsnIdType asn)
{
	cookie = cookie;
	name = name;
	namelen = namelen;
	asn = asn;
	return (smpErrorReadOnly);
}

static MixOpsType	ralink_eswOps = {
	eswRelease,
	eswCreate,
	eswDestroy,
	eswNext,
	eswGet,
	eswSet
};

CVoidType ralink_esw_init(void)
{
	int esw_count;
	unsigned long	result;
	unsigned long 	eswcount;
	FILE	*in;
	struct 		ralink_esw_mib esw_stat;
	char		line[1024];

	in = fopen("/proc/rt2880/snmp","r");

	if(!in) {
		printf("proc open failed!\n");
		return;
	}
#if 1
	while ( line == fgets(line, 1024, in))
	{
		if ( 7 == sscanf(line, "rx counters: %lu %lu %lu %lu %lu %lu %lu\n", &esw_stat.gdma_rx_gbcnt0, &esw_stat.gdma_rx_gpcnt0, &esw_stat.gdma_rx_oercnt0, &esw_stat.gdma_rx_fercnt0, &esw_stat.gdma_rx_sercnt0, &esw_stat.gdma_rx_lercnt0, &esw_stat.gdma_rx_cercnt0)) {
//			printf("rx counters - %d\n", esw_stat.gdma_rx_gbcnt0);
			continue;
		}

		if ( 3 == sscanf(line, "fc config: %lu %lu %lu\n", &esw_stat.cdma_fc_cfg, &esw_stat.gdma1_fc_cfg, &esw_stat.pdma_fc_cfg)) {
//			printf("0x%08x", esw_stat.cdma_fc_cfg);
			continue;
		} 

		if ( 3 == sscanf(line, "scheduler: %lu %lu %lu\n", &esw_stat.gdma1_sch_cfg, &esw_stat.gdma2_sch_cfg, &esw_stat.pdma_sch_cfg)) {
//			printf("0x%08x", esw_stat.pdma_sch_cfg);
			continue;
		}  
		if ( 6 == sscanf(line, "ports: %lu %lu %lu %lu %lu %lu\n", &esw_stat.port0_counter, &esw_stat.port0_counter, &esw_stat.port1_counter, &esw_stat.port2_counter, &esw_stat.port3_counter, &esw_stat.port4_counter, &esw_stat.port5_counter))
			break;
	}	/* while */
#endif
//	printf("\nvalue -- %d %d\n", esw_stat.gdma_rx_gbcnt0, esw_stat.gdma_rx_gpcnt0);
	fclose(in);


	for (esw_count = 0; esw_count <=RALINKESW_GDMACERCNT0; esw_count++)
	{
		switch(esw_count) {
			case RALINKESW_CDMAFCCFG:
				result = esw_stat.cdma_fc_cfg;
				break;
			case RALINKESW_GDMA1FCCFG:
				result = esw_stat.gdma1_fc_cfg;
				break;
			case RALINKESW_PDMAFCCFG:
				result = esw_stat.pdma_fc_cfg;
				break;
			case RALINKESW_GDMA1SCHCFG:
				result = esw_stat.gdma1_sch_cfg;
				break;
			case RALINKESW_GDMA2SCHCFG:
				result = esw_stat.gdma2_sch_cfg;
				break;
			case RALINKESW_PDMASCHCFG:
				result = esw_stat.pdma_sch_cfg;
				break;
			case RALINKESW_GDMAGBCNT0:
				result = esw_stat.gdma_rx_gbcnt0;
				break;
			case RALINKESW_GDMAGPCNT0:
				result = esw_stat.gdma_rx_gpcnt0;
				break;
			case RALINKESW_GDMAOERCNT0:
				result = esw_stat.gdma_rx_oercnt0;
				break;
			case RALINKESW_GDMAFERCNT0:
				result = esw_stat.gdma_rx_fercnt0;
				break;
			case RALINKESW_GDMASERCNT0:
				result = esw_stat.gdma_rx_sercnt0;
				break;
			case RALINKESW_GDMALERCNT0:
				result = esw_stat.gdma_rx_lercnt0;
				break;
			case RALINKESW_GDMACERCNT0:
				result = esw_stat.gdma_rx_cercnt0;
				break;
			case RALINKESW_PORT0CNT:
				result = esw_stat.port0_counter;
				break;
			case RALINKESW_PORT1CNT:
				result = esw_stat.port1_counter;
				break;
			case RALINKESW_PORT2CNT:
				result = esw_stat.port2_counter;
				break;
			case RALINKESW_PORT3CNT:
				result = esw_stat.port3_counter;
				break;
			case RALINKESW_PORT4CNT:
				result = esw_stat.port4_counter;
				break;
			case RALINKESW_PORT5CNT:
				result = esw_stat.port5_counter;
				break;
			default:
				break;
		}	/* switch-case */
			 eswSnmp = (CUnslType)result;
			 (void) misExport ((MixNamePtrType) ESW_OID,
			 	(MixLengthType) 6, &ralink_eswOps, (MixCookieType) 0);
	}	/* for */
}
