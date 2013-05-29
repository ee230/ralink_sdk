#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <asm/types.h>
#include <linux/if.h>
#include <linux/wireless.h>

#include <ctype.h>

#include "nvram.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "wireless.h"
#include "linux/autoconf.h"									/* for CONFIG_RT2860V2_STA_WSC */

#include "wps.h"

#define DD printf("%s(),  %d\n", __FUNCTION__, __LINE__);	fflush(stdout);
#define AP_MODE
#include "oid.h"

#define WPS_AP_TIMEOUT_SECS				120000				// 120 seconds
#define WPS_AP_TIMEOUT_SECS_SEND_M7		120000				// 120 seconds
#define WPS_AP_CATCH_CONFIGURED_TIMER	100					// 0.1 sec 

static int g_wps_timer_state = 0;

int g_wsc_configured = 0;							// export for wireless.c
int g_Raix_wsc_configured = 0;							// export for wireless.c
													// We can't know if WSC process success or not by the variable 
													// g_wsc_configured when AP being a WSC proxy.
static int g_WscResult = 0;							// for AP only ( STA WPS don't need this)
static int g_isEnrollee = 0;						// for AP only

#ifdef CONFIG_RT2860V2_STA_WSC						// if RT2880 support Wifi - STA 
#define WPS_STA_TIMEOUT_SECS			120000				// 120 seconds
#define WPS_STA_CATCH_CONFIGURED_TIMER	10					// 10 * 1000 microsecond = every 0.010 sec
#define REGISTRAR_TIMER_MODE			0xdeadbeef			// okay, this is a magic number
unsigned int ConvertRssiToSignalQuality(long RSSI);
int OidQueryInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength);
int OidSetInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength);
void WPSSTAPBCStartEnr(void);
static int getStaWPSBSSIDListASP(int eid, webs_t wp, int argc, char_t **argv);
static void updateWPSStaStatus(webs_t wp, char_t *path, char_t *query);
static void WPSSTAPINEnr(webs_t wp, char_t *path, char_t *query);
static void WPSSTAPBCEnr(webs_t wp, char_t *path, char_t *query);
static void WPSSTAStop(webs_t wp, char_t *path, char_t *query);
static char_t *addWPSSTAProfile(char_t *result);
static char_t *addWPSSTAProfile2(WSC_CREDENTIAL *);
void WPSSTAEnrolleeTimerHandler(int);
void WPSSTARegistrarTimerHandler(int);
void freeHeaderProfileSettings(void);

extern PAIR_CHANNEL_FREQ_ENTRY ChannelFreqTable[] ;
extern int G_nChanFreqCount;
extern char G_bRadio ;
extern NDIS_802_11_SSID        G_SSID;
extern PRT_PROFILE_SETTING headerProfileSetting;


static unsigned char *getStaMacAddr(void);
static char	*g_pAPListData = NULL;

#endif	/* CONFIG_RT2860V2_STA_WSC */


/* I believe it has been defined in oid.h, but just make a sure. */
int getWscProfile(char *interface, WSC_PROFILE *wsc_profile)
{
	int socket_id;
	struct iwreq wrq;

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(wrq.ifr_name, interface);
	wrq.u.data.length = sizeof(WSC_PROFILE);
	wrq.u.data.pointer = (caddr_t) wsc_profile;
	wrq.u.data.flags = RT_OID_802_11_WSC_QUERY_PROFILE;
	if( ioctl(socket_id, RT_PRIV_IOCTL, &wrq) == -1){
		printf("ioctl error, getWscProfile:%s\n", strerror(errno));
		close(socket_id);
		return -1;
	}
	close(socket_id);
	return 0;
}

static void resetTimerAll(void)
{
	stopTimer();
	g_wps_timer_state = 0;
}

typedef struct _WSC_UUID_T {
	unsigned int 	timeLow;
	unsigned short 	timeMid;
	unsigned short 	timeHi_Version;
	unsigned char  	clockSeqHi_Var;
	unsigned char  	clockSeqLow;
	unsigned char  	node[6];
} WSC_UUID_T;

static void GenerateUUID(void)
{
	WSC_UUID_T uuid_t;
	unsigned long long uuid_time = 0x3883309238833092LL;
	unsigned short clkSeq;
	int skfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct ifreq ifr;
	struct iwreq wrq;
	unsigned char uuidAscStr[37];
	unsigned char uuidHexNum[16];
	unsigned char uuidHexStr[33];
	union {
		unsigned int 	u32;
		unsigned short 	u16;
	} tmp;

	srand(time(NULL));
	clkSeq = rand() & 0xFFFF;
	uuid_t.timeLow = (unsigned int)uuid_time & 0xFFFFFFFF;
	uuid_t.timeMid = (unsigned short)((uuid_time >>32) & 0xFFFF);
	uuid_t.timeHi_Version = (unsigned short)((uuid_time >> 48) & 0x0FFF);
	uuid_t.timeHi_Version |= (1 << 12);
	uuid_t.clockSeqLow = clkSeq & 0xFF;
	uuid_t.clockSeqHi_Var = (clkSeq & 0x3F00) >> 8;
	uuid_t.clockSeqHi_Var |= 0x80;
	strncpy(ifr.ifr_name, "ra0", 16);
	ioctl(skfd, SIOCGIFHWADDR, &ifr);
	memcpy(uuid_t.node, (unsigned char *)&ifr.ifr_addr.sa_data, 6);
	sprintf(uuidAscStr, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", 
			    uuid_t.timeLow, uuid_t.timeMid, uuid_t.timeHi_Version,
			    uuid_t.clockSeqHi_Var, uuid_t.clockSeqLow, uuid_t.node[0],
			    uuid_t.node[1], uuid_t.node[2], uuid_t.node[3], uuid_t.node[4], uuid_t.node[5]);
	tmp.u32 = htonl(uuid_t.timeLow);
	memcpy(&uuidHexNum[0], &tmp.u32, 4);
	tmp.u16 = htons(uuid_t.timeMid);
	memcpy(&uuidHexNum[4], &tmp.u16, 2);
	tmp.u16 = htons(uuid_t.timeHi_Version);
	memcpy(&uuidHexNum[6], &tmp.u16, 2);
	memcpy(&uuidHexNum[8], &uuid_t.clockSeqHi_Var, 1);
	memcpy(&uuidHexNum[9], &uuid_t.clockSeqLow, 1);
	memcpy(&uuidHexNum[10], uuid_t.node, 6);
	sprintf(uuidHexStr, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
			    uuidHexNum[0], uuidHexNum[1], uuidHexNum[2], uuidHexNum[3], uuidHexNum[4], uuidHexNum[5],
			    uuidHexNum[6], uuidHexNum[7], uuidHexNum[8], uuidHexNum[9], uuidHexNum[10], uuidHexNum[11],
			    uuidHexNum[12], uuidHexNum[13], uuidHexNum[14], uuidHexNum[15]);

	strcpy(wrq.ifr_name, "ra0");
	wrq.u.data.length = sizeof(uuidAscStr) - 1;
	wrq.u.data.pointer = (caddr_t) &uuidAscStr;
	wrq.u.data.flags = RT_OID_WSC_UUID | OID_GET_SET_TOGGLE;
	ioctl(skfd, RT_PRIV_IOCTL, &wrq);
	wrq.u.data.length = sizeof(uuidHexNum);
	wrq.u.data.pointer = (caddr_t) &uuidHexNum;
	wrq.u.data.flags = RT_OID_WSC_UUID | OID_GET_SET_TOGGLE;
	ioctl(skfd, RT_PRIV_IOCTL, &wrq);
	strcpy(wrq.ifr_name, "rai0");
	wrq.u.data.length = sizeof(uuidAscStr) - 1;
	wrq.u.data.pointer = (caddr_t) &uuidAscStr;
	wrq.u.data.flags = RT_OID_WSC_UUID | OID_GET_SET_TOGGLE;
	ioctl(skfd, RT_PRIV_IOCTL, &wrq);
	wrq.u.data.length = sizeof(uuidHexNum);
	wrq.u.data.pointer = (caddr_t) &uuidHexNum;
	wrq.u.data.flags = RT_OID_WSC_UUID | OID_GET_SET_TOGGLE;
	ioctl(skfd, RT_PRIV_IOCTL, &wrq);
	
	close(skfd);

	// fprintf(stderr, "\nGUI ASCII string: %s\n", uuidAscStr);
	// fprintf(stderr, "\nGUI HEX string: %s\n", uuidHexStr);
	nvram_bufset(RT2860_NVRAM, "WSC_UUID_Str1", uuidAscStr);
	nvram_bufset(RT2860_NVRAM, "WSC_UUID_E1", uuidHexStr);
	nvram_commit(RT2860_NVRAM);
	nvram_bufset(RTDEV_NVRAM, "WSC_UUID_Str1", uuidAscStr);
	nvram_bufset(RTDEV_NVRAM, "WSC_UUID_E1", uuidHexStr);
	nvram_commit(RTDEV_NVRAM);
}

unsigned int getAPPIN(char *interface)
{
	int socket_id;
	struct iwreq wrq;
	unsigned int data = 0;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(wrq.ifr_name, interface);
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) &data;
	wrq.u.data.flags = RT_OID_WSC_PIN_CODE;
	if( ioctl(socket_id, RT_PRIV_IOCTL, &wrq) == -1)
		printf("ioctl error\n");
	close(socket_id);
	return data;
}

int getWscStatus(char *interface)
{
	int socket_id;
	struct iwreq wrq;
	int data = 0;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(wrq.ifr_name, interface);
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) &data;
	wrq.u.data.flags = RT_OID_WSC_QUERY_STATUS;
	if( ioctl(socket_id, RT_PRIV_IOCTL, &wrq) == -1)
		printf("ioctl error\n");
	close(socket_id);
	return data;
}

int getCurrentWscProfile(char *interface, WSC_CONFIGURED_VALUE *data, int len)
{
	int socket_id;
	struct iwreq wrq;

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy((char *)data, "get_wsc_profile");
	strcpy(wrq.ifr_name, interface);
	wrq.u.data.length = len;
	wrq.u.data.pointer = (caddr_t) data;
	wrq.u.data.flags = 0;
	ioctl(socket_id, RTPRIV_IOCTL_WSC_PROFILE, &wrq);
	close(socket_id);
	return 0;
}

/*
 * these definitions are from rt2860v2 driver include/wsc.h 
 */
char *getWscStatusStr(int status)
{
	switch(status){
	case 0:
		return "Not used";
	case 1:
		return "Idle";
	case 2:
		return "WSC Fail(Ignore this if Intel/Marvell registrar used)";
	case 3:
		return "Start WSC Process";
	case 4:
		return "Received EAPOL-Start";
	case 5:
		return "Sending EAP-Req(ID)";
	case 6:
		return "Receive EAP-Rsp(ID)";
	case 7:
		return "Receive EAP-Req with wrong WSC SMI Vendor Id";
	case 8:
		return "Receive EAPReq with wrong WSC Vendor Type";
	case 9:
		return "Sending EAP-Req(WSC_START)";
	case 10:
		return "Send M1";
	case 11:
		return "Received M1";
	case 12:
		return "Send M2";
	case 13:
		return "Received M2";
	case 14:
		return "Received M2D";
	case 15:
		return "Send M3";
	case 16:
		return "Received M3";
	case 17:
		return "Send M4";
	case 18:
		return "Received M4";
	case 19:
		return "Send M5";
	case 20:
		return "Received M5";
	case 21:
		return "Send M6";
	case 22:
		return "Received M6";
	case 23:
		return "Send M7";
	case 24:
		return "Received M7";
	case 25:
		return "Send M8";
	case 26:
		return "Received M8";
	case 27:
		return "Processing EAP Response (ACK)";
	case 28:
		return "Processing EAP Request (Done)";
	case 29:
		return "Processing EAP Response (Done)";
	case 30:
		return "Sending EAP-Fail";
	case 31:
		return "WSC_ERROR_HASH_FAIL";
	case 32:
		return "WSC_ERROR_HMAC_FAIL";
	case 33:
		return "WSC_ERROR_DEV_PWD_AUTH_FAIL";
	case 34:
		return "Configured";
	case 35:
		return "SCAN AP";
	case 36:
		return "EAPOL START SENT";
	case 37:
		return "WSC_EAP_RSP_DONE_SENT";
	case 38:
		return "WAIT PINCODE";
	case 39:
		return "WSC_START_ASSOC";
	case 0x101:
		return "PBC:TOO MANY AP";
	case 0x102:
		return "PBC:NO AP";
	case 0x103:
		return "EAP_FAIL_RECEIVED";
	case 0x104:
		return "EAP_NONCE_MISMATCH";
	case 0x105:
		return "EAP_INVALID_DATA";
	case 0x106:
		return "PASSWORD_MISMATCH";
	case 0x107:
		return "EAP_REQ_WRONG_SMI";
	case 0x108:
		return "EAP_REQ_WRONG_VENDOR_TYPE";
	case 0x109:
		return "PBC_SESSION_OVERLAP";
	default:
		return "Unknown";
	}
}

int isSafeForShell(char *str)
{
	if(strchr(str, ';')) return 0;
	if(strchr(str, '\'')) return 0;
	if(strchr(str, '\n')) return 0;
	if(strchr(str, '`')) return 0;
	if(strchr(str, '\"')) return 0;
	return 1;
}

void getWPSAuthMode(WSC_CONFIGURED_VALUE *result, char *ret_str)
{
	if(result->WscAuthMode & 0x1)
		strcat(ret_str, "Open");
	if(result->WscAuthMode & 0x2)
		strcat(ret_str, "WPA-PSK");
	if(result->WscAuthMode & 0x4)
		strcat(ret_str, "Shared");
	if(result->WscAuthMode & 0x8)
		strcat(ret_str, "WPA");
	if(result->WscAuthMode & 0x10)
		strcat(ret_str, "WPA2");
	if(result->WscAuthMode & 0x20)
		strcat(ret_str, "WPA2-PSK");	
}

void getWPSEncrypType(WSC_CONFIGURED_VALUE *result, char *ret_str)
{
	if(result->WscEncrypType & 0x1)
		strcat(ret_str, "None");
	if(result->WscEncrypType & 0x2)
		strcat(ret_str, "WEP");
	if(result->WscEncrypType & 0x4)
		strcat(ret_str, "TKIP");
	if(result->WscEncrypType & 0x8)
		strcat(ret_str, "AES");
}

/*
 *  Browsers will poll WPS info. from this funsction.
 */
