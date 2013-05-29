/*
 *  This implementation is for RT3052 Switch.
 *
 *  We maintain an IGMP group list:
 *
 *				group[ 01:00:5E:00:00:02 ]
 *				group[ 01:00:5E:00:00:05 ]
 *		            ...
 *
 *  In every IGMP group entry, an IP address list keeps the record who has sent IGMP report.
 *
 *       group[ 01:00:5E:00:00:02 ] --> 10.10.10.100(port1) --> 10.10.10.102(port3) -> ... -> NULL
 *       group[ 01:00:5E:00:00:05 ] --> 10.10.10.103(port3) --> NULL
 *        ...
 *
 *  By steping the IP address list, we can know the ports which the IGMP group is interesting in.
 *
 *  A mirror of rt3052 internal mac table is created in memory to improve performance. The time 
 *  interval of sync is 10 secs.
 *
 */

/*
 *  History:
 *
 * 1) Add an entry for igmp query on WAN
 * 2) Broadcast the multicast packets which not matched.
 * 3) Delay mac table deletion.
 * 4) Don't follow igmpproxy to remove an invalid (unknown destination) group entry
 * 5) "no wan port" condition.
 * 6) WiFi Snooping support: a) search STA in WiFi tables  b) using WiFi(ra0)'s internal snooping
 * 7) Overflow bug fixed while handling high mac address(ex: F0:12:34:56:78:90)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "linux/autoconf.h"

#include "defs.h"

#include <linux/wireless.h>

#define MAX_MULTICASTS_GROUP		256
#define INTERNAL_SYNC_TIMEOUT		10		// secs

#define END_OF_MAC_TABLE			0xFFFFFFFF

/* ioctl commands */
#define RAETH_ESW_REG_READ			0x89F1
#define RAETH_ESW_REG_WRITE			0x89F2

/* rt3052 embedded ethernet switch registers */
#define REG_ESW_VLAN_ID_BASE		0x50
#define REG_ESW_VLAN_MEMB_BASE		0x70
#define REG_ESW_TABLE_SEARCH		0x24
#define REG_ESW_TABLE_STATUS0		0x28
#define REG_ESW_TABLE_STATUS1		0x2C
#define REG_ESW_TABLE_STATUS2		0x30
#define REG_ESW_WT_MAC_AD0			0x34
#define REG_ESW_WT_MAC_AD1			0x38
#define REG_ESW_WT_MAC_AD2			0x3C
#define REG_ESW_MAX					0xFC

#define IP_GET_LOST_MAPPING(mip)    ((mip & 0x0F800000) >> 23) 
#define IP_MULTICAST_A0(a0)			((a0 >> 1) | 0xE0)
#define IP_MULTICAST_A1(a0, a1)		(((a0 & 0x1) << 7) | a1)

#if defined (CONFIG_WAN_AT_P0)
#define WANPORT			0x1			/* 0000001 */
#define LANPORT_RANGE		{1,2,3,4}
#elif defined (CONFIG_WAN_AT_P4)
#define WANPORT			0x10		/* 0010000 */
#define LANPORT_RANGE		{0,1,2,3}
#else
#define WANPORT			0x0			/* no wan port */
#define LANPORT_RANGE		{0,1,2,3,4}
#endif

#define OTHER_INTERFACE				7		/* port 7  (wifi)  */

/* delay mac table deletion */
#define DELETED			1
#define ZEROED			2

#define ADDENTRY		1
#define DELENTRY		2

#ifdef CONFIG_RAETH_SPECIAL_TAG
#define LAN_VLAN_IDX              6
#define WAN_VLAN_IDX              7
#else
#define LAN_VLAN_IDX              0
#define WAN_VLAN_IDX              1
#endif


typedef struct rt3052_esw_reg {
	unsigned int off;
	unsigned int val;
} esw_reg;

struct group_member
{
	unsigned long 		ip_addr;
	unsigned char		a0;		// class D, the 1st byte.
	char 				port_num;
	char				has_report;	// 0 or 1
	struct group_member *next;
};

struct group
{
	unsigned char	a1;			// class D, the 2nd byte
	unsigned char	a2;			// class D, the 3rd byte
	unsigned char	a3;			// class D, the 4th byte
	unsigned char	port_map;	// port map
	struct group_member	*members;
	struct group	*next;
};

struct mac_table{
	unsigned int		mac1;		// 4 byte mac address
	unsigned short int	mac2;		// 2 byte mac address
	unsigned char		vidx;
	unsigned char		port_map;
};

// function prototype
static void update_group_port_map(struct group *entry);
static void updateMacTable(struct group *entry, int delay_deleted);
static void updateMacTable_specialtag(struct group *entry, unsigned char new_portmap, int delay_delete);
static int	portLookUpByIP(char *ip);
static inline int reg_read(int offset, int *value);
static inline int reg_write(int offset, int value);
static void ZeroEntriesBarrier(struct group *entry, int mode);

// global variables.
static struct mac_table internal_mac_table[1024];
static int 				esw_fd = -1;
static struct group 	*group_list = NULL;

