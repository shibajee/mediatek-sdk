/*
** $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/linux-2.6.36MT.x/drivers/char/pcm/pcm_slic/slic_ctrl.c#1 $
*/
/************************************************************************
 *
 *	Copyright (C) 2008 Trendchip Technologies, Corp.
 *	All Rights Reserved.
 *
 * Trendchip Confidential; Need to Know only.
 * Protected as an unpublished work.
 *
 * The computer program listings, specifications and documentation
 * herein are the property of Trendchip Technologies, Co. and shall
 * not be reproduced, copied, disclosed, or used in whole or in part
 * for any reason without the prior express written permission of
 * Trendchip Technologeis, Co.
 *
 *************************************************************************/
/*
** $Log: slic_ctrl.c,v $
** Revision 1.2.2.1  2012-03-06 13:01:47  kurtis
** remove rt63365 keyword to rt6855A
**
** Revision 1.2  2012-01-10 07:27:14  qwert
** Add GPIO >= 16 case
**
** Revision 1.1  2011-11-14 07:49:29  qwert
** Add RT6855A pcm driver
**
** Revision 1.3  2011/02/25 09:16:06  serenahuang_hc
** delaytime is the the same value on all slic devices
**
** Revision 1.2  2011/01/06 15:03:30  pork
** commit TDI layer with Zarlink 2S1O
**
** Revision 1.1.1.1  2010/09/30 21:14:56  josephxu
** modules/public, private
**
** Revision 1.2  2010/06/17 11:41:50  jrchen_hc
** Add TC3182 PCM and SLIC support for VoIP
**
** Revision 1.1  2010/04/09 11:05:00  feiyan
** tclinux_phoenix new Trunk
** Add all voip driver in one folder
**
** Revision 1.1.1.1  2009/12/17 01:47:53  josephxu
** 20091217, from Hinchu ,with VoIP
**
 */
//#include <asm/semaphore.h>
#include <linux/semaphore.h>
#include <linux/autoconf.h>
#include <linux/param.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include "slic_ctrl.h"

/*MODULE_LICENSE("Dual BSD/GPL");*/

/************************************************************************
*                          C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                            M A C R O S
*************************************************************************
*/

/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                        P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                      E X T E R N A L   D A T A
*************************************************************************
*/
//extern struct semaphore SPI_SEM;//The semaphore exported from the kernel and protecting atomic SPI Controller
struct semaphore SPI_SEM;
DECLARE_MUTEX(SPI_SEM);//Make sure all related SPI operations are atomic
/************************************************************************
*                       P R I V A T E   D A T A
*************************************************************************
*/
/**********************************************
 * Related Declaration of SPI Module Registers *
 **********************************************/
static __u32 reg;
static __u32 reg0x04;
static __u32 reg0x08;
static __u32 reg0x28;
static __u32 reg0x2c;


const int delaytime = 2;

/************************************************************************
*        F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
/**********************************************
 * Read/write functions of SPI Module Registers *
 **********************************************/
/*_____________________________________________________________________________
**      function name: spi_regread32
**      descriptions:
**           	Read SPI Controller register by the offset reg
**      parameters:
**     	 	reg: Register Offset
**      global:
**            	None
**      return:
**	          The value of the register
**      call:
**      	None
**      revision:
**      	1.1 2008/09/26 14:30  Ian
**____________________________________________________________________________
*/
static __u32
spi_regread32(int reg)
{
	volatile __u32 *addr = (__u32 *)(CR_SPI_BASE + reg);
	__u32 data = *addr;

	return (data);
}

/*_____________________________________________________________________________
**      function name: spi_regwrite32
**      descriptions:
**           	Write SPI Controller register by the offset reg
**      parameters:
**     	 	reg: Register Offset
**      global:
**            	None
**      return:
**	          None
**      call:
**      	None
**      revision:
**      	1.1 2008/09/26 14:30  Ian
**____________________________________________________________________________
*/
static void
spi_regwrite32(int reg, __u32 data)
{
	volatile __u32 *addr = (__u32 *)(CR_SPI_BASE + reg);

	*addr = data;

	return;
}