void updateWPS( webs_t wp, char_t *path, char_t *query)
{
	int i, status1, status2;
	char tmp_str1[128], tmp_str2[128];

	WSC_CONFIGURED_VALUE result1, result2;

	getCurrentWscProfile("ra0", &result1, sizeof(WSC_CONFIGURED_VALUE));
	getCurrentWscProfile("rai0", &result2, sizeof(WSC_CONFIGURED_VALUE));

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

	//1. WPSConfigured
	websWrite(wp, T("%d"), result1.WscConfigured);
	if (result1.WscConfigured != result2.WscConfigured)
		websWrite(wp, T(" / %d"), result2.WscConfigured);
	websWrite(wp, T("\n"));
	
	//2. WPSSSID
	if(strchr(result1.WscSsid, '\n') && strchr(result2.WscSsid, '\n')) {
		websWrite(wp, T("Invalid SSID character: new line"));
	} else {
		websWrite(wp, T("%s"), result1.WscSsid);
		if (strcmp(result1.WscSsid, result2.WscSsid) != 0)
			websWrite(wp, T(" / %s"), result2.WscSsid);
	}
	websWrite(wp, T("\n"));

	//3. WPSAuthMode
	tmp_str1[0] = '\0';
	getWPSAuthMode(&result1, tmp_str1);
	websWrite(wp, T("%s"), tmp_str1);
	tmp_str2[0] = '\0';
	getWPSAuthMode(&result2, tmp_str2);
	if (strcmp(tmp_str1, tmp_str2) != 0)
		websWrite(wp, T(" / %s"), tmp_str2);
	websWrite(wp, T("\n"));

	//4. EncrypType
	tmp_str1[0] = '\0';
	getWPSEncrypType(&result1, tmp_str1);
	websWrite(wp, T("%s"), tmp_str1);
	tmp_str2[0] = '\0';
	getWPSEncrypType(&result2, tmp_str2);
	if (strcmp(tmp_str1, tmp_str2) != 0)
		websWrite(wp, T(" / %s"), tmp_str2);
	websWrite(wp, T("\n"));
	
	//5. DefaultKeyIdx
	websWrite(wp, T("%d"), result1.DefaultKeyIdx);
	if (result1.DefaultKeyIdx != result2.DefaultKeyIdx)
		websWrite(wp, T(" / %d\n"), result2.DefaultKeyIdx);
	websWrite(wp, T("\n"));
	
	//6. Key
	for (i=0; i<64; i++) {		// WPA key default length is 64 (defined & hardcode in driver) 
		if (i!=0 && !(i % 32))
			websWrite(wp, T("<br>"));
		websWrite(wp, T("%c"), result1.WscWPAKey[i]);
	}
	if (memcmp(result1.WscWPAKey, result2.WscWPAKey, 64) != 0) {
		websWrite(wp, T("<br><br>"));
		for (i=0; i<64; i++) {		// WPA key default length is 64 (defined & hardcode in driver) 
			if (i!=0 && !(i % 32))
				websWrite(wp, T("<br>"));
			websWrite(wp, T("%c"), result2.WscWPAKey[i]);
		}
	}
	websWrite(wp, T("\n"));

	//7. WSC Status
	status1 = getWscStatus("ra0");
	status2 = getWscStatus("rai0");
	if (status1 == 34 || status2 == 34) {
		websWrite(wp, T("%s"), getWscStatusStr(34));
	} else {
		websWrite(wp, T("%s"), getWscStatusStr(status1));
		if (status1 != status2)
			websWrite(wp, T(" / %s"), getWscStatusStr(status2));
	}
	websWrite(wp, T("\n"));

	//8. WSC Result
	websWrite(wp, T("%d\n"), g_WscResult);

	//9. WSC Status Index
	if (status2 > status1)
		websWrite(wp, T("%d"), status2);
	else
		websWrite(wp, T("%d"), status1);

	websDone(wp, 200);	
	return;
}

void WPSRestart(void)
{
	const char *wordlist;
	const char *mode = nvram_bufget(RT2860_NVRAM, "OperationMode");

	doSystem("killall wscd");
	doSystem("iwpriv ra0 set WscConfMode=0 1>/dev/null 2>&1");	// WPS disable
	doSystem("iwpriv rai0 set WscConfMode=0 1>/dev/null 2>&1");	// WPS disable

	if(!strcmp(mode, "0" )){		//bridge 
		// nop
	}else if(!strcmp(mode, "1")){	//gateway
		// nop
	}else if(!strcmp(mode, "2")){	// wireless isp
		return;						// no need to init and just return;
	}else if(!strcmp(mode, "3")){	// apcli
		// not support yet
		return;
	}else
		return;

	wordlist = nvram_bufget(RT2860_NVRAM, "WscModeOption");
	if(wordlist && (strcmp(wordlist, "0") > 0)) {
		// WPS Enable
		doSystem("iwpriv ra0 set WscConfMode=%d", 7);
		doSystem("iwpriv rai0 set WscConfMode=%d", 7);
		wordlist = nvram_bufget(RT2860_NVRAM, "WscConfigured");
		if (strcmp(wordlist, "0") == 0) {
        		doSystem("iwpriv ra0 set WscConfStatus=1");
        		doSystem("iwpriv rai0 set WscConfStatus=1");
		}
		doSystem("route add -host 239.255.255.250 dev br0 1>/dev/null 2>&1");
		doSystem("wscd -m 1 -a %s -i ra0 &", nvram_bufget(RT2860_NVRAM, "lan_ipaddr"));
	} else {
			doSystem("route delete 239.255.255.250 1>/dev/null 2>&1");
	}
		
	wordlist = nvram_get(RT2860_NVRAM, "WscConfigured");
	if(wordlist) {
		g_wsc_configured = atoi(wordlist);
		g_Raix_wsc_configured = atoi(wordlist);
	}
	else {
		g_wsc_configured = 0;
		g_Raix_wsc_configured = 0;
		g_WscResult = 0;
	}

	g_isEnrollee = 0;
}


/*
 * used
 */
static int getPINASP(int eid, webs_t wp, int argc, char_t **argv)
{
	websWrite(wp, T("%08d"), getAPPIN("ra0"));
	return 0;
}

