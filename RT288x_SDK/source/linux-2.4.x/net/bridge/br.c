/*
 *	Generic parts
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	$Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/linux-2.4.x/net/bridge/br.c#1 $
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/init.h>
#include <linux/if_bridge.h>
#include <linux/brlock.h>
#include <linux/rtnetlink.h>
#include <asm/uaccess.h>
#include "br_private.h"

#if defined(CONFIG_ATM_LANE) || defined(CONFIG_ATM_LANE_MODULE)
#include "../atm/lec.h"
#endif

void br_dec_use_count()
{
	MOD_DEC_USE_COUNT;
}

void br_inc_use_count()
{
	MOD_INC_USE_COUNT;
}

static int __init br_init(void)
{
	printk(KERN_INFO "NET4: Ethernet Bridge 008 for NET4.0\n");

	br_handle_frame_hook = br_handle_frame;
	br_ioctl_hook = br_ioctl_deviceless_stub;
#if defined(CONFIG_ATM_LANE) || defined(CONFIG_ATM_LANE_MODULE)
	br_fdb_get_hook = br_fdb_get;
	br_fdb_put_hook = br_fdb_put;
#endif
	register_netdevice_notifier(&br_device_notifier);

	return 0;
}

static void __exit br_deinit(void)
{
	unregister_netdevice_notifier(&br_device_notifier);

	rtnl_lock();
	br_ioctl_hook = NULL;
	rtnl_unlock();

	br_write_lock_bh(BR_NETPROTO_LOCK);
	br_handle_frame_hook = NULL;
	br_write_unlock_bh(BR_NETPROTO_LOCK);

#if defined(CONFIG_ATM_LANE) || defined(CONFIG_ATM_LANE_MODULE)
	br_fdb_get_hook = NULL;
	br_fdb_put_hook = NULL;
#endif
}

EXPORT_NO_SYMBOLS;

module_init(br_init)
module_exit(br_deinit)
MODULE_LICENSE("GPL");