/*_____________________________________________________________________________
**      function name: SPI_cfg
**      descriptions:
**           	Set the SPI controller to the more buffer mode, and choose the SLIC device by id
**      parameters:
**     	 	id: The identification number of SLIC device
**      global:
**            	SPI_SEM: The semaphore exported from the kernel and protecting atomic SPI Controller
**      return:
**	          S_OK: Configure successfully
**		S_EROR: Configure incorrectly
**      call:
**      	down(): Check the SPI_SEM semaphore
**		spi_regread32(): Read SPI Controller register by the offset reg
**		spi_regwrite32(): Write SPI Controller register by the offset reg
**		up(): Release the SPI_SEM semaphore
**      revision:
**      	1.1 2008/09/26 14:30  Ian
**____________________________________________________________________________
*/
int SPI_cfg(int id)
{
	down(&SPI_SEM);

	do {
		reg = spi_regread32(SPI_FLASH_CTL);
	} while (reg & SPI_CTL_BUSY);

	//Turn on the more buffer mode and select device
	reg0x28 = spi_regread32(SPI_FLASH_MM);
	//printk("SPI_reg0x28 -- 0x%08lx \n",reg0x28);
	reg0x2c = spi_regread32(SPI_FLASH_MBC);
	reg0x28 |= 0x4;//Set bit [2] to 1 to enter more buffer mode
	reg0x28 &= ~(0x7 << 29);//Set bits [31:29] to 0
	/*add the margin between SPI CLK and CS*/
	reg0x28 |= (0x1 << 8);//Set bits [31:29] to 0
	reg0x28 |= (id + 1) << 29;//Set bits [31:29] to select SPI device
#ifdef SLIC_SILICON
	reg0x28 |= (1 << 4);  /*Rodney_test*/
	reg0x28 |= (1 << 5);  /*Rodney_test*/
#endif

#if 1
	/*set cpol and cpoa to 1*/
	//reg0x28 |= 0x3 <<4; // Set bits [5:4] to 0x3;
	/*lower the SPI clock = HCLK/64*/
	reg0x28 &= ~(0xfff << 16);//Set bits [27:16] to 0
	reg0x28 |= 0x3E << 16;
#endif
	//printk("after SPI_reg0x28 -- 0x%08lx \n",reg0x28);
	spi_regwrite32(SPI_FLASH_MM, reg0x28);
	//Check the initialized value

#if 0  /*Rodney_20091121*/
	if(reg0x28 == spi_regread32(SPI_FLASH_MM)){
		//printk("after read :SPI_reg0x28 -- 0x%08lx \n",reg0x28);
		up(&SPI_SEM);
		return S_OK;
	}
	else{
		up(&SPI_SEM);
		return S_ERROR;
	}
#endif
	up(&SPI_SEM);
	return S_OK;
}