/* Load from Web */
#define LFW(x, y)	do{												\
						if(! ( x = websGetVar(wp, T(#y), T(""))))	\
							return;									\
					}while(0)

#define LFWi(x, y)	do{														\
						char_t *x##_str;									\
						if(! ( x##_str = websGetVar(wp, T(#y), T(""))))		\
							return;											\
						x = atoi(x##_str);									\
					}while(0)

static void STF(int nvram, int index, char *flash_key, char *value)
{
	char *tmp = (char *) nvram_bufget(nvram, flash_key);
	nvram_bufset(nvram, flash_key, setNthValue(index, tmp, value));
	return;
}

static void WPSCancel(webs_t wp, char_t *path, char_t *query)
{
	resetTimerAll();
	doSystem("killall wscd");
	doSystem("iwpriv ra0 set WscConfMode=0 1>/dev/null 2>&1");
	doSystem("iwpriv ra0 set WscConfMode=%d", 7);
	doSystem("iwpriv ra0 set WscConfStatus=1");
	doSystem("iwpriv rai0 set WscConfMode=0 1>/dev/null 2>&1");
	doSystem("iwpriv rai0 set WscConfMode=%d", 7);
	doSystem("iwpriv rai0 set WscConfStatus=1");
	doSystem("wscd -m 1 -a %s -i ra0 &", nvram_bufget(RT2860_NVRAM, "lan_ipaddr"));
	return;
}

static void WPSSetup(webs_t wp, char_t *path, char_t *query)
{
	int     wsc_enable = 0;

	LFWi(wsc_enable, WPSEnable);

	resetTimerAll();
	g_WscResult = 0;
	LedReset();


	if (wsc_enable == 0){
		nvram_bufset(RT2860_NVRAM, "WscModeOption", "0");
		nvram_bufset(RTDEV_NVRAM, "WscModeOption", "0");
	}else{
		char pin_code[9];

		nvram_bufset(RT2860_NVRAM, "WscModeOption", "7");
		nvram_bufset(RTDEV_NVRAM, "WscModeOption", "7");
		sprintf(pin_code, "%08d", getAPPIN("ra0"));
		doSystem("iwpriv rai0 set WscVendorPinCode=%s", pin_code);
		nvram_bufset(RTDEV_NVRAM, "WscVendorPinCode", pin_code);
		GenerateUUID();
		doSystem("ralink_init make_wireless_config 2860");
		doSystem("ralink_init make_wireless_config rtdev");
	}
	nvram_commit(RT2860_NVRAM);
	nvram_commit(RTDEV_NVRAM);

	doSystem("killall wscd");
	if (wsc_enable == 0) {
		doSystem("iwpriv ra0 set WscConfMode=0 1>/dev/null 2>&1");
		doSystem("iwpriv rai0 set WscConfMode=0 1>/dev/null 2>&1");
		doSystem("route delete 239.255.255.250 1>/dev/null 2>&1");
	} else {
		doSystem("iwpriv ra0 set WscConfMode=%d", 7);
		doSystem("iwpriv rai0 set WscConfMode=%d", 7);
//		doSystem("iwpriv ra0 set WscConfMode=%d", wsc_enable + wsc_proxy + wsc_reg);
//		printf("wsc_enable:%d\nwsc_proxy:%d\nwsc_reg:%d\n",  wsc_enable ,wsc_proxy ,wsc_reg);
		doSystem("route add -host 239.255.255.250 dev br0");
		doSystem("wscd -m 1 -a %s -i ra0 &", nvram_bufget(RT2860_NVRAM, "lan_ipaddr"));
	}
	
	websRedirect(wp, "wps/wps_unique.asp");
	return;
}

int getAPMac(char *ifname, char *if_hw)
{
    struct ifreq ifr;
    char *ptr;
    int skfd;

    if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        error(E_L, E_LOG, T("getAPMac: open socket error"));
        return -1;
    }

    strncpy(ifr.ifr_name, ifname, 16);
    if(ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
        error(E_L, E_LOG, T("getAPMac: ioctl SIOCGIFHWADDR error for %s"),
                    ifname);
        return -1;
    }

    ptr = (char *)&ifr.ifr_addr.sa_data;
    sprintf(if_hw, "%02X%02X%02X", (ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));

    close(skfd);
    return 0;
}

static void GenPIN(webs_t wp, char_t *path, char_t *query)
{
	doSystem("iwpriv ra0 set WscGenPinCode");

	char new_pin[9];
	sprintf(new_pin, "%08d", getAPPIN("ra0"));

	nvram_bufset(RT2860_NVRAM, "WscVendorPinCode", new_pin);
	nvram_commit(RT2860_NVRAM);
	doSystem("ralink_init make_wireless_config rt2860");
	doSystem("iwpriv rai0 set WscVendorPinCode=%s", new_pin);
	nvram_bufset(RTDEV_NVRAM, "WscVendorPinCode", new_pin);
	nvram_commit(RTDEV_NVRAM);
	doSystem("ralink_init make_wireless_config rtdev");

	websRedirect(wp, "wps/wps_unique.asp");
}

/*
 *  AP: OOB
 */
static void OOB(webs_t wp, char_t *path, char_t *query)
{
        // clear timer
        resetTimerAll();

        // clear WSC result indicator
        g_WscResult = 0;
        LedReset();

        nvram_bufset(RT2860_NVRAM, "SSID1", "RalinkInitAPSoC");

        nvram_bufset(RT2860_NVRAM, "WscConfigured", "0");

        STF(RT2860_NVRAM, 0, "AuthMode", "OPEN");
        STF(RT2860_NVRAM, 0, "EncrypType", "NONE");
        /*
        STF(RT2860_NVRAM, "DefaultKeyID", "2");
        nvram_bufset(RT2860_NVRAM, "WPAPSK1", "12345678");
        */

        STF(RT2860_NVRAM, 0, "IEEE8021X", "0");
        nvram_commit(RT2860_NVRAM);
        /*
         *   IMPORTANT !!!!!
         *   5VT doesn't need it cause it will reboot after OOB reset, but RT2880 does.
         */
        g_wsc_configured = 0;

        nvram_bufset(RTDEV_NVRAM, "SSID1", "RalinkInitAPSoC");
        nvram_bufset(RTDEV_NVRAM, "WscConfigured", "0");
        STF(RTDEV_NVRAM, 0, "AuthMode", "OPEN");
        STF(RTDEV_NVRAM, 0, "EncrypType", "NONE");
        STF(RTDEV_NVRAM, 0, "IEEE8021X", "0");
        nvram_commit(RTDEV_NVRAM);
        g_Raix_wsc_configured = 0;

        doSystem("iwpriv ra0 set AuthMode=OPEN");
        doSystem("iwpriv ra0 set EncrypType=NONE");
        doSystem("iwpriv ra0 set SSID=%s", nvram_bufget(RT2860_NVRAM, "SSID1"));
        restart8021XDaemon(RT2860_NVRAM);
        doSystem("iwpriv rai0 set AuthMode=OPEN");
        doSystem("iwpriv rai0 set EncrypType=NONE");
        doSystem("iwpriv rai0 set SSID=%s", nvram_bufget(RT2860_NVRAM, "SSID1"));
        restart8021XDaemon(RTDEV_NVRAM);

        WPSRestart();

        websRedirect(wp, "wps/wps_unique.asp");
}

static void SetToFlash(int nvram, WSC_PROFILE *wsc_profile)
{
	nvram_bufset(nvram, "WscConfigured", "1");

	nvram_bufset(nvram, "SSID1", wsc_profile->Profile[0].SSID.Ssid);
	nvram_bufset(nvram, "WscSSID", wsc_profile->Profile[0].SSID.Ssid);

	if (wsc_profile->Profile[0].AuthType == 0x0001) {
		STF(nvram, 0, "AuthMode", "OPEN");
	} else if (wsc_profile->Profile[0].AuthType == 0x0002) {
		STF(nvram, 0, "AuthMode", "WPAPSK");
	} else if (wsc_profile->Profile[0].AuthType == 0x0004) {
		STF(nvram, 0, "AuthMode", "SHARED");
	} else if (wsc_profile->Profile[0].AuthType == 0x0008) {
		STF(nvram, 0, "AuthMode", "WPA");
	} else if (wsc_profile->Profile[0].AuthType == 0x0010) {
		STF(nvram, 0, "AuthMode", "WPA2");
	} else if (wsc_profile->Profile[0].AuthType == 0x0020) {
		STF(nvram, 0, "AuthMode", "WPA2PSK");
	} else if (wsc_profile->Profile[0].AuthType == 0x0022) {
		STF(nvram, 0, "AuthMode", "WPAPSKWPA2PSK");
	} else {
		printf("goahead: Warning: can't get invalid authmode\n.");
		STF(nvram, 0, "AuthMode", "OPEN");
	}
	if (wsc_profile->Profile[0].EncrType == 0x0001) {
		STF(nvram, 0, "EncrypType", "NONE");
		STF(nvram, 0, "DefaultKeyID", "1");
	} else if (wsc_profile->Profile[0].EncrType == 0x0002) {
		STF(nvram, 0, "EncrypType", "WEP");
		if (wsc_profile->Profile[0].KeyLength == 5 ||
				wsc_profile->Profile[0].KeyLength == 13) {
			// Key Entry Method == ASCII 
			STF(nvram, 0, "Key1Type", "1");
			STF(nvram, 0, "Key2Type", "1");
			STF(nvram, 0, "Key3Type", "1");
			STF(nvram, 0, "Key4Type", "1");
		} else if (wsc_profile->Profile[0].KeyLength == 10 ||
				wsc_profile->Profile[0].KeyLength == 26) {
			// Key Entry Method == HEX 
			STF(nvram, 0, "Key1Type", "0");
			STF(nvram, 0, "Key2Type", "0");
			STF(nvram, 0, "Key3Type", "0");
			STF(nvram, 0, "Key4Type", "0");
		} else {
			// Key Entry Method == ASCII
			STF(nvram, 0, "Key1Type", "1");
			STF(nvram, 0, "Key2Type", "1");
			STF(nvram, 0, "Key3Type", "1");
			STF(nvram, 0, "Key4Type", "1");
		}

		if (wsc_profile->Profile[0].KeyIndex == 1) {
			STF(nvram, 0, "Key1Str1", wsc_profile->Profile[0].Key);
			STF(nvram, 0, "DefaultKeyID", "1");
		} else if (wsc_profile->Profile[0].KeyIndex == 2) {
			STF(nvram, 0, "Key2Str1", wsc_profile->Profile[0].Key);
			STF(nvram, 0, "DefaultKeyID", "2");
		} else if (wsc_profile->Profile[0].KeyIndex == 3) {
			STF(nvram, 0, "Key3Str1", wsc_profile->Profile[0].Key);
			STF(nvram, 0, "DefaultKeyID", "3");
		} else if (wsc_profile->Profile[0].KeyIndex == 4) {
			STF(nvram, 0, "Key4Str1", wsc_profile->Profile[0].Key);
			STF(nvram, 0, "DefaultKeyID", "4");
		}
	} else if (wsc_profile->Profile[0].EncrType == 0x0004) {
		STF(nvram, 0, "EncrypType", "TKIP");
		STF(nvram, 0, "DefaultKeyID", "2");
		char temp[65];
		memset(temp, 0, 65);
		memcpy(temp, wsc_profile->Profile[0].Key, wsc_profile->Profile[0].KeyLength);
		nvram_bufset(nvram, "WPAPSK1", temp);
	} else if (wsc_profile->Profile[0].EncrType == 0x0008) {
		STF(nvram, 0, "EncrypType", "AES");
		STF(nvram, 0, "DefaultKeyID", "2");
		char temp[65];
		memset(temp, 0, 65);
		memcpy(temp, wsc_profile->Profile[0].Key, wsc_profile->Profile[0].KeyLength);
		nvram_bufset(nvram, "WPAPSK1", temp);
	} else if (wsc_profile->Profile[0].EncrType == 0x000C) {
		STF(nvram, 0, "EncrypType", "TKIPAES");
		STF(nvram, 0, "DefaultKeyID", "2");
		char temp[65];
		memset(temp, 0, 65);
		memcpy(temp, wsc_profile->Profile[0].Key, wsc_profile->Profile[0].KeyLength);
		nvram_bufset(nvram, "WPAPSK1", temp);
	} else {
		printf("goahead: Warning: can't get invalid encryptype\n.");
		STF(nvram, 0, "EncrypType", "NONE");
		STF(nvram, 0, "DefaultKeyID", "1");
	}

	STF(nvram, 0, "IEEE8021X", "0");
	nvram_commit(nvram);
}

static void WPSAPTimerHandler(int signo)
{
	int RaxWscStatus = 0;
	int RaixWscStatus = 0;
	int wireless_mode = strtoul(nvram_bufget(RT2860_NVRAM, "WirelessMode"), NULL, 10);
	static int wsc_timeout_counter = 0;
	struct _WSC_CONFIGURED_VALUE wsc_value;

	RaxWscStatus = getWscStatus("ra0");
	printf("RaxWscStatus == %d\n", RaxWscStatus);
	if (wireless_mode == 2 || wireless_mode == 8 || wireless_mode == 11) {
		RaixWscStatus = getWscStatus("rai0");
		printf("RaixWscStatus == %d\n", RaixWscStatus);
	}

	if((RaxWscStatus == 3 
	    || RaixWscStatus == 3
	    ) && g_wps_timer_state == 0){	// 3 == "Start WSC Process"
		printf("goahead: Start to monitor WSC Status...\n");
		g_wps_timer_state = 1;
		wsc_timeout_counter = 0;
		LedInProgress();
	}

	// check if timeout
	wsc_timeout_counter += WPS_AP_CATCH_CONFIGURED_TIMER;
	if(wsc_timeout_counter > WPS_AP_TIMEOUT_SECS){

#if 0
		// keep for reference
		// ==    Work around for "Send M7" issue	  ==
		//if(g_wsc_configured == 1){
		//	if( WscStatus == 1 /* Idle */){
		//		g_WscResult = 1;
		//		wsc_timeout_counter = 0;
		//		resetTimerAll();
		//		LedSuccess();
		//		return;
		//	}
		//	if( WscStatus == 23 /* Send M7 */ && wsc_timeout_counter < WPS_AP_TIMEOUT_SECS_SEND_M7 )
		//		return;	// keep monitoring
		//}
		// ==    Work around for "Send M7" issue	  ==
#endif

		// Timeout happened.
		// Set g_WscResult to indicate WSC process failed.
		g_WscResult = -1;
		wsc_timeout_counter = 0;
		resetTimerAll();
		LedError();
		trace(0, T("-- WSC failed, Timeout\n"));
		return;
	}

	// deal with error
	if(RaxWscStatus == 2 && RaixWscStatus == 2) {			// 2 == "Wsc Process failed"
		if(g_isEnrollee){
			return;		// go on monitoring
		}else{
			g_WscResult = -1;
		fprintf(stderr, "%s", "Error occured. Is the PIN correct?\n");
		}

		// set g_WscResult to indicate WSC process failed.
		wsc_timeout_counter = 0;
		resetTimerAll();

		return;
	}

	// Driver 1.9 supports AP PBC Session Overlapping Detection.
	if(RaxWscStatus == 0x109 && RaixWscStatus == 0x109) {		/* PBC_SESSION_OVERLAP */
		g_WscResult = -1;
		wsc_timeout_counter = 0;
		resetTimerAll();                
		LedSessionOverlapDetected();
		return;
	}

	// then check idle status
	if ((RaxWscStatus == 1 && g_wsc_configured == 1) 
	    && (RaixWscStatus == 1 && g_Raix_wsc_configured == 1)) {	/*Idle*/ 
		// This means a proxy WPS AP (has got profile from other registrar)
		// transfer registrar's profile to enrollee successfully.
		wsc_timeout_counter = 0;
		resetTimerAll();
		return;
	}

	if (RaxWscStatus != 34 && RaixWscStatus != 34) {		// 34 == "Configured"
		// still in progress and keep monitoring.
		return;
	}else{
		wsc_timeout_counter = 0;
		resetTimerAll();
		g_WscResult = 1;
		LedSuccess();

		if (RaixWscStatus == 34)
			getCurrentWscProfile("rai0", &wsc_value, sizeof(WSC_CONFIGURED_VALUE));
		else
			getCurrentWscProfile("ra0", &wsc_value, sizeof(WSC_CONFIGURED_VALUE));
		if ((g_wsc_configured == 0 || g_Raix_wsc_configured == 0) 
		    && wsc_value.WscConfigured == 2) {	
			WSC_PROFILE wsc_profile;
			if (RaixWscStatus == 34)
				getWscProfile("rai0", &wsc_profile);
			else
				getWscProfile("ra0", &wsc_profile);
			SetToFlash(RT2860_NVRAM, &wsc_profile);
			SetToFlash(RTDEV_NVRAM, &wsc_profile);
			doSystem("ifconfig ra0 down");
			doSystem("ralink_init make_wireless_config 2860");
			doSystem("ifconfig ra0 up");
			sleep(1);
			doSystem("ifconfig rai0 down");
			doSystem("ralink_init make_wireless_config rtdev");
			doSystem("ifconfig rai0 up");
			sleep(1);
			restart8021XDaemon(RT2860_NVRAM);
			restart8021XDaemon(RTDEV_NVRAM);
			WPSRestart();
		}
	}

	return;
}

void WPSAPPBCStartAll(void)
{
	const char *wsc_enable = nvram_bufget(RT2860_NVRAM, "WscModeOption");
	
	// It is possible user press PBC button when WPS is disabled.
	if(!strcmp(wsc_enable, "0")){
		printf("The PBC button is pressed but WPS is disabled now.\n");
		return;
	}

	doSystem("iwpriv ra0 set WscMode=2");
	doSystem("iwpriv ra0 set WscGetConf=1");
	int wireless_mode = strtoul(nvram_bufget(RT2860_NVRAM, "WirelessMode"), NULL, 10);
	if (wireless_mode == 2 || wireless_mode == 8 || wireless_mode == 11) {
		doSystem("iwpriv rai0 set WscMode=2");
		doSystem("iwpriv rai0 set WscGetConf=1");
	}

	resetTimerAll();
	setTimer(WPS_AP_CATCH_CONFIGURED_TIMER * 1000, WPSAPTimerHandler);

}

/*
 * WPS Single Trigger Signal handler.
 */
void WPSSingleTriggerHandler(int signo)
{
	// WPS single trigger is launch now and AP is as enrollee
	g_isEnrollee = 1;
	resetTimerAll();
	setTimer(WPS_AP_CATCH_CONFIGURED_TIMER * 1000, WPSAPTimerHandler);
}

static void WPS(webs_t wp, char_t *path, char_t *query)
{
	int	pin_code = 0;

	char_t *wsc_config_option;
	char_t *wsc_pin_code_w;

	LFW(wsc_config_option, PINPBCRadio);

	// reset wsc result indicator
	g_WscResult = 0;
	LedReset();
	if(! strcmp(wsc_config_option, "1") ){
		doSystem("iwpriv ra0 set WscMode=1");

		// get pin code
		wsc_pin_code_w = websGetVar(wp, T("PIN"), T(""));
		if(!wsc_config_option || strlen(wsc_config_option) == 0){
			pin_code = 0;
		}else{
			pin_code = atoi(wsc_pin_code_w);
		}

		g_isEnrollee = pin_code ? 0 : 1;
//		nvram_commit(RT2860_NVRAM);

		if (g_isEnrollee == 0)
			doSystem("iwpriv ra0 set WscPinCode=%d", atoi(wsc_pin_code_w));
		doSystem("iwpriv ra0 set WscGetConf=1");
		doSystem("iwpriv rai0 set WscMode=1");
		if (g_isEnrollee == 0)
			doSystem("iwpriv rai0 set WscPinCode=%d", atoi(wsc_pin_code_w));
		doSystem("iwpriv rai0 set WscGetConf=1");

		resetTimerAll();
		setTimer(WPS_AP_CATCH_CONFIGURED_TIMER * 1000, WPSAPTimerHandler);
	}else if(! strcmp(wsc_config_option, "2") ){
		g_isEnrollee = 1;
		WPSAPPBCStartAll();
	}else{
		printf("ignore unknown WSC method: %s\n", wsc_config_option);
	}

	websRedirect(wp, "wps/wps_unique.asp"); 
}

#ifdef CONFIG_RT2860V2_STA_WSC

static char_t *DevicePasswordIDTranslate(unsigned short num)
{
	static char_t result[32];
	if(num == 0x0000)
		gstrncpy(result, T("PIN (default)"), 32);
	else if(num == 0x0001)
		gstrncpy(result, T("User-specified"), 32);
	else if(num == 0x0002)
		gstrncpy(result, T("Machine-specified"), 32);
	else if(num == 0x0003)
		gstrncpy(result, T("Rekey"), 32);
	else if(num == 0x0004)
		gstrncpy(result, T("Push Button"), 32);
	else if(num == 0x0005)
		gstrncpy(result, T("Registrar-specified"), 32);
	else
		gstrncpy(result, T("Reserved or unknown"), 32);
	return result;
}

static char_t *ConfigMethodTranslate(unsigned short num)
{
	static char_t result[128];
	result[0] = '\0';

	if(num & 0x0001)
		gstrcat(result, T("USBA(Flash Drive),"));
	if(num & 0x0002)
		gstrcat(result, T("Ethernet,"));
	if(num & 0x0004)
		gstrcat(result, T("Label,"));
	if(num & 0x0008)
		gstrcat(result, T("Display,"));
	if(num & 0x0010)
		gstrcat(result, T("External NFC Token,"));
	if(num & 0x0020)
		gstrcat(result, T("Integrated NFC Token,"));
	if(num & 0x0040)
		gstrcat(result, T("NFC Interface,"));
	if(num & 0x0080)
		gstrcat(result, T("Push Button,"));
	if(num & 0x0100)
		gstrcat(result, T("Keypad"));

	return result;
}

static char_t *RFBandTranslate(unsigned char rfband)
{
	static char_t result[16];
	gstrncpy(result, T("Unknown"), 16);
	switch(rfband){
	case 1:
		gstrncpy(result, T("2.4G"), 16);
	case 2:
		gstrncpy(result, T("5G"), 16);
	case 3:
		gstrncpy(result, T("2.4G/5G"), 16);
	}
	return result;
}

static char_t *PrimaryDeviceTypeTranslate(unsigned short category, unsigned short sub_cat)
{
	static char_t result[32];
	snprintf(result, 32, "Unknown:%d,%d", category, sub_cat);
	switch(category){
	case 1:
		if(sub_cat == 1)
			gstrncpy(result, T("PC"), 32);
		if(sub_cat == 2)
			gstrncpy(result, T("Server"), 32);
		if(sub_cat == 3)
			gstrncpy(result, T("Media Center"), 32);
		break;
	case 2:
		gstrncpy(result, T("Input Device"), 32);
		break;
	case 3:
		if(sub_cat == 1)
			gstrncpy(result, T("Printer"), 32);
		if(sub_cat == 2)
			gstrncpy(result, T("Scanner"), 32);
		break;
	case 4:
		gstrncpy(result, T("Digital Still Camera"), 32);
	case 5:
		gstrncpy(result, T("Network Storage"), 32);
	case 6:
		if(sub_cat == 1)
			gstrncpy(result, T("Access Point"), 32);
		if(sub_cat == 2)
			gstrncpy(result, T("Router"), 32);
		if(sub_cat == 3)
			gstrncpy(result, T("Switch"), 32);
		break;
	case 7:
		if(sub_cat == 1)
			gstrncpy(result, T("TV"), 32);
		if(sub_cat == 2)
			gstrncpy(result, T("Eletronic Picture Frame"), 32);
		if(sub_cat == 3)
			gstrncpy(result, T("Projector"), 32);
		break;
	case 8:
		if(sub_cat == 1)
			gstrncpy(result, T("Access Point"), 32);
		if(sub_cat == 2)
			gstrncpy(result, T("Router"), 32);
		if(sub_cat == 3)
			gstrncpy(result, T("Switch"), 32);
		break;
	case 9:
		if(sub_cat == 1)
			gstrncpy(result, T("DAR"), 32);
		if(sub_cat == 2)
			gstrncpy(result, T("PVR"), 32);
		if(sub_cat == 3)
			gstrncpy(result, T("MCX"), 32);
		if(sub_cat == 4)
			gstrncpy(result, T("DMR"), 32);
		break;
	case 10:
		gstrncpy(result, T("Windows Mobile"), 32);

	}
	return result;
}

static void WPSSTABSSIDListReset(webs_t wp, char_t *path, char_t *query)
{
	if(g_pAPListData)
		*g_pAPListData = '\0';
	websHeader(wp);
	websWrite(wp, T("BSSIDList reset\n"));
	websFooter(wp);
	websDone(wp, 200);
	return;
}

#define SITE_SURVEY_APS_MAX		(16*1024)
/*
 * description: STA WPS: write the WPS BSSID list (WPS AP site survey)
 */
static int getStaWPSBSSIDListASP(int eid, webs_t wp, int argc, char_t **argv)
{
	int                         s, ret, retry;
	unsigned int                lBufLen = 4096, we_version = 16; // 64K
	PNDIS_802_11_BSSID_LIST_EX	pBssidList;
	PNDIS_WLAN_BSSID_EX  		pBssid;
	unsigned int                ConnectStatus = NdisMediaStateDisconnected;
	unsigned char               BssidQuery[6];
	int EAGAIN_Count,QueryCount=0;

	/*
	 * dont free(g_pAPListData) during goahead life time because we want it persistant
	 */
	if(g_pAPListData == NULL){
		printf("wps site survey cache created.\n");
		if((g_pAPListData = (char *)malloc(SITE_SURVEY_APS_MAX)) == NULL)
			return -1;
		memset(g_pAPListData, 0, SITE_SURVEY_APS_MAX);
	}

	if(strlen(g_pAPListData)){
		websLongWrite(wp, g_pAPListData);
		return 0;
	}
	s = socket(AF_INET, SOCK_DGRAM, 0);
	pBssidList = (PNDIS_802_11_BSSID_LIST_EX) malloc(65536);  //64k
	memset(pBssidList, 0x00, sizeof(char)*65536);

	//step 1
	while(ConnectStatus != NdisMediaStateConnected && QueryCount < 3) {
	if (OidQueryInformation(OID_GEN_MEDIA_CONNECT_STATUS, s, "ra0", &ConnectStatus, sizeof(ConnectStatus)) < 0) {
		websError(wp, 500, "Query OID_GEN_MEDIA_CONNECT_STATUS failed!");
			free(pBssidList); 
			close(s);
		return -1;
	}
		sleep(2);
		QueryCount++;
	}

	//step 2
	if (OidQueryInformation(RT_OID_802_11_RADIO, s, "ra0", &G_bRadio, sizeof(G_bRadio)) < 0) {
		websError(wp, 500, "Query RT_OID_802_11_RADIO failed!");
		free(pBssidList); 
		close(s);
		return -1;
	}

	if (ConnectStatus == NdisMediaStateConnected && G_bRadio) {
		// for handshake
		memset(&BssidQuery, 0x00, sizeof(BssidQuery));
		OidQueryInformation(OID_802_11_BSSID, s, "ra0", &BssidQuery, sizeof(BssidQuery));
	}

	//step 3
	if (OidSetInformation(OID_802_11_BSSID_LIST_SCAN, s, "ra0", 0, 0) < 0) {
		websError(wp, 500, "Set OID_802_11_BSSID_LIST_SCAN failed!");
		free(pBssidList); 
		close(s);
		return -1;
	}
	// wait a few seconds to get all AP.
	Sleep(2);

	/*
	for (retry = 0; retry < 5; retry++) {
		ret = OidQueryInformation(OID_802_11_BSSID_LIST, s, "ra0", pBssidList, lBufLen);
		if (ret < 0) {
			if (retry < 4) {
				Sleep(1);
				continue;
			}
			websError(wp, 500, "Query OID_802_11_BSSID_LIST error! return=%d", ret);
			free(pBssidList);
			pBssidList = NULL;
			close(s);
			return -1;
		}
		break;
	}
	*/
	ret = OidQueryInformation(RT_OID_WE_VERSION_COMPILED, s, "ra0", &we_version, sizeof(we_version));
	if (ret< 0)
	{
		websError(wp, 500, "Query RT_OID_WE_VERSION_COMPILED error! return=%d", ret);
		free(pBssidList);
		close(s);
		return -1;
	}
	if(we_version >= 17)
		lBufLen=8192;
	else
		lBufLen=4096;

	ret=-1;
	retry=1;
	EAGAIN_Count=0;

	while(ret < 0)
	{
		memset(pBssidList, 0x00, sizeof(char)*65536);
		ret = OidQueryInformation(OID_802_11_BSSID_LIST, s, "ra0", pBssidList, lBufLen);
		//int retry=0;
#if 1
		if (errno == EAGAIN)
		{
			sleep(1);
			fprintf(stderr, "errno == EAGAIN\n");
			EAGAIN_Count++;
			if(EAGAIN_Count>25)
			{
				websError(wp, 500, "Query OID_802_11_BSSID_LIST error! errno == EAGAIN");
				free(pBssidList);
				close(s);
				return -1;
			}
			else
				continue;
		}
		else if (errno == E2BIG)
		{
			fprintf(stderr, "errno == E2BIG\n");
			lBufLen = lBufLen + 4096*retry;
			if (lBufLen < 65536)
			{
				retry++;
				fprintf(stderr,"lBufLen=%d\n",lBufLen);
				continue;
			}
			else
			{
				websError(wp, 500, "Query OID_802_11_BSSID_LIST error! E2BIG");
				free(pBssidList);
				close(s);
				return -1;
			}
		}
		else if( ret != 0 )
		{
			websError(wp, 500, "Query OID_802_11_BSSID_LIST error! return=%d", ret);
			free(pBssidList);
			close(s);
			return -1;
		}
#else
		switch(errno)
		{
		case EAGAIN:
			do
			{
				sleep(1);
				ret = OidQueryInformation(OID_802_11_BSSID_LIST, s, "ra0", pBssidList, lBufLen);
				retry++;
			}while((errno == EAGAIN) && (ret <0) && (retry<5));
			//while((errno == EAGAIN) && (ret <0) && (retry<25));
			break;

		case E2BIG:
			{
				retry=1;
				do
				{
					lBufLen = lBufLen + 4096*retry;
					free(pBssidList);
					pBssidList = (PNDIS_802_11_BSSID_LIST_EX) malloc(lBufLen);
					ret = OidQueryInformation(OID_802_11_BSSID_LIST, s, "ra0", pBssidList, lBufLen);
					retry++;
				}while((errno == E2BIG) && (ret <0) && (retry<10));
			}
		default:
			if (ret < 0)
			{
				fprintf(stderr, "Query OID_802_11_BSSID_LIST error! return=%d\n", ret);
				websError(wp, 500, "Query OID_802_11_BSSID_LIST error! return=%d", ret);
				close(s);
				return;
			}
			else
				break;
		}
#endif
	}

	if ( pBssidList->NumberOfItems == 0)
	{
		fprintf(stderr, "Bssid List number is 0!\n");
		free(pBssidList);
		close(s);
		return -1;
	}
	else
	{
	unsigned char tmpRadio[188], tmpBSSIDII[16], tmpBSSID[28], tmpSSID[64+NDIS_802_11_LENGTH_SSID], tmpRSSI[16], tmpChannel[16], tmpAuth[32], tmpEncry[20], tmpImg[40];
		unsigned char tmpSSIDII[NDIS_802_11_LENGTH_SSID+1];
	unsigned char version = 0x0, wsc_state=0, ap_setup_lock=0, selected_registrar=0, uuid_e[16], rf_band=0;
	unsigned short device_password_id=0, selected_registrar_config_method=0, primary_device_type_category=0, primary_device_type_subcategory=0;
	unsigned int IEFlags = 0;
	unsigned char extend[1024];

	int i=0, j=0;
	unsigned int nSigQua;
	int nChannel = 1;
	unsigned char radiocheck[8];
	int wps_ap_flag;
	
	pBssid = (PNDIS_WLAN_BSSID_EX) pBssidList->Bssid;
	for (i = 0; i < pBssidList->NumberOfItems; i++)
	{
		memset(radiocheck, 0x00, sizeof(radiocheck));
		memset(tmpRadio, 0x00, sizeof(tmpRadio));
		memset(tmpBSSID, 0x00, sizeof(tmpBSSID));
		memset(tmpRSSI, 0x00, sizeof(tmpRSSI));
		memset(tmpSSID, 0x00, sizeof(tmpSSID));
		memset(tmpChannel, 0x00, sizeof(tmpChannel));
		memset(tmpAuth, 0x00, sizeof(tmpAuth));
		memset(tmpEncry, 0x00, sizeof(tmpEncry));
		memset(tmpBSSIDII, 0x00, sizeof(tmpBSSIDII));
		memset(tmpImg, 0x00, sizeof(tmpImg));
		memset(tmpSSIDII, 0x00, sizeof(tmpSSIDII));
		memset(extend, 0x00, sizeof(extend));
		wps_ap_flag = 0;

		if (strcmp((char *)pBssid->Ssid.Ssid, "") == 0)
			sprintf((char *)tmpSSID, "%s", "&nbsp;");
		else {
			int i = 0;
			do {
				if (pBssid->Ssid.Ssid[i] < 32 || pBssid->Ssid.Ssid[i] > 126 || pBssid->Ssid.Ssid[i] == 13) // 13 is string end of Dos
				{
					gstrncpy(tmpSSID, "&nbsp;", sizeof(tmpSSID));
					break;
				}else if(pBssid->Ssid.Ssid[i] == '\\'){
					snprintf(tmpSSID, sizeof(tmpSSID), "%s%s", tmpSSID, "\\\\");
				}else
					snprintf(tmpSSID, sizeof(tmpSSID), "%s%c", tmpSSID, pBssid->Ssid.Ssid[i]);
				i++;
			} while(i < pBssid->Ssid.SsidLength);
		}

		sprintf((char *)tmpBSSID, "<td>%02X-%02X-%02X-%02X-%02X-%02X</td>", pBssid->MacAddress[0], pBssid->MacAddress[1], pBssid->MacAddress[2], pBssid->MacAddress[3], pBssid->MacAddress[4], pBssid->MacAddress[5]);
		sprintf((char *)tmpBSSIDII, "%02X%02X%02X%02X%02X%02X",	pBssid->MacAddress[0], pBssid->MacAddress[1], pBssid->MacAddress[2], pBssid->MacAddress[3], pBssid->MacAddress[4], pBssid->MacAddress[5]);

		nSigQua = ConvertRssiToSignalQuality(pBssid->Rssi);

		sprintf((char *)tmpRSSI,"%d", nSigQua);
		nChannel = -1;	
		for(j = 0; j < G_nChanFreqCount; j++){
			if (pBssid->Configuration.DSConfig == ChannelFreqTable[j].lFreq) {
				nChannel = ChannelFreqTable[j].lChannel;
				break;
			}
		}

		if (nChannel == -1)
			continue;

		sprintf((char *)tmpChannel, "%u", nChannel);
		
		// work with NDIS_WLAN_BSSID_EX
		unsigned char bTKIP = FALSE;
		unsigned char bAESWRAP = FALSE;
		unsigned char bAESCCMP = FALSE;
		unsigned char bWPA = FALSE;
		unsigned char bWPAPSK = FALSE;
		unsigned char bWPANONE = FALSE;
		unsigned char bWPA2 = FALSE;
		unsigned char bWPA2PSK = FALSE;
		unsigned char bWPA2NONE = FALSE;
		unsigned char bCCKM = FALSE; // CCKM for Cisco, add by candy 2006.11.24

		if ((pBssid->Length > sizeof(NDIS_WLAN_BSSID)) && (pBssid->IELength > sizeof(NDIS_802_11_FIXED_IEs)))
		{
			unsigned int lIELoc = 0;
			PNDIS_802_11_FIXED_IEs pFixIE = (PNDIS_802_11_FIXED_IEs)pBssid->IEs;
			PNDIS_802_11_VARIABLE_IEs pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pFixIE + sizeof(NDIS_802_11_FIXED_IEs));
			lIELoc += sizeof(NDIS_802_11_FIXED_IEs);

			while (pBssid->IELength > (lIELoc + sizeof(NDIS_802_11_VARIABLE_IEs)))
			{
				/* WPS section  WPS OUI: 00 50 F2 04 , little endian form */
				if ((pVarIE->ElementID == 221) && *((unsigned int*)((char*)pVarIE->data)) == 0x04f25000)
				{
					unsigned char *pos, *data_head;
					unsigned short len_in_hdr;
					unsigned char tot_len =  pVarIE->Length;

					data_head = pos = (unsigned char*)pVarIE->data + 4;

					version = 0x0;
					wsc_state = 0;
					ap_setup_lock = 0xff;
					selected_registrar = 0xff;
					memset(uuid_e, 16, 0);
					rf_band = 0xff;
					device_password_id = 0xffff;
					selected_registrar_config_method = 0xffff;
					IEFlags = 0;
					
					/* TLV:  T=2bytes, L=2bytes, V=... */
					while(pos < data_head + tot_len - 4)
					{
						len_in_hdr = ntohs(*(unsigned short *)(pos+2));

						if(tot_len - (pos - data_head) < len_in_hdr){
							printf("Error: crafted WSC packet? tot_len = %d, len_in_hdr = %d\n", tot_len, len_in_hdr);
							break;
						}
									
#define CASE(x)	case x:\
				if(len_in_hdr != x##_LEN){ printf("Error: crafted WSC packet? %s.\n", #x); goto WSC_FAILED;}  \
				IEFlags |= x##_BEACON;
#define TESTBIT(x)	(x##_BEACON & IEFlags)

						switch( ntohs(*(unsigned short *)pos)){
							CASE( WSC_ID_VERSION )
								version = *(unsigned char *)(pos + 2 + 2);
								break;

							CASE( WSC_ID_SC_STATE )
								wsc_state = *(unsigned char *)(pos + 2 + 2);
								break;
							
							CASE( WSC_ID_AP_SETUP_LOCKED )
								ap_setup_lock = *(unsigned char *)(pos + 2 + 2);
								break;

							CASE( WSC_ID_SEL_REGISTRAR )
								selected_registrar = *(unsigned char *)(pos + 2 + 2);
								break;

							CASE( WSC_ID_DEVICE_PWD_ID )
								device_password_id = ntohs(*(unsigned short *)(pos + 2 + 2));
								break;

							CASE( WSC_ID_SEL_REG_CFG_METHODS )
								selected_registrar_config_method = ntohs(*(unsigned short *)(pos + 2 + 2));
								break;

							CASE( WSC_ID_UUID_E )
								memcpy(uuid_e, (unsigned char *)pos + 2 + 2, 16);
								break;

							CASE( WSC_ID_RF_BAND )
								rf_band = *(unsigned char *)(pos + 2 + 2);
								break;

							CASE( WSC_ID_PRIMARY_DEVICE_TYPE )
								primary_device_type_category =  *(unsigned short *)(pos + 2 + 2);
								primary_device_type_subcategory =  *(unsigned short *)(pos + 2 + 2 + 2 + 2);
							default:
								//printf("unknown tlv:%04x\n", ntohs(*(unsigned short *)pos));
								break;

						}
						pos = pos+2+2+ len_in_hdr;
					}

					if( ! TESTBIT(WSC_ID_VERSION) || ! TESTBIT( WSC_ID_SC_STATE)){
						printf("No Version and WPS setup state\n.");
						break;
					}
					
					wps_ap_flag = 1;
				}
WSC_FAILED:

				if ((pVarIE->ElementID == 221) && (pVarIE->Length >= 16))
				{
					//UINT* pOUI = (UINT*)((char*)pVarIE + 2);
					unsigned int* pOUI = (unsigned int*)((char*)pVarIE->data);
					//fprintf(stderr, "pOUI=0x%08x\n", pOUI);
					if (*pOUI != WPA_OUI_TYPE)
					{
						lIELoc += pVarIE->Length;
						lIELoc += 2;
						pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

						if(pVarIE->Length <= 0)
							break;

						continue;
					}

					unsigned int* plGroupKey; 
					unsigned short* pdPairKeyCount;
					unsigned int* plPairwiseKey=NULL;
					unsigned int* plAuthenKey=NULL;
					unsigned short* pdAuthenKeyCount;
					plGroupKey = (unsigned int*)((char*)pVarIE + 8);
			
					unsigned int lGroupKey = *plGroupKey & 0x00ffffff;
					//fprintf(stderr, "lGroupKey=%d\n", lGroupKey);
					if (lGroupKey == WPA_OUI) {
						lGroupKey = (*plGroupKey & 0xff000000) >> 0x18;
						if (lGroupKey == 2)
							bTKIP = TRUE;
						else if (lGroupKey == 3)
							bAESWRAP = TRUE;
						else if (lGroupKey == 4)
							bAESCCMP = TRUE;
					}
					else{
						lIELoc += pVarIE->Length;
						lIELoc += 2;
						pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

						if(pVarIE->Length <= 0)
							break;
							
						continue;
					}
			
					pdPairKeyCount = (unsigned short*)((char*)plGroupKey + 4);
					plPairwiseKey = (unsigned int*) ((char*)pdPairKeyCount + 2);
					unsigned short k = 0;
					for (k = 0; k < *pdPairKeyCount; k++) {
						unsigned int lPairKey = *plPairwiseKey & 0x00ffffff;
						if (lPairKey == WPA_OUI )//|| (lPairKey & 0xffffff00) == WPA_OUI_1)
						{
							lPairKey = (*plPairwiseKey & 0xff000000) >> 0x18;
							if (lPairKey == 2)
								bTKIP = TRUE;
							else if (lPairKey == 3)
								bAESWRAP = TRUE;
							else if (lPairKey == 4)
								bAESCCMP = TRUE;
						}
						else
							break;
						
						plPairwiseKey++;
					}
			
					pdAuthenKeyCount = (unsigned short*)((char*)pdPairKeyCount + 2 + 4 * (*pdPairKeyCount));
					plAuthenKey = (unsigned int*)((char*)pdAuthenKeyCount + 2);

					for(k = 0; k < *pdAuthenKeyCount; k++)
					{
						unsigned int lAuthenKey = *plAuthenKey & 0x00ffffff;
						if (lAuthenKey == CISCO_OUI) {
							bCCKM = TRUE; // CCKM for Cisco
						}
						else if (lAuthenKey == WPA_OUI) {
							lAuthenKey = (*plAuthenKey & 0xff000000) >> 0x18;

							if (lAuthenKey == 1)
								bWPA = TRUE;
							else if (lAuthenKey == 0 || lAuthenKey == 2) {
								if (pBssid->InfrastructureMode)
									bWPAPSK = TRUE;
								else
									bWPANONE = TRUE;
							}
						}					
						plAuthenKey++;
					}
				//break;
				}
				else if (pVarIE->ElementID == 48 && pVarIE->Length >= 12)
				{
					unsigned int* plGroupKey; 
					unsigned int* plPairwiseKey; 
					unsigned short* pdPairKeyCount;
					unsigned int* plAuthenKey; 
					unsigned short* pdAuthenKeyCount;
					plGroupKey = (unsigned int*)((char*)pVarIE + 4);

					unsigned int lGroupKey = *plGroupKey & 0x00ffffff;
					if (lGroupKey == WPA2_OUI) {
						lGroupKey = (*plGroupKey & 0xff000000) >> 0x18;
						if (lGroupKey == 2)
							bTKIP = TRUE;
						else if (lGroupKey == 3)
							bAESWRAP = TRUE;
						else if (lGroupKey == 4)
							bAESCCMP = TRUE;
					}
					else{
						lIELoc += pVarIE->Length;
						lIELoc += 2;
						pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

						if(pVarIE->Length <= 0)
							break;
							
						continue;
					}

					pdPairKeyCount = (unsigned short*)((char*)plGroupKey + 4);
					plPairwiseKey = (unsigned int*)((char*)pdPairKeyCount + 2);
					unsigned short k = 0;

					for (k = 0; k < *pdPairKeyCount; k++)
					{
						unsigned int lPairKey = *plPairwiseKey & 0x00ffffff;
						if (lPairKey == WPA2_OUI) {
							lPairKey = (*plPairwiseKey & 0xff000000) >> 0x18;
							if (lPairKey == 2)
								bTKIP = TRUE;
							else if (lPairKey == 3)
								bAESWRAP = TRUE;
							else if (lPairKey == 4)
								bAESCCMP = TRUE;
						}
						else
							break;
						plPairwiseKey++;
					}
		
					pdAuthenKeyCount = (unsigned short*)((char*)pdPairKeyCount + 2 + 4 * *pdPairKeyCount);
					plAuthenKey = (unsigned int*)((char*)pdAuthenKeyCount + 2);
					for (k = 0; k < *pdAuthenKeyCount; k++)
					{
						unsigned int lAuthenKey = *plAuthenKey & 0x00ffffff;
						if (lAuthenKey == CISCO_OUI) {
							bCCKM = TRUE; // CCKM for Cisco
						}
						else if (lAuthenKey == WPA2_OUI) {
							lAuthenKey = (*plAuthenKey & 0xff000000) >> 0x18;
							if (lAuthenKey == 1)
								bWPA2 = TRUE;
							else if (lAuthenKey == 0 || lAuthenKey == 2) {
								if (pBssid->InfrastructureMode)
									bWPA2PSK = TRUE;
								else
									bWPA2NONE = TRUE;
							}
						}					
						plAuthenKey++;
					}
				}
		
				lIELoc += pVarIE->Length;
				lIELoc += 2;
				pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

				if (pVarIE->Length <= 0)
					break;
			}
		}
		
		char strAuth[32], strEncry[32];
		memset( strAuth, 0x00, sizeof(strAuth) );
		memset( strEncry, 0x00, sizeof(strEncry) );
		if (bCCKM)
			strcpy(strAuth, "CCKM; ");
		if (bWPA)
			strcpy(strAuth, "WPA; ");
		if (bWPAPSK)
			strcat(strAuth, "WPA-PSK; ");
		if (bWPANONE)
			strcat(strAuth, "WPA-NONE; ");
		if (bWPA2)
			strcat(strAuth, "WPA2; ");
		if (bWPA2PSK)
			strcat(strAuth, "WPA2-PSK; ");
		if (bWPA2NONE)
			strcat(strAuth, "WPA2-NONE; ");
		
		if (strlen(strAuth) > 0) {
			strncpy((char *)tmpAuth, strAuth, strlen(strAuth) - 2);
			strcpy(strAuth, (char *)tmpAuth);
		}else {
			strcpy((char *)strAuth, "Unknown");
		}

		if (bTKIP)
			strcpy(strEncry, "TKIP; ");
		if (bAESWRAP || bAESCCMP)
			strcat(strEncry, "AES; ");

		if (strlen(strEncry) > 0) {
			strncpy((char *)tmpEncry, strEncry, strlen(strEncry) - 2);
			strcpy(strEncry, (char *)tmpEncry);
		}else {
			if (pBssid->Privacy)  // privacy value is on/of
				strcpy(strEncry, "WEP");
			else {
				strcpy(strEncry, "Not Use");
				strcpy(strAuth, "OPEN");
			}
		}

		sprintf((char *)tmpAuth, "%s", strAuth);
		sprintf((char *)tmpEncry, "%s", strEncry);

		strcpy((char *)tmpSSIDII, pBssid->Ssid.Ssid);

//		ConverterStringToDisplay((char *)tmpSSIDII);
//		if (strlen(G_SSID.Ssid)>0 && strcmp(pBssid->Ssid.Ssid, G_SSID.Ssid) == 0)
//			strcpy(radiocheck, "checked");
//		else
//			strcpy(radiocheck, "");

//		sprintf((char *)tmpRadio, "<td><input type=radio name=selectedSSID %s onClick=\"selectedSSIDChange('%s','%s',%d,%d,'%s','%s')\"></td>", radiocheck, tmpSSIDII, tmpBSSIDII, pBssid->InfrastructureMode, nChannel, strEncry, strAuth);
//		websWrite(wp, "<tr> %s %s %s %s %s %s %s </tr>\n", tmpRadio, tmpSSID, tmpBSSID, tmpRSSI, tmpChannel, tmpEncry, tmpAuth);

		/*
		 *  "extend" will become a literal part of Javascript, so we use "\\n" instead of "\n".
		 */
		if( TESTBIT( WSC_ID_SEL_REG_CFG_METHODS)){
			strncat(extend, "Config Method:", sizeof(extend));
			snprintf(extend, sizeof(extend), "%s%s\\n", extend, ConfigMethodTranslate(selected_registrar_config_method));
		}

		if( TESTBIT( WSC_ID_DEVICE_PWD_ID)){
			strncat(extend, "Device Password:", sizeof(extend));
			snprintf(extend, sizeof(extend), "%s%s\\n", extend, DevicePasswordIDTranslate(device_password_id));
		}

		if( TESTBIT( WSC_ID_SEL_REGISTRAR)){
			strncat(extend, "Seleted Registrar:", sizeof(extend));
			snprintf(extend, sizeof(extend), "%s%d\\n", extend, selected_registrar);
		}

		if( TESTBIT( WSC_ID_AP_SETUP_LOCKED)){
			strncat(extend, "AP Locked:", sizeof(extend));
			snprintf(extend, sizeof(extend), "%s%d\\n", extend, ap_setup_lock);
		}

		if( TESTBIT( WSC_ID_UUID_E)){
			int pos;
			char uuid_str[WSC_ID_UUID_E_LEN * 2 + 1];
			memset(uuid_str, 0, WSC_ID_UUID_E_LEN * 2 + 1);
			strncat(extend, "UUID:", sizeof(extend));
			for(pos=0; pos< WSC_ID_UUID_E_LEN; pos++){
				snprintf(uuid_str, WSC_ID_UUID_E_LEN * 2 + 1, "%s%02x", uuid_str, uuid_e[pos]);
			}
			snprintf(extend, sizeof(extend), "%s%s\\n", extend, uuid_str);
		}

		if( TESTBIT( WSC_ID_RF_BAND)){
			strncat(extend, "RF Band:", sizeof(extend));
			snprintf(extend, sizeof(extend), "%s%s\\n", extend, RFBandTranslate(rf_band));
		}

		if( TESTBIT( WSC_ID_PRIMARY_DEVICE_TYPE )){
			strncat(extend, "Primary Device Type:", sizeof(extend));
			snprintf(extend, sizeof(extend), "%s%s", extend, PrimaryDeviceTypeTranslate(primary_device_type_category, primary_device_type_subcategory));
		}

		/* This space is important for seperating from two possible closed '\n' characters. */
		strncat(extend, " ", sizeof(extend));

		/*
		 * TODO:  test wps_ap_flag early to save time.
		 */ 
		if(wps_ap_flag){
			printf("%s,%s,%s,%s,%s,%s,%d,%d,%s\n", tmpSSID, tmpBSSIDII, tmpRSSI, tmpChannel, tmpAuth, tmpEncry, version, wsc_state, extend);
			snprintf(g_pAPListData, SITE_SURVEY_APS_MAX, "%s%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%d\\n%d\\n%s\\n\\n", g_pAPListData, tmpSSID, tmpBSSIDII, tmpRSSI, tmpChannel, tmpAuth, tmpEncry, version, wsc_state, extend);
		}

		pBssid = (PNDIS_WLAN_BSSID_EX)((char *)pBssid + pBssid->Length);
	}
	}
	free(pBssidList);
	close(s);

	websLongWrite(wp, g_pAPListData);
	return 0;
}

static char *getSTAEnrolleePIN(void *result)
{
	unsigned int pin;
	char *str;
	char long_buf[4096];
	FILE *fp;
	memset(long_buf, 0, 4096);
	if(!(fp = popen("iwpriv ra0 stat", "r")))
		return NULL;
	fread(long_buf, 1, 4096, fp);
	pclose(fp);

	if(!(str = strstr(long_buf, "RT2860 Linux STA PinCode")))
		return NULL;

	str = str + strlen("RT2860 Linux STA PinCode");
	pin = atoi(str);
	sprintf(result, "%08d", pin);
	return result;	
}

static void WPSSTAPINStartEnr(char *ssid)
{
	resetTimerAll();
	setTimer(WPS_STA_CATCH_CONFIGURED_TIMER * 1000, WPSSTAEnrolleeTimerHandler);	

	if(!isSafeForShell(ssid))
		return;

	doSystem("iwpriv ra0 wsc_cred_count 0");			// reset  creditial count
	doSystem("iwpriv ra0 wsc_conf_mode 1");				// Enrollee
	doSystem("iwpriv ra0 wsc_mode 1");					// PIN
	doSystem("iwpriv ra0 wsc_ssid %s\n", ssid);
	doSystem("iwpriv ra0 wsc_start");
}

void WPSSTAPBCStartEnr(void)
{
	resetTimerAll();
	setTimer(WPS_STA_CATCH_CONFIGURED_TIMER * 1000, WPSSTAEnrolleeTimerHandler);	

	doSystem("iwpriv ra0 wsc_cred_count 0");			// reset  creditial count
	doSystem("iwpriv ra0 wsc_conf_mode 1");				// Enrollee
	doSystem("iwpriv ra0 wsc_mode 2");					// PBC
	doSystem("iwpriv ra0 wsc_start");
}

static void WPSSTAPINStartReg(char *ssid, char *pin)
{
	char *wsc_cred_ssid, *wsc_cred_key;
	const char *wsc_cred_auth, *wsc_cred_encr, *wsc_cred_keyIdx;
	
	wsc_cred_ssid = (char *) nvram_bufget(RT2860_NVRAM, "staRegSSID");
	wsc_cred_auth = nvram_bufget(RT2860_NVRAM, "staRegAuth");
	wsc_cred_encr = nvram_bufget(RT2860_NVRAM, "staRegEncry");
	wsc_cred_keyIdx = nvram_bufget(RT2860_NVRAM, "staRegKeyIndex");
	wsc_cred_key = (char *) nvram_bufget(RT2860_NVRAM, "staRegKey");
	// The strange driver has no wep key type here

	resetTimerAll();
	setTimer(WPS_STA_CATCH_CONFIGURED_TIMER * 1000, WPSSTARegistrarTimerHandler);

	if(!isSafeForShell(wsc_cred_ssid) || !isSafeForShell(wsc_cred_key) || !isSafeForShell(pin) || !isSafeForShell(ssid))
		return ;

	doSystem("iwpriv ra0 wsc_cred_ssid \"0 %s\"", wsc_cred_ssid);
	doSystem("iwpriv ra0 wsc_cred_auth \"0 %s\"", wsc_cred_auth);
	doSystem("iwpriv ra0 wsc_cred_encr \"0 %s\"", wsc_cred_encr);
	doSystem("iwpriv ra0 wsc_cred_keyIdx \"0 %s\"", wsc_cred_keyIdx);
	doSystem("iwpriv ra0 wsc_cred_key \"0 %s\"", wsc_cred_key);
	doSystem("iwpriv ra0 wsc_cred_count 1");

	doSystem("iwpriv ra0 wsc_conn_by_idx 0");
	doSystem("iwpriv ra0 wsc_auto_conn 2");
	doSystem("iwpriv ra0 wsc_conf_mode 2");			// We are the Registrar.
	doSystem("iwpriv ra0 wsc_mode 1");				// PIN
	doSystem("iwpriv ra0 wsc_pin %s", pin);
	doSystem("iwpriv ra0 wsc_ssid \"%s\"", ssid);	
	doSystem("iwpriv ra0 wsc_start");
}

void WPSSTAPBCStartReg(void)
{
	char *wsc_cred_ssid, *wsc_cred_key;
	const char *wsc_cred_auth, *wsc_cred_encr, *wsc_cred_keyIdx;

	wsc_cred_ssid = (char *) nvram_bufget(RT2860_NVRAM, "staRegSSID");
	wsc_cred_auth = nvram_bufget(RT2860_NVRAM, "staRegAuth");
	wsc_cred_encr = nvram_bufget(RT2860_NVRAM, "staRegEncry");
	wsc_cred_keyIdx = nvram_bufget(RT2860_NVRAM, "staRegKeyIndex");
	wsc_cred_key = (char *) nvram_bufget(RT2860_NVRAM, "staRegKey");
	// The strange driver has no wep key type here

	if(!isSafeForShell(wsc_cred_ssid) || !isSafeForShell(wsc_cred_key))
		return ;

	resetTimerAll();
	setTimer(WPS_STA_CATCH_CONFIGURED_TIMER * 1000, WPSSTARegistrarTimerHandler);

	doSystem("iwpriv ra0 wsc_cred_ssid \"0 %s\"", wsc_cred_ssid);
	doSystem("iwpriv ra0 wsc_cred_auth \"0 %s\"", wsc_cred_auth);
	doSystem("iwpriv ra0 wsc_cred_encr \"0 %s\"", wsc_cred_encr);
	doSystem("iwpriv ra0 wsc_cred_keyIdx \"0 %s\"", wsc_cred_keyIdx);
	doSystem("iwpriv ra0 wsc_cred_key \"0 %s\"", wsc_cred_key);
	doSystem("iwpriv ra0 wsc_cred_count 1");

	doSystem("iwpriv ra0 wsc_conn_by_idx 0");
	doSystem("iwpriv ra0 wsc_auto_conn 2");
	doSystem("iwpriv ra0 wsc_conf_mode 2");			// Registrar.
	doSystem("iwpriv ra0 wsc_mode 2");
	doSystem("iwpriv ra0 wsc_start");
}


/*
 * TODO:
 * All I know is to see if creditial is set or not...
 */
static int isWPSSuccess(void)
{
	char *str;
	char long_buf[4096];
	FILE *fp;
	memset(long_buf, 0, 4096);
	if(!(fp = popen("iwpriv ra0 stat", "r")))
		return 0;
	fread(long_buf, 1, 4096, fp);
	pclose(fp);

	if(!(str = strstr(long_buf, "WPS Profile Count")))
		return 0;
	if(!(str = strchr(str, '=')))
		return 0;
	if(atoi(str+1))
		return 1;
	return 0;
}

void WPSSTARegistrarTimerHandler(int signo)
{
	WPSSTAEnrolleeTimerHandler(REGISTRAR_TIMER_MODE);
}

void WPSSTAEnrolleeTimerHandler(int signo)
{
	int status;
	char interface[] = "ra0";
	static int wsc_timeout_counter = 0;
	status =  getWscStatus(interface);

	if( (status == 3 || status == 35) && g_wps_timer_state == 0){	// 3 == "Start WSC Process",  35 == "SCAN_AP"
		printf("goahead: Start to monitor WSC Status...\n");
		g_wps_timer_state = 1;
		wsc_timeout_counter = 0;
	}

	if(g_wps_timer_state == 1){
//		printf("%s\n", getWscStatusStr(status));

		/* check if timeout is happened */
		wsc_timeout_counter += WPS_STA_CATCH_CONFIGURED_TIMER;
		if(wsc_timeout_counter > WPS_STA_TIMEOUT_SECS){				// 110 second
			wsc_timeout_counter = 0;
			resetTimerAll();

			doSystem("iwpriv %s wsc_stop", interface);
			trace(0, T("-- WSC failed, timeout\n"));
			return;
		}

		switch(status){
			case 34 /* WSC Configured */ :
				wsc_timeout_counter = 0;
				resetTimerAll();

				/*
				 * WPS STA Enrollee mode is strange here that driver still
				 * acknowledge us the success of WPS procedure even failed actually, so we use isWPSSuceess() to
				 * get truth. 
				 */
				if(isWPSSuccess() || signo == REGISTRAR_TIMER_MODE){
					WSC_PROFILE wsc_profile;
					trace(0, T("++ WSC success\n"));

					/*
					 * For WiFi STA WPS test plan case 5.1.1.
					 *
					 * We use ioctl(WSC_QUERY_PROFILE) to get possible multiple credentials,
					 * and the addWPSSTAProfile() should be replaced with new addWPSSTAProfile2() in the future.
					 */
					if( getWscProfile("ra0", &wsc_profile) != -1){
//						if(wsc_profile.ProfileCnt != 1){
							int i;
							fprintf(stderr, "%u credentials found.\n", wsc_profile.ProfileCnt);
							for(i=0; i< wsc_profile.ProfileCnt; i++)
								addWPSSTAProfile2(&wsc_profile.Profile[i]);
//						}else
//							addWPSSTAProfile(NULL);													
					}else{
						// add current link to station profile
						printf("Warning, can't get wsc profile!\n");
						addWPSSTAProfile(NULL);
					}
				}else{
					doSystem("iwpriv %s wsc_stop", interface);
				}

				return;
		/*	
			case 2: // WSC failed 
				wsc_timeout_counter = 0;
				resetTimerAll();
				doSystem("iwpriv %s wsc_stop", interface);
				trace(0, T("-- WSC failed, PIN incorrect\n"));
				break;
		*/
		}
	}
}

/*
 *  The Browser will pull STA WPS info. from this function.
 */
static void updateWPSStaStatus(webs_t wp, char_t *path, char_t *query)
{
	char interface[] = "ra0";
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("%s"), getWscStatusStr(getWscStatus(interface)));
    websDone(wp, 200);
	return;
}

/*
 */
static void WPSSTAStop(webs_t wp, char_t *path, char_t *query)
{
	char interface[] = "ra0";

	resetTimerAll();
	doSystem("iwpriv ra0 wsc_stop");

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("%s"), getWscStatusStr(getWscStatus(interface)));
    websDone(wp, 200);	
	return;
}

static void WPSSTAGenNewPIN(webs_t wp, char_t *path, char_t *query)
{
	char pin[16];
	doSystem("iwpriv ra0 wsc_gen_pincode");
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	if(!getSTAEnrolleePIN(pin))
		websWrite(wp, T("error"));
	else
		websWrite(wp, T("%s"), pin);
    websDone(wp, 200);
}

/*
 *  STA Enrollee
 *  The Browser would trigger PIN by this function.
 */
static void WPSSTAPINEnr(webs_t wp, char_t *path, char_t *query)
{
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

	printf("Query = %s\n", query);
	WPSSTAPINStartEnr(query);
	websWrite(wp, T("Enrollee PIN..."));
    websDone(wp, 200);	
}

/*
 *  STA Enrollee
 *  The Browser would trigger PBC by this function.
 */
static void WPSSTAPBCEnr(webs_t wp, char_t *path, char_t *query)
{
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

	printf("Query = %s\n", query);
	WPSSTAPBCStartEnr();
	websWrite(wp, T("Enrollee PBC..."));
    websDone(wp, 200);	
}
 
/*
 *  STA Registrar
 *  The Browser would trigger PIN by this function.
 */
static void WPSSTAPINReg(webs_t wp, char_t *path, char_t *query)
{
	int pin_int;
	char ssid[33], pin[16];
	char_t *sp;
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

	if(!query)
		return;
	if(!( pin_int = atoi(query)))
		return;
	if(!(sp = strchr(query, ' '))){
		return;
	}
	snprintf(pin, 15, "%d", pin_int);
	strncpy(ssid, sp+1, 32);
	printf("Query pin = %s\n", pin);
	printf("Query ssid = %s\n", ssid);
	WPSSTAPINStartReg(ssid, pin);
	websWrite(wp, T("Registrar PIN..."));
    websDone(wp, 200);	
}

/*
 *  STA Registrar
 *  The Browser would trigger PBC by this function.
 */
static void WPSSTAPBCReg(webs_t wp, char_t *path, char_t *query)
{
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

	printf("Query = %s\n", query);
	WPSSTAPBCStartReg();
	websWrite(wp, T("Registrar PBC..."));
    websDone(wp, 200);	
}


static void WPSSTARegistrarSetupSSID(webs_t wp, char_t *path, char_t *query)
{
	nvram_bufset(RT2860_NVRAM, "staRegSSID", query);
	nvram_commit(RT2860_NVRAM);
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("WPS STA Registrar settings: SSID done\n"));
	websDone(wp, 200);

}

static void WPSSTARegistrarSetupKey(webs_t wp, char_t *path, char_t *query)
{
	nvram_bufset(RT2860_NVRAM, "staRegKey", query);
	nvram_commit(RT2860_NVRAM);
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("WPS STA Registrar settings: Key done\n"));
	websDone(wp, 200);
}


