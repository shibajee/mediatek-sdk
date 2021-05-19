/*
 * aec_util.h
 *
 *  Created on: 2014/3/18
 *      Author: MTK04880
 */

#ifndef AEC_UTIL_H_
#define AEC_UTIL_H_

enum {WAIT_NOWAIT = 0, WAIT_FOREVER = -1};

extern void MUTX_Get(int mid, int mutx_wait);
extern void MUTX_Put(int mid);
extern int MUTX_Init(int mid);

#endif /* AEC_UTIL_H_ */
