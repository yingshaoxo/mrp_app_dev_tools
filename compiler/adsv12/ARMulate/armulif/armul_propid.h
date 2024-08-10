/* -*-C-*-
 *
 * $Revision: 1.3.10.5 $
 *   $Author: lmacgreg $
 *     $Date: 2001/08/24 12:57:50 $
 *
 * Copyright (c) 1999 - 2001 ARM Limited
 * All Rights Reserved
 *
 * armprop.h - enums for ARMulator Signals
 */
#ifndef armul_propid__h
#define armul_propid__h

#include "rdi_prop.h"
#include "armsignal.h" /* These are RDI-properties too! */
#include "simplelinks.h" /* for GenericAccessCallback */

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

/* This is used to allow Semihost to add properties. */
typedef struct ARMul_RDIPropertyDesc
{
    RDIProperty_Desc desc;

    GenericAccessCallback gacb;

} ARMul_RDIPropertyDesc;
#define ARMul_DynamicPropIDBase 0xFFFF0000


/*
 * RDIPropID_ARMulProp_* are defined in rdi/rdi_priv.h now.
 */
typedef int ARMul_PropertyID;

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /* ndef armul_propid__h */


