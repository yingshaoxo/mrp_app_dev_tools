/* armul_cnf.h - toolconf tags for all ARMulators. */
/* Copyright (C) 2000 - 2001 ARM Limited. All rights reserved.
 *
 * RCS $Revision: 1.8.2.7 $
 * Checkin $Date: 2001/09/06 14:08:21 $
 * Revising $Author: lmacgreg $
 */

#ifndef armul_cnf__h
#define armul_cnf__h

/*#define RDI_VERSION 100*/

#include "toolconf.h"
#include "rdi_conf.h"


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

#define ARMulCnf_ICacheLines (tag_t)"ICACHE_LINES"
#define ARMulCnf_ICacheAssociativity (tag_t)"ICACHE_ASSOCIATIVITY"
#define ARMulCnf_DCacheLines (tag_t)"DCACHE_LINES"
#define ARMulCnf_DCacheAssociativity (tag_t)"DCACHE_ASSOCIATIVITY"



#define ARMulCnf_Boards (tag_t)"BOARDS"
#define ARMulCnf_Processors (tag_t)"PROCESSORS"
#define ARMulCnf_Default (tag_t)"DEFAULT"
/*
 * This tells the component-constructing agents that they are called by
 * an Agent rather than the debugger.
 */
#define ARMulCnf_CONTROLLER_IS_AGENT (tag_t)"TARGET_CONTROLLER_IS_AGENT"


#define ARMulCnf_ControllerCaps   (tag_t)"TARGET_CONTROLLER_CAPABILITIES"

/* Defines which procvec within a core-DLL to use. */
#define ARMulCnf_ARMulator (tag_t)"ARMULATOR"


/*
 * Memory Clock Configuration (divisor-ratio)
 */
#define ARMulCnf_MCCfg (tag_t)"MCCFG"
/* Speed to use when debugger requests real-time by setting CPUSPEED=0. */
#define ARMulCnf_DefaultCPUSpeed (tag_t)"DEFAULT_CPUSPEED"
/* One of {REAL(CPUSPEED==0),EMULATED(CPUSPEED>0),COUNTER(obsolete)}.*/
#define ARMulCnf_Clock (tag_t)"CLOCK"
#define ARMulCnf_Range (tag_t)"RANGE"
#define ARMulCnf_MapFileToLoad (tag_t)"MAPFILETOLOAD"
#define ARMulCnf_UseMapFile (tag_t)"USEMAPFILE"

/* pagetab.c */
#define ARMulCnf_MMU (tag_t)"MMU"
#define ARMulCnf_AlignFaults (tag_t)"ALIGNFAULTS"
#define ARMulCnf_Cache (tag_t)"CACHE"
#define ARMulCnf_WriteBuffer (tag_t)"WRITEBUFFER"
#define ARMulCnf_Prog32 (tag_t)"PROG32"
#define ARMulCnf_Data32 (tag_t)"DATA32"
#define ARMulCnf_LateAbort (tag_t)"LATEABORT"
#define ARMulCnf_BigEnd (tag_t)"BIGEND"
#define ARMulCnf_SystemProt (tag_t)"SYSTEMPROT"
#define ARMulCnf_ROMProt (tag_t)"ROMPROT"
#define ARMulCnf_BranchPredict (tag_t)"BRANCHPREDICT"
#define ARMulCnf_ICache (tag_t)"ICACHE"
#define ARMulCnf_DAC (tag_t)"DAC"
#define ARMulCnf_PageTableBase (tag_t)"PAGETABLEBASE"
#define ARMulCnf_VirtualBase (tag_t)"VIRTUALBASE"
#define ARMulCnf_PhysicalBase (tag_t)"PHYSICALBASE"
#define ARMulCnf_Pages (tag_t)"PAGES"
#define ARMulCnf_Size (tag_t)"SIZE"
#define ARMulCnf_Cacheable (tag_t)"CACHEABLE"
#define ARMulCnf_Bufferable (tag_t)"BUFFERABLE"
#define ARMulCnf_Updateable (tag_t)"UPDATEABLE"
#define ARMulCnf_Domain (tag_t)"DOMAIN"
#define ARMulCnf_AccessPermissions (tag_t)"ACCESSPERMISSIONS"
#define ARMulCnf_Translate (tag_t)"TRANSLATE"
#define ARMulCnf_FastBus (tag_t)"FASTBUS"
#define ARMulCnf_HighExceptionVectors (tag_t)"HIGHEXCEPTIONVECTORS"
#define ARMulCnf_PExtensions    (tag_t)"ARMV5PEXTENSIONS"

/* used for 946 / 966 processors with  */

#define ARMulCnf_TCIRAM (tag_t)"IRAM"
#define ARMulCnf_TCDRAM (tag_t)"DRAM"


/* used for VFP */
#define ARMulCnf_VFPr2 (tag_t)"VFPR2"


/*
 * V6 configuration options
 */
#define ARMulCnf_ITCRAMSize (tag_t)"ITCRAM_SIZE"
#define ARMulCnf_DTCRAMSize (tag_t)"DTCRAM_SIZE"
#define ARMulCnf_TCRAMIsUnified (tag_t)"TCRAM_IS_UNIFIED"
#define ARMulCnf_TLBLockableSize (tag_t)"TLB_LOCKABLE_SIZE"
#define ARMulCnf_TLBNonLockableSize (tag_t)"TLB_NONLOCKABLE_SIZE"
#define ARMulCnf_DMAChannels (tag_t)"DMA_CHANNELS"
#define ARMulCnf_MixedEndian (tag_t)"MIXEDEND"


#define ARMulCnf_BreakPoints (tag_t)"BREAKPOINTS"
#define ARMulCnf_ContextPoints (tag_t)"CONTEXTPOINTS"
#define ARMulCnf_WatchPoints (tag_t)"WATCHPOINTS"

/* peripheral models */
#define ARMulCnf_BaseAddress (tag_t)"BASEADDRESS"
#define ARMulCnf_Limit (tag_t)"LIMIT"
#define ARMulCnf_Mask (tag_t)"MASK"
#define ARMulCnf_Warn (tag_t)"WARN"
#define ARMulCnf_Waits (tag_t)"WAITS"
#define ARMulCnf_Verbose (tag_t)"VERBOSE"

/* memory models */
/* Each core model should define FCLK from CPUSPEED.
 * (Only SAMemIniy actually reads that, though.)
 * Each core model should define MCLK from CPUSPEED.
 * There is no toolconf entry "BCLK" - the name used is MCLK. */
#define ARMulCnf_FCLK (tag_t)"FCLK"
#define ARMulCnf_MCLK (tag_t)"MCLK"
#define ARMulCnf_MCCfg (tag_t)"MCCFG"
/* Flatmem */
#define ARMulCnf_MemoryByteSex (tag_t)"MEMORYBYTESEX"

#define ARMulCnf_RDI_Log (tag_t)"RDI_LOG"

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /*ndef armul_cnf__h*/










