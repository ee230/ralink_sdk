#ifndef _CMDPARSE_H_
#define _CMDPARSE_H_
/************************************************************************
 *
 *	Copyright (C) 2006 Trendchip Technologies, Corp.
 *	All Rights Reserved.
 *
 * Trendchip Confidential; Need to Know only.
 * Protected as an unpublished work.
 *
 * The computer program listings, specifications and documentation
 * herein are the property of Trendchip Technologies, Co. and shall
 * not be reproduced, copied, disclosed, or used in whole or in part
 * for any reason without the prior express written permission of
 * Trendchip Technologeis, Co.
 *
 *************************************************************************/

typedef	struct {
	char *name;		
	int	(*func)(int argc,char *argv[],void *p);
	int	flags;	
	int	argcmin;
	char *argc_errmsg;	
} cmds_t;

int cmd_register(cmds_t *cmds_p);
int cmd_unregister(cmds_t *cmds_p);

#endif /* _CMDPARSE_ */
