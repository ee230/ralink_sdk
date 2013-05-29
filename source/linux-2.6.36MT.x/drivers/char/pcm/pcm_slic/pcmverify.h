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
#ifndef PCMVERIFY_H
#define PCMVERIFY_H

/*variable type definition*/
#ifndef UINT32
#define UINT32
typedef unsigned long int uint32; 		 //32-bit unsigned integer
#endif

#ifndef UINT16
#define UINT16
typedef unsigned short uint16;         //  16-bit unsigned integer
#endif

#ifndef UINT8
#define UINT8
typedef unsigned char uint8;            // 8-bit unsigned integer
#endif

#ifndef UINT4
#define UINT4
typedef unsigned long uint4;            // 8-bit unsigned integer
#endif

#ifndef VPint
#define VPint			*(volatile unsigned long int *)
#endif

#ifndef timerVlrGet
#define timerVirGet
#define CR_TIMER_BASE  		0xBFBF0100
#define CR_TIMER0_VLR    	(CR_TIMER_BASE + 0x08)
#define timerVlrGet(timer_no,val) (val)=*(volatile uint32 *)(CR_TIMER0_VLR+timer_no*0x08)
#endif
/*Register addr base*/
#define OPERATIONAL_BASE 				0xbfbd0000
#define AHB_BUS_ADDR 				0xbfb000b8
#define IMR 0x28
#define TX_POLLING_OFF 0x2c
#define RX_POLLING_OFF 0x30
#define TX_RX_DMA_CTRL 0x40
/*Register number*/
enum pcm_reg_num{
	PCM_INTFACE_Ctrl=0,//value=0x0100_040a
	PCM_TX_TIME_SLOT_CFG0, // value=0x0008_0000
	PCM_TX_TIME_SLOT_CFG1, // value=0x0018_0010
	PCM_TX_TIME_SLOT_CFG2, // value=0x0028_0020
	PCM_TX_TIME_SLOT_CFG3,// value=0x0038_0030
	PCM_RX_TIME_SLOT_CFG0, // value=0x0008_0000
	PCM_RX_TIME_SLOT_CFG1, // value=0x0018_0010
	PCM_RX_TIME_SLOT_CFG2, // value=0x0028_0020
	PCM_RX_TIME_SLOT_CFG3, // value=0x0038_0030
	PCM_ISR,  			// value=0x0000_0000
	PCM_IMR,  				// value=0x0000_0000
	PCM_TX_POLLING_DEMAND,	// value=0x0000_0000
	PCM_RX_POLLING_DEMAND,  		// value=0x0000_0000
	PCM_TX_DESC_RING_BASE,  			 // value=0x0000_0000
	PCM_RX_DESC_RING_BASE,  			 // value=0x0000_0000
	PCM_TX_RX_DESC_RING_SIZE_OFFSET,  	// value=0x0000_00c0
	PCM_TX_RX_DMA_Ctrl, 	 			 // value=0x0f00_0000
};



/*Variable Definition*/
#ifdef CONFIG_MIPS_TC3262
#define PCM_INT	12
#else
#define PCM_INT 12
#endif
#define BYTEALIMENT	32
#define MAX_REG_NUM 17
#define TX 0
#define RX 1
#define DISABLE 0
#define ENABLE 1
#define MAX_TX_DESC_NUM 15
#define MAX_RX_DESC_NUM 15
#define MAX_BUF_NUM 2
#define MAX_CH_NUM 2
#define MAX_CH_NUM_REG ((1<<MAX_CH_NUM)-1)


#define DEFAULT_BIT_ORDER 0
#define DEFAULT_SAMPLE_RATE 0
#define MAX_TIME_POOL_NUM 5
#define MAX_SAMPLE_POOL_NUM 4

#define MAX_SAMPLE_COUNT 1020
#define MAX_BIT_WIDTH 16
#define MAX_BUF_SIZE ((MAX_BIT_WIDTH>>3)*MAX_SAMPLE_COUNT)
#define MAX_DATA_PAT 4
#define MAX_FRAME_COUNT 31
#define MAX_FRAME_COUNT_REG (MAX_FRAME_COUNT<<18)
#define RAND_MAX   0x000000ff
/*the interrupt mask*/

#define HUNT_ERR_AFTER_FINISH_INT 0x400
#define HUNT_OVERTIME_INT 0x200
#define AHB_BUS_ERR_INT 0x100
#define RX_BUF_UNDER_RUN_INT 0x80
#define TX_BUF_UNDER_RUN_INT 0x40

#define END_OF_RX_DESC_INT 0x20
#define END_OF_TX_DESC_INT 0x10
#define RX_DESC_UPDATE_COMPLETED_INT 0x8
#define TX_DESC_UPDATE_COMPLETED_INT 0x4
#define RX_FRAME_BOUNDARY_INT  0x2
#define TX_FRAME_BOUNDARY_INT  0x1
/*Return Error definition*/
#define TX_DESC_OVER_TEN 4 /*20110217_serena_modify for pcm loopbakc test*/

#define DESC_FULL 1
#define CHANNEL_BUF_LACK 2
#define SUCCESS 1
#define FAIL 0
/*Loop back Mode*/
#define NO_LOOP_BACK 0
#define BACK_TO_BACK 1
#define ONE_BY_ONE 2
/*bit width mix mode*/
#define MIX_EIGHT_SIXTEEN 0
#define MIX_SIXTEEN_EIGHT 1
#define MIX_ALL_EIGHT 2
#define MIX_ALL_SIXTEEN 3
/*bit and byte ordering*/
#define LITTLE_ENDIAN_LSB 0
#define LITTLE_ENDIAN_MSB 1
#define BIG_ENDIAN_LSB 2
#define BIG_ENDIAN_MSB 3
/*header pattern*/
//#define HEADER_PATTERN 0x12345678
//#define HEADER_PATTERN_FIRST_BYTE 0x12
//#define HEADER_PATTERN_SECOND_BYTE 0x34
//#define HEADER_PATTERN_THIRD_BYTE 0x56
//#define HEADER_PATTERN_FOUTH_BYTE 0x78
//#define MAX_LOOP_BACK_COUNTER 50000
/*
 * Next Descriptor's offset.
 * Its value depends on MAX_BUF_NUM used in DESC structure
 */
