/*
 * pcmdrivertest.c
 *
 *  Created on: 2008/10/5
 *      Author: pork
 */
#include <asm/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <asm/addrspace.h>
#include <asm/tc3162/cmdparse.h>
#include "pcmdrivertest.h"
//#include "../../pcm_slic/pcmverify.h"
#include "pcmverify.h"
#include "../../pcm_slic/error.h"

#define PCM_LOOPBACK_TEST	1
unsigned int slic_type = 3227;
extern int CustomerVoice_HWInit();
extern int ProSLIC_HWInit();
/************************************************************************
*				MACRO DEFINITIONS
*************************************************************************
*/
#define DEFAULT_RX_QUEUE_SIZE 300
#define DEFAULT_RX_SAMPLE_SIZE	160
#define NOT_TO_INIT 0
#define CONFIG_INIT 1
/************************************************************************
*				F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
#define regRead(reg) 			VPint(reg)
#define regWrite(reg,regValue) 	VPint(reg)=regValue
#define pause(x)		mdelay(x)
int doSwc(int argc, char *argv[], void *p);
int doPcmConfig(int argc, char *argv[], void *p);
int getRealsampleFreq(int sampleFreq);
int getRealPcmBitClock(int pcmBitCLock);
int MaxChNumCal(int sampleFreq,int pcmBitClock,int bitWidCase);
void pcmConfigCall(int bitWidth,int chNum,int pcmBitClock, int sampleFreq);
int doPcmRestart(int argc, char *argv[], void *p);
int doPcmStop(int argc, char *argv[], void *p);
int doPcmSend(int argc, char *argv[], void *p);
int doLoopBackTest(int argc, char *argv[], void *p);
int doLoopBackTest1(int argc, char *argv[], void *p);
int doLoopBackTest2(int argc, char *argv[], void *p);
int doPcmRecv(int argc, char *argv[], void *p);
void sampleSizeIncrease(int direct);
void bufWrite(char* chTxBuf,uint16 sampleSize);
/*serena_modify*/
// void rxBufCmp(uint8* rxBuf,int chIdx,int sampleSize);
int rxBufCmp(uint8* rxBuf,int chIdx,int sampleSize);
int headerMatch(uint8* rxBuf,int* bufIdx,int chIdx,int sampleSize);
int rxDataCmp(uint8* rxBuf,int chIdx,int* bufIdx,int sampleSize);
void rxChDataCmp(void);
void loopBackTest(void);
/************************************************************************
*				GLOBE VARIABLES
*************************************************************************
*/
cmds_t swcCmd;

uint32 dataPat[MAX_DATA_PAT]={0x0,0xffffffff,0x5A5A5A5A,0xA5A5A5A5};
/*Rx Data header pattrern segment and its index*/
static int headerPatIdx[MAX_CH_NUM];
/*Used to record the comming Tx buffer header*/
static uint8 headerPat[MAX_CH_NUM][sizeof(txBufHeader_t)];
static int txLoopBackSampleSize = 12;
static int rxLoopBackSampleSize = 20;
static int loopBackCounter = 0;
static int loopBackFailCounter = 0;
int dataPatType = 0;
spinlock_t loopLock = SPIN_LOCK_UNLOCKED;
/*txBuf header*/
static txBufHeader_t txBufHeader;
/*announce 8 channel data information to record current Rx buffer compare needed information for each coming channel data*/
static chDataInfor_t chDataInfor[MAX_CH_NUM];
configNode_t configNode;
/*serena_modify*/
static int failBreak = 0;

//static DECLARE_WAIT_QUEUE_HEAD(wq);
//static int wakeFlag =0;
//mm_segment_t oldfs;
//struct file *filp=NULL;
//struct file *lbFilp=NULL;

static cmds_t swcCmds[] = {
	{"config",		doPcmConfig, 0x02,	2,	"Usage: swc config <chnum> <bitWid>"},
	{"restart",		doPcmRestart, 0x02,	0,	NULL},
	{"stop", 	doPcmStop, 0x02,	1,	"Usage: swc stop <0(Tx),1(Rx)>"},
	{"send", 	doPcmSend, 0x02,	1,	"Usage: swc send <chbufNum -nodelay -overch -descfull -exceedq>"},
	{"recv",		doPcmRecv, 0x02,	0,	NULL},
	{"loopback",	doLoopBackTest, 		0x02,	0,	"Usage: pcmctrl loopback <mode 1:back-to-back 2:one Tx at one time 3: several Tx desc> <descCount>"},
	{NULL,			NULL,			0x10,	0,	NULL},
};

