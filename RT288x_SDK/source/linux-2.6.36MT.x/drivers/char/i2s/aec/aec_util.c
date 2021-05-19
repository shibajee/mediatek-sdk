/*
 * aec_util.c
 *
 *  Created on: 2014/3/18
 *      Author: MTK04880
 */
#include <asm/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/spinlock.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/dma-mapping.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <asm/addrspace.h>
#include "aec_inc/aec_util.h"
#include "aec_api.h"

#define SEM_NUM_MAX 4

static struct semaphore mutex[SEM_NUM_MAX];
enum {WAIT_NOWAIT = 0, WAIT_FOREVER = -1};
#define TRY	do{}while(0)

void MUTX_Get(int mid, int mutx_wait){
	if((mid <0) || (mid > SEM_NUM_MAX))
		return AEC_FAIL;

	switch(mutx_wait){
	case WAIT_NOWAIT:
		if(down_trylock(&mutex[mid])){
				TRY;
		}
		break;
	case WAIT_FOREVER:
		down(&mutex[mid]);
		break;
	default:
		TRY;
		break;
	}
	return AEC_SUCCESS;
}

void MUTX_Put(int mid){
	if((mid <0) || (mid > SEM_NUM_MAX))
		return AEC_FAIL;
	up(&mutex[mid]);

	return AEC_SUCCESS;
}

int MUTX_Init(int mid){
	if((mid <0) || (mid > SEM_NUM_MAX))
		return AEC_FAIL;
	init_MUTEX(&mutex[mid]);
	return AEC_SUCCESS;
}

