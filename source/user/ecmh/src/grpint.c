/**************************************
 ecmh - Easy Cast du Multi Hub
 by Jeroen Massar <jeroen@unfix.org>
***************************************
 $Author: yy $
 $Id: grpint.c,v 1.3 2011-05-23 12:52:20 yy Exp $
 $Date: 2011-05-23 12:52:20 $
**************************************/

#include "ecmh.h"

struct grpintnode *grpint_create(const struct intnode *interface)
{
	struct grpintnode *grpintn = malloc(sizeof(*grpintn));

	if (!grpintn) return NULL;

	/* Fill her in */
	grpintn->ifindex = interface->ifindex;

	/* Setup the list */
	grpintn->subscriptions = list_new();
	grpintn->subscriptions->del = (void(*)(void *))subscr_destroy;

	/* All okay */
	return grpintn;
}

void grpint_destroy(struct grpintnode *grpintn)
{
	if (!grpintn) return;

	/* Empty the subscriber list */
	list_delete_all_node(grpintn->subscriptions);

	/* Free the node */
	free(grpintn);
}

struct grpintnode *grpint_find(const struct list *list, const struct intnode *interface)
{
	struct grpintnode	*grpintn;
	struct listnode		*ln;

	LIST_LOOP(list, grpintn, ln)
	{
		if (grpintn->ifindex == interface->ifindex) return grpintn;
	}
	return NULL;
}

/*
 * grpintn	= The GroupInterface node
 * ipv6		= Source IPv6 address
 *		  ff3x::/96  : The source IPv6 address that wants to (not) receive this S<->G channel
 * mode		= MLD2_MODE_IS_INCLUDE/MLD2_MODE_IS_EXCLUDE
 */
bool grpint_refresh(struct grpintnode *grpintn, const struct in6_addr *ipv6, unsigned int mode)
{
	struct subscrnode *subscrn;

	/* Find our beloved group */
	subscrn = subscr_find(grpintn->subscriptions, ipv6);

	/* Exclude all ? -> Unsubscribe */
	if (	mode == MLD2_MODE_IS_EXCLUDE &&
		IN6_IS_ADDR_UNSPECIFIED(ipv6))
	{
		/*
		 * Don't refresh the subscription,
		 * causing it to be removed later on
		 * because of a timeout
		 */
		return true;
	}

	if (!subscrn)
	{
		/* Create the subscr node */
		subscrn = subscr_create(ipv6, mode, -1);

		/* Add the group to the list */
		if (subscrn)
		{
			listnode_add(grpintn->subscriptions, (void *)subscrn);
		}
	}

	if (!subscrn) return false;

	/* Mode still the same? */
	if (mode != subscrn->mode)
	{
		dolog(LOG_DEBUG, "grpint_refresh() - Mode changed from %u to %u\n", subscrn->mode, mode);
		subscrn->mode = mode;
	}

	/* Refresh it */
	subscrn->refreshtime = time(NULL);

	/* All Okay */
	return true;
}

#ifdef MLD_SNOOPING
bool grpint_refresh_snooping(struct grpintnode *grpintn, struct in6_addr *mca, const struct in6_addr *ipv6, unsigned char *src_mac, unsigned int mode, int any)
{
	struct subscrnode *subscrn;
	int portnum = -1;

	/* Find our beloved group */
	subscrn = subscr_find(grpintn->subscriptions, ipv6);

	/* Exclude all ? -> Unsubscribe */
	if (	mode == MLD2_MODE_IS_EXCLUDE &&
		IN6_IS_ADDR_UNSPECIFIED(ipv6))
	{
		/*
		 * Don't refresh the subscription,
		 * causing it to be removed later on
		 * because of a timeout
		 */
		return true;
	}

	sync_internal_mac_table(NULL);
	portnum = portLookUpByMac(src_mac);

	if (!subscrn)
	{
		/* Create the subscr node */
		subscrn = subscr_create(ipv6, mode, portnum);

		/* Add the group to the list */
		if (!subscrn)
			return false;

		listnode_add(grpintn->subscriptions, (void *)subscrn);
#ifdef WIFI_IGMPSNOOP_SUPPORT
		if(portnum == OTHER_INTERFACE){
			char cmd[1024], buf[128];

			memset(buf, 0, sizeof(buf));
			inet_ntop(AF_INET6, mca, buf, sizeof(buf));

			sprintf(cmd, "iwpriv ra0 set IgmpAdd=%s-%s", buf, src_mac),
			dolog(LOG_DEBUG, ">>>>==== %s \n", cmd);
			system(cmd);
		}
#endif
		if(grpintn->ifindex != g_conf->upstream_id)
			update_group_port_map(mca, MACTABLE_ENTRY_DONT_CARE);
	}else{
		/* ralink: dont handle upstrem interface */
		if(grpintn->ifindex != g_conf->upstream_id){
			if(portnum == -1){
				subscrn->portnum = -1;
			}else{
				if(portnum != subscrn->portnum ){
					/* new bit*/
					subscrn->portnum = portnum;
					update_group_port_map(mca, MACTABLE_ENTRY_DONT_CARE);
				}
			}
		}
	}

	if(any)
		subscrn->any_address_flag = 1;

	/* if (!subscrn)
		return false; */

	/* Mode still the same? */
	if (mode != subscrn->mode)
	{
		dolog(LOG_DEBUG, "grpint_refresh() - Mode changed from %u to %u\n", subscrn->mode, mode);
		subscrn->mode = mode;
	}

	/* Refresh it */
	subscrn->refreshtime = time(NULL);

	/* All Okay */
	return true;
}
#endif