/************************************************************************
*				F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

/*_____________________________________________________________________________
**      function name: doSwc
**      descriptions:
**            get parameter from swc command
**
**      parameters:
**		None
**      global:
**            None
**
**      return:
**	          None
**      call:
**      		subcmd
**      revision:
**      	1.1 2008/08/13 19:00  pork
**____________________________________________________________________________
*/
int doSwc(int argc, char *argv[], void *p){
	return 0;//subcmd(swcCmds, argc, argv, p);
}
/*_____________________________________________________________________________
**      function name: doPcmConfig
**      descriptions:
**            do pcm config
**
**      parameters:
**		None
**      global:
**            None
**
**      return:
**	          None
**      call:
**          pcmConfig
**      revision:
**      	1.1 2008/08/13 19:00  pork
**____________________________________________________________________________
*/
int doPcmConfig(int argc, char *argv[], void *p){
	int i = 0;
	int tmpBitWidth = 0;
	int flag;
	configNode.loopbackMode = LOOPBACK_ON;
	configNode.frameCount = MIN_FRAME_COUNT;
	configNode.byteOrder = LITTLE_ENDIAN;
	configNode.bitOrder = LSB;
	configNode.fsEdge = FRAME_SYNC_EDGE_FALLING;
	configNode.fsLen = FRAME_SYNC_LEN_1;
	configNode.sampleClock = PCM_SAMPLE_CLOCK_8;
	configNode.bitClock = PCM_BIT_CLOCK_2048;
	configNode.pcmMode = MASTER_MODE;
	configNode.maxRxQueueSize = DEFAULT_RX_QUEUE_SIZE;
	configNode.chNum = (int)simple_strtoul(argv[1],NULL,10);
	configNode.debugCategory = 0;
	configNode.debugLevel = 0;
	if((int)simple_strtoul(argv[2],NULL,10) == 0){
		tmpBitWidth = BITWIDTH_8;
	}
	else{
		tmpBitWidth = BITWIDTH_16;
	}
	for(i=0;i<MAX_CH_NUM;i++){
			configNode.bitWidth[i]=tmpBitWidth;
			//configNode.bitWidth[i]=BITWIDTH_8;
	}
	configNode.rxSampleSize = rxLoopBackSampleSize;
	//sampleSizeIncrease(RX);
	pcmConfig(&configNode,&flag);
	return 0;
}
int getRealsampleFreq(int sampleFreq){
	return (sampleFreq+1)*8;
}
int getRealPcmBitClock(int pcmBitClock){
	return 256<<pcmBitClock;
}
int MaxChNumCal(int sampleFreq,int pcmBitClock,int bitWidCase){
	int ret = 0;
	int tmpVal = getRealPcmBitClock(pcmBitClock)/getRealsampleFreq(sampleFreq);
	int tmpBitWid = 0;
	int remainder = 0;
	printk("bit clock:%d sample freq:%d ",getRealPcmBitClock(pcmBitClock),getRealsampleFreq(sampleFreq));
	switch(bitWidCase){
	case MIX_EIGHT_SIXTEEN:
		tmpBitWid = 12;
		if(tmpVal%12 == 8){
			remainder = 1;
		}
		break;
	case MIX_SIXTEEN_EIGHT:
		tmpBitWid = 12;
		break;
	case MIX_ALL_EIGHT:
		tmpBitWid = 8;
		break;
	case MIX_ALL_SIXTEEN:
		tmpBitWid = 16;
		break;
	default:
		break;
	}
	ret = (int)(tmpVal/tmpBitWid)+remainder;
	if(ret > MAX_CH_NUM){
		return MAX_CH_NUM;
	}
	else{
		return ret;
	}

}

