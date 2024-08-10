/* -*-C-*-
 *
 * $Revision: 1.10.2.7 $
 *   $Author: lmacgreg $
 *     $Date: 2001/08/24 12:57:49 $
 *
 * Copyright (c) 1999 - 2001 ARM Limited
 * All Rights Reserved
 *
 * armul_callbackid.h - enums for ARMulator Signals
 */
#ifndef armul_callbackid__h
#define armul_callbackid__h

#include "armsignal.h" /* These are RDI-properties too! */

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif


typedef enum ARMul_CallbackID
{
    /* These are for the Integer Pipe, and instruction-bus. */

         /* Deprecated - use the later ARMulCallbackID_ScheduleFunction's */
    ARMulCallbackID_ScheduleFunctionEvery = 1,

    ARMulCallbackID_ExceptionHandler,
    ARMulCallbackID_EventHandler,
    ARMulCallbackID_Coprocessor,
    ARMulCallbackID_InterruptController =5,
    ARMulCallbackID_PerfMonCtr,
    ARMulCallbackID_Hourglass,
    ARMulCallbackID_DebugInstrFetch, /* for Feeder1 */
    /*
     * A peripheral calls this to tell its decoder (core) where it lives.
     */
    ARMulCallbackID_PeripheralAddressRange =10,

    ARMulCallbackID_TraceMemoryEventHandler,
    ARMulCallbackID_TraceInstructionEventHandler,
    ARMulCallbackID_ExecChangeHandler,
    ARMulCallbackID_BusMemoryAccess,
    ARMulCallbackID_NewLeafPageAllocated,
        
    ARMulCallbackID_ScheduleFunctionCpuCycles = 256, /* FCLK */
    ARMulCallbackID_ScheduleFunctionCoreCycles, /* GCLK */
        /* NB This has further options in arg1[1]...
         * bit 0 = relative, not absolute time.
         * bits 2,1  00 : Just call after memory-access ("UPDATE")
         *           01 : Just call before memory-access ("EXECUTE")
         *           10 : Call after and before.
         *           11 : reserved.
         */
    ARMulCallbackID_ScheduleFunctionBCLK
} ARMul_CallbackID;


#define ARMulBusID_Core    1 /* Core, usually to cache. */
#define ARMulBusID_Bus     2 /* AHB or ASB */
#define ARMulBusID_Memory  3 /* Leaf memory, usually flat */
/* NB a naked core will not support ARMulBusID_Bus. */

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif


#endif /* ndef armul_callbackid__h */













