/*
 * addcounter.h
 * Structures to allow an agent for a core-module
 * (e.g. ARMiss or ARMulator)  to instantiate sub-modules.
 * Copyright (C) 2000 - 2001 ARM Ltd.  All rights reserved.
 *
 * RCS $Revision: 1.2.6.2 $
 * Checkin $Date: 2001/08/24 12:57:49 $
 * Revising $Author: lmacgreg $
 */

#ifndef armulif_addcounter__h
#define armulif_addcounter__h

#ifndef armtypes_h
# include "armtypes.h"
#endif

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

int ARMul_AddCounterDesc(void *handle,
    ARMword *arg1, ARMword *arg2, const char *cname);

int ARMul_AddCounterValue(void *handle,
    ARMword *arg1, ARMword *arg2,
    bool i64, const ARMword *counter);

int ARMul_AddCounterValue64(void *handle,
    ARMword *arg1, ARMword *arg2,
    const uint64 counterval);

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif
