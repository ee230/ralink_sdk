#ifndef __QOS_H__
#define __QOS_H__

struct entry_s{
	char *name;
	char *value;
};

#define QOS_PROFILE_ENTRYS_MAX		64			/* maximum rule count */

void formDefineQoS(void);
inline void QoSInit(void);

#endif