static void WPSSTARegistrarSetupRest(webs_t wp, char_t *path, char_t *query)
{
	char_t auth[32], encrypt[32], keytype[2], keyindex[2];
	printf("query = %s\n", query);
	sscanf(query, "%32s %32s %2s %2s", auth, encrypt, keytype, keyindex);
	printf("auth = %s\n", auth);
	nvram_bufset(RT2860_NVRAM, "staRegAuth", auth);
	nvram_bufset(RT2860_NVRAM, "staRegEncry", encrypt);
	nvram_bufset(RT2860_NVRAM, "staRegKeyType", keytype);
	nvram_bufset(RT2860_NVRAM, "staRegKeyIndex", keyindex);
	nvram_commit(RT2860_NVRAM);
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("WPS STA Registrar settings: rest all done\n"));
	websDone(wp, 200);
}

static void WPSSTAMode(webs_t wp, char_t *path, char_t *query)
{
	if(!gstrcmp(query, "0")) {
		if (strcmp(nvram_bufget(RT2860_NVRAM, "staWPSMode"), "1") == 0) {
			unsigned char mac[6];
			char defaultSSID[33];
			memcpy(mac, getStaMacAddr(), 6);
			snprintf(defaultSSID, 32, "STARegistrar%02X%02X%02X", mac[3], mac[4], mac[5]);
			nvram_bufset(RT2860_NVRAM, "staRegSSID", defaultSSID);
			nvram_bufset(RT2860_NVRAM, "staRegAuth", "WPAPSK");
			nvram_bufset(RT2860_NVRAM, "staRegEncry", "TKIP");
			nvram_bufset(RT2860_NVRAM, "staRegKeyType", "1");
			nvram_bufset(RT2860_NVRAM, "staRegKeyIndex", "1");
			nvram_bufset(RT2860_NVRAM, "staRegKey", "12345678");
		}
		nvram_bufset(RT2860_NVRAM, "staWPSMode", "0");
	} else if(!gstrcmp(query, "1")) {
		nvram_bufset(RT2860_NVRAM, "staWPSMode", "1");
	} else
		return;
	nvram_commit(RT2860_NVRAM);
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("WPS STA mode setting done\n"));
	websDone(wp, 200);
}

