#ifndef RT61APD_H
#define RT61APD_H

#include "common.h"
#include "ap.h"
#include <linux/wireless.h>

#define MAX_MBSSID_NUM              4
#define WEP8021X_KEY_LEN            13

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif
#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif
#ifndef ETH_P_ALL
#define ETH_P_ALL 0x0003
#endif

#include "config.h"
#define NIC_DBG_STRING      (" ")

#define RT_DEBUG_HOSTAPD	0
#define RT_DEBUG_TEMP		1

#define RT_DEBUG_ERROR		1
#define RT_DEBUG_WARN		2
#define RT_DEBUG_TRACE		3
#define RT_DEBUG_INFO		4

// OID definition
#define OID_GET_SET_TOGGLE							0x8000
#define RT_QUERY_SIGNAL_CONTEXT						0x0402
#define	RT_SET_APD_PID								0x0405
#define RT_SET_DEL_MAC_ENTRY						0x0406

#define RT_PRIV_IOCTL								(SIOCIWFIRSTPRIV + 0x01)
#define RTPRIV_IOCTL_ADD_PMKID_CACHE                (SIOCIWFIRSTPRIV + 0x0A)
#define RTPRIV_IOCTL_RADIUS_DATA                    (SIOCIWFIRSTPRIV + 0x0C)
#define RTPRIV_IOCTL_ADD_WPA_KEY                    (SIOCIWFIRSTPRIV + 0x0E)
#define RTPRIV_IOCTL_STATIC_WEP_COPY                (SIOCIWFIRSTPRIV + 0x10)

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#ifndef ETH_P_PAE
#define ETH_P_PAE 0x888E /* Port Access Entity (IEEE 802.1X) */
#endif /* ETH_P_PAE */

#ifndef ETH_P_PRE_AUTH
#define ETH_P_PRE_AUTH 0x88C7 /* Port Access Entity (WPA2 pre-auth mode) */
#endif /* ETH_P_PRE_AUTH */

#ifndef ETH_P_VLAN
#define ETH_P_VLAN 0x8100 /* VLAN Protocol */
#endif /* ETH_P_VLAN */

#define BIT(x) (1 << (x))
#define REAUTH_TIMER_DEFAULT_reAuthEnabled TRUE
#define REAUTH_TIMER_DEFAULT_reAuthPeriod 3600

#if DBG
u32    RTDebugLevel;
#define DBGPRINT(Level, fmt, args...) 					\
{                                   \
    if (Level <= RT_DEBUG_TRACE)      \
    {                               \
        printf(NIC_DBG_STRING);   \
		printf( fmt, ## args);			\
    }                               \
}
#else
#define DBGPRINT(Level, fmt, args...) 	
#endif

struct ieee8023_hdr {
	u8 dAddr[6];
	u8 sAddr[6];
	u16 eth_type;
} __attribute__ ((packed));

typedef struct apd_data {
	struct rtapd_config *conf;
	char *config_fname;

	int wlan_sock;				/* raw packet socket for wireless interface access */		
	int eth_sock; /* raw packet socket for ethernet interface access */
	int ioctl_sock; /* socket for ioctl() use */
	u8 own_addr[MAX_MBSSID_NUM][6];

	int num_sta; /* number of entries in sta_list */
	struct sta_info *sta_list; /* STA info list head */
	struct sta_info *sta_hash[STA_HASH_SIZE];

	/* pointers to STA info; based on allocated AID or NULL if AID free
	 * AID is in the range 1-2007, so sta_aid[0] corresponders to AID 1
	 * and so on
	 */
	struct sta_info *sta_aid[MAX_AID_TABLE_SIZE];

	struct radius_client_data *radius;

} rtapd;

typedef struct recv_from_ra {
    u8 daddr[6];
    u8 saddr[6];
    u8 ethtype[2];
    u8 xframe[1];    
} __attribute__ ((packed)) priv_rec;

void ieee802_1x_receive(rtapd *apd, u8 *sa, u8 *apidx, u8 *buf, size_t len, u16 ethertype);
u16	RTMPCompareMemory(void *pSrc1,void *pSrc2, u16 Length);
void Handle_term(int sig, void *eloop_ctx, void *signal_ctx);
int RT_ioctl(rtapd *rtapd, int param, char  *data, int data_len, unsigned char apidx, int flags);

#endif // RT61APD_H
