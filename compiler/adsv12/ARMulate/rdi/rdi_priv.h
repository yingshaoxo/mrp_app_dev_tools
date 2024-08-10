/* -*-C-*-
 *
 * $Revision: 1.17.2.9 $
 *   $Author: dsinclai $
 *     $Date: 2001/08/28 09:48:32 $
 *
 * Copyright (c) ARM Limited 1999-2001.  All Rights Reserved.
 *
 * rdi_priv.h - Private RDI definitions.  Used for RDI properties that
 *              are defined by ARM, but the meaning of the definition should
 *              not be exported, in the general case.
 */
#ifndef rdi_priv_h
#define rdi_priv_h

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

/*
 *      ID: RDIPropID_SwBreaksPreferred
 *    Name: sw_breakpoints_preferred
 *  Target: Multi-ICE
 * Meaning: If set indicates Multi-ICE will set S/W bpts rather than H/W
 *          ones whenever possible.
 */
#define RDIPropID_SwBreaksPreferred        0x00000000

/*
 *      ID: RDIPropID_KS32SpecialAddress
 *    Name: ks32_special_base_address
 *  Target: Multi-ICE
 * Meaning: The base address of the KS32C50100 Special Register area.
 *          Default 0x3ff0000
 */
#define RDIPropID_KS32SpecialAddress       0x00000001

/*
 *      ID: RDIPropID_CacheCleanCodeAddr
 *    Name: cache_clean_code_address
 *  Target: Multi-ICE
 * Meaning: The address at which (currently ARM10) cache clean code will
 *          be written.
 */
#define RDIPropID_CacheCleanCodeAddr       0x00000002

/*
 *      ID: RDIPropID_DebugHandlerBaseAddress 0x00000003
 *    Name: debug_handler_base_address
 *  Target: Multi-ICE
 * Meaning: Base address of debug handler
 *          The default value will be 0xF0000000, and only values which
 *          are 2kb aligned are allowed.
 */
#define RDIPropID_DebugHandlerBaseAddress  0x00000003

/*
 *      ID: RDIPropID_InternalCacheEnable 0x00000004
 *    Name: internal_cache_enable
 *  Target: Multi-ICE
 * Meaning: Use this to enable or disable the Multi-ICE internal memory cache:
 *            0 = disabled
 *            non-zero = enabled
 */
#define RDIPropID_InternalCacheEnable      0x00000004

/*
 *      ID: RDIPropID_InternalCacheFlush 0x00000005
 *    Name: internal_cache_flush
 *  Target: Multi-ICE
 * Meaning: Setting this to a non-zero value will cause Multi-ICE to flush its internal memory cache.
 */
#define RDIPropID_InternalCacheFlush       0x00000005

/* ========================== ARMulator =============================*/
/*
 * Reserve ranges for ARMulator.
 */
#define RDIPropID_ARMulProp_Base 100
#define RDIPropID_ARMulProp_Top  199
#define RDIPropID_ARMSignal_Base 200
#define RDIPropID_ARMSignal_Top  299

/*
 * Cycle-counts to and from the core.
 */
/* RDIPropID_ARMulProp_CycleCount is the number of BUS cycles advanced.
 * This is often the best time-estimate the model can provide.
 * It is >not< core-cycles, except in ARMISS, because ARM9 core-cycles
 * do not happen while the core is stalled.
 */
#define RDIPropID_ARMulProp_CycleCount RDIPropID_ARMulProp_Base
/* This is a preferred name for the above. */
#define RDIPropID_ARMulProp_BusCycleCount RDIPropID_ARMulProp_Base
/* If supported, this is the total number of CPUSPEED cycles. */
#define RDIPropID_ARMulProp_CpuCycleCount (RDIPropID_ARMulProp_Base+1)


#define RDIPropID_ARMulProp_CoreCycleCount (RDIPropID_ARMulProp_Base+3)

#define RDIPropID_ARMulProp_Accuracy (RDIPropID_ARMulProp_Base+10)
#define RDIPropID_ARMulProp_RDILog (RDIPropID_ARMulProp_Base+11)
/* This is the bus cycle-duration in 0.1ns units. */
#define RDIPropID_ARMulProp_CycleLength (RDIPropID_ARMulProp_Base+12)

/*
 * This is sent when a core receives RDIOpen()
 */