static struct ifreq		ifr;
static esw_reg			reg;
static int				snooping_enabled = 0;

static struct group *find_entry(uint32 ip_addr)
{
	unsigned ua1, ua2, ua3;
	struct group *pos = group_list;

	ua1 = ( ip_addr & 0x007F0000 ) >> 16;
	ua2 = ( ip_addr & 0x0000FF00 ) >> 8;
	ua3 = ( ip_addr & 0x000000FF );

	while(pos){
		if(pos->a1 == ua1 && pos->a2 == ua2 && pos->a3 == ua3)
			return pos;
		pos = pos->next;
	}
	return NULL;
}

static void dump_table(void)
{
	int i=0;
	unsigned int mac1;
	char show_buf[128];
	mac1 = internal_mac_table[i].mac1;
	while( i< 1024 && mac1 != END_OF_MAC_TABLE){
		sprintf(show_buf, "%08x%04x, %d, %08x\n", internal_mac_table[i].mac1, internal_mac_table[i].mac2, internal_mac_table[i].vidx, internal_mac_table[i].port_map);
		printf("%s\n", show_buf);
		i++;
		mac1 = internal_mac_table[i].mac1;
	}
}

static void dump_entry(void)
{
	unsigned char port_map, i;
	struct group *pos = group_list;
	printf("=== Dump group entries:\n");
	while(pos){
		struct group_member *member;

		printf("01:00:5e:%02x:%02x:%02x\t", pos->a1, pos->a2, pos->a3);
		port_map = pos->port_map;
		for(i=0; i<sizeof(unsigned char); i++){
			if(port_map & 0x1){
				printf("%d ",  i);
			}
			port_map = port_map >> 1;
		}

		printf("\n");
		member = pos->members;
		while(member){
			struct in_addr tmp;
			tmp.s_addr = htonl(member->ip_addr);
			printf("\t\t%d.%d.%d.%d\t%s(%d)\tport%d\t%s\n", IP_MULTICAST_A0(member->a0), IP_MULTICAST_A1(member->a0, pos->a1), pos->a2, pos->a3, inet_ntoa(tmp), member->a0, member->port_num, member->has_report ? "reported" : "not-reported");
			member = member->next;
		}

		pos = pos->next;
		printf("\n");
	}
	printf("===\n");
}

static struct group *build_entry(uint32 m_ip_addr, uint32 u_ip_addr)
{
	unsigned char 		a1, a2, a3;
	struct in_addr 		tmp;
	struct group 		*new_entry;

	static int group_count = 0;

	// crowd control
	if(group_count++ > MAX_MULTICASTS_GROUP)
		return NULL;

	a1 = ( m_ip_addr & 0x007F0000 ) >> 16;
	a2 = ( m_ip_addr & 0x0000FF00 ) >> 8;
	a3 = ( m_ip_addr & 0x000000FF );

	/* check if "all hosts" address*/
	if(a1 == 0 && a2 == 0 && a3 == 1)
		return NULL;

	new_entry = (struct group *)malloc(sizeof(struct group));
	if(!new_entry){
		log(LOG_WARNING, 0, "*** RT3052: Out of memory.");
		return NULL;
	}
	printf("%s, %s\n", __FUNCTION__,  inetFmt(htonl(m_ip_addr), s1));
	
	/* set up address */
	new_entry->a1 = a1;
	new_entry->a2 = a2;
	new_entry->a3 = a3;

	/* set up ip address bitmap */
	new_entry->port_map = 0x0;
	new_entry->members = NULL;

	/* GND */
	new_entry->next = NULL;
	return new_entry;
}

static struct group_member *lookup_ip_group(struct group *entry, uint32 m_ip_addr)
{
	struct group_member *pos = entry->members;
	while(pos){
		unsigned char a0 = IP_GET_LOST_MAPPING(m_ip_addr);
		if(pos->a0 == a0)
			return pos;
		pos = pos->next;
	}
	return NULL;
} 

static struct group_member *lookup_member(struct group *entry, uint32 m_ip_addr, uint32 u_ip_addr)
{
	struct group_member *pos = entry->members;
	while(pos){
		unsigned char a0 = IP_GET_LOST_MAPPING(m_ip_addr);
		if(pos->ip_addr == u_ip_addr && pos->a0 == a0)
			return pos;
		pos = pos->next;
	}
	return NULL;
} 

