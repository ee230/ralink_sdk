/* vi: set sw=4 ts=4 sts=4: */
/*
 *	inic.c -- Intelligent NIC Settings 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: inic.c,v 1.51.2.4 2012-03-30 12:31:45 chhung Exp $
 */

#include	<stdlib.h>
#include	<sys/ioctl.h>
#include	<arpa/inet.h>
#include	"../../autoconf.h"
#ifdef CONFIG_DEFAULTS_KERNEL_2_6_21
  #include	<linux/types.h>
  #include	<linux/socket.h>
  #include	<linux/if.h>
#endif
#include	<linux/wireless.h>

#include	"internet.h"
#include	"nvram.h"
#include	"utils.h"
#include	"webs.h"
#include	"inic.h"
#include	"wireless.h"		// inic.c inherits many security functions from it(wireless.c).
#include	"oid.h"
#include	"stapriv.h"			//for statistics

#include	"linux/autoconf.h"  //kernel config

#define AP_MODE
extern int g_Raix_wsc_configured;

static int	getRaixAutoProvisionBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getRaixDLSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int	getRaixDFSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int	getRaixCarrierBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int	getRaixWDSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int	getRaixRVTBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int	getRaixWSCBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int	getRaixHTStream(int eid, webs_t wp, int argc, char_t **argv);
static int	getRaixABand(int eid, webs_t wp, int argc, char_t **argv);
static int	getRaix11nDraft3Built(int eid, webs_t wp, int argc, char_t **argv);
static int  getRaixWAPIBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getInic11aChannels(int eid, webs_t wp, int argc, char_t **argv);
static int  getInic11bChannels(int eid, webs_t wp, int argc, char_t **argv);
static int  getInic11gChannels(int eid, webs_t wp, int argc, char_t **argv);
static int  getInicChannel(int eid, webs_t wp, int argc, char_t **argv);
static int  getInicCurrentMac(int eid, webs_t wp, int argc, char_t **argv);
static int  getInicWdsEncType(int eid, webs_t wp, int argc, char_t **argv);
static int	getRaixWlanStaInfo(int eid, webs_t wp, int argc, char_t **argv);
static int	getRaixApStats(int eid, webs_t wp, int argc, char_t **argv);
static int	getRaixApSNR(int eid, webs_t wp, int argc, char_t **argv);
static void inicBasic(webs_t wp, char_t *path, char_t *query);
static void inicAdvanced(webs_t wp, char_t *path, char_t *query);
static void inicWmm(webs_t wp, char_t *path, char_t *query);
static void inicWds(webs_t wp, char_t *path, char_t *query);
static void inicwGetSecurity(webs_t wp, char_t *path, char_t *query);
static void INICSecurity(webs_t wp, char_t *path, char_t *query);
static void INICDeleteAccessPolicyList(webs_t wp, char_t *path, char_t *query);
static void inicRvt(webs_t wp, char_t *path, char_t *query);

typedef struct _RT_802_11_MAC_ENTRY {
	unsigned char			ApIdx;
	unsigned char           Addr[6];
	unsigned char           Aid;
	unsigned char           Psm;     // 0:PWR_ACTIVE, 1:PWR_SAVE
	unsigned char           MimoPs;  // 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled
	char                    AvgRssi0;
	char                    AvgRssi1;
	char                    AvgRssi2;
	unsigned int            ConnectedTime;
	MACHTTRANSMIT_SETTING	TxRate;
#if 0
	unsigned int			LastRxRate;
	int						StreamSnr[3];
	int						SoundingRespSnr[3];
#endif
} RT_802_11_MAC_ENTRY;

#if defined (RTDEV_WAPI_SUPPORT)
#define MAX_NUMBER_OF_MAC               96
#else
#define MAX_NUMBER_OF_MAC               32 // if MAX_MBSSID_NUM is 8, this value can't be larger than 211
#endif

typedef struct _RT_802_11_MAC_TABLE {
	unsigned long            Num;
	RT_802_11_MAC_ENTRY      Entry[MAX_NUMBER_OF_MAC]; //MAX_LEN_OF_MAC_TABLE = 32
} RT_802_11_MAC_TABLE;

void formDefineInic(void)
{
	websAspDefine(T("getRaixAutoProvisionBuilt"), getRaixAutoProvisionBuilt);
	websAspDefine(T("getRaixDLSBuilt"), getRaixDLSBuilt);
	websAspDefine(T("getRaixDFSBuilt"), getRaixDFSBuilt);
	websAspDefine(T("getRaixCarrierBuilt"), getRaixCarrierBuilt);
	websAspDefine(T("getRaixWDSBuilt"), getRaixWDSBuilt);
	websAspDefine(T("getRaixRVTBuilt"), getRaixRVTBuilt);
	websAspDefine(T("getRaixWSCBuilt"), getRaixWSCBuilt);
	websAspDefine(T("getRaixHTStream"), getRaixHTStream);
	websAspDefine(T("getRaixABand"), getRaixABand);
	websAspDefine(T("getRaix11nDraft3Built"), getRaix11nDraft3Built);
	websAspDefine(T("getRaixWAPIBuilt"), getRaixWAPIBuilt);
	websAspDefine(T("getInic11aChannels"), getInic11aChannels);
	websAspDefine(T("getInic11bChannels"), getInic11bChannels);
	websAspDefine(T("getInic11gChannels"), getInic11gChannels);
	websAspDefine(T("getInicChannel"), getInicChannel);
	websAspDefine(T("getInicCurrentMac"), getInicCurrentMac);
	websAspDefine(T("getInicWdsEncType"), getInicWdsEncType);
	websAspDefine(T("getRaixWlanStaInfo"), getRaixWlanStaInfo);
	websAspDefine(T("getRaixApStats"), getRaixApStats);
	websAspDefine(T("getRaixApSNR"), getRaixApSNR);
	websFormDefine(T("inicBasic"), inicBasic);
	websFormDefine(T("inicAdvanced"), inicAdvanced);
	websFormDefine(T("inicWmm"), inicWmm);
	websFormDefine(T("inicWds"), inicWds);
	websFormDefine(T("inicwGetSecurity"), inicwGetSecurity);
	websFormDefine(T("INICSecurity"), INICSecurity);
	websFormDefine(T("INICDeleteAccessPolicyList"), INICDeleteAccessPolicyList);
	websFormDefine(T("inicRvt"), inicRvt);
}

/*
 * description: write 802.11a channels in <select> tag
 */
static int getInic11aChannels(int eid, webs_t wp, int argc, char_t **argv)
{
	int  idx = 0, channel;
	const char *value = nvram_bufget(RTDEV_NVRAM, "CountryRegionABand");
	const char *channel_s = nvram_bufget(RTDEV_NVRAM, "Channel");

	channel = (channel_s == NULL)? 0 : atoi(channel_s);
	if ((value == NULL) || (strcmp(value, "") == 0) ||
		(strcmp(value, "7") == 0)) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "0") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "1") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "2") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "3") == 0) {
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "4") == 0) {
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "5") == 0) {
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "6") == 0) {
		/* 36~48 */
		for (idx = 0; idx < 4; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "8") == 0) {
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "9") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~116 */
		for (idx = 16; idx < 21; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 132~140 */
		for (idx = 24; idx < 27; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "10") == 0) {
		/* 36~48 */
		for (idx = 0; idx < 4; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "11") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~120 */
		for (idx = 16; idx < 22; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	}

	return 0;
}

/*
 * description: write 802.11b channels in <select> tag
 */
