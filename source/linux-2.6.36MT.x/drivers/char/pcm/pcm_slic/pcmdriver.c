#include <asm/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/slab.h> 
//#include "error.h"
#include <asm/rt2880/surfboardint.h>
#include "_pcm.h"

int inloopback;

extern int SPI_cfg(int id);
extern int CustomerVoice_HWInit(void);
extern int ProSLIC_HWInit(void);

static irqreturn_t pcmIsr(int irq, void *devId) {
	pcmIsrTH();
	return IRQ_HANDLED;
}

static int __init pcmDriverInit(void) {
	int err = 0;
	int flag = 0;
	int data = 0;

	pcmInit();
	err = request_irq(SURFBOARDINT_PCM, pcmIsr, 0, "PCM", NULL);
	
	if (err) {
		printk(KERN_INFO "request irq fail \n");
		return err;
	}

	pcmStart();
	return 0;
}

static void __exit pcmDriverExit(void) {
	free_irq(SURFBOARDINT_PCM, NULL);
}

MODULE_LICENSE("GPL");
module_init(pcmDriverInit);
module_exit(pcmDriverExit);