void remove_member(uint32 m_ip_addr, uint32 u_ip_addr)
{
	unsigned char a0;
	struct group *entry;
	struct group_member *pos;
	struct group_member *del=NULL;

	if(!snooping_enabled)
		return;

#ifdef WIFI_IGMPSNOOP_SUPPORT
	{
		char mac[32], cmd[128];
		if( arpLookUp(inetFmt(htonl(u_ip_addr), s1), mac) != -1){
			sprintf(cmd, "iwpriv ra0 set IgmpDel=%s-%s", inetFmt(htonl(m_ip_addr), s1), mac);
			system(cmd);
		}else{
			log(LOG_WARNING, 0, "Can't find Mac address(%s)", inetFmt(htonl(u_ip_addr), s1));
		}
	}
#endif

	entry = find_entry(m_ip_addr);
	if(!entry){
		log(LOG_WARNING, 0, "*** RT3052: can't find the group [%s].", inetFmt(htonl(m_ip_addr), s1));
		return;
	}

	pos = entry->members;
	if(!pos){
		log(LOG_WARNING, 0, "*** RT3052: group [%s] member list is empty.", inetFmt(htonl(m_ip_addr), s1));
		return;
	}

	printf("%s, %s, %s\n", __FUNCTION__,  inetFmt(htonl(m_ip_addr), s1), inetFmt(htonl(u_ip_addr), s2));

	a0 = IP_GET_LOST_MAPPING(m_ip_addr);
	if(entry->members->ip_addr == u_ip_addr && entry->members->a0 == a0){
		del = pos;
		entry->members = entry->members->next;
	}else{
		while(pos->next){
			if(pos->next->ip_addr == u_ip_addr && pos->next->a0 == a0){
				del = pos->next;
				pos->next = del->next;
				break;
			}
			pos = pos->next;
		}
	}

	if(del){
		free(del);
	}else{
		log(LOG_WARNING, 0, "************************************************");
		log(LOG_WARNING, 0, "*** RT3052: can't delete [%s] in the group [%s].", inetFmt(htonl(u_ip_addr), s1) , inetFmt(htonl(m_ip_addr), s2));
		log(LOG_WARNING, 0, "************************************************");
	}
	update_group_port_map(entry);
	return;
}

/*
 *  ret:
 *	1 == new member is found
 *  0 == existed member
 *	-1 = error occured.
 */
static struct group_member *insert_member(struct group *entry, uint32 m_ip_addr, uint32 u_ip_addr)
{
	int rc = 0;
	struct in_addr 		tmp;
	struct group_member *new_member;
	struct group_member *pos = entry->members;

	if(entry->members != NULL){
		struct group_member *member = lookup_member(entry, m_ip_addr, u_ip_addr);
		if(member){
			log(LOG_DEBUG, 0, "*** RT3052: find the same member [%s] in [%s]. ", inetFmt(u_ip_addr, s1), inetFmt(m_ip_addr, s2));

			/* check if port changed */
			unsigned char port_num;

			tmp.s_addr = htonl(u_ip_addr);
			port_num = portLookUpByIP(inet_ntoa(tmp));
			if(port_num != member->port_num)
				member->port_num = port_num;
			/* update its report flag */
			member->has_report = 1;
			return NULL;
		}
	}

	/* create a new member */
	new_member = (struct group_member *)malloc(sizeof(struct group_member));
	if(!new_member){
			log(LOG_WARNING, 0, "*** RT3052: Out of memory.");
			return NULL;
	}
	tmp.s_addr				= htonl(u_ip_addr);
	new_member->ip_addr 	= u_ip_addr;
	new_member->a0			= IP_GET_LOST_MAPPING( m_ip_addr);
	new_member->port_num 	= portLookUpByIP(inet_ntoa(tmp));
	new_member->has_report	= 1;
	new_member->next		= entry->members;

	/* link onto list */
	entry->members = new_member;

	printf("%s, %s, %s(port%d), \n", __FUNCTION__,  inetFmt(htonl(m_ip_addr), s1), inetFmt(htonl(u_ip_addr), s2), new_member->port_num);

	return new_member;
}

void sweap_no_report_members(void)
{
	struct group *pos = group_list;

	if(!snooping_enabled)
		return;

	while(pos){
		struct group_member *member = pos->members;

		while(member){
			struct group_member *next_backup = NULL;
			if(!member->has_report){
				unsigned int craft_mip = 0x0;

				next_backup = member->next;

				/* craft a multicast ip for remove_member() */
				craft_mip |=  (unsigned long)( IP_MULTICAST_A0(member->a0)) ;
				craft_mip |= ((unsigned long)( IP_MULTICAST_A1(member->a0, pos->a1) ) << 8 );
				craft_mip |= ((unsigned long)(pos->a2) << 16) ;
				craft_mip |= ((unsigned long)(pos->a3) << 24) ;
				
				//log(LOG_WARNING, 0, "*** RT3052: remove [%s] in the group [%s].", inetFmt(htonl(member->ip_addr), s1) , inetFmt(craft_mip, s2));
				remove_member( ntohl(craft_mip), member->ip_addr);
			}

			if(next_backup)
				member = next_backup;
			else
				member = member->next;
		}
		pos = pos->next;
	}
}

void clear_all_entries_report(void)
{
	struct group *pos = group_list;

	if(!snooping_enabled)
		return;

	while(pos){
		struct group_member *member = pos->members;
		while(member){
			member->has_report = 0;
			member = member->next;
		}
		pos = pos->next;
	}
}

void remove_all_members(struct group *entry)
{
	struct group_member *del, *pos = entry->members;

	if(!snooping_enabled)
		return;

	while(pos){
		del = pos;
		pos = pos->next;
		free(del);
	}
	entry->members = NULL;
}