static int getInic11bChannels(int eid, webs_t wp, int argc, char_t **argv)
{
	int idx = 0, channel;
	const char *value = nvram_bufget(RTDEV_NVRAM, "CountryRegion");
	const char *channel_s = nvram_bufget(RTDEV_NVRAM, "Channel");

	channel = (channel_s == NULL)? 0 : atoi(channel_s);
	if ((value == NULL) || (strcmp(value, "") == 0) ||
		(strcmp(value, "5") == 0)) {
		/* ch1 ~ ch14 */
		for (idx = 0; idx < 14; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "0") == 0) {
		/* ch1 ~ ch11 */
		for (idx = 0; idx < 11; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "1") == 0) {
		/* ch1 ~ ch13 */
		for (idx = 0; idx < 13; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "2") == 0) {
		/* ch10 ~ ch11 */
		for (idx = 9; idx < 11; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "3") == 0) {
		/* ch10 ~ ch13 */
		for (idx = 9; idx < 13; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "4") == 0) {
		/* ch14 */
		websWrite(wp, T("<option value=14 %s>2484MHz (Channel 14)</option>\n"), 
				(14 == channel)? "selected" : "");
	} else if (strcmp(value, "6") == 0) {
		/* ch3 ~ ch9 */
		for (idx = 2; idx < 9; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "7") == 0) {
		/* ch5 ~ ch13 */
		for (idx = 4; idx < 13; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	}
	return 0;
}

/*
 * description: write 802.11g channels in <select> tag
 */
static int getInic11gChannels(int eid, webs_t wp, int argc, char_t **argv)
{
	int idx = 0, channel;
	const char *value = nvram_bufget(RTDEV_NVRAM, "CountryRegion");
	const char *channel_s = nvram_bufget(RTDEV_NVRAM, "Channel");

	channel = (channel_s == NULL)? 0 : atoi(channel_s);
	if ((value == NULL) || (strcmp(value, "") == 0) ||
		(strcmp(value, "5") == 0)) {
		/* ch1 ~ ch14 */
		for (idx = 0; idx < 14; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "0") == 0) {
		/* ch1 ~ ch11 */
		for (idx = 0; idx < 11; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "1") == 0) {
		/* ch1 ~ ch13 */
		for (idx = 0; idx < 13; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "2") == 0) {
		/* ch10 ~ ch11 */
		for (idx = 9; idx < 11; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "3") == 0) {
		/* ch10 ~ ch13 */
		for (idx = 9; idx < 13; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "4") == 0) {
		/* ch14 */
		websWrite(wp, T("<option value=14 %s>2484MHz (Channel 14)</option>\n"), 
				(14 == channel)? "selected" : "");
	} else if (strcmp(value, "6") == 0) {
		/* ch3 ~ ch9 */
		for (idx = 2; idx < 9; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	} else if (strcmp(value, "7") == 0) {
		/* ch5 ~ ch13 */
		for (idx = 4; idx < 13; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", idx+1,
					(idx+1 == channel)? "selected" : "", 2412+5*idx,
					"MHz (Channel ", idx+1, ")</option>");
	}
	return 0;
}

/*
 * description: write channel number or 0 if auto-select
 */
static int getInicChannel(int eid, webs_t wp, int argc, char_t **argv)
{
	const char *value = nvram_bufget(RTDEV_NVRAM, "AutoChannelSelect");

	if (NULL == value)
		return websWrite(wp, T("9"));
	if (!strncmp(value, "1", 2))
		return websWrite(wp, T("0"));

	value = nvram_bufget(RTDEV_NVRAM, "Channel");
	if (NULL == value)
		return websWrite(wp, T("9"));
	else
		return websWrite(wp, T("%s"), value);
}

/*
 * description: write MAC address from interface 'rai0'
 */
static int getInicCurrentMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_hw[18] = {0};

	if (-1 == getIfMac("rai0", if_hw))
		return websWrite(wp, T(" "));
	return websWrite(wp, T("%s"), if_hw);
}

/*
 * description: write the WDS Encryp Type
 */
static int getInicWdsEncType(int eid, webs_t wp, int argc, char_t **argv)
{
	const char *value = nvram_bufget(RTDEV_NVRAM, "WdsEncrypType");

	if (NULL == value)
		return websWrite(wp, T("0"));
	else if (strcmp(value, "NONE;NONE;NONE;NONE") == 0)
		return websWrite(wp, T("0"));
	else if (strcmp(value, "WEP;WEP;WEP;WEP") == 0)
		return websWrite(wp, T("1"));
	else if (strcmp(value, "TKIP;TKIP;TKIP;TKIP") == 0)
		return websWrite(wp, T("2"));
	else if (strcmp(value, "AES;AES;AES;AES") == 0)
		return websWrite(wp, T("3"));
	else
		return websWrite(wp, T("0"));
}

static void revise_mbss_value(int old_num, int new_num)
{
	/* {{{ The parameters that support multiple BSSID is listed as followed,
	   1.) SSID,                 char SSID[33];
	   2.) AuthMode,             char AuthMode[14];
	   3.) EncrypType,           char EncrypType[8];
	   4.) WPAPSK,               char WPAPSK[65];
	   5.) DefaultKeyID,         int  DefaultKeyID;
	   6.) Key1Type,             int  Key1Type;
	   7.) Key1Str,              char Key1Str[27];
	   8.) Key2Type,             int  Key2Type;
	   9.) Key2Str,              char Key2Str[27];
	   10.) Key3Type,            int  Key3Type;
	   11.) Key3Str,             char Key3Str[27];
	   12.) Key4Type,            int  Key4Type;
	   13.) Key4Str,             char Key4Str[27];
	   14.) AccessPolicy,
	   15.) AccessControlList,
	   16.) NoForwarding,
	   17.) IEEE8021X,           int  IEEE8021X;
	   18.) TxRate,              int  TxRate;
	   19.) HideSSID,            int  HideSSID;
	   20.) PreAuth,             int  PreAuth;
	   21.) WmmCapable
	                             int  SecurityMode;
                             	 char VlanName[20];
	                             int  VlanId;
	                             int  VlanPriority;
	}}} */
	char new_value[264], *p;
	const char *old_value;
	int i;

#define MBSS_INIT(field, default_value) \
	do { \
		old_value = nvram_bufget(RTDEV_NVRAM, #field); \
		snprintf(new_value, 264, "%s", old_value); \
		p = new_value + strlen(old_value); \
		for (i = old_num; i < new_num; i++) { \
			snprintf(p, 264 - (p - new_value), ";%s", default_value); \
			p += 1 + strlen(default_value); \
		} \
		nvram_bufset(RTDEV_NVRAM, #field, new_value); \
	} while (0)

#define MBSS_REMOVE(field) \
	do { \
		old_value = nvram_bufget(RTDEV_NVRAM, #field); \
		snprintf(new_value, 264, "%s", old_value); \
		p = new_value; \
		for (i = 0; i < new_num; i++) { \
			if (0 == i) \
				p = strchr(p, ';'); \
			else \
				p = strchr(p+1, ';'); \
			if (NULL == p) \
				break; \
		} \
		if (p) \
			*p = '\0'; \
		nvram_bufset(RTDEV_NVRAM, #field, new_value); \
	} while (0)

	if (new_num > old_num) {
		//MBSS_INIT(SSID, "ssid");
		MBSS_INIT(AuthMode, "OPEN");
		MBSS_INIT(EncrypType, "NONE");
		//MBSS_INIT(WPAPSK, "12345678");
		MBSS_INIT(DefaultKeyID, "1");
		MBSS_INIT(Key1Type, "0");
		//MBSS_INIT(Key1Str, "");
		MBSS_INIT(Key2Type, "0");
		//MBSS_INIT(Key2Str, "");
		MBSS_INIT(Key3Type, "0");
		//MBSS_INIT(Key3Str, "");
		MBSS_INIT(Key4Type, "0");
		//MBSS_INIT(Key4Str, "");
/*		MBSS_INIT(AccessPolicy0, "0");
		MBSS_INIT(AccessControlList0, "");
		MBSS_INIT(AccessPolicy1, "0");
		MBSS_INIT(AccessControlList1, "");
		MBSS_INIT(AccessPolicy2, "0");
		MBSS_INIT(AccessControlList2, "");
		MBSS_INIT(AccessPolicy3, "0");
		MBSS_INIT(AccessControlList3, ""); */
		MBSS_INIT(NoForwarding, "0");
		MBSS_INIT(IEEE8021X, "0");
		MBSS_INIT(TxRate, "0");
		//MBSS_INIT(HideSSID, "0");
		MBSS_INIT(PreAuth, "0");
		MBSS_INIT(WmmCapable, "1");
		for (i = old_num + 1; i <= new_num; i++) {
			nvram_bufset(RTDEV_NVRAM, racat("WPAPSK", i), "12345678");
			nvram_bufset(RTDEV_NVRAM, racat("Key1Str", i), "");
			nvram_bufset(RTDEV_NVRAM, racat("Key2Str", i), "");
			nvram_bufset(RTDEV_NVRAM, racat("Key3Str", i), "");
			nvram_bufset(RTDEV_NVRAM, racat("Key4Str", i), "");
			// The index of AccessPolicy & AccessControlList starts at 0.
			nvram_bufset(RTDEV_NVRAM, racat("AccessPolicy", i-1), "0");
			nvram_bufset(RTDEV_NVRAM, racat("AccessControlList", i-1), "");
		}
	}
	else if (new_num < old_num) {
		//MBSS_REMOVE(SSID);
		MBSS_REMOVE(AuthMode);
		MBSS_REMOVE(EncrypType);
		//MBSS_REMOVE(WPAPSK);
		MBSS_REMOVE(DefaultKeyID);
		MBSS_REMOVE(Key1Type);
		//MBSS_REMOVE(Key1Str);
		MBSS_REMOVE(Key2Type);
		//MBSS_REMOVE(Key2Str);
		MBSS_REMOVE(Key3Type);
		//MBSS_REMOVE(Key3Str);
		MBSS_REMOVE(Key4Type);
		//MBSS_REMOVE(Key4Str);
/*		MBSS_REMOVE(AccessPolicy0);
		MBSS_REMOVE(AccessControlList0);
		MBSS_REMOVE(AccessPolicy1);
		MBSS_REMOVE(AccessControlList1);
		MBSS_REMOVE(AccessPolicy2);
		MBSS_REMOVE(AccessControlList2);
		MBSS_REMOVE(AccessPolicy3);
		MBSS_REMOVE(AccessControlList3); */
		MBSS_REMOVE(NoForwarding);
		MBSS_REMOVE(IEEE8021X);
		MBSS_REMOVE(TxRate);
		MBSS_REMOVE(HideSSID);
		MBSS_REMOVE(PreAuth);
		MBSS_REMOVE(WmmCapable);
		for (i = new_num + 1; i <= old_num; i++) {
			nvram_bufset(RTDEV_NVRAM, racat("SSID", i), "");
			nvram_bufset(RTDEV_NVRAM, racat("WPAPSK", i), "");
			nvram_bufset(RTDEV_NVRAM, racat("Key1Str", i), "");
			nvram_bufset(RTDEV_NVRAM, racat("Key2Str", i), "");
			nvram_bufset(RTDEV_NVRAM, racat("Key3Str", i), "");
			nvram_bufset(RTDEV_NVRAM, racat("Key4Str", i), "");
			// The index of AccessPolicy & AccessControlList starts at 0.
			nvram_bufset(RTDEV_NVRAM, racat("AccessPolicy", i-1), "0");
			nvram_bufset(RTDEV_NVRAM, racat("AccessControlList", i-1), "");
		}
	}
}

/* goform/inicBasic */
static void inicBasic(webs_t wp, char_t *path, char_t *query)
{
	char_t	*wirelessmode, *radio;
	char_t	*ssid, *mssid_1, *mssid_2, *mssid_3, *mssid_4, *mssid_5, *mssid_6,
			*mssid_7, *bssid_num, *broadcastssid;
	char_t	*sz11aChannel, *sz11bChannel, *sz11gChannel, *abg_rate;
	char_t	*n_mode, *n_bandwidth, *n_gi, *n_mcs, *n_rdg, *n_extcha, *n_amsdu, *n_stbc;
	char_t	*n_autoba, *n_badecline, *n_disallow_tkip, *n_2040_coexit;
	char_t	*tx_stream, *rx_stream;
	int i = 0, is_n = 0, new_bssid_num, old_bssid_num = 1;

	radio = websGetVar(wp, T("radiohiddenButton"), T("2"));
	if (!strncmp(radio, "0", 2)) {
		doSystem("iwpriv rai0 set RadioOn=0");
		nvram_set(RTDEV_NVRAM, "RadioOff", "1");
		websRedirect(wp, "inic/basic.asp");
		return;
	}
	else if (!strncmp(radio, "1", 2)) {
		doSystem("iwpriv rai0 set RadioOn=1");
		nvram_set(RTDEV_NVRAM, "RadioOff", "0");
		websRedirect(wp, "inic/basic.asp");
		return;
	}
	//fetch from web input
	wirelessmode = websGetVar(wp, T("wirelessmode"), T("9")); //9: bgn mode
	ssid = websGetVar(wp, T("ssid"), T("")); 
	mssid_1 = websGetVar(wp, T("mssid_1"), T("")); 
	mssid_2 = websGetVar(wp, T("mssid_2"), T("")); 
	mssid_3 = websGetVar(wp, T("mssid_3"), T("")); 
	mssid_4 = websGetVar(wp, T("mssid_4"), T("")); 
	mssid_5 = websGetVar(wp, T("mssid_5"), T("")); 
	mssid_6 = websGetVar(wp, T("mssid_6"), T("")); 
	mssid_7 = websGetVar(wp, T("mssid_7"), T("")); 
	bssid_num = websGetVar(wp, T("bssid_num"), T("1"));
	broadcastssid = websGetVar(wp, T("broadcastssid"), T("1")); 
	sz11aChannel = websGetVar(wp, T("sz11aChannel"), T("")); 
	sz11bChannel = websGetVar(wp, T("sz11bChannel"), T("")); 
	sz11gChannel = websGetVar(wp, T("sz11gChannel"), T("")); 
	abg_rate = websGetVar(wp, T("abg_rate"), T("")); 
	n_mode = websGetVar(wp, T("n_mode"), T("0"));
	n_bandwidth = websGetVar(wp, T("n_bandwidth"), T("0"));
	n_gi = websGetVar(wp, T("n_gi"), T("0"));
	n_mcs = websGetVar(wp, T("n_mcs"), T("33"));
	n_rdg = websGetVar(wp, T("n_rdg"), T("0"));
	n_extcha = websGetVar(wp, T("n_extcha"), T("0"));
	n_stbc = websGetVar(wp, T("n_stbc"), T("0"));
	n_amsdu = websGetVar(wp, T("n_amsdu"), T("0"));
	n_autoba = websGetVar(wp, T("n_autoba"), T("0"));
	n_badecline = websGetVar(wp, T("n_badecline"), T("0"));
	n_disallow_tkip = websGetVar(wp, T("n_disallow_tkip"), T("0"));
	n_2040_coexit = websGetVar(wp, T("n_2040_coexit"), T("0"));
	tx_stream = websGetVar(wp, T("tx_stream"), T("0"));
	rx_stream = websGetVar(wp, T("rx_stream"), T("0"));

	old_bssid_num = atoi(nvram_bufget(RTDEV_NVRAM, "BssidNum"));
	new_bssid_num = atoi(bssid_num);

	nvram_bufset(RTDEV_NVRAM, "WirelessMode", wirelessmode);
	if (!strncmp(wirelessmode, "4", 2))
		nvram_bufset(RTDEV_NVRAM, "BasicRate", "351");
	else if (!strncmp(wirelessmode, "8", 2) || !strncmp(wirelessmode, "9", 2))
		is_n = 1;

#if 0
	i = atoi(mode);
	if (i == 1)
		nvram_bufset(RTDEV_NVRAM, "BasicRate", "3");
	else if (i == 0) {
		nvram_bufset(RTDEV_NVRAM, "TxPreamble","0");
		nvram_bufset(RTDEV_NVRAM, "TxBurst", "0");
		nvram_bufset(RTDEV_NVRAM, "PktAggregate", "0");
	}
	else if (i == 5) {
		nvram_bufset(RTDEV_NVRAM, "TxPreamble","1");
		nvram_bufset(RTDEV_NVRAM, "TxBurst", "1");
		nvram_bufset(RTDEV_NVRAM, "PktAggregate", "1");
		nvram_bufset(RTDEV_NVRAM, "DisableOLBC", "1");
	}
	if (i != 5) {
		nvram_bufset(RTDEV_NVRAM, "DisableOLBC", "0");
	}
#endif

	//SSID, Multiple SSID
	if (0 == strlen(ssid)) {
		nvram_commit(RTDEV_NVRAM);
		websError(wp, 403, T("'SSID' should not be empty!"));
		return;
	} else {
		i++;
		nvram_bufset(RTDEV_NVRAM, racat("SSID", i), ssid);
	}

//#WPS
	if (!strncmp(broadcastssid, "0", 2)) {
		nvram_bufset(RTDEV_NVRAM, "WscModeOption", "0");
		doSystem("kill -9 `cat /var/run/wscd.pid.rai0`");
		doSystem("route delete 239.255.255.250 1>/dev/null 2>&1");
	} else {
		const char *wordlist= nvram_bufget(RTDEV_NVRAM, "WscModeOption");
		if(wordlist){
			if (strcmp(wordlist, "0"))
				doSystem("iwpriv rai0 set WscConfStatus=1");
			nvram_bufset(RTDEV_NVRAM, "WscConfigured", "1");
			g_Raix_wsc_configured = 1;
		}
	}
//#WPS

	if (0 != strlen(mssid_1)) {
		i++;
		nvram_bufset(RTDEV_NVRAM, racat("SSID", i), mssid_1);
	}
	if (0 != strlen(mssid_2)) {
		i++;
		nvram_bufset(RTDEV_NVRAM, racat("SSID", i), mssid_2);
	}
	if (0 != strlen(mssid_3)) {
		i++;
		nvram_bufset(RTDEV_NVRAM, racat("SSID", i), mssid_3);
	}
	if (0 != strlen(mssid_4)) {
		i++;
		nvram_bufset(RTDEV_NVRAM, racat("SSID", i), mssid_4);
	}
	if (0 != strlen(mssid_5)) {
		i++;
		nvram_bufset(RTDEV_NVRAM, racat("SSID", i), mssid_5);
	}
	if (0 != strlen(mssid_6)) {
		i++;
		nvram_bufset(RTDEV_NVRAM, racat("SSID", i), mssid_6);
	}
	if (0 != strlen(mssid_7)) {
		i++;
		nvram_bufset(RTDEV_NVRAM, racat("SSID", i), mssid_7);
	}

	nvram_bufset(RTDEV_NVRAM, "BssidNum", bssid_num);
	if (new_bssid_num < 1 || new_bssid_num > 8) {
		nvram_commit(RTDEV_NVRAM);
		websError(wp, 403, T("'bssid_num' %s is out of range!"), bssid_num);
		return;
	}
	revise_mbss_value(old_bssid_num, new_bssid_num);

	//Broadcast SSID
	if (new_bssid_num == 1) {
		if (!strncmp(broadcastssid, "1", 2))
			nvram_bufset(RTDEV_NVRAM, "HideSSID", "0");
		else
			nvram_bufset(RTDEV_NVRAM, "HideSSID", "1");
	}
	else if (new_bssid_num == 2) {
		if (!strncmp(broadcastssid, "1", 2))
			nvram_bufset(RTDEV_NVRAM, "HideSSID", "0;0");
		else
			nvram_bufset(RTDEV_NVRAM, "HideSSID", "1;1");
	}
	else if (new_bssid_num == 3) {
		if (!strncmp(broadcastssid, "1", 2))
			nvram_bufset(RTDEV_NVRAM, "HideSSID", "0;0;0");
		else
			nvram_bufset(RTDEV_NVRAM, "HideSSID", "1;1;1");
	}
	else if (new_bssid_num == 4) {
		if (!strncmp(broadcastssid, "1", 2))
			nvram_bufset(RTDEV_NVRAM, "HideSSID", "0;0;0;0");
		else
			nvram_bufset(RTDEV_NVRAM, "HideSSID", "1;1;1;1");
	}
	else if (new_bssid_num == 5) {
		if (!strncmp(broadcastssid, "1", 2))
			nvram_bufset(RTDEV_NVRAM, "HideSSID", "0;0;0;0;0");
		else
			nvram_bufset(RTDEV_NVRAM, "HideSSID", "1;1;1;1;1");
	}
	else if (new_bssid_num == 6) {
		if (!strncmp(broadcastssid, "1", 2))
			nvram_bufset(RTDEV_NVRAM, "HideSSID", "0;0;0;0;0;0");
		else
			nvram_bufset(RTDEV_NVRAM, "HideSSID", "1;1;1;1;1;1");
	}
	else if (new_bssid_num == 7) {
		if (!strncmp(broadcastssid, "1", 2))
			nvram_bufset(RTDEV_NVRAM, "HideSSID", "0;0;0;0;0;0;0");
		else
			nvram_bufset(RTDEV_NVRAM, "HideSSID", "1;1;1;1;1;1;1");
	}
	else if (new_bssid_num == 8) {
		if (!strncmp(broadcastssid, "1", 2))
			nvram_bufset(RTDEV_NVRAM, "HideSSID", "0;0;0;0;0;0;0;0");
		else
			nvram_bufset(RTDEV_NVRAM, "HideSSID", "1;1;1;1;1;1;1;1");
	}

	//11abg Channel or AutoSelect
	if ((0 == strlen(sz11aChannel)) && (0 == strlen(sz11bChannel)) &&
			(0 == strlen(sz11gChannel))) {
		nvram_commit(RTDEV_NVRAM);
		websError(wp, 403, T("'Channel' should not be empty!"));
		return;
	}
	if (!strncmp(sz11aChannel, "0", 2) || !strncmp(sz11bChannel, "0", 2) ||
			!strncmp(sz11gChannel, "0", 2))
		nvram_bufset(RTDEV_NVRAM, "AutoChannelSelect", "1");
	else
		nvram_bufset(RTDEV_NVRAM, "AutoChannelSelect", "0");
	if (0 != strlen(sz11aChannel)) {
		nvram_bufset(RTDEV_NVRAM, "Channel", sz11aChannel);
		doSystem("iwpriv rai0 set Channel=%s", sz11aChannel);
	}
	if (0 != strlen(sz11bChannel)) {
		nvram_bufset(RTDEV_NVRAM, "Channel", sz11bChannel);
		doSystem("iwpriv rai0 set Channel=%s", sz11bChannel);
	}
	if (0 != strlen(sz11gChannel)) {
		nvram_bufset(RTDEV_NVRAM, "Channel", sz11gChannel);
		doSystem("iwpriv rai0 set Channel=%s", sz11gChannel);
	}

	//Rate for a, b, g
	if (strncmp(abg_rate, "", 1)) {
		int rate = atoi(abg_rate);
		if (!strncmp(wirelessmode, "0", 2) || !strncmp(wirelessmode, "2", 2) || !strncmp(wirelessmode, "4", 2)) {
			if (rate == 1 || rate == 2 || rate == 5 || rate == 11)
				nvram_bufset(RTDEV_NVRAM, "FixedTxMode", "1"); //CCK
			else
				nvram_bufset(RTDEV_NVRAM, "FixedTxMode", "2"); //OFDM

			if (rate == 1)
				nvram_bufset(RTDEV_NVRAM, "HT_MCS", "0");
			else if (rate == 2)
				nvram_bufset(RTDEV_NVRAM, "HT_MCS", "1");
			else if (rate == 5)
				nvram_bufset(RTDEV_NVRAM, "HT_MCS", "2");
			else if (rate == 6)
				nvram_bufset(RTDEV_NVRAM, "HT_MCS", "0");
			else if (rate == 9)
				nvram_bufset(RTDEV_NVRAM, "HT_MCS", "1");
			else if (rate == 11)
				nvram_bufset(RTDEV_NVRAM, "HT_MCS", "3");
			else if (rate == 12)
				nvram_bufset(RTDEV_NVRAM, "HT_MCS", "2");
			else if (rate == 18)
				nvram_bufset(RTDEV_NVRAM, "HT_MCS", "3");
			else if (rate == 24)
				nvram_bufset(RTDEV_NVRAM, "HT_MCS", "4");
			else if (rate == 36)
				nvram_bufset(RTDEV_NVRAM, "HT_MCS", "5");
			else if (rate == 48)
				nvram_bufset(RTDEV_NVRAM, "HT_MCS", "6");
			else if (rate == 54)
				nvram_bufset(RTDEV_NVRAM, "HT_MCS", "7");
			else
				nvram_bufset(RTDEV_NVRAM, "HT_MCS", "33");
		}
		else if (!strncmp(wirelessmode, "1", 2)) {
			nvram_bufset(RTDEV_NVRAM, "FixedTxMode", "1"); //CCK
			if (rate == 1)
				nvram_bufset(RTDEV_NVRAM, "HT_MCS", "0");
			else if (rate == 2)
				nvram_bufset(RTDEV_NVRAM, "HT_MCS", "1");
			else if (rate == 5)
				nvram_bufset(RTDEV_NVRAM, "HT_MCS", "2");
			else if (rate == 11)
				nvram_bufset(RTDEV_NVRAM, "HT_MCS", "3");
		}
		else //shall not happen
			error(E_L, E_LOG, T("wrong configurations from web UI"));
	}
	else
		nvram_bufset(RTDEV_NVRAM, "FixedTxMode", "0"); //None

	//HT_OpMode, HT_BW, HT_GI, HT_MCS, HT_RDG, HT_EXTCHA, HT_AMSDU, HT_TxStream, HT_RxStream
	if (is_n) {
		nvram_bufset(RTDEV_NVRAM, "HT_OpMode", n_mode);
		nvram_bufset(RTDEV_NVRAM, "HT_BW", n_bandwidth);
		nvram_bufset(RTDEV_NVRAM, "HT_GI", n_gi);
		nvram_bufset(RTDEV_NVRAM, "HT_MCS", n_mcs);
		nvram_bufset(RTDEV_NVRAM, "HT_RDG", n_rdg);
		nvram_bufset(RTDEV_NVRAM, "HT_EXTCHA", n_extcha);
		nvram_bufset(RTDEV_NVRAM, "HT_STBC", n_stbc);
		nvram_bufset(RTDEV_NVRAM, "HT_AMSDU", n_amsdu);
		nvram_bufset(RTDEV_NVRAM, "HT_AutoBA", n_autoba);
		nvram_bufset(RTDEV_NVRAM, "HT_BADecline", n_badecline);
		nvram_bufset(RTDEV_NVRAM, "HT_DisallowTKIP", n_disallow_tkip);
		nvram_bufset(RTDEV_NVRAM, "HT_BSSCoexistence", n_2040_coexit);
	}
	nvram_bufset(RTDEV_NVRAM, "HT_TxStream", tx_stream);
	nvram_bufset(RTDEV_NVRAM, "HT_RxStream", rx_stream);

	nvram_commit(RTDEV_NVRAM);
	initInternet();

	//debug print
	websHeader(wp);
	websWrite(wp, T("<h2>mode: %s</h2><br>\n"), wirelessmode);
	websWrite(wp, T("ssid: %s, bssid_num: %s<br>\n"), ssid, bssid_num);
	websWrite(wp, T("mssid_1: %s, mssid_2: %s, mssid_3: %s<br>\n"),
			mssid_1, mssid_2, mssid_3);
	websWrite(wp, T("mssid_4: %s, mssid_5: %s, mssid_6: %s, mssid_7: %s<br>\n"),
			mssid_4, mssid_5, mssid_6, mssid_7);
	websWrite(wp, T("broadcastssid: %s<br>\n"), broadcastssid);
	websWrite(wp, T("sz11aChannel: %s<br>\n"), sz11aChannel);
	websWrite(wp, T("sz11bChannel: %s<br>\n"), sz11bChannel);
	websWrite(wp, T("sz11gChannel: %s<br>\n"), sz11gChannel);
	if (strncmp(abg_rate, "", 1)) {
		websWrite(wp, T("abg_rate: %s<br>\n"), abg_rate);
	}
	if (is_n) {
		websWrite(wp, T("n_mode: %s<br>\n"), n_mode);
		websWrite(wp, T("n_bandwidth: %s<br>\n"), n_bandwidth);
		websWrite(wp, T("n_gi: %s<br>\n"), n_gi);
		websWrite(wp, T("n_mcs: %s<br>\n"), n_mcs);
		websWrite(wp, T("n_rdg: %s<br>\n"), n_rdg);
		websWrite(wp, T("n_extcha: %s<br>\n"), n_extcha);
		websWrite(wp, T("n_stbc: %s<br>\n"), n_stbc);
		websWrite(wp, T("n_amsdu: %s<br>\n"), n_amsdu);
		websWrite(wp, T("n_autoba: %s<br>\n"), n_autoba);
		websWrite(wp, T("n_badecline: %s<br>\n"), n_badecline);
		websWrite(wp, T("n_disallow_tkip: %s<br>\n"), n_disallow_tkip);
		websWrite(wp, T("n_2040_coexit: %s<br>\n"), n_2040_coexit);
	}
	websWrite(wp, T("tx_stream: %s<br>\n"), tx_stream);
	websWrite(wp, T("rx_stream: %s<br>\n"), rx_stream);
	websFooter(wp);
	websDone(wp, 200);
}

/* goform/inicAdvanced */
static void inicAdvanced(webs_t wp, char_t *path, char_t *query)
{
	char_t	*bg_protection, *basic_rate, *beacon, *dtim, *fragment, *rts,
			*tx_power, *short_preamble, *short_slot, *tx_burst, *pkt_aggregate;
	char_t	*ieee_80211h, *rd_region, *carrier_detect;
	char_t *wmm_capable, *apsd_capable, *dls_capable, *countrycode;
	int		i, ssid_num, wlan_mode;
	char	wmm_enable[8];
#ifdef CONFIG_RT3090_AUTO_PROVISION
	char_t  *auto_provision;
#endif

	//fetch from web input
	bg_protection = websGetVar(wp, T("bg_protection"), T("0"));
	basic_rate = websGetVar(wp, T("basic_rate"), T("15"));
	beacon = websGetVar(wp, T("beacon"), T("100"));
	dtim = websGetVar(wp, T("dtim"), T("1"));
	fragment = websGetVar(wp, T("fragment"), T("2346"));
	rts = websGetVar(wp, T("rts"), T("2347"));
	tx_power = websGetVar(wp, T("tx_power"), T("100"));
	short_preamble = websGetVar(wp, T("short_preamble"), T("0"));
	short_slot = websGetVar(wp, T("short_slot"), T("0"));
	tx_burst = websGetVar(wp, T("tx_burst"), T("0"));
	pkt_aggregate = websGetVar(wp, T("pkt_aggregate"), T("0"));
#ifdef CONFIG_RT3090_AUTO_PROVISION
	auto_provision = websGetVar(wp, T("auto_provision"), T("0"));
#endif
	ieee_80211h = websGetVar(wp, T("ieee_80211h"), T("0"));
	rd_region = websGetVar(wp, T("rd_region"), T(""));
	carrier_detect = websGetVar(wp, T("carrier_detect"), T("0"));
	wmm_capable = websGetVar(wp, T("wmm_capable"), T("0"));
	apsd_capable = websGetVar(wp, T("apsd_capable"), T("0"));
	dls_capable = websGetVar(wp, T("dls_capable"), T("0"));
	countrycode = websGetVar(wp, T("country_code"), T("NONE"));

	if (NULL != nvram_bufget(RTDEV_NVRAM, "BssidNum"))
		ssid_num = atoi(nvram_bufget(RTDEV_NVRAM, "BssidNum"));
	else
		ssid_num = 1;
	wlan_mode = atoi(nvram_bufget(RTDEV_NVRAM, "WirelessMode"));

	//set to nvram
	nvram_bufset(RTDEV_NVRAM, "BGProtection", bg_protection);
	nvram_bufset(RTDEV_NVRAM, "BasicRate", basic_rate);
	nvram_bufset(RTDEV_NVRAM, "BeaconPeriod", beacon);
	nvram_bufset(RTDEV_NVRAM, "DtimPeriod", dtim);
	nvram_bufset(RTDEV_NVRAM, "FragThreshold", fragment);
	nvram_bufset(RTDEV_NVRAM, "RTSThreshold", rts);
	nvram_bufset(RTDEV_NVRAM, "TxPower", tx_power);
	nvram_bufset(RTDEV_NVRAM, "TxPreamble", short_preamble);
	nvram_bufset(RTDEV_NVRAM, "ShortSlot", short_slot);
	nvram_bufset(RTDEV_NVRAM, "TxBurst", tx_burst);
	nvram_bufset(RTDEV_NVRAM, "PktAggregate", pkt_aggregate);
#ifdef CONFIG_RT3090_AUTO_PROVISION
	nvram_bufset(RTDEV_NVRAM, "AutoProvisionEn", auto_provision);
#endif
	nvram_bufset(RTDEV_NVRAM, "IEEE80211H", ieee_80211h);
	nvram_bufset(RTDEV_NVRAM, "RDRegion", rd_region);
	nvram_bufset(RTDEV_NVRAM, "CarrierDetect", carrier_detect);
	nvram_bufset(RTDEV_NVRAM, "WmmCapable", wmm_capable);
	nvram_bufset(RTDEV_NVRAM, "APSDCapable", apsd_capable);
	nvram_bufset(RTDEV_NVRAM, "DLSCapable", dls_capable);

	bzero(wmm_enable, sizeof(char)*8);
	for (i = 0; i < ssid_num; i++)
	{
		sprintf(wmm_enable+strlen(wmm_enable), "%d", atoi(wmm_capable));
		sprintf(wmm_enable+strlen(wmm_enable), "%c", ';');
	}
	wmm_enable[strlen(wmm_enable) - 1] = '\0';

	if (!strncmp(wmm_capable, "1", 2)) {
		if (wlan_mode < 5)
			nvram_bufset(RTDEV_NVRAM, "TxBurst", "0");
	}

	nvram_bufset(RTDEV_NVRAM, "CountryCode", countrycode);
	if (!strncmp(countrycode, "US", 3)) {
		nvram_bufset(RTDEV_NVRAM, "CountryRegion", "0");
		nvram_bufset(RTDEV_NVRAM, "CountryRegionABand", "7");
	}
	else if (!strncmp(countrycode, "JP", 3)) {
		nvram_bufset(RTDEV_NVRAM, "CountryRegion", "5");
		nvram_bufset(RTDEV_NVRAM, "CountryRegionABand", "6");
	}
	else if (!strncmp(countrycode, "FR", 3)) {
		nvram_bufset(RTDEV_NVRAM, "CountryRegion", "1");
		nvram_bufset(RTDEV_NVRAM, "CountryRegionABand", "2");
	}
	else if (!strncmp(countrycode, "TW", 3)) {
		nvram_bufset(RTDEV_NVRAM, "CountryRegion", "0");
		nvram_bufset(RTDEV_NVRAM, "CountryRegionABand", "8");
	}
	else if (!strncmp(countrycode, "IE", 3)) {
		nvram_bufset(RTDEV_NVRAM, "CountryRegion", "1");
		nvram_bufset(RTDEV_NVRAM, "CountryRegionABand", "1");
	}
	else if (!strncmp(countrycode, "HK", 3)) {
		nvram_bufset(RTDEV_NVRAM, "CountryRegion", "1");
		nvram_bufset(RTDEV_NVRAM, "CountryRegionABand", "0");
	}
	else {
		nvram_bufset(RTDEV_NVRAM, "CountryCode", "");
		nvram_bufset(RTDEV_NVRAM, "CountryRegion", "5");
		nvram_bufset(RTDEV_NVRAM, "CountryRegionABand", "7");
	}

	nvram_commit(RTDEV_NVRAM);
	initInternet();

	//debug print
	websHeader(wp);
	websWrite(wp, T("bg_protection: %s<br>\n"), bg_protection);
    websWrite(wp, T("basic_rate: %s<br>\n"), basic_rate);
    websWrite(wp, T("beacon: %s<br>\n"), beacon);
    websWrite(wp, T("dtim: %s<br>\n"), dtim);
    websWrite(wp, T("fragment: %s<br>\n"), fragment);
    websWrite(wp, T("rts: %s<br>\n"), rts);
    websWrite(wp, T("tx_power: %s<br>\n"), tx_power);
    websWrite(wp, T("short_preamble: %s<br>\n"), short_preamble);
    websWrite(wp, T("short_slot: %s<br>\n"), short_slot);
    websWrite(wp, T("tx_burst: %s<br>\n"), tx_burst);
    websWrite(wp, T("pkt_aggregate: %s<br>\n"), pkt_aggregate);
#ifdef CONFIG_RT3090_AUTO_PROVISION
    websWrite(wp, T("auto_provision: %s<br>\n"), auto_provision);
#endif
    websWrite(wp, T("ieee_80211h: %s<br>\n"), ieee_80211h);
	websWrite(wp, T("rd_region: %s<br>\n"), rd_region);
	websWrite(wp, T("carrier_detect: %s<br>\n"), carrier_detect);
    websWrite(wp, T("wmm_capable: %s<br>\n"), wmm_capable);
    websWrite(wp, T("apsd_capable: %s<br>\n"), apsd_capable);
    websWrite(wp, T("dls_capable: %s<br>\n"), dls_capable);
    websWrite(wp, T("countrycode: %s<br>\n"), countrycode);
	websFooter(wp);
	websDone(wp, 200);
}

#if defined CONFIG_UNIQUE_WPS
void WPSRestart(void);
#else
void RaixWPSRestart(void);
#endif

/* goform/inicWmm */
static void inicWmm(webs_t wp, char_t *path, char_t *query)
{
	char_t	*ap_aifsn_all, *ap_cwmin_all, *ap_cwmax_all, *ap_txop_all,
			*ap_acm_all, *ap_ackpolicy_all,
			*sta_aifsn_all, *sta_cwmin_all, *sta_cwmax_all, *sta_txop_all,
			*sta_acm_all;

	ap_aifsn_all = websGetVar(wp, T("ap_aifsn_all"), T(""));
	ap_cwmin_all = websGetVar(wp, T("ap_cwmin_all"), T(""));
	ap_cwmax_all = websGetVar(wp, T("ap_cwmax_all"), T(""));
	ap_txop_all = websGetVar(wp, T("ap_txop_all"), T(""));
	ap_acm_all = websGetVar(wp, T("ap_acm_all"), T(""));
	ap_ackpolicy_all = websGetVar(wp, T("ap_ackpolicy_all"), T(""));
	sta_aifsn_all = websGetVar(wp, T("sta_aifsn_all"), T(""));
	sta_cwmin_all = websGetVar(wp, T("sta_cwmin_all"), T(""));
	sta_cwmax_all = websGetVar(wp, T("sta_cwmax_all"), T(""));
	sta_txop_all = websGetVar(wp, T("sta_txop_all"), T(""));
	sta_acm_all = websGetVar(wp, T("sta_acm_all"), T(""));

	if (0 != strlen(ap_aifsn_all))
		nvram_bufset(RTDEV_NVRAM, "APAifsn", ap_aifsn_all);
	if (0 != strlen(ap_cwmin_all))
		nvram_bufset(RTDEV_NVRAM, "APCwmin", ap_cwmin_all);
	if (0 != strlen(ap_cwmax_all))
		nvram_bufset(RTDEV_NVRAM, "APCwmax", ap_cwmax_all);
	if (0 != strlen(ap_txop_all))
		nvram_bufset(RTDEV_NVRAM, "APTxop", ap_txop_all);
	if (0 != strlen(ap_acm_all))
		nvram_bufset(RTDEV_NVRAM, "APACM", ap_acm_all);
	if (0 != strlen(ap_ackpolicy_all))
		nvram_bufset(RTDEV_NVRAM, "AckPolicy", ap_ackpolicy_all);
	if (0 != strlen(sta_aifsn_all))
		nvram_bufset(RTDEV_NVRAM, "BSSAifsn", sta_aifsn_all);
	if (0 != strlen(sta_cwmin_all))
		nvram_bufset(RTDEV_NVRAM, "BSSCwmin", sta_cwmin_all);
	if (0 != strlen(sta_cwmax_all))
		nvram_bufset(RTDEV_NVRAM, "BSSCwmax", sta_cwmax_all);
	if (0 != strlen(sta_txop_all))
		nvram_bufset(RTDEV_NVRAM, "BSSTxop", sta_txop_all);
	if (0 != strlen(sta_acm_all))
		nvram_bufset(RTDEV_NVRAM, "BSSACM", sta_acm_all);

	nvram_commit(RTDEV_NVRAM);

	doSystem("ralink_init make_wireless_config rtdev");
	if (0 == getIfLive("rai0")) {
		doKillPid("/var/run/RaCfg.pid");
		doSystem("brctl delif br0 rai0");
		doSystem("ifconfig rai0 down");
		Sleep(1);
		doSystem("ifconfig rai0 up");
/*
#ifndef INICv2_SUPPORT
		doSystem("RaAP&");
#endif
*/
		Sleep(3);
		doSystem("brctl addif br0 rai0");
	}
	//after rai0 down&up we must restore WPS status
#if defined CONFIG_UNIQUE_WPS
	WPSRestart();
#else
	RaixWPSRestart();
#endif

	websHeader(wp);
	websWrite(wp, T("ap_aifsn_all: %s<br>\n"), ap_aifsn_all);
	websWrite(wp, T("ap_cwmin_all: %s<br>\n"), ap_cwmin_all);
	websWrite(wp, T("ap_cwmax_all: %s<br>\n"), ap_cwmax_all);
	websWrite(wp, T("ap_txop_all: %s<br>\n"), ap_txop_all);
	websWrite(wp, T("ap_acm_all: %s<br>\n"), ap_acm_all);
	websWrite(wp, T("ap_ackpolicy_all: %s<br>\n"), ap_ackpolicy_all);
	websWrite(wp, T("sta_aifsn_all: %s<br>\n"), sta_aifsn_all);
	websWrite(wp, T("sta_cwmin_all: %s<br>\n"), sta_cwmin_all);
	websWrite(wp, T("sta_cwmax_all: %s<br>\n"), sta_cwmax_all);
	websWrite(wp, T("sta_txop_all: %s<br>\n"), sta_txop_all);
	websWrite(wp, T("sta_acm_all: %s<br>\n"), sta_acm_all);
	websFooter(wp);
	websDone(wp, 200);
}

static int getRaixWlanStaInfo(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, s;
	struct iwreq iwr;
	RT_802_11_MAC_TABLE table = {0};
#if 0 //def CONFIG_RT2860V2_AP_TXBF
	char tmpBuff[32];
	char *phyMode[4] = {"CCK", "OFDM", "MM", "GF"};
#endif

	s = socket(AF_INET, SOCK_DGRAM, 0);
	strncpy(iwr.ifr_name, "rai0", IFNAMSIZ);
	iwr.u.data.pointer = (caddr_t) &table;

	if (s < 0) {
		websError(wp, 500, "ioctl sock failed!");
		return -1;
	}

#if 1
	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, &iwr) < 0) {
		websError(wp, 500, "ioctl -> RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT failed!");
		close(s);
		return -1;
	}
#else
	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) < 0) {
		websError(wp, 500, "ioctl -> RTPRIV_IOCTL_GET_MAC_TABLE failed!");
		close(s);
		return -1;
	}
#endif

	close(s);
#if 0 //def CONFIG_RT2860V2_AP_TXBF
	for (i = 0; i < table.Num; i++) {
		RT_802_11_MAC_ENTRY *pe = &(table.Entry[i]);
		unsigned int lastRxRate = pe->LastRxRate;
		unsigned int mcs = pe->LastRxRate & 0x7F;
		int hr, min, sec;

		hr = pe->ConnectedTime/3600;
		min = (pe->ConnectedTime % 3600)/60;
		sec = pe->ConnectedTime - hr*3600 - min*60;

		// MAC Address
		websWrite(wp, T("<tr><td>%02X:%02X:%02X:<br>%02X:%02X:%02X</td>"),
				pe->Addr[0], pe->Addr[1], pe->Addr[2], pe->Addr[3],
				pe->Addr[4], pe->Addr[5]);

		// AID, Power Save mode, MIMO Power Save
		websWrite(wp, T("<td align=center\">%d</td><td align=\"center\">%d</td><td align=\"center\">%d</td>"),
				pe->Aid, pe->Psm, pe->MimoPs);

		// TX Rate
		websWrite(wp, T("<td>MCS %d<br>%2dM, %cGI<br>%s%s</td>"),
				pe->TxRate.field.MCS,
				pe->TxRate.field.BW? 40: 20,
				pe->TxRate.field.ShortGI? 'S': 'L',
				phyMode[pe->TxRate.field.MODE],
				pe->TxRate.field.STBC? ", STBC": " ");

		// TxBF configuration
		websWrite(wp, T("<td align=\"center\">%c %c</td>"),
				pe->TxRate.field.iTxBF? 'I': '-', pe->TxRate.field.eTxBF? 'E': '-');

		// RSSI
		websWrite(wp, T("<td align=\"center\">%d<br>%d<br>%d</td>"),
				(int)(pe->AvgRssi0), (int)(pe->AvgRssi1), (int)(pe->AvgRssi2));

		// Per Stream SNR
		snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->StreamSnr[0]*0.25);
		websWrite(wp, T("<td>%s<br>"), tmpBuff);
		snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->StreamSnr[1]*0.25); //mcs>7? pe->StreamSnr[1]*0.25: 0.0);
		websWrite(wp, T("%s<br>"), tmpBuff);
		snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->StreamSnr[2]*0.25); //mcs>15? pe->StreamSnr[2]*0.25: 0.0);
		websWrite(wp, T("%s</td>"), tmpBuff);

		// Sounding Response SNR
		if (pe->TxRate.field.eTxBF) {
			snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->SoundingRespSnr[0]*0.25);
			websWrite(wp, T("<td>%s<br>"), tmpBuff);
			snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->SoundingRespSnr[1]*0.25);
			websWrite(wp, T("%s<br>"), tmpBuff);
			snprintf(tmpBuff, sizeof(tmpBuff), "%0.1f", pe->SoundingRespSnr[2]*0.25);
			websWrite(wp, T("%s</td>"), tmpBuff);
		}
		else {
			websWrite(wp, T("<td align=\"center\">-<br>-<br>-</td>"));
		}

		// Last RX Rate
		websWrite(wp, T("<td>MCS %d<br>%2dM, %cGI<br>%s%s</td>"),
				mcs,  ((lastRxRate>>7) & 0x1)? 40: 20,
				((lastRxRate>>8) & 0x1)? 'S': 'L',
				phyMode[(lastRxRate>>14) & 0x3],
				((lastRxRate>>9) & 0x3)? ", STBC": " ");

		// Connect time
		websWrite(wp, T("<td align=\"center\">%02d:%02d:%02d</td>"), hr, min, sec);
		websWrite(wp, T("</tr>"));
	}
