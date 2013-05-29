#ifndef	__CLS_IO_H__
#define	__CLS_IO_H__

#define GET_ALLFLOW_INFO	   		(0x01)
#define DBGPRN_ENABLE	   			(0x02)
#define DBGPRN_DISABLE	   			(0x03)
#define INBAND_QOS_ENABLE				(0x04)
#define OUTBAND_QOS_ENABLE				(0x05)
#define INBAND_QOS_DISABLE				(0x06)
#define OUTBAND_QOS_DISABLE				(0x07)

#define RA_CLASSIFIER_DEVNAME		"cls0"

#define MAX_LIST_FLOW				40

#define VOICE			1
#define VIDEO			2
#define BEST_EFFORT		3
#define BACKGROUND		4
#define UNKNOWN_TYPE	0

#define IDENT_BY_PLEN		1
#define IDENT_BY_ASFHDR		2
#define IDENT_BY_TSHDR		3

typedef struct {
	unsigned long sip;
    unsigned long dip;
    unsigned long proto;
    unsigned long sport;
    unsigned long dport;
    long data_rate;
    int fid;
    int ident_method;
    int type;
}finfo_t;

typedef struct {
	int listlen;
	finfo_t* plist;
} classifier_cmd_t;


#endif
