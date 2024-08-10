/*
 * javadis.h
 * Copyright (C) ARM Ltd., 2000-2001
 */

/*
 * RCS $Revision: 1.1.8.3 $
 * Checkin $Date: 2001/05/02 13:16:45 $
 * Revising $Author: dcleland $
 */

#ifndef __javadis_h
#define __javadis_h

#include "host.h"

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

/* Disassemble the Java instruction in the buffer pointed to by ip */
extern unsigned32 disass_java(unsigned char *ip, unsigned32 address, char *o);

#ifdef __cplusplus
}
#endif

#endif