#else
	for (i = 0; i < table.Num; i++) {
		websWrite(wp, T("<tr><td>%02X:%02X:%02X:%02X:%02X:%02X</td>"),
				table.Entry[i].Addr[0], table.Entry[i].Addr[1],
				table.Entry[i].Addr[2], table.Entry[i].Addr[3],
				table.Entry[i].Addr[4], table.Entry[i].Addr[5]);
		websWrite(wp, T("<td>%d</td><td>%d</td><td>%d</td>"),
				table.Entry[i].Aid, table.Entry[i].Psm, table.Entry[i].MimoPs);
		websWrite(wp, T("<td>%d</td><td>%s</td><td>%d</td><td>%d</td></tr>"),
				table.Entry[i].TxRate.field.MCS,
				(table.Entry[i].TxRate.field.BW == 0)? "20M":"40M",
				table.Entry[i].TxRate.field.ShortGI, table.Entry[i].TxRate.field.STBC);
	}
#endif
	return 0;
}

/* goform/inicWds */
static void inicWds(webs_t wp, char_t *path, char_t *query)
{
	char_t	*wds_mode, *wds_phy_mode, *wds_encryp_type, *wds_encryp_key0,
			*wds_encryp_key1,*wds_encryp_key2, *wds_encryp_key3, *wds_list;

	wds_mode = websGetVar(wp, T("wds_mode"), T("0"));
	wds_phy_mode = websGetVar(wp, T("wds_phy_mode"), T(""));
	wds_encryp_type = websGetVar(wp, T("wds_encryp_type"), T(""));
	wds_encryp_key0 = websGetVar(wp, T("wds_encryp_key0"), T(""));
	wds_encryp_key1 = websGetVar(wp, T("wds_encryp_key1"), T(""));
	wds_encryp_key2 = websGetVar(wp, T("wds_encryp_key2"), T(""));
	wds_encryp_key3 = websGetVar(wp, T("wds_encryp_key3"), T(""));
	wds_list = websGetVar(wp, T("wds_list"), T(""));

	nvram_bufset(RTDEV_NVRAM, "WdsEnable", wds_mode);
	if (strncmp(wds_mode, "0", 2)) {
		nvram_bufset(RTDEV_NVRAM, "WdsPhyMode", wds_phy_mode);
		nvram_bufset(RTDEV_NVRAM, "WdsEncrypType", wds_encryp_type);
		nvram_bufset(RTDEV_NVRAM, "Wds0Key", wds_encryp_key0);
		nvram_bufset(RTDEV_NVRAM, "Wds1Key", wds_encryp_key1);
		nvram_bufset(RTDEV_NVRAM, "Wds2Key", wds_encryp_key2);
		nvram_bufset(RTDEV_NVRAM, "Wds3Key", wds_encryp_key3);
		if (!strncmp(wds_mode, "2", 2) || !strncmp(wds_mode, "3", 2)) {
			if (0 != strlen(wds_list))
				nvram_bufset(RTDEV_NVRAM, "WdsList", wds_list);
		}
	}
	nvram_commit(RTDEV_NVRAM);

	initInternet();

	//debug print
	websHeader(wp);
	websWrite(wp, T("wds_mode: %s<br>\n"), wds_mode);
	websWrite(wp, T("wds_phy_mode: %s<br>\n"), wds_phy_mode);
	websWrite(wp, T("wds_encryp_type: %s<br>\n"), wds_encryp_type);
	websWrite(wp, T("wds_encryp_key0: %s<br>\n"), wds_encryp_key0);
	websWrite(wp, T("wds_encryp_key1: %s<br>\n"), wds_encryp_key1);
	websWrite(wp, T("wds_encryp_key2: %s<br>\n"), wds_encryp_key2);
	websWrite(wp, T("wds_encryp_key3: %s<br>\n"), wds_encryp_key3);
	websWrite(wp, T("wds_list: %s<br>\n"), wds_list);
	websFooter(wp);
	websDone(wp, 200);
}