void remove_multicast_ip(uint32 m_ip_addr)
{
	unsigned char a0;
	unsigned char new_portmap = 0;
	struct group *entry = find_entry(m_ip_addr);
	struct group *group_pos = group_list;

	int delete_found = 0;
	struct group_member *mem_pos, *tmp;

	if(!snooping_enabled)
		return;

#ifdef WIFI_IGMPSNOOP_SUPPORT
	{
	char cmd[128];
	sprintf(cmd, "iwpriv ra0 set IgmpDel=%s", inetFmt(htonl(m_ip_addr), s1));
	system(cmd);
	}
#endif
	if(!entry){
		// This entry isn't in the list.
		log(LOG_WARNING, 0, "*** RT3052: can't find group entry [%s].", inetFmt(htonl(m_ip_addr), s1));
		return;
	}

	a0 = IP_GET_LOST_MAPPING(m_ip_addr);

	while(entry->members && entry->members->a0 == a0){
		tmp = entry->members->next;
		free(entry->members);
		entry->members = tmp;
		delete_found = 1;
	}

	mem_pos = entry->members;
	while(mem_pos){
		if(mem_pos->next && mem_pos->next->a0 == a0){
			mem_pos->next = mem_pos->next->next;
			free(mem_pos->next);
			delete_found = 1;
		}else
			new_portmap = new_portmap | (0x1 << mem_pos->port_num);
		mem_pos = mem_pos->next;
	}

	if(delete_found){
		log(LOG_WARNING, 0, "*** RT3052: group entry [%s] found undeleted member.", inetFmt(htonl(m_ip_addr), s1));
	}

	if(entry->members == NULL || new_portmap == 0){
		entry->port_map = 0;

		/* remove from group_list */
		if(group_list == entry){
			group_list = entry->next;
		}else{
			int found = 0;
			while(group_pos->next){
				if(group_pos->next == entry){
					group_pos->next = entry->next;
					found = 1;
					break;
				}
				group_pos = group_pos->next;
			}
			if(!found){
				// impossible
				log(LOG_WARNING, 0, "*** RT3052: can't find group entry [%s].", inetFmt(htonl(m_ip_addr), s1));
				return;
			}
		}

#ifndef CONFIG_RAETH_SPECIAL_TAG
		updateMacTable(entry, DELETED);
#else
		ZeroEntriesBarrier(entry, DELENTRY);
#endif
		// free myself
		free(entry);
	}else if(entry->port_map != new_portmap){
		entry->port_map = new_portmap;
		update_group_port_map(entry);
	}

	return;
}

void remove_all_groups(void)
{
	struct group *del, *pos = group_list;	

	if(!snooping_enabled)
		return;

	/*  TODO: call remove_multicast_ip() instead. */
	while(pos){
		del = pos;
		pos = pos->next;
		del->port_map = 0x0;
#ifndef CONFIG_RAETH_SPECIAL_TAG
		updateMacTable(del, DELETED);
#else
		ZeroEntriesBarrier(del, DELENTRY);
#endif
		remove_all_members(del);
		free(del);
	}
	group_list =  NULL;	
}

static void update_group_port_map(struct group *entry)
{
	unsigned char new_portmap = 0x0;
	struct group_member *pos = entry->members;
	while(pos){
		if(pos->port_num == -1){
			// can't find which port it's in, so opens all ports for it.
			log(LOG_WARNING, 0, "****************************************");
			log(LOG_WARNING, 0, "*** RT3052: can't find %s's port number.", inetFmt(htonl(pos->ip_addr), s1));
			log(LOG_WARNING, 0, "****************************************");
			new_portmap =  (0x5f & ~(WANPORT)); // All Lan ports
			break;
		}else{
			new_portmap = new_portmap | (0x1 << pos->port_num);
			pos = pos->next;
		}
	}
	if(entry->port_map != new_portmap){
#ifndef CONFIG_RAETH_SPECIAL_TAG
		entry->port_map = new_portmap;
		updateMacTable(entry, ZEROED);
#else
		updateMacTable_specialtag(entry, new_portmap, ZEROED);
		entry->port_map = new_portmap;
#endif
	}
}

