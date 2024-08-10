/*
 * armul_loadconf.h
 *
 * RCS $Revision: 1.4.6.2 $
 * Checkin $Date: 2001/08/24 12:57:50 $
 * Revising $Author: lmacgreg $
 *
 * Interface for armul_loadconf.c
 * Copyright (C) 1999 - 2001 ARM Ltd.  All rights reserved.
 *
 */


#ifndef armul_loadconf__h
#define armul_loadconf__h

#include "toolconf.h"

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

struct RDI_HostosInterface;

/*
 * Loads all the .AMI and .DSC files it can find on paths in
 * environment-vars ARMCONF and ARMDLL.
 */
int ARMul_LoadToolConf(toolconf tconf, 
                       struct RDI_HostosInterface const *hostif);

/* 
 * Find, in tconf, the config with name "confname",
 * following chains of aliases if required to find a child-toolconf.
 * Returns: The child toolconf.
 */
toolconf ARMul_FindConfig(toolconf tconf,tag_t confname);

void ARMul_ToolConf_Merge(toolconf tc_dest, toolconf tc_src);

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /*ndef armul_loadconf__h*/