#define MAX_DESC_OFF (MAX_BUF_NUM+1)
#define READ_ONLY 1
#define WRITE_ONLY (1<<1)
#define K1_TO_PHYSICAL(x) 				((uint32)(x) & 0x1fffffff)  /* kseg1 to physical */
#define CACHE_TO_NONCACHE(addr)			((uint32)(addr) | 0xa0000000)
#define NONCACHE_TO_CACHE(addr)			((uint32)(addr) & 0xdfffffff)
#define PHYSICAL_TO_K1(x) 				((uint32)(x) | 0xa0000000)  /* physical to kseg1 */

#define DBG_TIME     (1 << 19)
#define DBG_MSG      (1 << 20)
#define DBG_DES      (1 << 21)
#define DBG_REG      (1 << 22)
#define DBG_BUF      (1 << 23)
#define DBG_HEAD      (1 << 24)
#define DBG_FUNC     (1 << 25)
#define DBG_WARN     (1 << 26)
#define DBG_ERR      (1 << 27)
#define DBG_INT		(1 << 28)
#define DBG_DESC_CHECK      (1 << 29)

#if 0
#define DEBUGMSG(dbgs,format) 	do { } while(0)
#define DEBUGMSGW(dbgs,format)	do { } while(0)
#define DEBUGMSGB(dbgs,format)	do { } while(0)
#define DEBUGMSGL(dbgs,format)	do { } while(0)
#define ETH_DUMP(dbgs,format )	do { } while(0)
#else

#define dbg_pline_1(x)		printk("%s", x)
#define dbg_plineb_1(x, y)	printk("%s%02x", x, (uint8) y)
#define dbg_plinew_1(x, y)	printk("%s%04x", x, (uint16) y)
#define dbg_plinel_1(x, y)	printk("%s%08lx", x, (uint32) y)
#define DEBUGMSG(dbgs,format) if(dbgs&DbgSettings) dbg_pline_1 format
#define DEBUGMSGB(dbgs,format)if(dbgs&DbgSettings) dbg_plineb_1 format
#define DEBUGMSGW(dbgs,format)if(dbgs&DbgSettings) dbg_plinew_1 format
#define DEBUGMSGL(dbgs,format)if(dbgs&DbgSettings) dbg_plinel_1 format
#endif


//#define swap32(x) ()
/*
 * Data Structure
 */

/*
 * It is the header attached before the Tx buffer.
 * there would be data pattern firstly.
 * And then this pattern type.
 * finally, it would be the total data length.
 * so If there is 10 bytes in Tx buffer and data pattern is 0x0.
 * the data content would be 0x12345678,0,10,000000000
 *
 * */
typedef struct txBufHeader_s {
	uint32 headerPat;
	uint16 dataPatType;
	uint16 dataLen;
}txBufHeader_t;
typedef struct chDataInfor_s {
	uint16 chDataIdx;
	uint16 chDataLen;
	uint16 dataPatType;
	uint8 randSeed;
	uint8 patMatch;
}chDataInfor_t;
typedef struct desc_s {
	uint32 descStatus;
	uint32 bufAddr[MAX_BUF_NUM];
}desc_t;
typedef struct regAttr_s{
	char* name;
	uint8 rw;
	uint32 mask;
	uint32 addr;
	uint32 defaultValue;
}regAttr_t;

typedef union{
/*
	struct {
		uint32 ownership : 1;
		uint32 reserved : 7;
		uint32 chValid : 8;
		uint32 reserved2 : 6;
		uint32 sampleSize : 10;
	}bits;
*/

	struct {
		uint32 sampleSize : 10;
		uint32 reserved2 : 6;
		uint32 chValid : 8;
		uint32 reserved : 7;
		uint32 ownership : 1;
	}bits;
	
	uint32 value;
}descStatus_t;

typedef union{
/*
	struct {		
		uint32 reserved : 3;
		uint32 bitWidthNext : 1;
		uint32 reserved2 : 2;
		uint32 bitCounterNext : 10;
		uint32 reserved3 : 3;
		uint32 bitWidth : 1;
		uint32 reserved4 : 2;
		uint32 bitCounter : 10;
	}bits;
*/

	struct {
		uint32 bitCounter : 10;
		uint32 reserved4 : 2;
		uint32 bitWidth : 1;
		uint32 reserved3 : 3;	
		uint32 bitCounterNext : 10;
		uint32 reserved2 : 2;
		uint32 bitWidthNext : 1;
		uint32 reserved : 3;
	}bits;

	uint32 value;
}slotCfgReg_t;

/*
 * the CI-command function located at arch/mips/trendchip/tc3162/tcadsl.c
 *
 * */
extern int subcmd(const cmds_t tab[], int argc, char *argv[], void *p);
extern int cmd_register(cmds_t *cmds_p);
extern int cmd_unregister(cmds_t *cmds_p);
/*typedef union{
	uint32 reserved : 24;
	uint32 off : 4;
	uint32 ringNum : 4;
	uint32 value;
}txRxDescRingOffReg_t;
*/
#endif
