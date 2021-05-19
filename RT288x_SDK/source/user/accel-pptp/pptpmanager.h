/*
 * pptpmanager.h
 *
 * Manager function prototype.
 *
 * $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/accel-pptp/pptpmanager.h#1 $
 */

#ifndef _PPTPD_PPTPSERVER_H
#define _PPTPD_PPTPSERVER_H

void slot_init(int count);
void slot_free();
void slot_set_local(int i, char *ip);
void slot_set_remote(int i, char *ip);
void slot_set_pid(int i, pid_t pid);
int slot_find_by_pid(pid_t pid);
int slot_find_empty();
char *slot_get_local(int i);
char *slot_get_remote(int i);

extern int pptp_manager(int argc, char **argv);

#endif	/* !_PPTPD_PPTPSERVER_H */
