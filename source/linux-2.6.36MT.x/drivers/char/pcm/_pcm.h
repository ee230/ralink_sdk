#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h> 

#include "pcm.h"

#ifndef _PCM_H_
#define _PCM_H_

#define PCM_CH_MAX		(8)
#define NUM_OF_CH		(2)
#define DMA_BUF_SZ		(2)
#define SAMPLE_SZ_10MS (80)
#define SAMPLE_8BIT   (0)
#define SAMPLE_16BIT  (1)
#define SAMPLE_SZ SAMPLE_16BIT
#define SAMPLE_SZ_LEN (8 << SAMPLE_SZ)

/* System Control Register: For PCM/GPIO Sharing Control */
#define R_PCM_GPIO_SHARE (0xBFB00860)

#define MS_PCM_GPIO_SHARE		(0)
#define ML_PCM_GPIO_SHARE		(0x1) 		/* 0: GPIO, 1:PCM */

#define MS_SPI_CS5_GPIO_SHARE	(5)
#define ML_SPI_CS5_GPIO_SHARE	(0x1) 		/* 0: GPIO, 1:SPI_CS */

/* PCM Register Addresses: R - Register */
#define R_PCM_BASEADDR     (0xBFBD0000)
#define R_PCM_CTRL             (R_PCM_BASEADDR + 0x00)
#define R_PCM_TX_SLOT_01  (R_PCM_BASEADDR + 0x04)
#define R_PCM_TX_SLOT_23  (R_PCM_BASEADDR + 0x08)
#define R_PCM_TX_SLOT_45  (R_PCM_BASEADDR + 0x0c)
#define R_PCM_TX_SLOT_67  (R_PCM_BASEADDR + 0x10)
#define R_PCM_RX_SLOT_01  (R_PCM_BASEADDR + 0x14)
#define R_PCM_RX_SLOT_23  (R_PCM_BASEADDR + 0x18)
#define R_PCM_RX_SLOT_45  (R_PCM_BASEADDR + 0x1c)
#define R_PCM_RX_SLOT_67  (R_PCM_BASEADDR + 0x20)
#define R_PCM_ITR_STAT       (R_PCM_BASEADDR + 0x24)
#define R_PCM_ITR_CTRL       (R_PCM_BASEADDR + 0x28)
#define R_PCM_TX_REQ         (R_PCM_BASEADDR + 0x2c)	/* Write any value to initiate Tx request */
#define R_PCM_RX_REQ         (R_PCM_BASEADDR + 0x30)	/* Write any value to initiate Rx request */
#define R_PCM_TX_DESC       (R_PCM_BASEADDR + 0x34)	/* 32-bit Tx Descriptor Base Address */
#define R_PCM_RX_DESC       (R_PCM_BASEADDR + 0x38)	/* 32-bit Rx Descriptor Base Address */
#define R_PCM_DESC_CTRL    (R_PCM_BASEADDR + 0x3c)
#define R_PCM_DMA_CTRL     (R_PCM_BASEADDR + 0x40)
#define R_PCM_DMA_BSWAP  (R_PCM_BASEADDR + 0x44)


