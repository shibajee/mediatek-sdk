/*
 * $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/linux-2.6.21.x/include/mtd/mtd-user.h#1 $
 *
 * MTD ABI header for use by user space only.
 */

#ifndef __MTD_USER_H__
#define __MTD_USER_H__

#include <stdint.h>

/* This file is blessed for inclusion by userspace */
#include <mtd/mtd-abi.h>

typedef struct mtd_info_user mtd_info_t;
typedef struct erase_info_user erase_info_t;
typedef struct region_info_user region_info_t;
typedef struct nand_oobinfo nand_oobinfo_t;
typedef struct nand_ecclayout nand_ecclayout_t;

#endif /* __MTD_USER_H__ */
