#ifndef HEADER_CURL_MEMRCHR_H
#define HEADER_CURL_MEMRCHR_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2009, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/curl/lib/curl_memrchr.h#1 $
 ***************************************************************************/

#include "setup.h"

#ifdef HAVE_MEMRCHR

#ifdef HAVE_STRING_H
#  include <string.h>
#endif
#ifdef HAVE_STRINGS_H
#  include <strings.h>
#endif

#else /* HAVE_MEMRCHR */

void *Curl_memrchr(const void *s, int c, size_t n);

#define memrchr(x,y,z) Curl_memrchr((x),(y),(z))

#endif /* HAVE_MEMRCHR */

#endif /* HEADER_CURL_MEMRCHR_H */
