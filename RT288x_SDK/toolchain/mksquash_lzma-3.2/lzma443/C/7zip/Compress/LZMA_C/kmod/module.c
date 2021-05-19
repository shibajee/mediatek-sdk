
/*
 * Copyright (C) 2006 Junjiro Okajima
 * Copyright (C) 2006 Tomas Matejicek, slax.org
 *
 * LICENSE follows the described one in lzma.txt.
 */

/* $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/toolchain/mksquash_lzma-3.2/lzma443/C/7zip/Compress/LZMA_C/kmod/module.c#1 $ */

#include <linux/init.h>
#include <linux/module.h>

#include "../LzmaDecode.c"

EXPORT_SYMBOL(LzmaDecodeProperties);
EXPORT_SYMBOL(LzmaDecode);

#if 0
static int __init unlzma_init(void)
{
	return 0;
}

static void __exit unlzma_exit(void)
{
}

module_init(unlzma_init);
module_exit(unlzma_exit);
#endif

MODULE_LICENSE("GPL");
MODULE_VERSION("$Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/toolchain/mksquash_lzma-3.2/lzma443/C/7zip/Compress/LZMA_C/kmod/module.c#1 $");
MODULE_DESCRIPTION("LZMA uncompress. "
		   "A tiny wrapper for LzmaDecode.c in LZMA SDK from www.7-zip.org.");