static int getRaixApStats(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *t;
	NDIS_802_11_STATISTICS stat;
	int s, ret;
	float txCount;
	char  tmpStatistics[256];

	if (ejArgs(argc, argv, T("%s"), &t) < 1) {
		return websWrite(wp, T("Insufficient args"));
	}

	s = socket(AF_INET, SOCK_DGRAM, 0);
	ret = ApOidQueryInformation(OID_802_11_STATISTICS, s, "rai0", &stat, sizeof(stat));
	close(s);
	if (ret >= 0) {
		txCount = stat.TransmittedFragmentCount.QuadPart;

		if (!strncmp(t, "TxSucc", 7))
			return websWrite(wp, T("%ld"), stat.TransmittedFragmentCount.QuadPart);
		else if (!strncmp(t, "TxRetry", 8))
		{
			sprintf(tmpStatistics, "%lld,  PER=%0.1f%%", stat.RetryCount.QuadPart,
				txCount==0? 0.0: 100.0f*(stat.RetryCount.QuadPart+stat.FailedCount.QuadPart)/(stat.RetryCount.QuadPart+stat.FailedCount.QuadPart+txCount));
			return websWrite(wp, T("%s"), tmpStatistics);
		}
		else if (!strncmp(t, "TxFail", 7))
		{
			sprintf(tmpStatistics, "%lld,  PLR=%0.1e", stat.FailedCount.QuadPart,
			txCount==0? 0.0: (float)stat.FailedCount.QuadPart/(stat.FailedCount.QuadPart+txCount));
			return websWrite(wp, T("%s"), tmpStatistics);
		}
		else if (!strncmp(t, "RTSSucc", 8))
			return websWrite(wp, T("%ld"), stat.RTSSuccessCount.QuadPart);
		else if (!strncmp(t, "RTSFail", 8))
			return websWrite(wp, T("%ld"), stat.RTSFailureCount.QuadPart);
		else if (!strncmp(t, "RxSucc", 7))
			return websWrite(wp, T("%ld"), stat.ReceivedFragmentCount.QuadPart);
		else if (!strncmp(t, "FCSError", 9))
		{
			sprintf(tmpStatistics, "%lld, PER=%0.1f%%", stat.FCSErrorCount.QuadPart,
					stat.ReceivedFragmentCount.QuadPart==0?
						0.0: 100.0f*stat.FCSErrorCount.QuadPart/(stat.FCSErrorCount.QuadPart+stat.ReceivedFragmentCount.QuadPart));
			return websWrite(wp, T("%s"), tmpStatistics);
		}
		else
			return websWrite(wp, T("type not supported"));
	}
	else
		return websWrite(wp, T("ioctl %d"), ret);
}