/*_____________________________________________________________________________
**      function name: SLIC_reset
**      descriptions:
**           	Reset or enable two SLIC devices
**      parameters:
**     	 	reset: Identify the type of the operation to be performed
**      global:
**            	None
**      return:
**	          None
**      call:
**      	None
**      revision:
**      	1.1 2008/09/26 14:30  Ian
**____________________________________________________________________________
*/
void SLIC_reset(int reset)
{
	//printk("slic gpio:%d \n",SLIC1_GPIO);
	//VPint(CR_GPIO_CTRL) |= (1<<((SLIC1_GPIO)*2));
	if (SLIC1_GPIO <= 15)
	{
		VPint(CR_GPIO_CTRL) |= (1<<((SLIC1_GPIO*2)));	
	VPint(CR_GPIO_ODRAIN) |= (1<<(SLIC1_GPIO));
	}
	else
	{
        VPint(CR_GPIO_CTRL1) |= (1<<(((SLIC1_GPIO-16)*2)));
        VPint(CR_GPIO_ODRAIN) |= (1<<(SLIC1_GPIO));
	}		
	//printk("slic gpio:%d \n",SLIC2_GPIO);
//	VPint(CR_GPIO_CTRL) |= (1<<((SLIC2_GPIO)*2));
//	VPint(CR_GPIO_ODRAIN) |= (1<<(SLIC2_GPIO));
	if (reset == RESET){
		/**(volatile unsigned int*)0xAFF01404 &= ~RDGR_SLIC_SW_RST_L;**/
		/*#warning "Must be Implement"*/
		VPint(CR_GPIO_DATA) &= ~(1<<(SLIC1_GPIO));
		//VPint(CR_GPIO_DATA) &= ~(1<<(SLIC2_GPIO));
	}
	else{
		/**(volatile unsigned int*)0xAFF01404 |= RDGR_SLIC_SW_RST_L;**/
		/*#warning "Must be Implement"*/
		VPint(CR_GPIO_DATA) |= (1<<(SLIC1_GPIO));
		//VPint(CR_GPIO_DATA) |= (1<<(SLIC2_GPIO));
	}

}

/*_____________________________________________________________________________
**      function name: SPI_bytes_write
**      descriptions:
**           	Issue a MPI command for writing SLIC's register
**      parameters:
**     	 	id: The identification number of SLIC device
**		cmd: MPI Command code
**		data_ptr: A pointer for the data which is going to be written
**		cmdLen: The data length of the data
**      global:
**            	SPI_SEM: The semaphore exported from the kernel and protecting atomic SPI Controller
**      return:
**	          S_OK: Finish writing
**      call:
**      	down(): Check the SPI_SEM semaphore
**		spi_regread32(): Read SPI Controller register by the offset reg
**		spi_regwrite32(): Write SPI Controller register by the offset reg
**		up(): Release the SPI_SEM semaphore
**      revision:
**      	1.1 2008/09/26 14:30  Ian
**____________________________________________________________________________
*/
#ifdef SLIC_SILICON
int SPI_bytes_write(int id, unsigned char ctrl, unsigned char cmd, unsigned char *data_ptr, unsigned char cmdLen)
#else
int SPI_bytes_write(int id, unsigned char cmd, unsigned char *data_ptr, unsigned char cmdLen)
#endif
{
	unsigned char i;

	down(&SPI_SEM);

	//printk("[%s] id=%d ctrl=%X, cmd=%X data=%02X\n",__FUNCTION__,id,ctrl,cmd,*(data_ptr + 0));
	do {
		reg = spi_regread32(SPI_FLASH_CTL);
	} while (reg & SPI_CTL_BUSY);

	//Prepare register for sending MPI command
	reg0x28 &= ~(0x7 << 29);//Set bits [31:29] to 0
	reg0x28 |= (id + 1) << 29;//Set bits [31:29] to select SPI device
	reg0x2c &= ~0x1ff;//Set bits [8:0] to 0 (Transmitted data bit counts)
	reg0x2c &= ~(0x1ff << 12);//Set bits [20:12] to 0 (Received data bit counts)
	reg0x2c &= ~(0x3f << 24);//Set bits[29:24] to 0 (command bit counts)
	reg0x2c |= (1 * 8) << 24;//Set bits [29:24] to 8 (Command bit counts)
	spi_regwrite32(SPI_FLASH_MM, reg0x28);
	spi_regwrite32(SPI_FLASH_MBC, reg0x2c);
#ifdef SLIC_SILICON
	spi_regwrite32(SPI_FLASH_OPCODE, ctrl);
	reg |= SPI_CTL_START;
	spi_regwrite32(SPI_FLASH_CTL, reg);
	udelay(delaytime);
	do {
      	reg = spi_regread32(SPI_FLASH_CTL);
      	udelay(delaytime);
    } while (reg & SPI_CTL_BUSY);
#endif


	//Send command for writing MPI
	spi_regwrite32(SPI_FLASH_OPCODE, cmd);

	reg |= SPI_CTL_START;
	spi_regwrite32(SPI_FLASH_CTL, reg);
	udelay(delaytime);
	do {
      	reg = spi_regread32(SPI_FLASH_CTL);
      	udelay(delaytime);
    } while (reg & SPI_CTL_BUSY);

	for(i = 0; i < cmdLen; i++){
		//Write the ith byte
		reg0x04 = (__u32) spi_regread32(SPI_FLASH_OPCODE);
		//printk("read opcode: %d \n",reg0x04);
		reg0x04 &= ~0xFF;//Set bits[7:0] to 0 (Opcode)
		reg0x04 |= *(data_ptr + i);//Set bits[7:0] to the content of the ith byte of *data_ptr (Opcode)
		//printk("write opcode: %d \n",reg0x04);
		spi_regwrite32(SPI_FLASH_OPCODE, reg0x04);
		reg |= SPI_CTL_START;
		spi_regwrite32(SPI_FLASH_CTL, reg);
		//printk("write CTL: %d \n",reg);
		udelay(delaytime);
		do {
	    	reg = spi_regread32(SPI_FLASH_CTL);
	    	udelay(delaytime);
	    } while (reg & SPI_CTL_BUSY);
	}

	up(&SPI_SEM);
	return S_OK;
}

