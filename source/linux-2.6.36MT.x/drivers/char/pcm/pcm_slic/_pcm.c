#include "_pcm.h"

#if defined(PCM_LOOPBACK_TEST)
#define TONE_PLAY
#else
#define EXT_LOOPBACK
#define EXT_LB_CROSS
#undef TONE_PLAY
#endif


                                        
#define M_SPI_RESET 		{_regSet(0xBFB00834, 0x800); udelay(200); _regSet(0xBFB00834, 0x0);}
#define M_PCM_RESET	{_regSet(0xBFB00834, 0x40000); udelay(200); _regSet(0xBFB00834, 0x0);}
#define M_PCM_ITR_DISABLE 	{_regSet(R_PCM_ITR_CTRL, 0x0);}
#define M_PCM_ITR_ENABLE 	{_regSet(R_PCM_ITR_CTRL, 0x1FC);}
#define M_DMA_TX_DISABLE	{_regSet(R_PCM_DMA_CTRL,  _regGet(R_PCM_DMA_CTRL) & ~(0x1));}
#define M_DMA_TX_ENABLE	{_regSet(R_PCM_DMA_CTRL,  _regGet(R_PCM_DMA_CTRL) | (0x1));}
#define M_DMA_RX_DISABLE	{_regSet(R_PCM_DMA_CTRL,  _regGet(R_PCM_DMA_CTRL) & ~(0x2));}
#define M_DMA_RX_ENABLE	{_regSet(R_PCM_DMA_CTRL,  _regGet(R_PCM_DMA_CTRL) | (0x2));}
#define M_PCM_SOFT_RST_OFF {_regSet(R_PCM_CTRL, \
							_regGet(R_PCM_CTRL) | (ML_PCM_CTRL_SOFT_RST << MS_PCM_CTRL_SOFT_RST));}
#define M_PCM_SOFT_RST_ON {_regSet(R_PCM_CTRL, \
							_regGet(R_PCM_CTRL) & ~(ML_PCM_CTRL_SOFT_RST << MS_PCM_CTRL_SOFT_RST));}

#define M_PCM_CONFIG_DONE {_regSet(R_PCM_CTRL, \
							_regGet(R_PCM_CTRL) | (ML_PCM_CTRL_CONFIG_SET << MS_PCM_CTRL_CONFIG_SET));}
#define M_PCM_CONFIG_SET {_regSet(R_PCM_CTRL, \
							_regGet(R_PCM_CTRL) & ~(ML_PCM_CTRL_CONFIG_SET << MS_PCM_CTRL_CONFIG_SET));}
	

static pcmCtrlReg_t _gPcmCtrlReg = {
	.bitField.clkSource 		= E_PCM_CLK_SOURCE_MASTER,
	.bitField.clkRate 		= E_PCM_CLK_2048K,
	.bitField.fsyncRate 	= E_PCM_FSYNC_8K,
	.bitField.reserved0 		= 0,
	.bitField.fsyncLen 		= E_PCM_FSYNC_LEN_1CLK,
	.bitField.fsyncEdge 	= E_PCM_CLK_EDGE_RISING,
	.bitField.dataClkEdge 	= E_PCM_CLK_EDGE_RISING,
	.bitField.dataFsyncEdge = E_PCM_FSYNC_EDGE_FALLING,
	.bitField.reserved1 		= 0,
	.bitField.bitOrder 		= E_PCM_BIT_ORDER_MSB,
	.bitField.byteOrder 	= E_PCM_BYTE_ORDER_LSB,
	.bitField.frameBound 	= 1,
	.bitField.reserved3 		= 0,
	.bitField.softRst 		= E_PCM_SOFT_RST_OFF,
#if defined(PCM_LOOPBACK_TEST)
	.bitField.loopback 		= E_PCM_LOOPBACK_ENABLE,
#else	
	.bitField.loopback 		= E_PCM_LOOPBACK_DISABLE,
#endif	
	.bitField.cfgSet 		= E_PCM_CONFIG_DONE,
	.bitField.reserved2 		= 0,
	.bitField.probing 		= 7,
	.bitField.loopbackData 	= 1
};

pcmDesc_t *_gPcmDescTx;
pcmDesc_t *_gPcmDescRx;

dma_addr_t _gDescTxHdl;
dma_addr_t _gDescRxHdl;

dmaBuf_t _gDmaBufTx[DMA_BUF_SZ][NUM_OF_CH];
dmaBuf_t _gDmaBufRx[DMA_BUF_SZ][NUM_OF_CH];

static uint16 _txBuf[NUM_OF_CH][SAMPLE_SZ_10MS];
static uint16 _rxBuf[NUM_OF_CH][SAMPLE_SZ_10MS];

static int _flagDMA = 0;
static int _IsrTHCnt = 0;
extern int inloopback;

