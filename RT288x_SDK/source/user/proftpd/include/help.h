/*
 * ProFTPD - FTP server daemon
 * Copyright (c) 2004 The ProFTPD Project team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307, USA.
 *
 * As a special exemption, The ProFTPD Project team and other respective
 * copyright holders give permission to link this program with OpenSSL, and
 * distribute the resulting executable, without including the source code for
 * OpenSSL in the source distribution.
 */

/* HELP management
 * $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/proftpd/include/help.h#1 $
 */

#ifndef PR_HELP_H
#define PR_HELP_H

void pr_help_add(const char *, const char *, int);
int pr_help_add_response(cmd_rec *, const char *);

#endif /* PR_HELP_H */