void pcmConfigCall(int bitWidTestCase,int chNum,int pcmBitClock, int sampleFreq){
	int i = 0;
	int tmpBitWidth = 0;
	int flag = 0;
#ifdef PCM_LOOPBACK_TEST
	configNode.loopbackMode = LOOPBACK_ON;
#else	
	configNode.loopbackMode = LOOPBACK_OFF;
#endif	
	configNode.frameCount = 1;//MIN_FRAME_COUNT;
	configNode.byteOrder = LITTLE_ENDIAN;
	//configNode.bitOrder = LSB;
	//configNode.fsEdge = FRAME_SYNC_EDGE_FALLING;
	
	configNode.bitOrder = MSB;
	configNode.fsEdge = FRAME_SYNC_EDGE_RISING;
	
	configNode.fsLen = FRAME_SYNC_LEN_1;
	configNode.sampleClock = sampleFreq;
	configNode.bitClock = pcmBitClock;
	configNode.pcmMode = MASTER_MODE;
	configNode.maxRxQueueSize = DEFAULT_RX_QUEUE_SIZE;
#ifdef PCM_LOOPBACK_TEST
	configNode.rxSampleSize = rxLoopBackSampleSize;
#else
	configNode.rxSampleSize = DEFAULT_RX_SAMPLE_SIZE;
#endif	
	configNode.chNum = chNum;
	configNode.debugCategory = CATEGORY_CONFIG|CATEGORY_RX;
	configNode.debugLevel = DBG_LEVEL_2;
	//printk("ch: %d \n",(int)simple_strtoul(argv[1],NULL,10));
	if((bitWidTestCase == MIX_ALL_EIGHT) || (bitWidTestCase == MIX_EIGHT_SIXTEEN)){
		tmpBitWidth = BITWIDTH_8;
	}
	else{
		tmpBitWidth = BITWIDTH_16;
	}
	for(i=0;i<MAX_CH_NUM;i++){
			configNode.bitWidth[i]=tmpBitWidth;
			if((bitWidTestCase == MIX_EIGHT_SIXTEEN) || (bitWidTestCase == MIX_SIXTEEN_EIGHT)){
				tmpBitWidth = (!tmpBitWidth);
			}
	}

	//sampleSizeIncrease(RX);
	pcmConfig(&configNode,&flag);
}
/*_____________________________________________________________________________
**      function name: doPcmRestart
**      descriptions:
**            call pcmrestart
**
**      parameters:
**		None
**      global:
**            None
**
**      return:
**	          0
**      call:
**      	pcmRestart
**      revision:
**      	1.1 2008/08/13 19:00  pork
**____________________________________________________________________________
*/
int doPcmRestart(int argc, char *argv[], void *p){
	pcmRestart(NOT_TO_INIT);
	return 0;
}
/*_____________________________________________________________________________
**      function name: doPcmStop
**      descriptions:
**            call pcmDMAStop
**
**      parameters:
**		None
**      global:
**            None
**
**      return:
**	          None
**      call:
**      		pcmDMAStop
**      revision:
**      	1.1 2008/08/13 19:00  pork
**____________________________________________________________________________
*/
int doPcmStop(int argc, char *argv[], void *p){
	if(argc == 1){
		if((int)simple_strtoul(argv[1],NULL,10) == TX){
			pcmDMAStop(TX);
		}
		else{
			pcmDMAStop(RX);
		}
	}
	return 0;
}
/*_____________________________________________________________________________
**      function name: doPcmSend
**      descriptions:
**            call loopbacktest
**
**      parameters:
**		None
**      global:
**            None
**
**      return:
**	          None
**      call:
**      		loopBackTest
**      revision:
**      	1.1 2008/08/13 19:00  pork
**____________________________________________________________________________
*/
int doPcmSend(int argc, char *argv[], void *p){
	return 0;
}
int doLoopBackTest(int argc, char *argv[], void *p){
	loopBackTest();
	return 0;
}
int doPcmRecv(int argc, char *argv[], void *p){
	return 0;
}
/*_____________________________________________________________________________
**      function name: sampleSizeIncrease
**      descriptions:
**      		increase sample size
**      parameters:
**			direct: Tx or Rx
**      global:
**
**      return:
**	          None
**      call:
**      		None
**      revision:
**_____________________________________________________________________________
*/
void sampleSizeIncrease(int direct){
	/*
	 * because PCM move 4 byte at one time,
	 * our incresing sample size(bit width =8) will be 4 multiple.
	 * */
	if(direct == TX){
		txLoopBackSampleSize = (txLoopBackSampleSize+4)%(configNode.rxSampleSize-4);
		if(txLoopBackSampleSize < 12){
			txLoopBackSampleSize = 12;
		}
	}
	else{
		rxLoopBackSampleSize = (rxLoopBackSampleSize+4)%MAX_SAMPLE_COUNT;
		if(rxLoopBackSampleSize < 20){
			rxLoopBackSampleSize = 20;
		}
	}
}
void bufWrite(char* chTxBuf,uint16 bufSize){
	//int i;
	uint8 tmpData = (uint8)(dataPat[dataPatType]&0x000000ff);
	//dataPatType = (dataPatType+1)%4;
	txBufHeader.headerPat = 0x78563412;//0x12345678;
	txBufHeader.dataPatType = dataPatType;
	txBufHeader.dataLen = bufSize - sizeof(txBufHeader_t);
	memset(chTxBuf,tmpData,bufSize);
	memcpy(chTxBuf,&txBufHeader,sizeof(txBufHeader_t));
	/*for(i = 0 ;i<bufSize;i++){
		printk("%02x ",chTxBuf[i]);
	}
	printk("\n");*/

}
/*_____________________________________________________________________________
**      function name: headerMatch
**      descriptions:
**           get rxBuf and analaze its header pattern
**      parameters:
**		rxBuf: desc's corresponding buffer
**		sampleSize: the capacity in the buffer
**      global:
**            HeaderPatIdx
**            HeaderPat
**
**      return:
**	          None
**      call:
**   		  None
**      revision:
**      	1.1 2008/08/13 19:00  pork
**____________________________________________________________________________
*/
int headerMatch(uint8* rxBuf,int* bufIdx,int chIdx,int sampleSize){

	while((*bufIdx) < sampleSize){
		if((headerPatIdx[chIdx] == sizeof(txBufHeader_t)) && (chDataInfor[chIdx].patMatch == 1)){
			/*if data len equal to 0 was found*/
			if((headerPat[chIdx][(headerPatIdx[chIdx])-1] == 0)&&(headerPat[chIdx][(headerPatIdx[chIdx])-2] == 0)){
				chDataInfor[chIdx].patMatch = 0;
				headerPatIdx[chIdx] = 0;
				return FAIL;
			}
			//printk("%2x %2x %2x %2x ",headerPat[chIdx][4],headerPat[chIdx][5],headerPat[chIdx][6],headerPat[chIdx][7]);
			chDataInfor[chIdx].patMatch = 0;
			headerPatIdx[chIdx] = 0;
			//printk("match\n");
			return SUCCESS;
		}
		else{
			/*If headerPatIdx[chIdx] >=4,
			 *it presents that header have been match totally.
			 *it would record the remained pat type and length.
			 * */
			if(headerPatIdx[chIdx] >= 4){
				headerPat[chIdx][headerPatIdx[chIdx]] = rxBuf[*bufIdx];
				headerPatIdx[chIdx]++;
				chDataInfor[chIdx].patMatch = 1;
			}
			else{
				/*Compare the header pattern with Rxbuffer
				 *If match, the PatIdx ++
				 *
				 * */
				if(headerPat[chIdx][(headerPatIdx[chIdx])] == rxBuf[*bufIdx]){
					headerPatIdx[chIdx]++;
				}
				else{
					headerPatIdx[chIdx] = 0;
					//printk("NM [%02X vs %02X]\n",headerPat[chIdx][(headerPatIdx[chIdx])],rxBuf[*bufIdx]);
				}
			}
		}
		(*bufIdx)++;
	}
	return FAIL;

}
/*_____________________________________________________________________________
**      function name: rxDataCmp
**      descriptions:
**           compare Rx buufer content with data pattern
**      parameters:
**			rxBuf: desc's corresponding buffer
**			bufIdx: the index to buffer current content
**			sampleSize: the capacity in the buffer
**      global:
**            chDataInfor
**
**      return:
**	          None
**      call:
**   		  headerMatch
**   		  rxDataCmp
**      revision:
**      	1.1 2008/08/13 19:00  pork
**____________________________________________________________________________
*/
int rxDataCmp(uint8* rxBuf,int chIdx,int* bufIdx,int sampleSize){

	int i; /*serena_modify*/
	uint32 tmpVal = 0;
	if(chDataInfor[chIdx].chDataIdx ==0){

		//printk("1where:%d ",chDataInfor[chIdx].dataPatType);
		memcpy(&(chDataInfor[chIdx].dataPatType),&(headerPat[chIdx][4]),sizeof(uint16));
		//chDataInfor[chIdx].dataPatType =
		if((chDataInfor[chIdx].dataPatType>3) || (chDataInfor[chIdx].dataPatType< 0) ){
			 printk("2 dataPatType %04x ,%04x %04x %04x %04x %04x,%04x\n",chDataInfor[chIdx].dataPatType,headerPat[chIdx][0],headerPat[chIdx][1],headerPat[chIdx][2],headerPat[chIdx][3],headerPat[chIdx][4],headerPat[chIdx][5]);
		 }
		memcpy(&(chDataInfor[chIdx].chDataLen),&(headerPat[chIdx][sizeof(txBufHeader.headerPat)+sizeof(txBufHeader.dataPatType)]),sizeof(txBufHeader.dataLen));
		if(chDataInfor[chIdx].dataPatType == 4){
			chDataInfor[chIdx].randSeed = rxBuf[*bufIdx];
		}
	}
	/*if(loopBackCounter > 495){
		printk("2 ");
	}*/
	while((*bufIdx) < sampleSize){

		//printk("where:%d %d %d %d",chDataInfor[chIdx].chDataIdx,chDataInfor[chIdx].chDataLen,(*bufIdx),sampleSize);
		if(chDataInfor[chIdx].chDataIdx == chDataInfor[chIdx].chDataLen){
			//printk(KERN_INFO "i:%d chDataInfor[chIdx].chDataIdx: %d and chDataInfor[chIdx].chDataLen: %d\n",i,chDataInfor[chIdx].chDataIdx,chDataInfor[chIdx].chDataLen);
			loopBackCounter = (loopBackCounter+1)%MAX_LOOP_BACK_COUNTER;
			//printk("Cmp-%d",loopBackCounter);
			memset(&chDataInfor[chIdx],0,sizeof(chDataInfor_t));
			/*if(loopBackCounter > 495){
				printk("6 ");
			}*/
			return SUCCESS;
		}
		else{
			tmpVal = (uint32)rxBuf+(*bufIdx);
			if((tmpVal & 0xC0000000) == 0xC0000000){
				printk("idx %d - 0x%08lx \n",*bufIdx,tmpVal);
			}
			if(chDataInfor[chIdx].dataPatType != 4){
				//printk("%d where:%2x ",*bufIdx,rxBuf[*bufIdx]);
				/*if(loopBackCounter > 495){
					printk("7 ");
				}*/
				if((chIdx >8) || (chIdx <0)){
					printk("chIdx %d \n",chIdx);
				}
				else{
					 if((chDataInfor[chIdx].dataPatType>3) || (chDataInfor[chIdx].dataPatType< 0) ){
						 printk("3 dataPatType %d \n",chDataInfor[chIdx].dataPatType);
					 }
				}
				if(rxBuf[*bufIdx] != (uint8)(dataPat[chDataInfor[chIdx].dataPatType])){
					//printk(KERN_INFO "Compare Fail:rxbuf%d: %2x dataPat:%2x\n",*bufIdx,rxBuf[*bufIdx],(uint8)dataPat[chDataInfor[chIdx].dataPatType]);
					loopBackFailCounter++;
					//wakeFlag = 1;c000f
					//wake_up_interruptible(&wq);
					printk("##Compare Fail:rxbuf%d: %2x samplesize: %d dataPat:%2x\n",*bufIdx,rxBuf[*bufIdx],sampleSize,(uint8)dataPat[chDataInfor[chIdx].dataPatType]);		
					
					printk("chDataInfor[%d].chDataIdx=%2x \n",chIdx,chDataInfor[chIdx].chDataIdx);
					printk("chDataInfor[%d].chDataLen=%2x \n",chIdx,chDataInfor[chIdx].chDataLen);
					for(i = 0; i <= 10; i++ ){
						printk("rxBuf[%d]=%2x \n",*bufIdx+i,rxBuf[*bufIdx+i]);
					}
					
					memset(&(chDataInfor[chIdx]),0,sizeof(chDataInfor_t));
					return FAIL;
				}
			}
			else{
				if(rxBuf[*bufIdx] != chDataInfor[chIdx].randSeed){
					//printk(KERN_INFO "Compare Fail:rxbuf%d: %2x dataPat:%2x\n",*bufIdx,rxBuf[*bufIdx],chDataInfor[chIdx].randSeed);
					memset(&(chDataInfor[chIdx]),0,sizeof(chDataInfor_t));
					loopBackFailCounter++;
					//wakeFlag = 1;
					//wake_up_interruptible(&wq);
					printk("##Compare Fail:rxbuf%d: %2x dataPat:%2x\n",*bufIdx,rxBuf[*bufIdx],chDataInfor[chIdx].randSeed);
					return FAIL;
				}
				chDataInfor[chIdx].randSeed++;
			}
			//printk("where5:%d ",(*bufIdx));
			chDataInfor[chIdx].chDataIdx++;
		}
		//printk("where6:%d ",(*bufIdx));
		(*bufIdx)++;
	}
	if(chDataInfor[chIdx].chDataIdx == chDataInfor[chIdx].chDataLen){
		//printk(KERN_INFO "i:%d chDataInfor[chIdx].chDataIdx: %d and chDataInfor[chIdx].chDataLen: %d\n",i,chDataInfor[chIdx].chDataIdx,chDataInfor[chIdx].chDataLen);
		loopBackCounter = (loopBackCounter+1)%MAX_LOOP_BACK_COUNTER;
		//printk("Cmp-%d",loopBackCounter);
		memset(&chDataInfor[chIdx],0,sizeof(chDataInfor_t));
		return SUCCESS;
	}
	//printk(KERN_INFO "chDataInfor[chIdx].chDataIdx: %d and chDataInfor[chIdx].chDataLen: %d\n",chDataInfor[chIdx].chDataIdx,chDataInfor[chIdx].chDataLen);
	/*serena_modify*/
// 	return 0;
	return SUCCESS;
}
/*_____________________________________________________________________________
**      function name: rxBufCmp
**      descriptions:
**           Call headerMatch to analyze Rx header and rxDataCmp to compare its content with data pattern
**      parameters:
**		rxBuf: desc's corresponding buffer
**		sampleSize: the capacity in the buffer
**      global:
**            chDataInfor
**            HeaderPat
**
**      return:
**	          None
**      call:
**   		  headerMatch
**   		  rxDataCmp
**      revision:
**      	1.1 2008/08/13 19:00  pork
**____________________________________________________________________________
*/
/*serena_modify*/
// void rxBufCmp(uint8* rxBuf,int chIdx,int sampleSize){
int rxBufCmp(uint8* rxBuf,int chIdx,int sampleSize){
	int bufIdx = 0 ;
	/*serena_modify*/
	int retVal = SUCCESS;
	//printk("ch:0x%08lx ",rxBuf);
	while(bufIdx < sampleSize){
		//loopBackCounter = (loopBackCounter+1)%MAX_LOOP_BACK_COUNTER;
		//bufIdx ++;
		/*the case that if header is at previous desc */
		if(chDataInfor[chIdx].chDataIdx != 0){
			bufIdx = 0;
			/*serena_modify*/
			retVal = rxDataCmp(rxBuf,chIdx,&bufIdx,sampleSize);
			if(retVal == FAIL){
				break;
			}
			//continue;	
			/*if(loopBackCounter > 495){
				printk("4 ");
			}*/
		}
		/*finding header pattern at current desc*/
		if(headerMatch(rxBuf,&bufIdx,chIdx,sampleSize) == SUCCESS){
			//printk("!0x%08lx\n",regRead(OPERATIONAL_BASE|TX_RX_DMA_CTRL));
			/*serena_modify*/
			retVal = rxDataCmp(rxBuf,chIdx,&bufIdx,sampleSize);
			if(retVal == FAIL){
				break;
			}
			
			/*if(loopBackCounter > 495){
				printk("5 ");
			}*/

		}
	}
	/*serena_modify*/
	if(retVal == FAIL){
		failBreak = 1;
	}
	return retVal;
}
/*_____________________________________________________________________________
**      function name: rxChBufCmp
**      descriptions:
**           channel buffer cmpare
**      parameters:
**      global:
**
**      return:
**	         void
**      call:
**
**      revision:
**      	1.1 2008/10/13 19:00  pork
**____________________________________________________________________________
*/
void rxChDataCmp(void){
	int i;
	int flag;
	//int j = 0;
	int tmpBitWidth=0;
	uint32 bufAddr = 0;
	uint8* rxChBuf;
	uint8* tmpRxChBuf;
	unsigned int sampleSize;
	/*serena_modify*/
	int retVal = SUCCESS;
	spin_lock_irqsave(&loopLock,flag);
	while((rxChBuf = pcmRecvChBufDequeue()) != NULL){
		tmpRxChBuf = rxChBuf;
		sampleSize = pcmRecvSampleSizeGet(rxChBuf);
		if(sampleSize > 0){
			for(i=0;i<configNode.chNum;i++){
				if(configNode.bitWidth[i] == BITWIDTH_8){
					tmpBitWidth = 8;
					bufAddr += sampleSize;
				}
				else{
					tmpBitWidth = 16;
					bufAddr += (sampleSize*2);
				}
				if(loopBackCounter <= MAX_CMP_COUNTER){
					/*serena_modify*/
					retVal = rxBufCmp(tmpRxChBuf,i,sampleSize*(tmpBitWidth>>3));
					if(retVal == FAIL)
						break;

					tmpRxChBuf +=(sampleSize*(tmpBitWidth>>3));
				}
				else{
					memset(chDataInfor,0,sizeof(chDataInfor_t)*MAX_CH_NUM);
					memset(headerPatIdx,0,sizeof(int)*MAX_CH_NUM);
				}
				//loopBackCounter++;
			}
		}
		else{
			printk("module test : Buf Full \n");
		}
		//printk("Cmp-%d ",loopBackCounter);
		//printk("deqb: 0x%08lx \n",(uint32)callBackParam.oriRxChBuf);
		//printk("@0x%08lx\n",regRead(OPERATIONAL_BASE|TX_RX_DMA_CTRL));
		pcmRecvChBufFree(rxChBuf);
		//printk("(%d)\n",queueSizeGet());
		/*serena_modify*/
		if(failBreak){
			break;
		}
	}
	spin_unlock_irqrestore(&loopLock,flag);
}