/*_____________________________________________________________________________
**      function name: SPI_bytes_read
**      descriptions:
**           	Issue a MPI command for reading SLIC's register
**      parameters:
**     	 	id: The identification number of SLIC device
**		cmd: MPI Command code
**		data_ptr: A pointer for the data which is going to be read
**		cmdLen: The length of the data
**      global:
**            	SPI_SEM: The semaphore exported from the kernel and protecting atomic SPI Controller
**      return:
**	          S_OK: Finish reading
**      call:
**      	down(): Check the SPI_SEM semaphore
**		spi_regread32(): Read SPI Controller register by the offset reg
**		spi_regwrite32(): Write SPI Controller register by the offset reg
**		up(): Release the SPI_SEM semaphore
**      revision:
**      	1.1 2008/09/26 14:30  Ian
**____________________________________________________________________________
*/
#ifdef SLIC_SILICON
int SPI_bytes_read(int id, unsigned char ctrl, unsigned char cmd, unsigned char *data_ptr, unsigned char cmdLen)
#else
int SPI_bytes_read(int id, unsigned char cmd, unsigned char *data_ptr, unsigned char cmdLen)
#endif
{
	unsigned char i;
	__u32 data;
	down(&SPI_SEM);
	
	//Prepare register for sending MPI command
	reg0x28 &= ~(0x7 << 29);//Set bits [31:29] to 0
	reg0x28 |= (id + 1) << 29;//Set bits [31:29] to select SPI device
	reg0x2c &= ~0x1ff;//Set bits [8:0] to 0 (Transmitted data bit counts)
	reg0x2c &= ~(0x1ff << 12);//Set bits [20:12] to 0 (Received data bit counts)
	reg0x2c &= ~(0x3f << 24);//Set bits[29:24] to 0 (command bit counts)
	reg0x2c |= (1 * 8) << 24;//Set bits [29:24] to 8 (Command bit counts)
	spi_regwrite32(SPI_FLASH_MM, reg0x28);
	spi_regwrite32(SPI_FLASH_MBC, reg0x2c);
#ifdef SLIC_SILICON
	spi_regwrite32(SPI_FLASH_OPCODE, ctrl);
	reg |= SPI_CTL_START;
	spi_regwrite32(SPI_FLASH_CTL, reg);
	udelay(delaytime);
	do {
      	reg = spi_regread32(SPI_FLASH_CTL);
      	udelay(delaytime);
    } while (reg & SPI_CTL_BUSY);
#endif


	//Send command for reading MPI
	spi_regwrite32(SPI_FLASH_OPCODE, cmd);
	//printk("R: cmd %d \n",cmd);

	reg |= SPI_CTL_START;
	spi_regwrite32(SPI_FLASH_CTL, reg);
	udelay(delaytime);
	do {
      	reg = spi_regread32(SPI_FLASH_CTL);
      	udelay(delaytime);
    } while (reg & SPI_CTL_BUSY);

	//Prepare register for reading MPI
	reg0x2c &= ~0x1ff;//Set bits [8:0] to 0 (Transmitted data bit counts)
	reg0x2c &= ~(0x1ff << 12);//Set bits [20:12] to 0 (Received data bit counts)
	reg0x2c &= ~(0x3f << 24);//Set bits[29:24] to 0 (command bit counts)
	reg0x2c |= (1 * 8) << 12;//Set bits [20:12] to 8 (Received data bit counts)
	//printk("reg0x2c: 0x%08lx \n",reg0x2c);
	spi_regwrite32(SPI_FLASH_MBC, reg0x2c);
	spi_regwrite32(SPI_FLASH_DATA, 0);

	for(i = 0; i < cmdLen; i++){
		//Read the ith byte
		reg |= SPI_CTL_START;
		spi_regwrite32(SPI_FLASH_CTL, reg);
		//printk("R: reg %d \n",reg);
		udelay(delaytime);
		do {
	    	reg = spi_regread32(SPI_FLASH_CTL);
	    	udelay(delaytime);
	   	} while (reg & SPI_CTL_BUSY);
	   	reg0x08 = (__u32) spi_regread32(SPI_FLASH_DATA);
	   	data = reg0x08;
	   	udelay(delaytime);
#ifdef SLIC_SILICON
//*(data_ptr + i) = ((reg0x08 >> 7) & 0x000000ff);
#ifdef TC3262_PCM
//software workaround for msb shift problem, jrchen modify 20100409
  __u32 tmp = ((reg0x08 << 1) & 0xfe);
  tmp |= ((reg0x08 >> 15) & 0x1);
  *(data_ptr + i) = tmp;
//  printk("reg0x08 = %08x\n", reg0x08);
//  printk("work around tmp = %08x\n", tmp);
//	   	*(data_ptr + i) = reg0x08;
//		printk("read workaround\n");
#else
	*(data_ptr + i) = reg0x08;
#endif
#else
		*(data_ptr + i) = reg0x08;
#endif
	}
	//printk("[%s] id=%d ctrl=%X, cmd=%X, data=%02X\n",__FUNCTION__,id,ctrl,cmd,data);
	up(&SPI_SEM);
	return S_OK;
}

