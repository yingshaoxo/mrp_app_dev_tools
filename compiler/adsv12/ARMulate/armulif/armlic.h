/* armlic.h - License management stuff for ARMulator
 * Copyright (C) 2001 ARM Limited. All rights reserved.
 *
 * RCS $Revision: 1.1.4.2 $
 * Checkin $Date: 2001/08/24 09:37:45 $
 * Revising $Author: dsinclai $
 */

#ifndef armulif__armlic__h
#define armulif__armlic__h

#include "rdi_conf.h"
#include "armul_agent.h"

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

/* Returns 0: License Ok, else 
 *         1: No license found
 *         2: Constraint violation.
 */
#define ARMUL_LICENSE_OK 0
#define ARMUL_LICENSE_NOTFOUND 1
#define ARMUL_LICENSE_CONSTRAINT_VIOLATION 2

int ARMul_GetLicense(
    ARMul_Agent *agent,
    toolconf boardcnf);

void ARMul_ReturnLicense (ARMul_Agent *agent);

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif


#endif /* def armulif__armlic__h */
/* EOF armul_lic.h */