#if 1
void insert_multicast_ip(uint32 m_ip_addr, uint32 u_ip_addr)
{
	char cmd[128];
	struct group_member *new_member;
	struct group *entry = find_entry(m_ip_addr);

	if(!snooping_enabled)
		return;

	if(!entry){
		// This entry isn't in the list, create one.
		if( (entry = build_entry(m_ip_addr, u_ip_addr)) == NULL)
			return;
#ifdef WIFI_IGMPSNOOP_SUPPORT
		sprintf(cmd, "iwpriv ra0 set IgmpAdd=%s", inetFmt(htonl(m_ip_addr), s1));
		system(cmd);
#endif

#ifdef CONFIG_RAETH_SPECIAL_TAG
		/* build a zero entry barrier for this new MC group in mac tables */
		ZeroEntriesBarrier(entry, ADDENTRY);
#endif
		if(group_list)
			entry->next = group_list;
		group_list = entry;
	}

	new_member = insert_member(entry, m_ip_addr, u_ip_addr);
#ifdef WIFI_IGMPSNOOP_SUPPORT
	if(new_member && new_member->port_num == OTHER_INTERFACE){
		char mac[32];
		if( arpLookUp(inetFmt( htonl(u_ip_addr), mac), s1) != -1){
			sprintf(cmd, "iwpriv ra0 set IgmpAdd=%s-%s", inetFmt(htonl(m_ip_addr), s1), mac);
			system(cmd);
		}else
			log(LOG_WARNING, 0, "Can't find Mac address(%s)", u_ip_addr);
	}
#endif
	update_group_port_map(entry);
	return;
}
#endif
static void create_all_hosts_rule(void)
{
	struct group entry	= {
		.a1 = 0x00,
		.a2 = 0x00,
		.a3 = 0x01,
		.port_map = (0x5f & ~(WANPORT)),	/* All LAN ports */
		.next 		= NULL
	};
	updateMacTable(&entry, ZEROED);
}

static void destory_all_hosts_rule()
{
	struct group entry	= {
		.a1 = 0x00,
		.a2 = 0x00,
		.a3 = 0x01,
		.port_map = 0x0,
		.next = NULL
	};
	updateMacTable(&entry, DELETED);
}

/*
 *	The cpu overhead of this function is low.
 */
static void sync_internal_mac_table(void *argu)
{
	unsigned int value, mac1, mac2, i = 0;

	//timer_setTimer(INTERNAL_SYNC_TIMEOUT, sync_internal_mac_table, NULL);

	reg_write(REG_ESW_TABLE_SEARCH, 0x1);
	while( i < 0x3fe) {
		reg_read(REG_ESW_TABLE_STATUS0, &value);
		if (value & 0x1) { //search_rdy
			if ((value & 0x70) == 0) {
				log(LOG_WARNING, 0, "*** RT3052: found an unused entry (age = 3'b000), please check!");
				reg_write(REG_ESW_TABLE_SEARCH, 0x2); //search for next address
				continue;
			}

			internal_mac_table[i].vidx = (value >> 7) & 0xf;

			// read mac1
			reg_read(REG_ESW_TABLE_STATUS2, &(internal_mac_table[i].mac1));

			// read mac2
			reg_read(REG_ESW_TABLE_STATUS1, &mac2);
			internal_mac_table[i].mac2 = (mac2 & 0xffff);

			internal_mac_table[i].port_map = (value & 0x0007f000) >> 12 ;

			if (value & 0x2) {
				log(LOG_WARNING, 0, "*** RT3052: end of table. %d", i);
				internal_mac_table[i+1].mac1 = END_OF_MAC_TABLE;
				return;
			}
			reg_write(REG_ESW_TABLE_SEARCH, 0x2); //search for next address
			i++;
		}else if (value & 0x2) { //at_table_end
			//log(LOG_WARNING, 0, "*** RT3052: found the last entry (not ready). %d", i);
			internal_mac_table[i].mac1 = END_OF_MAC_TABLE;
			return;
		}else
			usleep(2000);
	}
	internal_mac_table[i].mac1 = END_OF_MAC_TABLE;
	return;
}

#define READ	0
#define WRITE	1
void rt3052_fini(void)
{
	/*
	 *  handle RT3052 registers
	 */
	/* 1011009c */
	unsigned int value;

	if(!snooping_enabled)
		return;

	//value = rareg(READ, 0x1011009c, 0);
	//value = value & 0xF7FFFFFF;
	//rareg(WRITE, 0x1011009c, value);
	/* 10110014 */
	value = rareg(READ, 0x10110014, 0);
	value = value & 0xFF7FFFFF;
	rareg(WRITE, 0x10110014, value);

	/* del 224.0.0.1( 01:00:5e:00:00:01) from mac table */
	destory_all_hosts_rule();

	/*	delete all mac tables */
	remove_all_groups();

	if(esw_fd >= 0)
		close(esw_fd);

#ifdef WIFI_IGMPSNOOP_SUPPORT
	system("iwpriv ra0 set IgmpSnEnable=0");
#endif

}

void rt3052_init(int se)
{
	unsigned int value;

	snooping_enabled = se;

	if(!snooping_enabled)
		return;

	/* 1011009c */
	value = rareg(READ, 0x1011009c, 0);
	//value = value | 0x08000000;
	value = value & 0xE7FFFFFF;
	rareg(WRITE, 0x1011009c, value);

	/* 10110014 */
	value = rareg(READ, 0x10110014, 0);
	value = value | 0x00800000;
	rareg(WRITE, 0x10110014, value);

	esw_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (esw_fd < 0) {
		perror("socket");
		rt3052_fini();
		exit(0);
	}

	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = (char *)&reg;

	/* add 224.0.0.1( 01:00:5e:00:00:01) to mac table */
	create_all_hosts_rule();
	sync_internal_mac_table(NULL);

#ifdef WIFI_IGMPSNOOP_SUPPORT
	system("iwpriv ra0 set IgmpSnEnable=1");
#endif

}


