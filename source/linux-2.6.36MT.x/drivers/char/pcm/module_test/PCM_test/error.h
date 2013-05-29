/*
 * error.h
 *
 *  Created on: 2008/10/24
 *      Author: pork
 */

#ifndef ERROR_H_
#define ERROR_H_
/*variable type definition*/
#ifndef UINT32
#define UINT32
typedef unsigned long int uint32; 		/* 32-bit unsigned integer      */
#endif

#ifndef UINT16
#define UINT16
typedef unsigned short uint16;          /* 16-bit unsigned integer      */
#endif

#ifndef UINT8
#define UINT8
typedef unsigned char uint8;            /* 8-bit unsigned integer       */
#endif

#ifndef UINT4
#define UINT4
typedef unsigned long uint4;            /* 8-bit unsigned integer       */
#endif
#define MEOMORY_LACK 0
#define MIN_FRAME_COUNT 0
/*Err msg*/
#define BUFFER_MORE_THAN_CHANNEL 2
#define TX_SAMPLESIZE_RANGE_EXCCEDED 3
#define DESC_READY 1
/*config err */
#define CONFIG_FRAMECOUNT_ERR 1
#define CONFIG_BYTEORDER_ERR 2
#define CONFIG_BITORDER_ERR 3
#define CONFIG_FSEDGE_ERR 4
#define CONFIG_FSLEN_ERR 5
#define CONFIG_SAMPLECLOCK_ERR 6
#define CONFIG_BITCLOCK_ERR 7
#define CONFIG_PCMMODE_ERR 8
#define CONFIG_CHNUM_ERR 9
#define CONFIG_RX_SAMPLESIZE_ERR 10
#define CONFIG_CHANNEL_ERR 11

#if 0
#define PCMDEBUGMSG(dbgs,format) 	do { } while(0)
#define PCMDEBUGMSGW(dbgs,format)	do { } while(0)
#define PCMDEBUGMSGB(dbgs,format)	do { } while(0)
#define PCMDEBUGMSGL(dbgs,format)	do { } while(0)
#else

#define DBG_LEN 2000
#define NO_DBG 0
#define CATEGORY_CONFIG 1
#define CATEGORY_TX		(1 << 1)
#define CATEGORY_RX		(1 << 2)
#define DBG_LEVEL_1			1
#define DBG_LEVEL_2			(1<<1)
#define DBG_LEVEL_3			(1<<2)
#define pcm_dbg_pline(x)		printk("%s", x)
#define pcm_dbg_plineint(x, y)		printk("%s%d", x, (int) y)
#define pcm_dbg_plineb(x, y)	printk("%s%02x", x, (uint8) y)
#define pcm_dbg_plinew(x, y)	printk("%s%04x", x, (uint16) y)
#define pcm_dbg_plinel(x, y)	printk("%s%08lx", x, (uint32) y)
#define PCMDEBUGMSG(dbgCat,dbgLevel,format) if((dbgCat&curConfigNode.debugCategory) && (dbgLevel&curConfigNode.debugLevel)) pcm_dbg_pline format
#define PCMDEBUGMSGINT(dbgCat,dbgLevel,format) if((dbgCat&curConfigNode.debugCategory) && (dbgLevel&curConfigNode.debugLevel)) pcm_dbg_plineint format
#define PCMDEBUGMSGB(dbgCat,dbgLevel,format) if((dbgCat&curConfigNode.debugCategory) && (dbgLevel&curConfigNode.debugLevel)) pcm_dbg_plineb format
#define PCMDEBUGMSGW(dbgCat,dbgLevel,format) if((dbgCat&curConfigNode.debugCategory) && (dbgLevel&curConfigNode.debugLevel)) pcm_dbg_plinew format
#define PCMDEBUGMSGL(dbgCat,dbgLevel,format) if((dbgCat&curConfigNode.debugCategory) && (dbgLevel&curConfigNode.debugLevel)) pcm_dbg_plinel format
#endif

extern int enqueue(uint8* rxBuf);
extern unsigned char* dequeue(void);
extern int queueSizeGet(void);
extern void SLIC_reset(int reset);

#endif /* ERROR_H_ */
