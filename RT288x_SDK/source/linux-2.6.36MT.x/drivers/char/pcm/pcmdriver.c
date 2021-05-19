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
#include "slic_ctrl.h"
#define VPint			*(volatile unsigned int *)
#define regRead(reg) 			VPint(reg)
#define regWrite(reg,regValue) 	VPint(reg)=regValue

extern void 	SLIC_reset(int reset);

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

#ifdef PCM_LOOPBACK_TEST
#else	
	data = regRead(0xBFB00860);
	data |= 0x1;
	regWrite(0xBFB00860, data);
	data = regRead(0xBFB00860);
	data &= ~(0x1<<6);
	data &= ~(0x1<<5);
//	data &= ~(0x1<<1);
	data &= ~(0x1<<3);
	regWrite(0xBFB00860, data); 
	
	SLIC_reset(RESET);
	udelay(5000);
	SLIC_reset(ENABLE);
	udelay(5000);//convert 2 bit no convert
		
	//printk("Config GPIO2 for SPI CH1\n");
	printk("Config GPIO31 for SPI CH5\n");
	data = regRead(0xBFB00860);
	data &= ~(0x1<<6);
	data |= (0x1<<5);
//	data |= (0x1<<1);
	data &= ~(0x1<<3);
	regWrite(0xBFB00860, data); 
	printk("Config GPIO20~23 for PCM\n");
	
	SPI_cfg(4);
	printk("SPI_cfg(4)\n");	 
	ProSLIC_HWInit();
#endif	
	
#if !defined(CONFIG_PCM_SLT)
	printk("pcmStart\n");
	pcmStart();
#endif	
	return 0;
}

static void __exit pcmDriverExit(void) {
	free_irq(SURFBOARDINT_PCM, NULL);
	pcmStop();
	mdelay(100);
	pcmExit();

}

MODULE_LICENSE("GPL");
EXPORT_SYMBOL(inloopback);
EXPORT_SYMBOL(pcmStart);
EXPORT_SYMBOL(pcmStop);
module_init(pcmDriverInit);
module_exit(pcmDriverExit);
