#ifndef	__DRIVER_CHAR_SCAN_KEYB_H
#define	__DRIVER_CHAR_SCAN_KEYB_H
/*
 *	$Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/linux-2.4.x/drivers/char/scan_keyb.h#1 $
 *	Copyright (C) 2000 YAEGASHI Takeshi
 *	Generic scan keyboard driver
 */

int register_scan_keyboard(int (*scan)(unsigned char *buffer),
			   const unsigned char *table,
			   int length);

void __init scan_kbd_init(void);

#endif
