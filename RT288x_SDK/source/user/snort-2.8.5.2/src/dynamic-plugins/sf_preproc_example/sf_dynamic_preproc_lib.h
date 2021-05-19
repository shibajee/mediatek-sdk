/*
** Copyright (C) 2005-2009 Sourcefire, Inc.
** Author: Steven Sturges
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License Version 2 as
** published by the Free Software Foundation.  You may not use, modify or
** distribute this program under any other version of the GNU General
** Public License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/snort-2.8.5.2/src/dynamic-plugins/sf_preproc_example/sf_dynamic_preproc_lib.h#1 $ */

/* Snort Dynamic Preprocessor */

#ifndef __SF_DYNAMIC_PREPROC_LIB_H_
#define __SF_DYNAMIC_PREPROC_LIB_H_

#ifdef WIN32
#ifdef SF_SNORT_PREPROC_DLL
#define BUILDING_SO
#define PREPROC_LINKAGE SO_PUBLIC
#else
#define PREPROC_LINKAGE
#endif
#else /* WIN32 */
#define PREPROC_LINKAGE SO_PUBLIC
#endif

#endif  /* __SF_DYNAMIC_PREPROC_LIB_H_ */