static int arpLookUp(char *ip, char *arp)
{
	char buf[256];
	FILE *fp = fopen("/proc/net/arp", "r");
	if(!fp){
		log(LOG_ERR, 0, "*** RT3052: no proc fs!");
		return -1;
	}

	while(fgets(buf, sizeof(buf), fp)){
		char ip_entry[32], hw_type[8], flags[8], hw_address[32];
		sscanf(buf, "%s %s %s %s", ip_entry, hw_type, flags, hw_address);
		if(!strcmp(ip, ip_entry)){
			strcpy(arp, hw_address);
			fclose(fp);
			if(!strcmp("00:00:00:00:00:00", hw_address)){
				return -1;
			}
			return 0;
		}
	}

	fclose(fp);
	return -1;
}

static inline int reg_read(int offset, int *value)
{
    reg.off = offset;
    if (-1 == ioctl(esw_fd, RAETH_ESW_REG_READ, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
    *value = reg.val;
    return 0;
}

static inline int reg_write(int offset, int value)
{
    reg.off = offset;
    reg.val = value;
    if (-1 == ioctl(esw_fd, RAETH_ESW_REG_WRITE, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
    return 0;
}

static inline wait_switch_done(void)
{
	int i, value;
	for (i = 0; i < 20; i++) {
		reg_read(REG_ESW_WT_MAC_AD0, &value);
		if (value & 0x2) {	//w_mac_done
			//printf("done.\n");
			break;
		}
		usleep(1000);
	}
	if (i == 20)
		log(LOG_WARNING, 0, "*** RT3052: timeout.");
}

static void ZeroEntriesBarrier(struct group *entry, int mode)
{
	unsigned char lanport[] = LANPORT_RANGE;

	int i, value;
	char wholestr[13];
	char tmpstr[9];

	sprintf(wholestr, "%s%02x%02x%02x", "01005e", entry->a1, entry->a2, entry->a3);

	strncpy(tmpstr, wholestr, 8);
	tmpstr[8] = '\0';

	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD2, value);
	strncpy(tmpstr, &wholestr[8], 4);
	tmpstr[4] = '\0';
	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD1, value);

	for(i=0; i < sizeof(lanport)/sizeof(unsigned char); i++){
		value = 0x1;			//w_mac_cmd
		if(mode == ADDENTRY)
			value |= (7 << 4);	//w_age_field
		value |= (lanport[i] << 7);	//w_index
		reg_write(REG_ESW_WT_MAC_AD0, value);
		wait_switch_done();
	}
}

static void ZeroEntry(struct group *entry, int port, int mode)
{
	int i, value;
	char wholestr[13];
	char tmpstr[9];

	sprintf(wholestr, "%s%02x%02x%02x", "01005e", entry->a1, entry->a2, entry->a3);

	strncpy(tmpstr, wholestr, 8);
	tmpstr[8] = '\0';

	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD2, value);
	strncpy(tmpstr, &wholestr[8], 4);
	tmpstr[4] = '\0';
	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD1, value);


	/* update the zero entry */
	value = 0x1;				//w_mac_cmd
	if(mode == ADDENTRY)
		value |= (7 << 4);		//w_age_field
	value |= (port << 7);			//w_index
	reg_write(REG_ESW_WT_MAC_AD0, value);
	wait_switch_done();
	return;
}

static void updateMacTable_specialtag(struct group *entry, unsigned char new_portmap, int delay_delete)
{
	int i;
	unsigned char lanport[] = LANPORT_RANGE;

	for(i=0; i < sizeof(lanport)/sizeof(unsigned char); i++){
		unsigned char old_bit, new_bit;

		old_bit = entry->port_map & (0x1 << lanport[i]);
		new_bit = new_portmap  & (0x1 << lanport[i]);

		if(old_bit == new_bit)
			continue;
		
		if(old_bit == 0 /* new_bit == 0x1 */){
			ZeroEntry(entry, lanport[i], DELENTRY);
		}else{
			ZeroEntry(entry, lanport[i], ADDENTRY);
		}
	}
}

/*
 * ripped from user/rt2880/switch/switch.c
 */
