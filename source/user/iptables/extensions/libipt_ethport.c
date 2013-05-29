/*
 * IP tables module for matching the value of the incoming ether port
 * for Ralink SoC platform.
 *
 * (C) 2009 by yy_huang@ralinktech.com.tw
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <iptables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ipt_ethport.h>

static void help(void) 
{
	printf(
"Ethport match v%s options\n"
"[!] --portnum value		Match ether port number with numerical value\n"
"  		                This value can be in decimal (ex: 2)\n"
"               		or in hex (ex: 0x2)\n"
				, IPTABLES_VERSION
);
}

static struct option opts[] = {
	{ "portnum", 1, 0, 'P' },
	{ 0 }
};

static void
parse_ethport(const char *s, struct ipt_ethport_info *dinfo)
{
	unsigned int portnum;
       
	if (string_to_number(s, 0, 255, &portnum) == -1)
		exit_error(PARAMETER_PROBLEM,
			   "Invalid portnum `%s'\n", s);

	if (portnum > IPT_ETHPORT_MAX)
		exit_error(PARAMETER_PROBLEM,
			   "portnum `%d` out of range\n", portnum);

    	dinfo->portnum = (u_int8_t )portnum;
    	return;
}


static int
parse(int c, char **argv, int invert, unsigned int *flags,
      const struct ipt_entry *entry,
      unsigned int *nfcache,
      struct ipt_entry_match **match)
{
	struct ipt_ethport_info *dinfo	= (struct ipt_ethport_info *)(*match)->data;

	switch (c) {
	case 'P':
		if (*flags)
			exit_error(PARAMETER_PROBLEM,
			           "DSCP match: Only use --portnum ONCE!");
		check_inverse(optarg, &invert, &optind, 0);
		parse_ethport(argv[optind-1], dinfo);
		if (invert)
			dinfo->invert = 1;
		*flags = 1;
		break;

	default:
		return 0;
	}

	return 1;
}

static void
final_check(unsigned int flags)
{
	if (!flags)
		exit_error(PARAMETER_PROBLEM,
		           "Ethport match: Parameter --portnum is required");
}

static void
print_ethport(u_int8_t portnum, int invert, int numeric)
{
	if (invert)
		fputc('!', stdout);

 	printf("0x%02x ", portnum);
}

/* Prints out the matchinfo. */
static void
print(const struct ipt_ip *ip,
      const struct ipt_entry_match *match,
      int numeric)
{
	const struct ipt_ethport_info *dinfo = (const struct ipt_ethport_info *)match->data;
	printf("Ethport match ");
	print_ethport(dinfo->portnum, dinfo->invert, numeric);
}

/* Saves the union ipt_matchinfo in parsable form to stdout. */
static void
save(const struct ipt_ip *ip, const struct ipt_entry_match *match)
{
	const struct ipt_ethport_info *dinfo =
		(const struct ipt_ethport_info *)match->data;

	printf("--portnum ");
	print_ethport(dinfo->portnum, dinfo->invert, 1);
}

static struct iptables_match ethport = { 
	.next 		= NULL,
	.name 		= "ethport",
	.version 	= IPTABLES_VERSION,
	.size 		= IPT_ALIGN(sizeof(struct ipt_ethport_info)),
	.userspacesize	= IPT_ALIGN(sizeof(struct ipt_ethport_info)),
	.help		= &help,
	.parse		= &parse,
	.final_check	= &final_check,
	.print		= &print,
	.save		= &save,
	.extra_opts	= opts
};

void _init(void)
{
	register_match(&ethport);
}
