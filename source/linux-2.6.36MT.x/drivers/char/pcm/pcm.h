#ifndef PCM_H_
#define PCM_H_

#ifndef uint32
#define uint32 unsigned long int
#endif

#ifndef uint16
#define uint16 unsigned short int
#endif 

#ifndef uint8
#define uint8 unsigned char
#endif

typedef enum {
	E_PCM_CLK_SOURCE_MASTER,
	E_PCM_CLK_SOURCE_SLAVE
}pcmClkSource_e;

typedef enum {
	E_PCM_CLK_256K,
	E_PCM_CLK_512K,
	E_PCM_CLK_1024K,
	E_PCM_CLK_2048K,
	E_PCM_CLK_4096K,
	E_PCM_CLK_8192K
}pcmClk_e;

typedef enum {
	E_PCM_CLK_EDGE_RISING,
	E_PCM_CLK_EDGE_FALLING
}pcmClkEdge_e;

typedef enum {
	E_PCM_FSYNC_8K,
	E_PCM_FSYNC_16K
}pcmFsync_e;

typedef enum {
	E_PCM_FSYNC_EDGE_RISING,
	E_PCM_FSYNC_EDGE_FALLING
}pcmFsyncEdge_e;

typedef enum {
	E_PCM_FSYNC_LEN_1CLK = 0,
	E_PCM_FSYNC_LEN_8CLK = 2,
	E_PCM_FSYNC_LEN_16CLK = 3
}pcmFsyncLen_e;

typedef enum {
	E_PCM_SAMPLE_8BIT,
	E_PCM_SAMPLE_16BIT
}pcmSample_e;

typedef enum {
	E_PCM_BYTE_ORDER_LSB,
	E_PCM_BYTE_ORDER_MSB
}pcmByteOrder_e;

typedef enum {
	E_PCM_BIT_ORDER_LSB,
	E_PCM_BIT_ORDER_MSB
}pcmBitOrder_e;

typedef enum {
	E_PCM_DMA_BYTE_DEFAULT,
	E_PCM_DMA_BYTE_SWAP
}pcmDmaBSwap_e;

typedef enum {
	E_PCM_LOOPBACK_DISABLE,
	E_PCM_LOOPBACK_ENABLE
}pcmLookback_e;

typedef enum {
	E_PCM_SOFT_RST_ON,
	E_PCM_SOFT_RST_OFF
}pcmSoftRst_e;

typedef enum {
	E_PCM_CONFIG_SET,
	E_PCM_CONFIG_DONE
}pcmConfig_e;

typedef enum {
	E_PCM_STATE_INACTIVE,
	E_PCM_STATE_ACTIVE
}pcmState_e;

typedef struct {
	pcmState_e		pcmActiveState;

	pcmClkSource_e	pcmClkSource;
	pcmClk_e			pcmClkRate;
	pcmFsync_e 		pcmFsynRate;
	pcmFsyncLen_e	pcmFsyncLen;
	
	pcmClkEdge_e		pcmFsyncEdge;
	pcmClkEdge_e		pcmDataClkEdge;
	pcmFsyncEdge_e	pcmDataFsyncEdge;
	
	pcmByteOrder_e	pcmByteOrder;
	pcmBitOrder_e	pcmBitOrder;
	pcmDmaBSwap_e	pcmDmaBSwap;

	pcmLookback_e	pcmLoopback;
}pcmConfig_t;


#endif