uint16 tone1K[80] = {
 0x0000, 0x2d5d, 0x4026, 0x2d5c, 0x0000, 0xd2a3, 0xbfda, 0xd2a3,
 0x0000, 0x2d5d, 0x4026, 0x2d5c, 0x0000, 0xd2a3, 0xbfda, 0xd2a3,
 0x0000, 0x2d5d, 0x4026, 0x2d5c, 0x0000, 0xd2a3, 0xbfda, 0xd2a3,
 0x0000, 0x2d5d, 0x4026, 0x2d5c, 0x0000, 0xd2a3, 0xbfda, 0xd2a3,
 0x0000, 0x2d5d, 0x4026, 0x2d5c, 0x0000, 0xd2a3, 0xbfda, 0xd2a3,
 0x0000, 0x2d5d, 0x4026, 0x2d5c, 0x0000, 0xd2a3, 0xbfda, 0xd2a3,
 0x0000, 0x2d5d, 0x4026, 0x2d5c, 0x0000, 0xd2a3, 0xbfda, 0xd2a3,
 0x0000, 0x2d5d, 0x4026, 0x2d5c, 0x0000, 0xd2a3, 0xbfda, 0xd2a3,
 0x0000, 0x2d5d, 0x4026, 0x2d5c, 0x0000, 0xd2a3, 0xbfda, 0xd2a3,
 0x0000, 0x2d5d, 0x4026, 0x2d5c, 0x0000, 0xd2a3, 0xbfda, 0xd2a3
};

static uint32 _regGet(uint32 addr) {
	return *(volatile unsigned long int *)(addr);
}	

static void _regSet(uint32 addr, uint32 value) {
	*(volatile unsigned long int *)(addr) = value;
}

void pcmIsrTH() {
	uint32 interrupt = _regGet(R_PCM_ITR_STAT);
	
	_IsrTHCnt++;

	if (interrupt & (1 << E_ITR_RX_UPD)) {
#if defined(PCM_LOOPBACK_TEST)
		pcmTxWrite((void*)_txBuf);
		pcmRxRead((void*)_rxBuf);
#else		
		pcmRxRead((void*)_rxBuf);	
		pcmTxWrite((void*)_txBuf);
#endif		
	}
}

void pcmRxRead(void *pBuf) {
	int i, j;
#if defined(PCM_LOOPBACK_TEST)	 
	int k;
#endif
	for (i = 0; i < DMA_BUF_SZ; i++) {
		if (!(_gPcmDescRx[i].descCtrl.bitField.ownByDma))  {
			for (j = 0; j < NUM_OF_CH; j++) {
#ifdef EXT_LOOPBACK			
#ifdef EXT_LB_CROSS			
				memcpy(_txBuf[(j+(NUM_OF_CH >> 1)) % NUM_OF_CH], _gDmaBufRx[i][j].buf, _gDmaBufRx[i][j].size);
#else				
				memcpy(_txBuf[j], _gDmaBufRx[i][j].buf, _gDmaBufRx[i][j].size);
#endif
#endif
#if (defined(TONE_PLAY) && defined(PCM_LOOPBACK_TEST))
				if(_IsrTHCnt>8) {
					uint16* pDmaBufRx = (uint16*)_gDmaBufRx[i][j].buf;
					for (k = 0; k < (_gDmaBufRx[i][j].size>>1); k++) {
						if(pDmaBufRx[k]!=tone1K[k]) {
							printk("[%X vs %X](%d)(isr=%d)\n",(uint16)pDmaBufRx[k], tone1K[k],k,_IsrTHCnt);
							inloopback++;
							break;
						}
					}
				}
#endif				
			}
			_gPcmDescRx[i].descCtrl.bitField.ownByDma = 1;

			_flagDMA = i;
			break;
		}
	}	
}

void pcmTxWrite(void *pBuf) {
	int j;

	if (!(_gPcmDescTx[_flagDMA].descCtrl.bitField.ownByDma)) {
		for (j = 0; j < NUM_OF_CH; j++) {
#ifdef EXT_LOOPBACK		
			memcpy(_gDmaBufTx[_flagDMA][j].buf, _txBuf[j], _gDmaBufTx[_flagDMA][j].size);
#endif

#ifdef TONE_PLAY
      memcpy(_gDmaBufTx[_flagDMA][j].buf, tone1K, _gDmaBufTx[_flagDMA][j].size);
#endif      			
		}
		_gPcmDescTx[_flagDMA].descCtrl.bitField.ownByDma = 1;
	}
	_regSet(R_PCM_TX_REQ, 0x1);	
}

void pcmStop() {
	M_PCM_ITR_DISABLE
	M_DMA_RX_DISABLE
	M_DMA_TX_DISABLE
}

void pcmStart() {
	M_PCM_ITR_ENABLE
	M_DMA_RX_ENABLE
	M_DMA_TX_ENABLE

	_regSet(R_PCM_RX_REQ, 0x1);

}