/*
 * TODO:   now this function always returns 0.(AUTO)
 */
static int getStaChannel(char *interface)
{
	unsigned int ConnectStatus = 0;
	NDIS_802_11_CONFIGURATION Configuration;
//	RT_802_11_LINK_STATUS     LinkStatus;
	int s, ret, i;
	int nChannel = -1;
//	int Japan_channel = 200;

	s = socket(AF_INET, SOCK_DGRAM, 0);

	ret = OidQueryInformation(OID_GEN_MEDIA_CONNECT_STATUS, s, interface, &ConnectStatus, sizeof(ConnectStatus));
	if (ret < 0 || ConnectStatus == 0) {
		close(s);
		return 0;
	}

	// Current Channel
	OidQueryInformation(OID_802_11_CONFIGURATION, s, interface, &Configuration, sizeof(NDIS_802_11_CONFIGURATION));
	for (i = 0; i < G_nChanFreqCount; i++) {
		if (Configuration.DSConfig == ChannelFreqTable[i].lFreq) {
			nChannel = ChannelFreqTable[i].lChannel;
			break;
		}
	}

/*
	if (nChannel == -1)
		return 0;
	else if (nChannel == (Japan_channel + 8))
		websWrite(wp, "J8 <--> %ld KHz ; Central Channel: %ld", Configuration.DSConfig, LinkStatus.CentralChannel);
	else if (nChannel == (Japan_channel + 12))
		websWrite(wp, "J12 <--> %ld KHz ; Central Channel: %ld", Configuration.DSConfig, LinkStatus.CentralChannel);
	else if (nChannel == (Japan_channel + 16))
		websWrite(wp, "J16 <--> %ld KHz ; Central Channel: %ld", Configuration.DSConfig, LinkStatus.CentralChannel);
	else
		websWrite(wp, "%u <--> %ld KHz ; Central Channel: %ld", nChannel, Configuration.DSConfig, LinkStatus.CentralChannel);
*/
	close(s);
	return nChannel;
}

