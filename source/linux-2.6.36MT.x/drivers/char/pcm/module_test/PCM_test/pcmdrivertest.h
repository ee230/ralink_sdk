/*
 * pcmcdrivertest.h
 *
 *  Created on: 2008/10/5
 *      Author: pork
 */

#ifndef PCMCDRIVERTEST_H_
#define PCMCDRIVERTEST_H_
#include "../../pcm_slic/pcmdriver.h"

#define MAX_CMP_COUNTER 500
extern int rxBufMagicNumGet(unsigned char* rxBuf);
extern int pcmConfig(configNode_t* tmpconfigNode,int* flag);
extern void pcmRestart(int configSet);
extern void pcmDMAStop(int direction);
extern int pcmSend(unsigned char** txBuf,int sampleSize,int* flag);
extern void pcmRecvChBufFree(unsigned char* rxBuf);
extern void pcmRecv(void);
extern int pcmRecvFuncRegister(void* funcP);
extern void descGetAll(int direct);
extern void regGetAll(void);
extern unsigned char* pcmRecvChBufDequeue(void);
extern void* pcmSendBufAlloc(int size);
extern void pcmTxBufKfree(void* addr);
extern int pcmRecvSampleSizeGet(unsigned char* rxBuf);
#endif /* PCMCDRIVERTEST_H_ */
