/*
 * ctrlpacket.h
 *
 * Functions to parse and send pptp control packets.
 *
 * $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/accel-pptp/ctrlpacket.h#1 $
 */

#ifndef _PPTPD_CTRLPACKET_H
#define _PPTPD_CTRLPACKET_H

#include <linux/types.h>

int read_pptp_packet(int clientFd, unsigned char *packet, unsigned char *rply_packet, ssize_t * rply_size);
size_t send_pptp_packet(int clientFd, unsigned char *packet, size_t packet_size);
void make_echo_req_packet(unsigned char *rply_packet, ssize_t * rply_size, u_int32_t echo_id);
void make_call_admin_shutdown(unsigned char *rply_packet, ssize_t * rply_size);
void make_stop_ctrl_req(unsigned char *rply_packet, ssize_t * rply_size);

#endif	/* !_PPTPD_CTRLPACKET_H */