uint8* chTxBuf[MAX_CH_NUM];
void rxChDataCopy(void)
{
	int i;
	int flag;
	int ret;
	int k,j = 0;
	int tmpBitWidth=0;
	uint32 bufAddr = 0;
	uint8* rxChBuf;
	uint8* tmpRxChBuf;
	
	uint8* tmpTxChBuf = NULL;
	unsigned int sampleSize,total_size=0;
	/*serena_modify*/
	int retVal = SUCCESS;

	//spin_lock_irqsave(&loopLock,flag);
	k=0;
	{
		if((rxChBuf = pcmRecvChBufDequeue()) != NULL)
		{	
			total_size = 0;
			retVal = SUCCESS;
			
			tmpRxChBuf = rxChBuf;
			sampleSize = pcmRecvSampleSizeGet(rxChBuf);
			if(sampleSize > 0)
			{
				tmpTxChBuf = pcmSendBufAlloc(2*sampleSize*(16>>3));
				if(tmpTxChBuf!=NULL)
				{
					//for(i=0;i<2;i++)
					{
						tmpBitWidth = 16;
						chTxBuf[0] = tmpTxChBuf;
						chTxBuf[1] = tmpTxChBuf+(sampleSize*(tmpBitWidth>>3));
						//memcpy(chTxBuf[0], tmpRxChBuf+(sampleSize*(tmpBitWidth>>3)), sampleSize*(tmpBitWidth>>3));
						//memcpy(chTxBuf[1], tmpRxChBuf, sampleSize*(tmpBitWidth>>3));
						memset(chTxBuf[0], 0, sampleSize*(tmpBitWidth>>3));
						memset(chTxBuf[1], 0, sampleSize*(tmpBitWidth>>3));
						total_size = 2*sampleSize*(tmpBitWidth>>3);
					}
				
					ret = pcmSend(chTxBuf, total_size, &flag);
					if(!ret){
						//printk("pcmSend=0x%04X s=%d bytes ql=%d\n",flag,sampleSize,queueSizeGet());
						pcmTxBufKfree(tmpTxChBuf);
						//udelay(10);
					}
					//else
						//break;
						//printk("[G]\n");			
				}
				else
				{
					printk("pcmSendBufAlloc NULL\n");
					//break;
				}			
			}
			else{
				printk("module test : Buf Full \n");
			}
			
			pcmRecvChBufFree(rxChBuf);
			
		}
	}
	
	//spin_unlock_irqrestore(&loopLock,flag);
}
/*_____________________________________________________________________________
**      function name: loopBackTest
**      descriptions:
**            do loopback to test driver export function pcmsend,pcmrecv and pcmBufFree
**
**      parameters:
**		None
**      global:
**            None
**
**      return:
**	          None
**      call:
**      		loopBackTest
**      revision:
**      	1.1 2008/08/13 19:00  pork
**____________________________________________________________________________
*/
void loopBackTest(void){
	int i;
	int bitWidTestCase = MIX_ALL_EIGHT;
	int chNum = 1;
	int tmpBitWidth = 0;
	int pcmBitClock = PCM_BIT_CLOCK_256;
	int sampleFreq = PCM_SAMPLE_CLOCK_8;
	int tmpMaxChNum = 0;
	uint8* chTxBuf[MAX_CH_NUM];
	uint8* tmpChTxBuf = NULL;
	int tmpBufSize;
	int tmpChBufSize;
	int flag = 0;
	int lockFlag; /*20110217_serena_modify*/

	pcmRecvFuncRegister((void *)rxChDataCmp);
	while(1){
		for(bitWidTestCase = MIX_EIGHT_SIXTEEN; bitWidTestCase<=MIX_ALL_SIXTEEN;bitWidTestCase++){
			for(sampleFreq = PCM_SAMPLE_CLOCK_8;sampleFreq<=PCM_SAMPLE_CLOCK_16;sampleFreq++){
				for(pcmBitClock = PCM_BIT_CLOCK_256;pcmBitClock <=PCM_BIT_CLOCK_2048;pcmBitClock++){
					printk("bitWidTestCase:%d ",bitWidTestCase);
					tmpMaxChNum = MaxChNumCal(sampleFreq,pcmBitClock,bitWidTestCase);
					printk("ret %d \n\n",tmpMaxChNum);
					for(chNum =1;chNum <= tmpMaxChNum;chNum++){
						spin_lock_irqsave(&loopLock,lockFlag); /*20110217_serena_modify*/
#if 1
						memset(chDataInfor,0,sizeof(chDataInfor_t)*MAX_CH_NUM);
						for(i=0;i<MAX_CH_NUM;i++){
							headerPatIdx[i] = 0;
						}
#endif
						pcmConfigCall(bitWidTestCase,chNum,pcmBitClock,sampleFreq);
						//printk("- ");
						loopBackCounter = 0;
						loopBackFailCounter = 0;
						memset(chTxBuf,0,sizeof(char *)*MAX_CH_NUM);
						//printk("! ");
						pcmRecv();
						spin_unlock_irqrestore(&loopLock,lockFlag); /*20110217_serena_modify*/
						//printk("@ ");
						while(loopBackCounter < MAX_CMP_COUNTER){
							tmpBufSize = 0;
							tmpChBufSize = 0;
							flag= 0;	/*20110217_serena_modify*/

							for(i = 0;i<configNode.chNum;i++){
								if(configNode.bitWidth[i] == BITWIDTH_8){
									tmpBitWidth = 8;
								}
								else{
									tmpBitWidth = 16;
								}
								tmpBufSize += txLoopBackSampleSize*(tmpBitWidth>>3);
							}
							tmpChTxBuf = pcmSendBufAlloc(tmpBufSize);
							if(tmpChTxBuf == NULL){
								printk("buf alloc failed\n");
								break;
							}
							else{
								for(i = 0;i<configNode.chNum;i++){
									chTxBuf[i] = tmpChTxBuf;
									if(configNode.bitWidth[i] == BITWIDTH_8){
										tmpChBufSize = txLoopBackSampleSize;
									}
									else{
										tmpChBufSize = (txLoopBackSampleSize<<1);
									}
									bufWrite(tmpChTxBuf,tmpChBufSize);
									tmpChTxBuf += tmpChBufSize;
								}
								//bufWrite(chTxBuf,tmpBufSize);
								//printk("50x%08lx\n",regRead(OPERATIONAL_BASE|TX_RX_DMA_CTRL));
								dataPatType = (dataPatType+1)%4;
								if(!pcmSend(chTxBuf,txLoopBackSampleSize,&flag)){
									pcmTxBufKfree(chTxBuf[0]);
								}
							/*20110217_serena_modify*/
								else{
									if(flag == TX_DESC_OVER_TEN){
										schedule_timeout(msecs_to_jiffies(10));
									}
								}
							/*End 20110217_serena_modify*/
							}
							//loopBackCounter ++;
							/*serena_modify*/
							if(failBreak){
								break;
							}
						}
						sampleSizeIncrease(TX);
						if(txLoopBackSampleSize ==12){
							sampleSizeIncrease(RX);
						}
						printk("Cmp S-%d F-%d tx%d,rx %d qs:(%d)\n",loopBackCounter,loopBackFailCounter,txLoopBackSampleSize,rxLoopBackSampleSize,queueSizeGet());
						//pcmConfigCall(bitWidTestCase,chNum,pcmBitClock,sampleFreq);
						/*serena_modify*/
						if(failBreak){
							break;
					}
				}
					/*serena_modify*/
					if(failBreak){
						break;
			}
		}
				/*serena_modify*/
				if(failBreak){
					break;
				}
			}
			/*serena_modify*/
			if(failBreak)
				 break;
		}
		/*serena_modify*/
		if(failBreak)
			break;
	}
}

