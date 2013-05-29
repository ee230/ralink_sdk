/*
 *	wireless.h -- Wireless Configuration Header 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: wireless.h,v 1.11.6.1 2012-03-30 12:31:47 chhung Exp $
 */

void formDefineWireless(void);
#if defined (RT2860_WAPI_SUPPORT) || defined (RTDEV_WAPI_SUPPORT)
void restartWAPIDaemon(int nvram);
#endif
void restart8021XDaemon(int nvram);
void updateFlash8021x(int nvram);
void Security(int nvram, webs_t wp, char_t *path, char_t *query);
void confWPAGeneral(int nvram, webs_t wp, int mbssid);
void confWEP(int nvram, webs_t wp, int mbssid);
void conf8021x(int nvram, webs_t wp, int mbssid);
void getSecurity(int nvram, webs_t wp, char_t *path, char_t *query);
void DeleteAccessPolicyList(int nvram, webs_t wp, char_t *path, char_t *query);
int ApOidQueryInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength);

typedef union _MACHTTRANSMIT_SETTING {
	struct  {
		unsigned short  MCS:7;  // MCS
		unsigned short  BW:1;   //channel bandwidth 20MHz or 40 MHz
		unsigned short  ShortGI:1;
		unsigned short  STBC:2; //SPACE
		unsigned short	eTxBF:1;
		unsigned short	rsv:1;
		unsigned short	iTxBF:1;
		unsigned short  MODE:2; // Use definition MODE_xxx.
	} field;
	unsigned short      word;
} MACHTTRANSMIT_SETTING;