void pcmReset() {
  int i; 
  uint32 tmp, val;
              
	M_PCM_SOFT_RST_ON
	udelay(200);
	M_PCM_SOFT_RST_OFF

	_regSet(R_PCM_TX_DESC, _gDescTxHdl);
	_regSet(R_PCM_RX_DESC, _gDescRxHdl);

	_regSet(R_PCM_DESC_CTRL, ((sizeof(pcmDesc_t) >> 2) << MS_PCM_DESC_OFFSET) | (DMA_BUF_SZ << MS_PCM_DESC_SIZE));
	_regSet(R_PCM_DMA_CTRL, ((1 << NUM_OF_CH) -1) << MS_PCM_DMA_CH_SZ);
	
  for (i = 0; i < NUM_OF_CH; i++) {
    tmp = _regGet(R_PCM_TX_SLOT_01 + ((i >> 1) << 2)) & (0xFFFF << (((i+1) & 1) << 4));
    val = ((i * SAMPLE_SZ_LEN) << MS_PCM_SLOT_1ST_LEAD_BIT) | (SAMPLE_SZ << MS_PCM_SLOT_1ST_SAMPLE_SZ);

    _regSet(R_PCM_TX_SLOT_01 + ((i >> 1) << 2),  tmp | (val << ((i&1) << 4)));
  	_regSet(R_PCM_RX_SLOT_01 + ((i >> 1) << 2),  _regGet(R_PCM_TX_SLOT_01 + ((i >> 1) << 2)));
	}

	_regSet(R_PCM_CTRL, _gPcmCtrlReg.value);

	M_PCM_CONFIG_SET
	udelay(200);
	M_PCM_CONFIG_DONE
	
	_IsrTHCnt = 0;
	
}

void pcmInit() {
	int i, j;

	memset(_txBuf, 0, sizeof(_txBuf));
	memset(_rxBuf, 0, sizeof(_rxBuf));

	_gPcmDescTx = dma_alloc_coherent(NULL, sizeof(pcmDesc_t) * DMA_BUF_SZ, &(_gDescTxHdl), GFP_KERNEL);
	_gPcmDescRx = dma_alloc_coherent(NULL, sizeof(pcmDesc_t) * DMA_BUF_SZ, &(_gDescRxHdl), GFP_KERNEL);

	for (i = 0;i < DMA_BUF_SZ; i++) {
		_gPcmDescTx[i].descCtrl.bitField.sampleSz	= _gPcmDescRx[i].descCtrl.bitField.sampleSz 	= SAMPLE_SZ_10MS;
		_gPcmDescTx[i].descCtrl.bitField.rsvd0	 	= _gPcmDescRx[i].descCtrl.bitField.rsvd0	 	= 0;
		_gPcmDescTx[i].descCtrl.bitField.chanMask = _gPcmDescRx[i].descCtrl.bitField.chanMask 	= (1 << NUM_OF_CH) - 1;
		_gPcmDescTx[i].descCtrl.bitField.rsvd1	 	= _gPcmDescRx[i].descCtrl.bitField.rsvd1	 	= 0;
		_gPcmDescTx[i].descCtrl.bitField.ownByDma 	= _gPcmDescRx[i].descCtrl.bitField.ownByDma = 1;

		for (j = 0;j < NUM_OF_CH; j++) {
			_gDmaBufTx[i][j].size = _gDmaBufRx[i][j].size = SAMPLE_SZ_10MS << 1;
			
			_gDmaBufTx[i][j].buf = dma_alloc_coherent(NULL, _gDmaBufTx[i][j].size, &(_gDmaBufTx[i][j].handle), GFP_KERNEL);
			_gPcmDescTx[i].dmaBufAddr[j] = (_gDmaBufTx[i][j].handle);

			_gDmaBufRx[i][j].buf = dma_alloc_coherent(NULL, _gDmaBufRx[i][j].size, &(_gDmaBufRx[i][j].handle), GFP_KERNEL);
			_gPcmDescRx[i].dmaBufAddr[j] = (_gDmaBufRx[i][j].handle);
		}
	}

	M_SPI_RESET
	M_PCM_RESET

	pcmStop();
	udelay(100);
	pcmReset();
}

void pcmExit() {
	int i, j;

	dma_free_coherent(NULL, sizeof(pcmDesc_t) * DMA_BUF_SZ, _gPcmDescTx, _gDescTxHdl);
	dma_free_coherent(NULL, sizeof(pcmDesc_t) * DMA_BUF_SZ, _gPcmDescRx, _gDescRxHdl);

	for (i = 0;i < DMA_BUF_SZ; i++) {
		for (j = 0;j < NUM_OF_CH; j++) {
			dma_free_coherent(NULL, _gDmaBufTx[i][j].size, _gDmaBufTx[i][j].buf, _gDmaBufTx[i][j].handle);
			dma_free_coherent(NULL, _gDmaBufRx[i][j].size, _gDmaBufRx[i][j].buf, _gDmaBufRx[i][j].handle);
		}
	}

}

