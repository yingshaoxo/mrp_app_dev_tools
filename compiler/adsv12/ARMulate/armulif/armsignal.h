/* -*-C-*-
 *
 * $Revision: 1.5.6.2 $
 *   $Author: lmacgreg $
 *     $Date: 2001/08/24 12:57:49 $
 *
 * Copyright (c) 1999 ARM Limited
 * All Rights Reserved
 *
 * armsignal.h - enums for ARMulator Signals
 */
#ifndef armsignal__h
#define armsignal__h

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

#include "rdi.h"
#include "rdi_priv.h"


typedef int ARMSignalType;


typedef enum SignalState
{
   Signal_Off = FALSE,
   Signal_On = TRUE
} SignalState;

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /* def armsignal__h */