#define RDIPropID_ARMulProp_Hostif (RDIPropID_ARMulProp_Base+12)
/*
 * Properties originating from ADS 1.0 view
 */
#define RDIPropID_ARMulProp_ProcessorProperties (RDIPropID_ARMulProp_Base+20)
#define RDIPropID_ARMulProp_CondCheckInstr (RDIPropID_ARMulProp_Base+21)

/*
 * Signals to the core.
 */
#define RDIPropID_ARMSignal_RESET     (RDIPropID_ARMSignal_Base)
#define RDIPropID_ARMSignal_UNDEF     (RDIPropID_ARMSignal_Base + 1)
#define RDIPropID_ARMSignal_SWI       (RDIPropID_ARMSignal_Base + 2)
#define RDIPropID_ARMSignal_PABORT    (RDIPropID_ARMSignal_Base + 3)
#define RDIPropID_ARMSignal_DABORT    (RDIPropID_ARMSignal_Base + 4)
#define RDIPropID_ARMSignal_SE        (RDIPropID_ARMSignal_Base + 5)
#define RDIPropID_ARMSignal_IRQ       (RDIPropID_ARMSignal_Base + 6)
#define RDIPropID_ARMSignal_FIQ       (RDIPropID_ARMSignal_Base + 7)

#define RDIPropID_ARMSignal_BreakPt   (RDIPropID_ARMSignal_Base + 8)
#define RDIPropID_ARMSignal_WatchPt   (RDIPropID_ARMSignal_Base + 9)

#define RDIPropID_ARMSignal_Stop      (RDIPropID_ARMSignal_Base + 10)

/*
 * These are from the CP15 to the core.
 * They correspond to bits in Config.
 */
#define RDIPropID_ARMSignal_MMUEnable (RDIPropID_ARMSignal_Base + 16)
#define RDIPropID_ARMSignal_AddrFaultEnable (RDIPropID_ARMSignal_Base + 17)
#define RDIPropID_ARMSignal_CacheEnable (RDIPropID_ARMSignal_Base + 18)
#define RDIPropID_ARMSignal_WBEnable (RDIPropID_ARMSignal_Base + 19)
/* 20,21 reverved for 32/26 Program, 32/26 data */
#define RDIPropID_ARMSignal_LateAbort (RDIPropID_ARMSignal_Base + 22)
#define RDIPropID_ARMSignal_BigEnd (RDIPropID_ARMSignal_Base + 23)
/* SystemCtrl, UsrCtrl, F */
#define RDIPropID_ARMSignal_BranchPredictEnable (RDIPropID_ARMSignal_Base + 27)
#define RDIPropID_ARMSignal_ICacheEnable (RDIPropID_ARMSignal_Base + 28)
#define RDIPropID_ARMSignal_HighException (RDIPropID_ARMSignal_Base + 29)
/* 30 (bit 14) unassigned */
#define RDIPropID_ARMSignal_LDRSetTBITDisable (RDIPropID_ARMSignal_Base + 31)
/* Lots to spare. */
#define RDIPropID_ARMSignal_NotFastBus (RDIPropID_ARMSignal_Base + 46)
#define RDIPropID_ARMSignal_Asynch (RDIPropID_ARMSignal_Base + 47)
/* The whole bunch */
#define RDIPropID_ARMProp_Config      (RDIPropID_ARMSignal_Base + 48)

/* And some extra specialised ones. */
#define RDIPropID_ARMSignal_WaitForInterrupt (RDIPropID_ARMSignal_Base + 49)

/*
 * Signals from a validation trickbox (maybe) to the core
 */
#define RDIPropID_ARMSignal_DebugState (RDIPropID_ARMSignal_Base + 60)
#define RDIPropID_ARMSignal_BaseMemoryEnable (RDIPropID_ARMSignal_Base + 61)

/* Debugging properties  */
#define RDIPropID_ARMulProp_MTB (RDIPropID_ARMSignal_Base + 62)
/* From a prefetch unit to a core */
#define RDIPropID_ARMulProp_InstrAddr (RDIPropID_ARMSignal_Base + 63)

/* For extension-models. 
 * This is preferred over CycleLength because it is future-resistant. */
#define RDIPropID_ARMulProp_CoreClockFreq (RDIPropID_ARMSignal_Base + 64)



#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /* !def rdi_priv_h */

/* EOF rdi_priv.h */




