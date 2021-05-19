/* $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/linux-2.4.x/drivers/isdn/isdn_concap.h#1 $
 *
 * Linux ISDN subsystem, protocol encapsulation
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 *
 */

extern struct concap_device_ops isdn_concap_reliable_dl_dops;
extern struct concap_device_ops isdn_concap_demand_dial_dops;
extern struct concap_proto * isdn_concap_new( int );