/*_____________________________________________________________________________
**      function name: slic_ctrl_init
**      descriptions:
**           	Initialization function for this module
**      parameters:
**     	 	None
**      global:
**            	None
**      return:
**	          S_OK: Finish initialization
**      call:
**      	printk(): Print a message
**      revision:
**      	1.1 2008/09/26 14:30  Ian
**____________________________________________________________________________

static int __init slic_ctrl_init(void)
{
	SLIC_reset(RESET);
	udelay(5000);
	SLIC_reset(ENABLE);
	udelay(5000);
	printk(KERN_ALERT "SLIC Controller Module has been loaded\n");
	return S_OK;
}

_____________________________________________________________________________
**      function name: slic_ctrl_exit
**      descriptions:
**           	Exit function for this module
**      parameters:
**     	 	None
**      global:
**            	None
**      return:
**	          S_OK: Finish initialization
**      call:
**      	printk(): Print a message
**      revision:
**      	1.1 2008/09/26 14:30  Ian
**____________________________________________________________________________

static void __exit slic_ctrl_exit(void)
{
	printk(KERN_ALERT "SLIC Controller Module has been unloaded\n");
}*/

/* Register startup/shutdown routines */
/*module_init(slic_ctrl_init);
module_exit(slic_ctrl_exit);*/
EXPORT_SYMBOL(SPI_cfg);
EXPORT_SYMBOL(SLIC_reset);
EXPORT_SYMBOL(SPI_bytes_write);
EXPORT_SYMBOL(SPI_bytes_read);

