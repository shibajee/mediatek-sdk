/* $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/miniupnpd-1.6/upnpdescgen.h#1 $ */
/* MiniUPnP project
 * http://miniupnp.free.fr/ or http://miniupnp.tuxfamily.org/
 * (c) 2006-2011 Thomas Bernard
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */

#ifndef __UPNPDESCGEN_H__
#define __UPNPDESCGEN_H__

#include "config.h"

/* for the root description
 * The child list reference is stored in "data" member using the
 * INITHELPER macro with index/nchild always in the
 * same order, whatever the endianness */
struct XMLElt {
	const char * eltname;	/* begin with '/' if no child */
	const char * data;	/* Value */
};

/* for service description */
struct serviceDesc {
	const struct action * actionList;
	const struct stateVar * serviceStateTable;
};

struct action {
	const char * name;
	const struct argument * args;
};

struct argument {	/* the name of the arg is obtained from the variable */
	unsigned char dir;		/* MSB : don't append "New" Flag,
	                         * 5 Medium bits : magic argument name index
	                         * 2 LSB : 1 = in, 2 = out */
	unsigned char relatedVar;	/* index of the related variable */
};

struct stateVar {
	const char * name;
	unsigned char itype;	/* MSB: sendEvent flag, 7 LSB: index in upnptypes */
	unsigned char idefault;	/* default value */
	unsigned char iallowedlist;	/* index in allowed values list
	                             * or in allowed range list */
	unsigned char ieventvalue;	/* fixed value returned or magical values */
};

/* little endian
 * The code has now be tested on big endian architecture */
#define INITHELPER(i, n) ((char *)(((n)<<16)|(i)))

/* char * genRootDesc(int *);
 * returns: NULL on error, string allocated on the heap */
char *
genRootDesc(int * len);

#ifdef ENABLE_WSC_SERVICE
/* The data structure used the save the local Wsc UPnP Device. */
extern struct upnpDevice wscLocalDevice;	

char *
genWSCRootDesc(int * len);

char *
genWSC(int * len);
#endif /* ENABLE_WSC_SERVICE */

/* for the two following functions */
char *
genWANIPCn(int * len);

char *
genWANCfg(int * len);

#ifdef ENABLE_L3F_SERVICE
char *
genL3F(int * len);
#endif

#ifdef ENABLE_6FC_SERVICE
char *
gen6FC(int * len);
#endif

#ifdef ENABLE_DP_SERVICE
char *
genDP(int * len);
#endif

#ifdef ENABLE_EVENTS
char *
getVarsWANIPCn(int * len);

char *
getVarsWANCfg(int * len);

#ifdef ENABLE_L3F_SERVICE
char *
getVarsL3F(int * len);
#endif
#ifdef ENABLE_6FC_SERVICE
char *
getVars6FC(int * len);
#endif
#ifdef ENABLE_DP_SERVICE
char *
getVarsDP(int * len);
#endif
#ifdef ENABLE_WSC_SERVICE
char *
getVarsWSC(int * len);
#endif /* ENABLE_WSC_SERVICE */
#endif /* ENABLE_EVENTS */

#endif

