/* ppp_fcs.h ... header file for PPP-HDLC FCS 
 *               C. Scott Ananian <cananian@alumni.princeton.edu>
 *
 * $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/pptp-client/ppp_fcs.h#1 $
 */ 

#define PPPINITFCS16    0xffff  /* Initial FCS value */
#define PPPGOODFCS16    0xf0b8  /* Good final FCS value */

u_int16_t pppfcs16(u_int16_t fcs, void *cp, int len);
