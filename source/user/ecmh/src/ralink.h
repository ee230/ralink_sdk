
#ifndef __RALINK_H__
#define __RALINK_H__

#include "linux/autoconf.h"

#define RALINK_TIMEOUT	10	/* secs, sync switch mac table timer */

#define MAX_MULTICASTS_GROUP		256

#define MACTABLE_ENTRY_DELETED                          1
#define MACTABLE_ENTRY_ZEROED                           2
#define MACTABLE_ENTRY_DONT_CARE                        3

#define IGNORE_INTERFACE1		"eth2.1"
#define IGNORE_INTERFACE2		"eth2"

#if defined (CONFIG_WAN_AT_P0)
#define WANPORT			0x1			/* 0000001 */
#elif defined (CONFIG_WAN_AT_P4)
#define WANPORT			0x10			/* 0010000 */
#else
#define WANPORT			0x0			/* no wan port */
#endif

#define LAN_VLAN_IDX		0		/* lan side vlan table index */

#define OTHER_INTERFACE				7		/* port 7  (wifi)  */

#define IN6_GET_ADDR_SINGLE_BYTE(a, x)   (((__const uint8_t *) (a))[(x)])
#define IN6_SET_ADDR_SINGLE_BYTE(a, x, y)   (((uint8_t *) (a))[(x)]) = y

#define GetGroup(a)			(((unsigned int *)(a))[3])

typedef struct rt3052_esw_reg {
	unsigned int off;
	unsigned int val;
} esw_reg;

struct mac_table{
	unsigned int		mac1;		/* 4 byte mac address */
	unsigned short int	mac2;		/* 2 byte mac address */
	unsigned char		vidx;		/* vlan table index */
	unsigned char		port_map;
};

void dolog(int level, const char *fmt, ...);
void ralink_fini(void);
int  ralink_init(void);
void dump_table(void);

void dump_mactable(void);
void update_group_port_map(struct in6_addr *mca, int delete_flag);
void sync_internal_mac_table(void *argu);

#endif
