/*
 * PCMdriver.h
 *
 *  Created on: 2008/9/19
 *      Author: pork
 */
#ifndef PCMDRIVER_H
#define PCMDRIVER_H

/*Variable Definition*/

#define LITTLE_ENDIAN 0
#define BIG_ENDIAN 1

#define LSB 0
#define MSB 1

#define FRAME_SYNC_EDGE_RISING 0
#define FRAME_SYNC_EDGE_FALLING 1

#define FRAME_SYNC_LEN_1 0
#define FRAME_SYNC_LEN_8 2
#define FRAME_SYNC_LEN_16 3

#define PCM_SAMPLE_CLOCK_8 0
#define PCM_SAMPLE_CLOCK_16 1

#define PCM_BIT_CLOCK_256 0
#define PCM_BIT_CLOCK_512 1
#define PCM_BIT_CLOCK_1024 2
#define PCM_BIT_CLOCK_2048 3
#define PCM_BIT_CLOCK_4096 4
#define PCM_BIT_CLOCK_8192 5

#define MASTER_MODE 0
#define SLAVE_MODE 1

#define BITWIDTH_8 0
#define BITWIDTH_16 1



/*SLIC control flag*/
#define RESET		0
#define ENABLE		1

#define BUF_HEADER_SIZE 32
/*Data Structure*/
typedef struct configNode_s{
	unsigned char frameCount;
	unsigned char byteOrder;
	unsigned char bitOrder;
	unsigned char fsEdge;
	unsigned char fsLen;
	unsigned char sampleClock;
	unsigned char bitClock;
	unsigned char pcmMode;
	unsigned char chNum;
	unsigned int maxRxQueueSize;
	unsigned char bitWidth[8];
	unsigned int rxSampleSize;
	unsigned int debugLevel;
	unsigned int debugCategory;
}configNode_t;

typedef struct callBackParam_s{
	unsigned long int rxSampleSize;
	unsigned long int magicNum;
	unsigned char* next;
	char reserved[20];
}callBackParam_t;

#endif