/*
 * description: get a station new/uniq profile name
 */
#define WSC_PROFILE_NAME_POSTFIX_LEN 3
static char *getStaNewProfileName(char *prefix)
{
	PRT_PROFILE_SETTING currentProfileSetting;
	int prefix_len = strlen(prefix);
	int total_len;
	int postfix_candidate = 1;
	static char result[32+1]; //refer to _RT_PROFILE_SETTING.

	if(strlen(prefix) > (32 - WSC_PROFILE_NAME_POSTFIX_LEN) ){
		// we force the prefix length can't exceed 29.
		prefix[32-WSC_PROFILE_NAME_POSTFIX_LEN] = '\0';
	}

	result[0] = '\0';
	if(headerProfileSetting == NULL){
		strncpy(result,  prefix, 32);
		strcat(result, "001");
		return result;
	}

	currentProfileSetting = headerProfileSetting;
	while(currentProfileSetting){
		total_len = strlen(currentProfileSetting->Profile);
		if(!strncmp(currentProfileSetting->Profile, prefix, prefix_len)) {
			char *profile_num = &currentProfileSetting->Profile[prefix_len];	// AAA001, BBB001 
			int profile_num_int;

			profile_num_int = atoi(profile_num);
			if(postfix_candidate == profile_num_int){
				postfix_candidate++;

				if(postfix_candidate > 999)
					return NULL;

				// loop whole chain again
				currentProfileSetting = headerProfileSetting;
				continue;
			}
		}
		currentProfileSetting = currentProfileSetting->Next;
	}

	snprintf(result, 32, "%s%03d", prefix, postfix_candidate);
	return result;
}

char *getValueFromDat(char *key, char *result, int len)
{
	char a_line[512];
	FILE *file = fopen("/etc/Wireless/RT2860/RT2860.dat", "r");
	if(!file){
		error(E_L, E_LOG, T("FATAL: open file failed!!! %s\n"), "/etc/Wireless/RT2860/RT2860.dat");
		return NULL;
	}

	while(fgets(a_line, 512, file)){
		char *nl1, *nl2;
		char *eq = strchr(a_line, '=');
		if(!eq)
			continue;
		*eq = '\0';
		if(!strcmp(key, a_line)){
			nl1 = strchr(eq+1, '\r');
			nl2 = strchr(eq+1, '\n');
			if(nl1)	*nl1 = '\0';
			if(nl2)	*nl2 = '\0';
			strncpy(result, eq+1, len);
			break;
		}
	}
	fclose(file);
	return result;
}

