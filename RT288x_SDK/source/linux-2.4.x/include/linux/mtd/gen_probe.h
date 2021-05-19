/*
 * (C) 2001, 2001 Red Hat, Inc.
 * GPL'd
 * $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/linux-2.4.x/include/linux/mtd/gen_probe.h#1 $
 */

#ifndef __LINUX_MTD_GEN_PROBE_H__
#define __LINUX_MTD_GEN_PROBE_H__

#include <linux/mtd/flashchip.h>
#include <linux/mtd/map.h> 
#include <linux/mtd/cfi.h>

struct chip_probe {
	char *name;
	int (*probe_chip)(struct map_info *map, __u32 base,
			  struct flchip *chips, struct cfi_private *cfi);

};

struct mtd_info *mtd_do_chip_probe(struct map_info *map, struct chip_probe *cp);

#endif /* __LINUX_MTD_GEN_PROBE_H__ */