static void updateMacTable(struct group *entry, int delay_delete)
{
	int i, value;
	char wholestr[13];
	char tmpstr[9];

	sprintf(wholestr, "%s%02x%02x%02x", "01005e", entry->a1, entry->a2, entry->a3);

	strncpy(tmpstr, wholestr, 8);
	tmpstr[8] = '\0';

	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD2, value);
	strncpy(tmpstr, &wholestr[8], 4);
	tmpstr[4] = '\0';
	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD1, value);

	value = 0;
	if(entry->port_map){
		/*
		 * force all mulicast addresses to bind with CPU.
		 */
		value = value | (0x1 << 18);

		/*
		 * fill the port map
		 */
		value = value | (entry->port_map & (0x7f)) << 12;
		value += (7 << 4); //w_age_field
		value += (LAN_VLAN_IDX << 7); //w_age_field
		value += 1;				//w_mac_cmd
		reg_write(REG_ESW_WT_MAC_AD0, value);
		wait_switch_done();

		/*
		 * new an additional entry for IGMP Inquery/Report on WAN.
		 */
		if(WANPORT){
			value = (WANPORT << 12);
			value |= (1 << 18);
			value |= (7 << 4);		//w_age_field
			value |= (WAN_VLAN_IDX << 7);		//w_index
			value |= 1;				//w_mac_cmd
			reg_write(REG_ESW_WT_MAC_AD0, value);
			wait_switch_done();
		}
	}else{
		if(delay_delete == ZEROED){
			/*
			 * Can't delete this entry too early.
			 *
			 * Because multicast packets from WAN may still come even receiver on LAN has left, and
			 * at the same time the kernel routing rule is not yet deleted by igmpproxy.
			 *
			 * If we delete mac entry earier than deleting routing rule (by igmpproxy),
			 * these packets would be forwarded to "br0" and then flood on eth2.1(vlan1) due to our 
			 * default policy -- "Broadcast if not found". So we may see flooding packets on
			 * LAN until the kernel routing rule is deleted.
			 *
			 * So we keep the mac entry alive to avoid the our default policy until the igmp group
			 * is actually eliminated.
			 */

			/*
			 * zero the entry
			 */
			value |= (7 << 4);		//w_age_field, keep it alive
			value |= 1;				//w_mac_cmd
			reg_write(REG_ESW_WT_MAC_AD0, value);
			wait_switch_done();
		}else if (delay_delete == DELETED){
			/*
			 * delete the entry
			 */
			value |= 1;				//w_mac_cmd
			value |= (LAN_VLAN_IDX << 7); //w_age_field
			reg_write(REG_ESW_WT_MAC_AD0, value);
			wait_switch_done();

			/*
			 * delete the additional entry on WAN.
			 */
			value = 0;
			value |= (WAN_VLAN_IDX  << 7);		//w_index
			value |= 1;				//w_mac_cmd
			reg_write(REG_ESW_WT_MAC_AD0, value);
			wait_switch_done();
		}
	}
}


#define RALINK_WIFI_INTF	"ra0"
#define RTPRIV_IOCTL_GET_MAC_TABLE          (SIOCIWFIRSTPRIV + 0x0F)
#define RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT   (SIOCIWFIRSTPRIV + 0x1F)

typedef union _MACHTTRANSMIT_SETTING {
	struct  {
		unsigned short  MCS:7;  // MCS
		unsigned short  BW:1;   //channel bandwidth 20MHz or 40 MHz
		unsigned short  ShortGI:1;
		unsigned short  STBC:2; //SPACE
		unsigned short  eTxBF:1;
		unsigned short  rsv:1;
		unsigned short  iTxBF:1;
		unsigned short  MODE:2; // Use definition MODE_xxx.
	} field;
	unsigned short      word;
} MACHTTRANSMIT_SETTING;

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
	unsigned int			LastRxRate;
	int						StreamSnr[3];
	int						SoundingRespSnr[3];
#if 0
	short					TxPER;
	short					reserved;
#endif
} RT_802_11_MAC_ENTRY;

#if defined (CONFIG_RT2860V2_AP_WAPI) || defined (CONFIG_RT3090_AP_WAPI) || \
    defined (CONFIG_RT3572_AP_WAPI) || defined (CONFIG_RT5392_AP_WAPI) || \
    defined (CONFIG_RT5572_AP_WAPI) || defined (CONFIG_RT5592_AP_WAPI) || \
    defined (CONFIG_RT3593_AP_WAPI) || defined (CONFIG_RT3680_iNIC_AP_WAPI)
#define MAX_NUMBER_OF_MAC               96
#else
#define MAX_NUMBER_OF_MAC               32 // if MAX_MBSSID_NUM is 8, this value can't be larger than 211
#endif

typedef struct _RT_802_11_MAC_TABLE {
	unsigned long            Num;
	RT_802_11_MAC_ENTRY      Entry[MAX_NUMBER_OF_MAC]; //MAX_LEN_OF_MAC_TABLE = 32
} RT_802_11_MAC_TABLE;