/*
 * description: write AP SNR
 */
static int getRaixApSNR(int eid, webs_t wp, int argc, char_t **argv)
{
	int s, n, ret;
	unsigned long SNR;

	if (ejArgs(argc, argv, T("%d"), &n) < 1) {
		return websWrite(wp, T("Insufficient args\n"));
	}

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (n == 0)
		ret = ApOidQueryInformation(RT_OID_802_11_SNR_0, s, "rai0", &SNR, sizeof(SNR));
	else if (n == 1)
		ret = ApOidQueryInformation(RT_OID_802_11_SNR_1, s, "rai0", &SNR, sizeof(SNR));
	else if (n == 2)
		ret = ApOidQueryInformation(RT_OID_802_11_SNR_2, s, "rai0", &SNR, sizeof(SNR));
	else
		ret = -1;
	close(s);

	//fprintf(stderr, "SNR%d = %ld\n", n, SNR);
	if (ret < 0)
		return websWrite(wp, "n/a");
	else
		return websWrite(wp, "%ld", SNR);
}

/* goform/inicRvt */
static void inicRvt(webs_t wp, char_t *path, char_t *query)
{
	char_t	*macenhance, *classifier;

	macenhance = websGetVar(wp, T("macenhance"), T("0"));
	classifier = websGetVar(wp, T("classifier"), T("0"));

	nvram_bufset(RTDEV_NVRAM, "VideoTurbine", macenhance);
	nvram_commit(RTDEV_NVRAM);

	if (!strncmp(classifier, "0", 2))
		doSystem("rmmod cls");
	else
		doSystem("insmod -q cls");
	initInternet();

	//debug print
	websHeader(wp);
	websWrite(wp, T("macenhance: %s<br>\n"), macenhance);
	websWrite(wp, T("classifier: %s<br>\n"), classifier);
	websFooter(wp);
	websDone(wp, 200);
	return;
}

