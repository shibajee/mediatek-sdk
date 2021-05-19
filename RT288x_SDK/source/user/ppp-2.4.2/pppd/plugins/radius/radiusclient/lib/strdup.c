/*
 * $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/ppp-2.4.2/pppd/plugins/radius/radiusclient/lib/strdup.c#1 $
 *
 * Copyright (C) 1996 Lars Fenneberg and Christian Graefe
 *
 * This file is provided under the terms and conditions of the GNU general
 * public license, version 2 or any later version, incorporated herein by
 * reference.
 *
 */

#include "config.h"
#include "includes.h"

/*
 * Function: strdup
 *
 * Purpose:  strdup replacement for systems which lack it
 *
 */

char *strdup(char *str)
{
	char *p;

	if (str == NULL)
		return NULL;

	if ((p = (char *)malloc(strlen(str)+1)) == NULL)
		return p;

	return strcpy(p, str);
}
