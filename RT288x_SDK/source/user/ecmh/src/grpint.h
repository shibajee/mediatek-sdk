/**************************************
 ecmh - Easy Cast du Multi Hub
 by Jeroen Massar <jeroen@unfix.org>
***************************************
 $Author: red.hung $
 $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/ecmh/src/grpint.h#1 $
 $Date: 2014/05/07 $
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