/*_____________________________________________________________________________
**      function name: exloopBackTest
**      descriptions:
**            do exloopback to test driver export function pcmsend,pcmrecv and pcmBufFree
**
**      parameters:
**		None
**      global:
**            None
**
**      return:
**	          None
**      call:
**      		exloopBackTest
**____________________________________________________________________________
*/
void exloopBackTest(void){
	int i;
	int bitWidTestCase = MIX_ALL_SIXTEEN;
	int chNum = 1;
	int tmpBitWidth = 0;
	int pcmBitClock = PCM_BIT_CLOCK_2048;
	int sampleFreq = PCM_SAMPLE_CLOCK_8;
	int tmpMaxChNum = 0;
	uint8* chTxBuf[MAX_CH_NUM];
	uint8* tmpChTxBuf = NULL;
	int tmpBufSize;
	int tmpChBufSize;
	int flag = 0;
	int lockFlag; /*20110217_serena_modify*/

	
	pcmRecv();
	return;
	
}
/*_____________________________________________________________________________
**      function name: pcmDriverTestInit
**      descriptions:
**            pcm driver test init
**
**      parameters:
**		None
**      global:
**            None
**
**      return:
**	          None
**      call:
**      		subcmd
**      revision:
**      	1.1 2008/08/13 19:00  pork
**____________________________________________________________________________
*/
extern int SPI_cfg(int id);
extern int inloopback = 0;
static int __init pcmDriverTestInit(void){
	int i;
	u32 data;
	/*set up pcm ci-command and register it*/
	/*swcCmd.name= "swc";
	swcCmd.func=doSwc;
	swcCmd.flags=0x12;
	swcCmd.argcmin=0;
	swcCmd.argc_errmsg=NULL;*/
	memset(chDataInfor,0,sizeof(chDataInfor_t)*MAX_CH_NUM);
	memset(headerPat,0,sizeof(headerPat));
	printk("<<pcmdriver test 15>>\n");
	for(i=0;i<MAX_CH_NUM;i++){
		headerPat[i][0]=HEADER_PATTERN_FIRST_BYTE;
		headerPat[i][1]=HEADER_PATTERN_SECOND_BYTE;
		headerPat[i][2]=HEADER_PATTERN_THIRD_BYTE;
		headerPat[i][3]=HEADER_PATTERN_FOUTH_BYTE;
	}
	//memset(&configNode,0,sizeof(configNode_t));
	//cmd_register(&swcCmd);
#ifdef PCM_LOOPBACK_TEST	
//	loopBackTest();
	pcmConfigCall(MIX_ALL_SIXTEEN,2,PCM_BIT_CLOCK_2048,PCM_SAMPLE_CLOCK_8);
	while(inloopback==0)
	{
	schedule_timeout(msecs_to_jiffies(5000));
	printk("inloopback=%d\n",inloopback);
	}
#else

#if 1	
	data = regRead(0xBFB00860);
	data |= 0x1;
	regWrite(0xBFB00860, data);
	pcmConfigCall(MIX_ALL_SIXTEEN,2,PCM_BIT_CLOCK_2048,PCM_SAMPLE_CLOCK_8);
	pcmRecvFuncRegister((void *)rxChDataCopy);
	//pcmRestart(NOT_TO_INIT);
	//pcmRestart(CONFIG_INIT);
	data = regRead(0xBFB00860);
	data &= ~(0x1<<6);
	data &= ~(0x1<<5);
	data &= ~(0x1<<3);
	regWrite(0xBFB00860, data); 
#endif	
	SLIC_reset(RESET);
	udelay(5000);
	SLIC_reset(ENABLE);
	udelay(5000);//convert 2 bit no convert
		
	printk("Config GPIO2 for SPI CH5\n");
	data = regRead(0xBFB00860);
	data &= ~(0x1<<6);
	data |= (0x1<<5);
	data &= ~(0x1<<3);
	regWrite(0xBFB00860, data); 
	printk("Config GPIO20~23 for PCM\n");
	
	
	SPI_cfg(4);
	printk("SPI_cfg(4)\n");
	printk("si%d initialization\n",slic_type);
	if (slic_type==32178)
	{
		if(CustomerVoice_HWInit()==0)
		{
			printk("si%d initialization failed\n",slic_type);
			return 0;
		}	
	}
	else 
	{ 
		if(ProSLIC_HWInit()==0)
		{
			printk("si%d initialization failed\n",slic_type);
			return 0;
		}
	}
	//exloopBackTest();
#endif	
	return 0;
}
/*_____________________________________________________________________________
**      function name: pcmDriverTestExit
**      descriptions:
**            pcm driver test exit
**      parameters:
**		None
**      global:
**            None
**      return:
** 		None
**      call:
**      revision:
**____________________________________________________________________________
*/

static void __exit pcmDriverTestExit(void){
	//cmd_unregister(&swcCmd);
	printk(KERN_INFO "PCM driver test program exit\n");
}
MODULE_LICENSE("GPL");
module_param_named(slic, slic_type, int, S_IRUGO);
module_init(pcmDriverTestInit);
module_exit(pcmDriverTestExit);