/* PCM Register Control Bit Mask: MS - Mask Start (bit-th), ML - Mask Length (bits) */
#define MS_PCM_CTRL_CLK_SOURCE		(0) 
#define ML_PCM_CTRL_CLK_SOURCE		(0x1)		/* 0: Master, 1: Slave */
#define MS_PCM_CTRL_CLK_RATE			(1)
#define ML_PCM_CTRL_CLK_RATE			(0x7)		/* 0: 256K, 1: 512K, 2: 1024K, 3: 2048K, 4: 4096K, 5: 8192K */
#define MS_PCM_CTRL_FSYNC_RATE		(4)
#define ML_PCM_CTRL_FSYNC_RATE		(0x1)		/* 0: 8KHz, 1: 16KHz */
#define MS_PCM_CTRL_FSYNC_LEN			(8)
#define ML_PCM_CTRL_FSYNC_LEN			(0x3)		/* 0: 1 PCLK, 2: 8 PCLK, 3: 16: PCLK */
#define MS_PCM_CTRL_FSYNC_EDGE		(10)
#define ML_PCM_CTRL_FSYNC_EDGE		(0x1)		/* 0: Rising Edge, 1: Falling Edge */
#define MS_PCM_CTRL_DATA_CLK_EDGE	(11)
#define ML_PCM_CTRL_DATA_CLK_EDGE	(0x1)		/* 0: Rising Edge, 1: Falling Edge */		
#define MS_PCM_CTRL_DATA_FSYNC_EDGE	(12)
#define ML_PCM_CTRL_DATA_FSYNC_EDGE	(0x1)		/* 0: Rising Edge, 1: Falling Edge */
#define MS_PCM_CTRL_BIT_ORDER			(16)
#define ML_PCM_CTRL_BIT_ORDER			(0x1)		/* 0: LSB First, 1: MSB First */
#define MS_PCM_CTRL_BYTE_ORDER		(17)
#define ML_PCM_CTRL_BYTE_ORDER		(0x1)		/* 0: Little Endian, 1: Big Endian */
#define MS_PCM_CTRL_FRM_BND			(18)
#define ML_PCM_CTRL_FRM_BND			(0x1f)		/* (0~31)+1 Frames Boundary to trigger Interrupt */
#define MS_PCM_CTRL_SOFT_RST			(24)
#define ML_PCM_CTRL_SOFT_RST			(0x1)		/* 0: Reset, 1: Normal */
#define MS_PCM_CTRL_LOOPBACK			(25)
#define ML_PCM_CTRL_LOOPBACK			(0x1)		/* 0: Disable, 1: Enable */
#define MS_PCM_CTRL_CONFIG_SET		(26)
#define ML_PCM_CTRL_CONFIG_SET		(0x1)		/* 0: DMA Config Set, 1: DMA Config Done */

#define MS_PCM_SLOT_1ST_LEAD_BIT		(0)
#define ML_PCM_SLOT_1ST_LEAD_BIT		(0x3ff)		/* 0~512 bit-th*/
#define MS_PCM_SLOT_1ST_SAMPLE_SZ	(12)
#define ML_PCM_SLOT_1ST_SAMPLE_SZ	(0x1)		/* 0: 8-bit Sample, 1: 16-bit Sample */
#define MS_PCM_SLOT_2ND_LEAD_BIT		(16)
#define ML_PCM_SLOT_2ND_LEAD_BIT		(0x3ff)		/* 0~512 bit-th*/
#define MS_PCM_SLOT_2ND_SAMPLE_SZ	(28)
#define ML_PCM_SLOT_2ND_SAMPLE_SZ	(0x1)		/* 0: 8-bit Sample, 1: 16-bit Sample */

#define MS_ITR_FRM				(0)
#define ML_ITR_FRM				(0x1)		/* 0: Disable/No-interrupt, 1: Enable/Interrupt */ 
#define MS_ITR_TX_UPD			(2)
#define ML_ITR_TX_UPD			(0x1)		/* 0: Disable/No-interrupt, 1: Enable/Interrupt */ 
#define MS_ITR_RX_UPD			(3)
#define ML_ITR_RX_UPD			(0x1)		/* 0: Disable/No-interrupt, 1: Enable/Interrupt */ 
#define MS_ITR_TX_E_DESC			(4)
#define ML_ITR_TX_E_DESC			(0x1)		/* 0: Disable/No-interrupt, 1: Enable/Interrupt */ 
#define MS_ITR_RX_E_DESC			(5)
#define ML_ITR_RX_E_DESC			(0x1)		/* 0: Disable/No-interrupt, 1: Enable/Interrupt */ 
#define MS_ITR_TX_E_DMA			(6)
#define ML_ITR_TX_E_DMA			(0x1)		/* 0: Disable/No-interrupt, 1: Enable/Interrupt */ 
#define MS_ITR_RX_E_DMA			(7)
#define ML_ITR_RX_E_DMA			(0x1)		/* 0: Disable/No-interrupt, 1: Enable/Interrupt */ 
#define MS_ITR_AHB_ERR			(8)
#define ML_ITR_AHB_ERR			(0x1)		/* 0: Disable/No-interrupt, 1: Enable/Interrupt */ 
#define MS_ITR_FSYNC_OVT		(9)
#define ML_ITR_FSYNC_OVT		(0x1)		/* 0: Disable/No-interrupt, 1: Enable/Interrupt */ 
#define MS_ITR_FSYNC_ERR		(10)
#define ML_ITR_FSYNC_ERR		(0x1)		/* 0: Disable/No-interrupt, 1: Enable/Interrupt */ 

