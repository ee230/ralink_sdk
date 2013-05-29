/*
 * queue.c
 *
 *  Created on: 2008/10/7
 *      Author: pork
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/gfp.h>
#include <linux/spinlock.h>
#include "queue.h"

#define CACHE_TO_NONCACHE(addr)			((uint32)(addr) | 0xa0000000)
#define NONCACHE_TO_CACHE(addr)			((uint32)(addr) & 0xdfffffff)

int queueSize = 0;
uint8* rxQueueHead = NULL;
uint8* rxQueueTail = NULL;
spinlock_t PCM_RX_QUEUE_LOCK = SPIN_LOCK_UNLOCKED;

/*_____________________________________________________________________________
**      function name: enqueue
**      descriptions:
**      		fill pass-in rxbuf in queue
**      parameters:
**      	rxBuf: rx buffer address
**      global:
**      	rxQueueHead
**      	rxQueueTail
**      return:
**	         queue size
**      call:
**      		rxBufNextSet
**      revision:
*/
int enqueue(uint8* rxBuf){

	int flag;
	if(rxBuf == NULL){
		return 0;
	}
	spin_lock_irqsave(&PCM_RX_QUEUE_LOCK,flag);
	//printk("enq:%08lx \n",rxBuf);
	//printk("enq:%08lx \n",rxBufMagicNumGet(rxBuf));
	//printk("enq:%d \n",getRxBufSampleSize(rxBuf));
	rxBufNextSet(rxBuf,NULL);
	//rxBufNumSet(rxBuf);
	if(rxQueueHead == NULL){
		rxQueueTail = rxQueueHead = rxBuf;
	}
	else{
		rxBufNextSet(rxQueueTail,rxBuf);
		rxQueueTail = rxBuf;
	}
	queueSize++;
	spin_unlock_irqrestore(&PCM_RX_QUEUE_LOCK,flag);
	return queueSize;
}
/*_____________________________________________________________________________
**      function name: dequeue
**      descriptions:
**      		dequeue rx buffer and return
**      parameters:
**      	void
**      global:
**      return:
**	         rx buffer address
**      call:
**      		rxBufNextSet
**      revision:
*/
uint8* dequeue(void){
	uint8* retBuf = NULL;
	int flag;
	spin_lock_irqsave(&PCM_RX_QUEUE_LOCK,flag);
	retBuf = rxQueueHead;
	if(retBuf){
		//rxBufNumGet(retBuf);
		rxQueueHead = rxBufNextGet(rxQueueHead);
		rxBufNextSet(retBuf,NULL);
		queueSize--;
	}
	spin_unlock_irqrestore(&PCM_RX_QUEUE_LOCK,flag);
	return retBuf;
}
/*_____________________________________________________________________________
**      function name: queueSizeGet
**      descriptions:
**      		return currnet queue size
**      parameters:
**      	void
**      global:
**      	queueSize
**      return:
**	         queueSize
**      call:
**
**      revision:
*/
int queueSizeGet(void){
	return queueSize;
}

