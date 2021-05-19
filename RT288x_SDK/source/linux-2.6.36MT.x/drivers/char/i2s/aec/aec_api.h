/*
 * aec_api.h
 *
 *  Created on: 2014/2/13
 *      Author: mtk04880
 */

#ifndef AEC_API_H_
#define AEC_API_H_

#include <linux/proc_fs.h>
#include "aec_inc/bli_exp.h"

#define AEC_SUCCESS (0)
#define AEC_FAIL	(-1)

typedef enum supportedSR{
	SR_8000 = 0,
	SR_11025,
	SR_12000,
	SR_16000,
	SR_22050,
	SR_24000,
	SR_32000,
	SR_44100,
	SR_48000,
	SR_88200,
	SR_96000
}supportedSR_e;

typedef	enum e_lec_mode
{
	LEC_DISABLE = 0x1,
	LEC_16MS = 0x2,
	LEC_32MS = 0x3,
	LEC_48MS = 0x4,
	LEC_128MS = 0x5
} E_LEC_MODE;

typedef struct aecCtrlBlk_s{
	unsigned int cid;
	unsigned int ecTailLen;
	unsigned int ecEnable;
	unsigned int streamSR;
	unsigned int reserved;
}aecCtrlBlk_t;

typedef struct aecFuncPtr_s{
	int (*AECQueryConfig)(aecCtrlBlk_t* aecCtrlBlk_p);
	int (*AECReInit)(aecCtrlBlk_t* aecCtrlBlk_p);
	int (*AECFeEnq)(int cid,void* buf,int size);
	int (*AECECDeq)(int cid,void* buf,int size);
	int (*AECNeEnq)(int cid,void* buf,int size);
}aecFuncTbl_t;

int AEC_init(aecCtrlBlk_t* aecCtrlBlk_p);
int AEC_fe_enq(void* buf,int size);
int AEC_ne_deq(void* buf,int size);
int AEC_ne_enq(void* buf,int size);

#endif /* AEC_API_H_ */