static void inicwGetSecurity(webs_t wp, char_t *path, char_t *query)
{
	getSecurity(RTDEV_NVRAM, wp, path, query);		//in wireless.c
}

static void INICSecurity(webs_t wp, char_t *path, char_t *query)
{
	Security(RTDEV_NVRAM, wp, path, query);		// in wireless.c
}

static void INICDeleteAccessPolicyList(webs_t wp, char_t *path, char_t *query)
{
	DeleteAccessPolicyList(RTDEV_NVRAM, wp, path, query);
}

static int getRaixABand(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RTDEV_ABAND_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}
 
static int getRaixDLSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RTDEV_DLS_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getRaixAutoProvisionBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (CONFIG_RT3090_AUTO_PROVISION)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getRaixWDSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RTDEV_WDS_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getRaixRVTBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (CONFIG_RA_CLASSIFIER_MODULE) && \
    (defined (CONFIG_RT3090_AP_VIDEO_TURBINE) || defined (CONFIG_RT3592_AP_VIDEO_TURBINE) || \
	defined (CONFIG_RT5392_AP_VIDEO_TURBINE))
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getRaixDFSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RTDEV_DFS_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getRaixCarrierBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RTDEV_CARRIER_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getRaixWSCBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RTDEV_WSC_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getRaixHTStream(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RTDEV_3T3R_SUPPORT)
		websWrite(wp, T("3"));
#else
		websWrite(wp, T("2"));	
#endif
	return 0;
}

static int getRaix11nDraft3Built(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RTDEV_11NDRAFT3_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getRaixWAPIBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (RTDEV_WAPI_SUPPORT)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

