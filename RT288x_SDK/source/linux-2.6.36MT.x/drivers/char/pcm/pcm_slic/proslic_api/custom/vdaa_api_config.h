/*
** Copyright ?2008-2010 by Silicon Laboratories
**
** $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/linux-2.6.36MT.x/drivers/char/pcm/pcm_slic/proslic_api/custom/vdaa_api_config.h#1 $
**
** vdaa_api_config.h
** VoiceDAA header config file
**
** Author(s): 
** naqamar, laj
**
** Distributed by: 
** Silicon Laboratories, Inc
**
** This file contains proprietary information.	 
** No dissemination allowed without prior written permission from
** Silicon Laboratories, Inc.
**
** File Description:
** This file is used 
** in the VoiceDAA demonstration code. 
**
**
*/

#ifndef VDAA_API_CFG_H
#define VDAA_API_CFG_H

/* #define DISABLE_MALLOC */
/* #define DISABLE_VDAA_RING_DETECT_SETUP */
/* #define DISABLE_VDAA_AUDIO_GAIN_SETUP */
/* #define DISABLE_VDAA_PCM_SETUP */
/* #define DISABLE_VDAA_COUNTRY_SETUP */
/* #define DISABLE_VDAA_HYBRID_SETUP */
#define DISABLE_VDAA_LOOPBACK_SETUP
#define DISABLE_VDAA_IMPEDANCE_SETUP

#ifndef ENABLE_DEBUG
#define ENABLE_DEBUG
#endif


//#include <linux/config.h>
#include <linux/kernel.h>

//#include "stdio.h"
#define LOGPRINT(fmt, args...) printk("DAA_API: " fmt, ## args)

#endif