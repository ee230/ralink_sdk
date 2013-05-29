/* This ALG just used by Ralink HNAT */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/in.h>
#include <linux/udp.h>
#include <linux/netfilter.h>

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_tuple.h>
#include <net/netfilter/nf_conntrack_expect.h>
#include <net/netfilter/nf_conntrack_ecache.h>
#include <net/netfilter/nf_conntrack_helper.h>

MODULE_AUTHOR("Steven <steven_liu@ralinktech.com.tw>");
MODULE_DESCRIPTION("connection tracking white list");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ip_conntrack_hnat");

#define MAX_PORTS 1
//#define HTTP_PORT 80
static unsigned short ports[]={/*HTTP_PORT*/};

unsigned int (*nf_nat_hnat_hook)(struct sk_buff **pskb,
				 enum ip_conntrack_info ctinfo,
				 struct nf_conntrack_expect *exp) __read_mostly;
EXPORT_SYMBOL_GPL(nf_nat_hnat_hook);

static int hnat_help(struct sk_buff **pskb,
		     unsigned int protoff,
		     struct nf_conn *ct,
		     enum ip_conntrack_info ctinfo)
{
    return NF_ACCEPT;
}

static struct nf_conntrack_helper Hnat[MAX_PORTS] __read_mostly;
static char hnat_names[MAX_PORTS][sizeof("Hnat-65535")] __read_mostly;

static void nf_conntrack_hnat_fini(void)
{
	int i;

	for (i = 0; i < MAX_PORTS ; i++) {
			nf_conntrack_helper_unregister(&Hnat[i]);
	}
}

static int __init nf_conntrack_hnat_init(void)
{
	int i, ret;
	char *tmpname;

	for (i = 0; i < MAX_PORTS; i++) {

			Hnat[i].tuple.dst.protonum = IPPROTO_TCP;
			Hnat[i].tuple.src.u.udp.port = htons(ports[i]);
			Hnat[i].mask.src.l3num = 0xFFFF;
			Hnat[i].mask.dst.protonum = 0xFF;
			Hnat[i].mask.src.u.udp.port = htons(0xFFFF);
			Hnat[i].max_expected = 1;
			Hnat[i].timeout = 5 * 60; /* 5 minutes */
			Hnat[i].me = THIS_MODULE;
			Hnat[i].help = hnat_help;

			tmpname = &hnat_names[i][0];
			sprintf(tmpname, "HNAT");
			Hnat[i].name = tmpname;

			ret = nf_conntrack_helper_register(&Hnat[i]);
			if (ret) {
				printk("nf_ct_hnat: failed to register helper "
				       "for pf: %u port: %u\n",
					Hnat[i].tuple.src.l3num, ports[i]);
				nf_conntrack_hnat_fini();
				return ret;
			}
	}
	return 0;
}

module_init(nf_conntrack_hnat_init);
module_exit(nf_conntrack_hnat_fini);