int WiFiSTALookUPByMac(unsigned int mac1, unsigned int mac2)
{
	int i, s;
	struct iwreq iwr;
	RT_802_11_MAC_TABLE table = {0};

	s = socket(AF_INET, SOCK_DGRAM, 0);
	strncpy(iwr.ifr_name, RALINK_WIFI_INTF, IFNAMSIZ);
	iwr.u.data.pointer = (caddr_t) &table;
	if (s < 0) {
		log(LOG_WARNING, 0, "ioctl sock failed!");
		return 0;
	}
#if 1 //def CONFIG_RT2860V2_AP_V24_DATA_STRUCTURE
	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, &iwr) < 0) {
		log(LOG_WARNING, 0, "ioctl -> RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT failed!");
#else
	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) < 0) {
		log(LOG_WARNING, 0, "ioctl -> RTPRIV_IOCTL_GET_MAC_TABLE failed!");
#endif
		close(s);
		return 0;
	}
	close(s);

	for (i = 0; i < table.Num; i++) {
		unsigned int c_mac1 = 0, c_mac2 = 0;
#ifdef CONFIG_RT2860V2_AP_TXBF
		RT_802_11_MAC_ENTRY *pe = &(table.Entry[i]);
		c_mac1 = (pe->Addr[3]) | (pe->Addr[2] << 8) | (pe->Addr[1] << 16) | (pe->Addr[0] << 24);
		c_mac2 = pe->Addr[5] | (pe->Addr[4] << 8);
#else
		c_mac1 = (table.Entry[i].Addr[3]) | (table.Entry[i].Addr[2] << 8) | (table.Entry[i].Addr[1] << 16) | (table.Entry[i].Addr[0] << 24);
		c_mac2 = (table.Entry[i].Addr[5]) | (table.Entry[i].Addr[4] << 8);
#endif
		if(c_mac1 == mac1 && c_mac2 == mac2)
 			return 1;
	}
	return 0;
}

static int portLookUpByMac(char *mac)
{
	unsigned long long int  mac1, mac2;
	unsigned int i = 0, mac_iter;
	char mac_entry1[16], mac_entry2[16];

	memset(mac_entry1, 0, sizeof(mac_entry1));
	memset(mac_entry2, 0, sizeof(mac_entry2));

	strncpy(mac_entry1, mac, 8);
	strncpy(mac_entry2, &mac[8], 4);

	mac1 = strtoll(mac_entry1, 0, 16);
	mac2 = strtol(mac_entry2, 0, 16);

	mac_iter = internal_mac_table[i].mac1;
	while(i < 0x3fe && mac_iter != END_OF_MAC_TABLE) {
		//log(LOG_WARNING, 0, "look for [%s] (%d)%08x %04x, %08x %04x\n", mac, i, internal_mac_table[i].mac1, internal_mac_table[i].mac2, mac1, mac2);

		if(internal_mac_table[i].vidx != LAN_VLAN_IDX)
			goto next_entry;

		if(	internal_mac_table[i].mac1 == mac1 &&
			internal_mac_table[i].mac2 == mac2){
			switch( internal_mac_table[i].port_map ){
			case 0x1:
				return 0;
			case 0x2:
				return 1;
			case 0x4:
				return 2;
			case 0x8:
				return 3;
			case 0x10:
				return 4;
			case 0x40:	/* CPU Only */
				break;
			default:
				log(LOG_WARNING, 0, "No/Multi ports found:%x", internal_mac_table[i].port_map);
				return -1;
			}
		}
next_entry:
		i++;
		mac_iter = internal_mac_table[i].mac1;
	}

#ifdef SEARCH_CLIENT_IN_WIFI_MACTABLE
	if(WiFiSTALookUPByMac(mac1, mac2)){
		return OTHER_INTERFACE;
	}else
#endif
		return -1;
}

static void strip_mac(char *mac)
{
	char *pos = mac, *strip = mac;
	while(*pos != '\0'){
		if(*pos == ':')
			pos++;
		else{
			*strip = *pos;
			strip++;
			pos++;
		}
	}
	*strip = '\0';
}

/*
 * send a udp packet to target if its mac address can't be found.
 */
static void sendUDP(char *ip)
{
	int socket_fd;
	struct sockaddr_in user_addr;
	char buf[16];

	memset(&user_addr, 0, sizeof(struct sockaddr_in));
	user_addr.sin_family	= AF_INET;
	user_addr.sin_port		= htons(53);
	user_addr.sin_addr.s_addr = inet_addr(ip);

	if((socket_fd = socket(AF_INET,SOCK_DGRAM, 0)) == -1) {
		log(LOG_WARNING, 0, "*** RT3052: socket error");
		return;
	}
	strcpy(buf, "arp please");
	sendto(socket_fd, buf, strlen(buf), 0, (struct sockaddr *)&user_addr, sizeof(user_addr));
	close(socket_fd);

	return ;
}

/*
 * Unfortunately IGMP packets from linux raw socket layer don't have layer2 header, but we
 * can get the mac address from ARP table. If no matched entry found in the table, then we send a dummy
 * udp packet to target, and wait for its ARP reply.
 *
 * Hope we won't cause target's IDS alarm.
 */
static int portLookUpByIP(char *ip)
{
	int rc;
	char mac[32];
	if( arpLookUp(ip, mac) == -1){
		log(LOG_WARNING, 0, "*** RT3052: Warning, Can't get mac address for %s", ip);

		// send an udp then wait.
		sendUDP(ip);
		usleep(20000);	
		if(arpLookUp(ip, mac) == -1){
			log(LOG_WARNING, 0, "*** RT3052: Give up for %s", ip);
			// means flooding.
			return -1;
		}
	}
	strip_mac(mac);
	sync_internal_mac_table(NULL);
	rc = portLookUpByMac(mac);

	return rc;
}

void sigUSR1Handler(int signo)
{
	if(!snooping_enabled)
		return;

	dump_entry();
	dump_table();
}