#define MS_PCM_DESC_SIZE 		(0)
#define ML_PCM_DESC_SIZE 		(0xf)		/* 0~8 Descriptors */
#define MS_PCM_DESC_OFFSET 	(4)
#define ML_PCM_DESC_OFFSET 	(0xf)		/* DWord (4-byte) offset, default: 9 */

#define MS_PCM_DMA_ENABLE_TX 	(0)
#define ML_PCM_DMA_ENABLE_TX	(0x1)		/* 0: Disable, 1: Enable */
#define MS_PCM_DMA_ENABLE_RX 	(1)
#define ML_PCM_DMA_ENABLE_RX	(0x1)		/* 0: Disable, 1: Enable */
#define MS_PCM_DMA_AHB_POL	(2)
#define ML_PCM_DMA_AHB_POL	(0x3)		/* 0: Round-robin, 2: Tx Fisrt, 3: Rx First */
#define MS_PCM_DMA_CH_SZ 		(24)
#define ML_PCM_DMA_CH_SZ 		(0xff)		/* 8-bit Mask for 8-ch, 0: Disable, 1: Enable */

#define MS_PCM_DMA_BSWAP		(0)
#define ML_PCM_DMA_BSWAP		(0x1)		/* 0: Byte Default, 1: Byte Swap */

#define MS_DESC_SAMPLE_SZ		(0)
#define ML_DESC_SAMPLE_SZ		(0x3ff)		/* 0~1024 !!!Samples!!! */
#define MS_DESC_CH_DATA_VALID	(16)			
#define ML_DESC_CH_DATA_VALID	(0xff)		/* 8-bit Mask for 8-ch, 0: Invalid, 1: Valid	 */		
#define MS_DESC_OWN_BY_DMA	(31)
#define ML_DESC_OWN_BY_DMA	(0x1)		/* 0: False, 1: True */

typedef enum {
	E_ITR_FRM_BND,
	E_ITR_RSV0,
	E_ITR_TX_UPD,
	E_ITR_RX_UPD,
	E_ITR_TX_E_DESC,
	E_ITR_RX_E_DESC,
	E_ITR_TX_E_DMA,
	E_ITR_RX_E_DMA,
	E_ITR_AHB_ERR,
	E_ITR_HUNT_OT,
	E_ITR_HUNT_ERR,

	E_ITR_INVALID
} interrupt_e;
#define ITR_SZ 	E_ITR_INVALID

typedef 	union {
	struct {
		uint32 clkSource :1;
		uint32 clkRate :3;
		uint32 fsyncRate :1;
		uint32 reserved0 :3;
		uint32 fsyncLen :2;
		uint32 fsyncEdge :1;
		uint32 dataClkEdge :1;
		uint32 dataFsyncEdge :1;
		uint32 reserved1 :3;
		uint32 bitOrder :1;
		uint32 byteOrder :1;
		uint32 frameBound :5;
		uint32 reserved2 :1;
		uint32 softRst :1;
		uint32 loopback :1;
		uint32 cfgSet :1;
		uint32 reserved3 :1;
		uint32 probing :3;
		uint32 loopbackData :1;
	}bitField;

	uint32 value;
} pcmCtrlReg_t;


typedef struct {
	dma_addr_t handle;
	void*	    buf;
	uint32	    size;
} dmaBuf_t;

typedef union {
	struct {
		uint32 sampleSz		:10;
		uint32 rsvd0			:6;
		uint32 chanMask 		:8;
		uint32 rsvd1 			:7;
		uint32 ownByDma 	:1;
	} bitField;

	uint32 value;	
} pcmDescCtrl_t;

typedef struct {
	pcmDescCtrl_t 	descCtrl;
	uint32			dmaBufAddr[PCM_CH_MAX];	
} pcmDesc_t;

uint32 regGet(uint32 addr);
void regSet(uint32 addr, uint32 value);

void pcmIsrTH(void);
void pcmStop(void);
void pcmStart(void);
void pcmRxRead(void *pBuf);
void pcmTxWrite(void *pBuf);

void pcmReset(void);
void pcmInit(void);
void pcmExit(void);

#endif

