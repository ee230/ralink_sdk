/*
 * queue.h
 *
 *  Created on: 2008/10/7
 *      Author: pork
 */

#ifndef QUEUE_H_
#define QUEUE_H_
/*variable type definition*/
#ifndef UINT32
#define UINT32
typedef unsigned long int uint32; 		// 32-bit unsigned integer
#endif

#ifndef UINT16
#define UINT16
typedef unsigned short uint16;         //  16-bit unsigned integer
#endif

#ifndef UINT8
#define UINT8
typedef unsigned char uint8;          //   8-bit unsigned integer
#endif

#ifndef UINT4
#define UINT4
typedef unsigned long uint4;           //  8-bit unsigned integer
#endif
#define MAX_QUEUE_SIZE 300
#define QUEUE_EMPTY 0
extern void rxBufNextSet(uint8* rxBuf,uint8* nextBuf);
extern uint8* rxBufNextGet(uint8* rxBuf);
extern void pcmRecvBufFree(unsigned char* rxBuf);
extern void rxBufNumGet(uint8* rxBuf);
extern void rxBufNumSet(uint8* rxBuf);
#endif /* QUEUE_H_ */
