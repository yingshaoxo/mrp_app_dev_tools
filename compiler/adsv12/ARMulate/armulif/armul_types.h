/*
 * armul_types.h - standard toolkit types - things about ARMulator.
 * Copyright (C) 1998 - 2001 ARM Limited. All Rights Reserved.
 *
 * RCS $Revision: 1.4.6.3 $
 * Checkin $Date: 2001/08/24 12:57:50 $
 * Revising $Author: lmacgreg $
 */

#ifndef armul_types_h
#define armul_types_h

#ifndef host_h_LOADED
# include "host.h"
#endif
#ifndef armtypes_h
# include "armtypes.h" /* for ARMword */
#endif

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

typedef uint64 ARMTime; /* for ARMISS, initially */

/*
 * Execution state change enumeration
 */
typedef enum ARMul_ExecChangeEnum
{
    ARMul_ExecChange_Idle,
    ARMul_ExecChange_Executing,
    ARMul_ExecChange_Stepping
} ARMul_ExecChangeEnum;

typedef void armul_Hourglass(void *handle,ARMword pc,ARMword instr,
                                                         ARMword cpsr, ARMword condpassed);

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /*ndef armul_types_h */




