/**************************************
 ecmh - Easy Cast du Multi Hub
 by Jeroen Massar <jeroen@unfix.org>
***************************************
 $Author: yy $
 $Id: grpint.h,v 1.2 2011-05-23 07:19:23 yy Exp $
 $Date: 2011-05-23 07:19:23 $
**************************************/

/* The node used to hold the interfaces which a group joined */
struct grpintnode
{
	unsigned int		ifindex;		/* The interface */
	struct list		*subscriptions;		/* Subscriber list */
};

struct grpintnode *grpint_create(const struct intnode *interface);
void grpint_destroy(struct grpintnode *grpintn);
struct grpintnode *grpint_find(const struct list *list, const struct intnode *interface);
bool grpint_refresh(struct grpintnode *grpintn, const struct in6_addr *ipv6, unsigned int mode);
bool grpint_refresh_snooping(struct grpintnode *grpintn, struct in6_addr *mca, const struct in6_addr *ipv6, unsigned char *src_mac, unsigned int mode, int any);