void SaveToFlashStr(char *key, char *value)
{
	char tmp_buffer[512];
	const char *wordlist = nvram_bufget(RT2860_NVRAM, key);
	if (wordlist && strcmp(wordlist,"") != 0)
		snprintf(tmp_buffer, 512, "%s\t%s", wordlist, value);
	else
		snprintf(tmp_buffer, 512, "%s", value);
	nvram_bufset(RT2860_NVRAM, key, tmp_buffer);
}

void SaveToFlashInt(char *key, int value)
{
	char tmp_buffer[512];
	const char *wordlist = nvram_bufget(RT2860_NVRAM, key);
	if (wordlist && strcmp(wordlist,"") != 0)
		snprintf(tmp_buffer, 512, "%s\t%d", wordlist, value);
	else
		snprintf(tmp_buffer, 512, "%d", value);
	nvram_bufset(RT2860_NVRAM, key, tmp_buffer);
}

static int getWPSSTAPINCodeASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char pin[16];
	if(!getSTAEnrolleePIN(pin))
		websWrite(wp, T(""));
	else
		websWrite(wp, T("%s"), pin);
	return 0;
}

static unsigned char *getStaMacAddr(void)
{
	static unsigned char CurrentAddress[6];
	int s;
	memset(CurrentAddress, 0, 6);
	s = socket(AF_INET, SOCK_DGRAM, 0);
	OidQueryInformation(OID_802_3_CURRENT_ADDRESS, s, "ra0", &CurrentAddress, sizeof(CurrentAddress));
	close(s);
	return CurrentAddress;
}


static int getWPSSTARegSSIDASP(int eid, webs_t wp, int argc, char_t **argv)
{
	const char *regSSID = nvram_bufget(RT2860_NVRAM, "staRegSSID");
	if(!regSSID || !strlen(regSSID)){
		unsigned char mac[6];
		char defaultSSID[33];
		memcpy(mac, getStaMacAddr(), 6);
		snprintf(defaultSSID, 32, "STARegistrar%02X%02X%02X", mac[3], mac[4], mac[5]);
		websWrite(wp, T("%s"), defaultSSID);
	}else
		websWrite(wp, T("%s"), regSSID);
	return 0;
}

static int getWPSSTARegAuthASP(int eid, webs_t wp, int argc, char_t **argv)
{
	const char *regAuth = nvram_bufget(RT2860_NVRAM, "staRegAuth");
	if(!regAuth || !strlen(regAuth))
		websWrite(wp, T(""));
	else
		websWrite(wp, T("%s"), regAuth);
	return 0;
}

static int getWPSSTARegEncryASP(int eid, webs_t wp, int argc, char_t **argv)
{
	const char *tmp = nvram_bufget(RT2860_NVRAM, "staRegEncry");
	if(!tmp || !strlen(tmp))
		websWrite(wp, T(""));
	else
		websWrite(wp, T("%s"), tmp);
	return 0;
}

static int getWPSSTARegKeyTypeASP(int eid, webs_t wp, int argc, char_t **argv)
{
	const char *tmp = nvram_bufget(RT2860_NVRAM, "staRegKeyType");
	if(!tmp || !strlen(tmp))
		websWrite(wp, T(""));
	else
		websWrite(wp, T("%s"), tmp);
	return 0;
}

static int getWPSSTARegKeyIndexASP(int eid, webs_t wp, int argc, char_t **argv)
{
	const char *tmp = nvram_bufget(RT2860_NVRAM, "staRegKeyIndex");
	if(!tmp || !strlen(tmp))
		websWrite(wp, T(""));
	else
		websWrite(wp, T("%s"), tmp);
	return 0;
}

static int getWPSSTARegKeyASP(int eid, webs_t wp, int argc, char_t **argv)
{
	const char *tmp = nvram_bufget(RT2860_NVRAM, "staRegKey");
	if(!tmp || !strlen(tmp))
		websWrite(wp, T(""));
	else
		websWrite(wp, T("%s"), tmp);
	return 0;
}

static int getWPSSTAModeASP(int eid, webs_t wp, int argc, char_t **argv)
{
	const char *tmp = nvram_bufget(RT2860_NVRAM, "staWPSMode");
	if(!tmp || !strlen(tmp))
		websWrite(wp, T("0"));	//default is "enrollee mode"
	else
		websWrite(wp, T("%s"), tmp);
	return 0;
}

/*
 * for WiFi STA Test Plan Case 5.1.1
 */
