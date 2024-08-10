/* armul_cyc.h - Interface for counter-helpers */
/* Copyright (C) 1996-2001 ARM Limited. All rights reserved. */

/*
 * RCS $Revision: 1.1.2.2 $
 * Checkin $Date: 2001/08/24 12:57:49 $
 * Revising $Author: lmacgreg $
 */

#ifndef armul_cyc__h
#define armul_cyc__h

#include "armtypes.h"
#include "armul_mem.h"
#include "rdi_stat.h"   /* for RDI_CycleDesc */


void ARMulif_ReadCycleNames(char **CycleNames, unsigned numNames,
                            int memtype, toolconf config);


#endif
/* EOF armul_cyc.h */