static char_t *addWPSSTAProfile2(WSC_CREDENTIAL *wsc_cre)
{
	RT_PROFILE_SETTING  tmpProfileSetting;

	char_t				tmp_value[512];
	USHORT              AuthType	= wsc_cre->AuthType;           // mandatory, 1: open, 2: wpa-psk, 4: shared, 8:wpa, 0x10: wpa2, 0x20: wpa
	USHORT              EncrType	= wsc_cre->EncrType;           // mandatory, 1: none, 2: wep, 4: tkip, 8: aes
	UCHAR               *Key		= &wsc_cre->Key[0];            // mandatory, Maximum 64 byte
	USHORT              KeyLength	= wsc_cre->KeyLength;
	UCHAR               KeyIndex	= wsc_cre->KeyIndex;           // optional, default is 1
                        
	char buffer[512] = {0}, *p;
	const char *wordlist = nvram_bufget(RT2860_NVRAM, "staActive");
	strcpy(buffer, wordlist);
	p = buffer;
	while (*p != '\0') {
		if (*p == '1')
			*p = '0';
		p++;
	}
	nvram_bufset(RT2860_NVRAM, "staActive", buffer);
	nvram_commit(RT2860_NVRAM);

	memset(&tmpProfileSetting, 0x00, sizeof(RT_PROFILE_SETTING));
	tmpProfileSetting.Next = NULL;

	strncpy(tmpProfileSetting.SSID, wsc_cre->SSID.Ssid, 32);
	printf("SSID1=%s\n", tmpProfileSetting.SSID);
	SaveToFlashStr("staSSID", tmpProfileSetting.SSID);

	//profile name, gen a uniq name
	snprintf(tmp_value, 512, "WPS_%s", tmpProfileSetting.SSID);
	strncpy(tmp_value, getStaNewProfileName(tmp_value), 512);
	if (!tmp_value || !strlen(tmp_value)) {
		fprintf(stderr, "Error profile name !\n");
		return NULL;
	}
	strncpy((char *)tmpProfileSetting.Profile, tmp_value, 32);
	SaveToFlashStr("staProfile", tmpProfileSetting.Profile);

	//network type
	tmpProfileSetting.NetworkType = 1;
	SaveToFlashInt("staNetworkType", 1);

	//Adhoc mode
	tmpProfileSetting.AdhocMode = 0;
	SaveToFlashInt("staAdhocMode", 0);

	//power saving mode
	tmpProfileSetting.PSmode = Ndis802_11PowerModeCAM;
	SaveToFlashInt("staPSMode", Ndis802_11PowerModeCAM);

	//channel
	tmpProfileSetting.Channel = getStaChannel("ra0");
	SaveToFlashInt("staChannel", tmpProfileSetting.Channel);

	//b preamble type
	tmpProfileSetting.PreamType = Rt802_11PreambleAuto;
	SaveToFlashInt("staPreamType", tmpProfileSetting.PreamType);

	//rts threshold value
	tmpProfileSetting.RTSCheck = 0;
	SaveToFlashInt("staRTSCheck", tmpProfileSetting.RTSCheck);
	tmpProfileSetting.RTS = 2347;
	SaveToFlashInt("staRTS", tmpProfileSetting.RTS);

	//fragment threshold value
	tmpProfileSetting.FragmentCheck = 0;
	SaveToFlashInt("staFragmentCheck", tmpProfileSetting.FragmentCheck);
	tmpProfileSetting.Fragment = 2346;
	SaveToFlashInt("staFragment", tmpProfileSetting.Fragment);

	// AuthMode
	//security policy (security_infra_mode or security_adhoc_mode)
	// get Security from .dat
	switch(AuthType){
	case 0x1:
		tmpProfileSetting.Authentication = Ndis802_11AuthModeOpen;
		break;
	case 0x2:
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPAPSK;
		if(EncrType != 0x4 && EncrType != 0x8)
			return NULL;
		break;
	case 0x4:
		tmpProfileSetting.Authentication = Ndis802_11AuthModeShared;
		if(EncrType != 0x1 && EncrType != 0x2)
			return NULL;
		break;
	case 0x8:
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPA;
		break;
	case 0x10:
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPA2;
		break;
	case 0x20:
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPA2PSK;		
		if(EncrType != 0x4 && EncrType != 0x8)
			return NULL;
		break;
	default:
		return NULL;
	}
	SaveToFlashInt("staAuth", tmpProfileSetting.Authentication);

	// Encrypt mode
	//Encrypt
	switch(EncrType){
	case 0x1:	/* None */
		tmpProfileSetting.Encryption = Ndis802_11WEPDisabled;
		break;
	case 0x2:	/* WEP */
		if(KeyLength && (KeyLength != 5 && KeyLength != 13) && (KeyLength != 10 && KeyLength != 26))
			return NULL;
		tmpProfileSetting.Encryption = Ndis802_11WEPEnabled;
		break;
	case 0x8:	/* AES */
		tmpProfileSetting.Encryption = Ndis802_11Encryption3Enabled;
		break;
	case 0x4:	/* TKIP */
		;
	default:	/* default: TKIP */
		tmpProfileSetting.Encryption = Ndis802_11Encryption2Enabled;
		break;
	}
	SaveToFlashInt("staEncrypt", tmpProfileSetting.Encryption);
	
	//wep default key
	tmpProfileSetting.KeyDefaultId = KeyIndex;
	SaveToFlashInt("staKeyDefaultId", tmpProfileSetting.KeyDefaultId);

#ifdef WPA_SUPPLICANT_SUPPORT
	if(tmpProfileSetting.Authentication  == Ndis802_11AuthModeWPA ||
		tmpProfileSetting.Authentication == Ndis802_11AuthModeWPA2){
		tmpProfileSetting.KeyMgmt = Rtwpa_supplicantKeyMgmtWPAEAP;
	}else if(tmpProfileSetting.Authentication == Ndis802_11AuthModeMax){
		tmpProfileSetting.KeyMgmt = Rtwpa_supplicantKeyMgmtIEEE8021X;
	}else
		tmpProfileSetting.KeyMgmt = Rtwpa_supplicantKeyMgmtNONE;
	SaveToFlashInt("sta8021xKeyMgmt", tmpProfileSetting.KeyMgmt);
#endif

	/*
	 *	Deal with Key
	 */
	switch(AuthType){
	case 0x1:	/* Open */
	case 0x4:	/* Shared */
		//tmpProfileSetting.Authentication = Ndis802_11AuthModeOpen;
		if(EncrType == 2 /* WEP */){
			char_t hex_wep[128];
			if(KeyLength == 5)
				sprintf(hex_wep, "%02X%02X%02X%02X%02X", Key[0], Key[1], Key[2], Key[3], Key[4]);
			else if (KeyLength == 13)
				sprintf(hex_wep, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", Key[0], Key[1], Key[2], Key[3], Key[4], Key[5], Key[6], Key[7], Key[8], Key[9], Key[10], Key[11], Key[12]);
			if(KeyLength == 10)
				sprintf(hex_wep, "%c%c%c%c%c%c%c%c%c%c", Key[0], Key[1], Key[2], Key[3], Key[4], Key[5], Key[6], Key[7], Key[8], Key[9]);
			else if (KeyLength == 26)
				sprintf(hex_wep, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 
					Key[0], Key[1], Key[2], Key[3], Key[4], Key[5], Key[6], Key[7], Key[8], Key[9], Key[10], Key[11], Key[12], Key[13], Key[14], Key[15], Key[16], Key[17], Key[18], Key[19], Key[20], Key[21], Key[22], Key[23], Key[24], Key[25]);
			switch(KeyIndex){
			case 1:
				SaveToFlashStr("staKey1", hex_wep);
				SaveToFlashInt("staKey1Type", 0);
				break;
			case 2:
				SaveToFlashStr("staKey2", hex_wep);
				SaveToFlashInt("staKey2Type", 0);
				break;
			case 3:
				SaveToFlashStr("staKey3", 		hex_wep);
				SaveToFlashInt("staKey3Type", 0);
				break;
			case 4:
				SaveToFlashStr("staKey4", 		hex_wep);
				SaveToFlashInt("staKey4Type", 0);
				break;
			}
		}else{
			// clear WEP Keys
			SaveToFlashStr("staKey1", "0");
			SaveToFlashInt("staKey1Type", 0);
			SaveToFlashStr("staKey2", "0");
			SaveToFlashInt("staKey2Type", 0);
			SaveToFlashStr("staKey3", "0");
			SaveToFlashInt("staKey3Type", 0);
			SaveToFlashStr("staKey4", "0");
			SaveToFlashInt("staKey4Type", 0);
		}

		// clear WPAPSK
		SaveToFlashStr("staWpaPsk", "0");
		break;
    case 0x2:	/* WPAPSK */
	case 0x20:	/* WPAPSK2 */
		// set WPAPSK Key
		strncpy(tmpProfileSetting.WpaPsk, Key, 64);
		SaveToFlashStr("staWpaPsk", tmpProfileSetting.WpaPsk);

		// clear WEP Keys
		SaveToFlashStr("staKey1", "0");
		SaveToFlashInt("staKey1Type", 0);
		SaveToFlashStr("staKey2", "0");
		SaveToFlashInt("staKey2Type", 0);
		SaveToFlashStr("staKey3", "0");
		SaveToFlashInt("staKey3Type", 0);
		SaveToFlashStr("staKey4", "0");
		SaveToFlashInt("staKey4Type", 0);
        break;
    case 0x8:	/* WPA */
    case 0x10:	/* WPA2 */
    	printf("Warning,WPS WPA/WPA\n");
        break;
    default:
        return NULL;
    }

	// can't find "key length" in .dat and ioctl()...
	tmpProfileSetting.Key1Length = tmpProfileSetting.Key2Length = 
		tmpProfileSetting.Key3Length = tmpProfileSetting.Key4Length = 0;
	SaveToFlashInt("staKey1Length", 0);
	SaveToFlashInt("staKey2Length", 0);
	SaveToFlashInt("staKey3Length", 0);
	SaveToFlashInt("staKey4Length", 0);

#ifdef WPA_SUPPLICANT_SUPPORT
	tmpProfileSetting.EAP = Rtwpa_supplicantEAPNONE;
	SaveToFlashInt("sta8021xEAP", tmpProfileSetting.EAP);

	tmpProfileSetting.Tunnel = Rtwpa_supplicantTUNNENONE;
	SaveToFlashInt("sta8021xTunnel", tmpProfileSetting.Tunnel);

	strncpy(tmpProfileSetting.Identity, "", IDENTITY_LENGTH);
	SaveToFlashStr("sta8021xIdentity", "");

	strncpy(tmpProfileSetting.Password, "", 32);
	SaveToFlashStr("sta8021xPassword", "");

	strncpy(tmpProfileSetting.ClientCert, "", CERT_PATH_LENGTH);
	SaveToFlashStr("sta8021xClientCert", "");

	strncpy(tmpProfileSetting.PrivateKey, "", PRIVATE_KEY_PATH_LENGTH);
	SaveToFlashStr("sta8021xPrivateKey", "");

	strncpy(tmpProfileSetting.PrivateKeyPassword, "", 32);
	SaveToFlashStr("sta8021xPrivateKeyPassword", "");

	strncpy(tmpProfileSetting.CACert, "", CERT_PATH_LENGTH);
	SaveToFlashStr("sta8021xCACert", "");
#else /* WPA_SUPPLICANT_SUPPORT */
	SaveToFlashStr("sta8021xEAP", "7");
	SaveToFlashStr("sta8021xTunnel", "3");
	SaveToFlashStr("sta8021xKeyMgmt", "3");
	SaveToFlashStr("sta8021xIdentity", "0");
	SaveToFlashStr("sta8021xPassword", "0");
	SaveToFlashStr("sta8021xClientCert", "0");
	SaveToFlashStr("sta8021xPrivateKey", "0");
	SaveToFlashStr("sta8021xPrivateKeyPassword", "0");
	SaveToFlashStr("sta8021xCACert", "0");
#endif /* WPA_SUPPLICANT_SUPPORT */

	//write into /etc/rt61sta.ui
	//writeProfileToFile(&tmpProfileSetting);

	tmpProfileSetting.Active = 1;
	SaveToFlashInt("staActive", tmpProfileSetting.Active);

	nvram_commit(RT2860_NVRAM);

	freeHeaderProfileSettings();
	headerProfileSetting = NULL;

	return NULL;
}

static char_t *addWPSSTAProfile(char_t *result)
{
	RT_PROFILE_SETTING  tmpProfileSetting;

	char_t value[512];
	
	char buffer[512] = {0}, *p;
	const char *wordlist = nvram_bufget(RT2860_NVRAM, "staActive");
	strcpy(buffer, wordlist);
	p = buffer;
	while (*p != '\0') {
		if (*p == '1')
			*p = '0';
		p++;
	}
	nvram_bufset(RT2860_NVRAM, "staActive", buffer);
	nvram_commit(RT2860_NVRAM);


	memset(&tmpProfileSetting, 0x00, sizeof(RT_PROFILE_SETTING));
	tmpProfileSetting.Next = NULL;

	//SSID, get SSID from Dat
	if(!getValueFromDat("SSID1", tmpProfileSetting.SSID, NDIS_802_11_LENGTH_SSID+1))			// get SSID from .DAT
		return NULL;
	printf("SSID1=%s\n", tmpProfileSetting.SSID);
	SaveToFlashStr("staSSID", tmpProfileSetting.SSID);

	//profile name, gen a uniq name
	snprintf(value, 512, "WPS_%s", tmpProfileSetting.SSID);
	strncpy(value, getStaNewProfileName(value), 512);
	if (!value || !strlen(value)) {
		fprintf(stderr, "Error profile name !\n");
		return NULL;
	}
	strncpy((char *)tmpProfileSetting.Profile, value, 32);
	SaveToFlashStr("staProfile", tmpProfileSetting.Profile);

	//network type
	tmpProfileSetting.NetworkType = 1;
	SaveToFlashInt("staNetworkType", 1);

	//Adhoc mode
	tmpProfileSetting.AdhocMode = 0;
	SaveToFlashInt("staAdhocMode", 0);

	//power saving mode
	tmpProfileSetting.PSmode = Ndis802_11PowerModeCAM;
	SaveToFlashInt("staPSMode", Ndis802_11PowerModeCAM);

	//channel
	tmpProfileSetting.Channel = getStaChannel("ra0");
	SaveToFlashInt("staChannel", tmpProfileSetting.Channel);

	//b preamble type
	tmpProfileSetting.PreamType = Rt802_11PreambleAuto;
	SaveToFlashInt("staPreamType", tmpProfileSetting.PreamType);

	//rts threshold value
	tmpProfileSetting.RTSCheck = 0;
	SaveToFlashInt("staRTSCheck", tmpProfileSetting.RTSCheck);
	tmpProfileSetting.RTS = 2347;
	SaveToFlashInt("staRTS", tmpProfileSetting.RTS);

	//fragment threshold value
	tmpProfileSetting.FragmentCheck = 0;
	SaveToFlashInt("staFragmentCheck", tmpProfileSetting.FragmentCheck);
	tmpProfileSetting.Fragment = 2346;
	SaveToFlashInt("staFragment", tmpProfileSetting.Fragment);

	//security policy (security_infra_mode or security_adhoc_mode)
	// get Security from .dat
	if(!getValueFromDat("AuthMode", value, 512))			// get Auth from .DAT
		return NULL;
	if(!strlen(value))
		return NULL;

	if(!strcmp(value, "OPEN")){
		tmpProfileSetting.Authentication = Ndis802_11AuthModeOpen;
	}else if(!strcmp(value, "SHARED")){
		tmpProfileSetting.Authentication = Ndis802_11AuthModeShared;
	}else if(!strcmp(value, "WEPAUTO")){
		tmpProfileSetting.Authentication = Ndis802_11AuthModeAutoSwitch;
	}else if(!strcmp(value, "WPAPSK")){
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPAPSK;
	}else if(!strcmp(value, "WPA2PSK")){
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPA2PSK;
	}else if(!strcmp(value, "WPANONE")){
		return NULL;										//no WPANONE in WPS.
	}else if(!strcmp(value, "WPA")){
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPA;
	}else if(!strcmp(value, "WPA2")){
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPA2;
	}else{
		tmpProfileSetting.Authentication = Ndis802_11AuthModeWPAPSK;
	}
	SaveToFlashInt("staAuth", tmpProfileSetting.Authentication);

#ifdef WPA_SUPPLICANT_SUPPORT
	if(tmpProfileSetting.Authentication  == Ndis802_11AuthModeWPA ||
		tmpProfileSetting.Authentication == Ndis802_11AuthModeWPA2){
		tmpProfileSetting.KeyMgmt = Rtwpa_supplicantKeyMgmtWPAEAP;
	}else if(tmpProfileSetting.Authentication == Ndis802_11AuthModeMax){
		tmpProfileSetting.KeyMgmt = Rtwpa_supplicantKeyMgmtIEEE8021X;
	}else
		tmpProfileSetting.KeyMgmt = Rtwpa_supplicantKeyMgmtNONE;
	SaveToFlashInt("sta8021xKeyMgmt", tmpProfileSetting.KeyMgmt);
#endif

	if(!getValueFromDat("Key1Str", value, 512))			
		return NULL;
	if(!strlen(value))
		strcpy(value, "0");
	strcpy(tmpProfileSetting.Key1, value);
	SaveToFlashStr("staKey1", tmpProfileSetting.Key1);

	if(!getValueFromDat("Key2Str", value, 512))			
		return NULL;
	if(!strlen(value))
		strcpy(value, "0");
	strcpy(tmpProfileSetting.Key2, value);
	SaveToFlashStr("staKey2", tmpProfileSetting.Key2);

	if(!getValueFromDat("Key3Str", value, 512))			
		return NULL;
	if(!strlen(value))
		strcpy(value, "0");
	strcpy(tmpProfileSetting.Key3, value);
	SaveToFlashStr("staKey3", tmpProfileSetting.Key3);

	if(!getValueFromDat("Key4Str", value, 512))			
		return NULL;
	if(!strlen(value))
		strcpy(value, "0");
	strcpy(tmpProfileSetting.Key4, value);
	SaveToFlashStr("staKey4", tmpProfileSetting.Key4);

	//wep key entry method
	if(!getValueFromDat("Key1Type", value, 512))			
		return NULL;
	if(!strlen(value))
		strcpy(value, "0");
	tmpProfileSetting.Key1Type = atoi(value);
	SaveToFlashInt("staKey1Type", tmpProfileSetting.Key1Type);

	if(!getValueFromDat("Key2Type", value, 512))			
		return NULL;
	if(!strlen(value))
		strcpy(value, "0");
	tmpProfileSetting.Key2Type = atoi(value);
	SaveToFlashInt("staKey2Type", tmpProfileSetting.Key2Type);

	if(!getValueFromDat("Key3Type", value, 512))			
		return NULL;
	if(!strlen(value))
		strcpy(value, "0");
	tmpProfileSetting.Key3Type = atoi(value);
	SaveToFlashInt("staKey3Type", tmpProfileSetting.Key3Type);

	if(!getValueFromDat("Key4Type", value, 512))			
		return NULL;
	if(!strlen(value))
		strcpy(value, "0");
	tmpProfileSetting.Key4Type = atoi(value);
	SaveToFlashInt("staKey4Type", tmpProfileSetting.Key4Type);

	// can't find "key length" in .dat and ioctl()...
	tmpProfileSetting.Key1Length = tmpProfileSetting.Key2Length = 
		tmpProfileSetting.Key3Length = tmpProfileSetting.Key4Length = 0;
	SaveToFlashInt("staKey1Length", 0);
	SaveToFlashInt("staKey2Length", 0);
	SaveToFlashInt("staKey3Length", 0);
	SaveToFlashInt("staKey4Length", 0);

	//wep default key
	if(!getValueFromDat("DefaultKeyID", value, 512))
		return NULL;
	if(!strlen(value))
		strcpy(value, "1");
	tmpProfileSetting.KeyDefaultId = atoi(value);
	SaveToFlashInt("staKeyDefaultId", tmpProfileSetting.KeyDefaultId);

	//Encrypt
	if(!getValueFromDat("EncrypType", value, 512))
		return NULL;
	if(!strlen(value))
		strcpy(value, "TKIP");
	if(!strcmp(value, "TKIP")){
		tmpProfileSetting.Encryption = Ndis802_11Encryption2Enabled;
	}else if(!strcmp(value, "AES")){
		tmpProfileSetting.Encryption = Ndis802_11Encryption3Enabled;
	}else if(!strcmp(value, "WEP")){
		tmpProfileSetting.Encryption = Ndis802_11WEPEnabled;
	}else if(!strcmp(value, "NONE")){
		tmpProfileSetting.Encryption = Ndis802_11WEPDisabled;
	}else
		tmpProfileSetting.Encryption = Ndis802_11Encryption2Enabled;
	SaveToFlashInt("staEncrypt", tmpProfileSetting.Encryption);

	//passphrase
	if(!getValueFromDat("WPAPSK", value, 512))
		return NULL;
	strncpy(tmpProfileSetting.WpaPsk, value, 64);
	SaveToFlashStr("staWpaPsk", tmpProfileSetting.WpaPsk);

#ifdef WPA_SUPPLICANT_SUPPORT
	tmpProfileSetting.EAP = Rtwpa_supplicantEAPNONE;
	SaveToFlashInt("sta8021xEAP", tmpProfileSetting.EAP);

	tmpProfileSetting.Tunnel = Rtwpa_supplicantTUNNENONE;
	SaveToFlashInt("sta8021xTunnel", tmpProfileSetting.Tunnel);

	strncpy(tmpProfileSetting.Identity, "", IDENTITY_LENGTH);
	SaveToFlashStr("sta8021xIdentity", "");

	strncpy(tmpProfileSetting.Password, "", 32);
	SaveToFlashStr("sta8021xPassword", "");

	strncpy(tmpProfileSetting.ClientCert, "", CERT_PATH_LENGTH);
	SaveToFlashStr("sta8021xClientCert", "");

	strncpy(tmpProfileSetting.PrivateKey, "", PRIVATE_KEY_PATH_LENGTH);
	SaveToFlashStr("sta8021xPrivateKey", "");

	strncpy(tmpProfileSetting.PrivateKeyPassword, "", 32);
	SaveToFlashStr("sta8021xPrivateKeyPassword", "");

	strncpy(tmpProfileSetting.CACert, "", CERT_PATH_LENGTH);
	SaveToFlashStr("sta8021xCACert", "");
#else /* WPA_SUPPLICANT_SUPPORT */
	SaveToFlashStr("sta8021xEAP", "7");
	SaveToFlashStr("sta8021xTunnel", "3");
	SaveToFlashStr("sta8021xKeyMgmt", "3");
	SaveToFlashStr("sta8021xIdentity", "0");
	SaveToFlashStr("sta8021xPassword", "0");
	SaveToFlashStr("sta8021xClientCert", "0");
	SaveToFlashStr("sta8021xPrivateKey", "0");
	SaveToFlashStr("sta8021xPrivateKeyPassword", "0");
	SaveToFlashStr("sta8021xCACert", "0");
#endif /* WPA_SUPPLICANT_SUPPORT */

	//write into /etc/rt61sta.ui
	//writeProfileToFile(&tmpProfileSetting);

	tmpProfileSetting.Active = 1;
	SaveToFlashInt("staActive", tmpProfileSetting.Active);

	nvram_commit(RT2860_NVRAM);

	freeHeaderProfileSettings();
	headerProfileSetting = NULL;

	if(result)
		gstrcpy(result, tmpProfileSetting.Profile);
	return result;
}

#endif /* CONFIG_RT2860V2_STA_WSC */

void formDefineWPS(void){
	websAspDefine(T("getPINASP"), getPINASP);

	websFormDefine(T("WPSSetup"), WPSSetup);
	websFormDefine(T("GenPIN"), GenPIN);
	websFormDefine(T("OOB"), OOB);
	websFormDefine(T("updateWPS"), updateWPS);
	websFormDefine(T("WPS"), WPS);
	websFormDefine(T("WPSCancel"), WPSCancel);


#ifdef CONFIG_RT2860V2_STA_WSC
	websAspDefine(T("getStaWPSBSSIDListASP"), getStaWPSBSSIDListASP);
	websFormDefine(T("WPSSTABSSIDListReset"), WPSSTABSSIDListReset);
	websFormDefine(T("WPSSTAGenNewPIN"), WPSSTAGenNewPIN);
	websFormDefine(T("WPSSTAPINEnr"), WPSSTAPINEnr);
	websFormDefine(T("WPSSTAPBCEnr"), WPSSTAPBCEnr);
	websFormDefine(T("updateWPSStaStatus"), updateWPSStaStatus);
	websFormDefine(T("WPSSTAStop"), WPSSTAStop);

	websAspDefine(T("getWPSSTAPINCodeASP"), getWPSSTAPINCodeASP);
	websAspDefine(T("getWPSSTARegSSIDASP"), getWPSSTARegSSIDASP);
	websAspDefine(T("getWPSSTARegAuthASP"), getWPSSTARegAuthASP);
	websAspDefine(T("getWPSSTARegEncryASP"), getWPSSTARegEncryASP);
	websAspDefine(T("getWPSSTARegKeyTypeASP"), getWPSSTARegKeyTypeASP);
	websAspDefine(T("getWPSSTARegKeyIndexASP"), getWPSSTARegKeyIndexASP);
	websAspDefine(T("getWPSSTARegKeyASP"), getWPSSTARegKeyASP);
	websAspDefine(T("getWPSSTAModeASP"), getWPSSTAModeASP);

	websFormDefine(T("WPSSTARegistrarSetupSSID"), WPSSTARegistrarSetupSSID);
	websFormDefine(T("WPSSTARegistrarSetupKey"), WPSSTARegistrarSetupKey);
	websFormDefine(T("WPSSTARegistrarSetupRest"), WPSSTARegistrarSetupRest);

	websFormDefine(T("WPSSTAPINReg"), WPSSTAPINReg);
	websFormDefine(T("WPSSTAPBCReg"), WPSSTAPBCReg);

	websFormDefine(T("WPSSTAMode"), WPSSTAMode);
#endif /* CONFIG_RT2860V2_STA_WSC */
}
