
/* armul_mem.h - Interface for memory models to the ARMulator */
/* Copyright (C) 1996-2000 ARM Limited. All rights reserved. */

/*
 * RCS $Revision: 1.13.2.24 $
 * Checkin $Date: 2001/10/08 15:18:32 $
 * Revising $Author: dsinclai $
 */

#ifndef armul_mem__h
#define armul_mem__h


#include "host.h"               /* void * */
#include "rdi_stat.h"           /* RDI_MemAccessStats */
#ifndef armul_defs__h
# include "armul_defs.h"
#endif

#include "toolconf.h"
#include "armul_types.h"        /* ARMTime */


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif


typedef unsigned int ARMul_acc; /* A flag word, containing.... */

#define acc_Nrw     0x10        /* not read/write */
#ifdef FAST
# error "No cheap and nasty models please"
#endif

#define acc_seq     0x20        /* sequential */
#define acc_Nmreq   0x40        /* not memory request */
#define acc_Nopc   0x100        /* not opcode fetch */
#define acc_rlw    0x400        /* read-lock-write */
/* Trans is not supported - use TransChangeUpcall instead */
/* #define acc_Ntrans 0x200      * not trans (only for LDRT etc.) */
#define acc_kill   0x800        /* [I/D]KIll signal for ARM9EJ-S */

/*
 * HGB 07-06-2000
 * InstructionEvent types used by models hooking into InstructionTrace
 * Note, these are instruction EXECUTED events, not instruction fetch
 */
#define acc_INSTR(w)  ((w) & 0x40000000)

#define acc_armpass     0x40000000
#define acc_armfail     0x40000001
#define acc_thumbpass   0x40000002
#define acc_thumbfail   0x40000003


#define acc_burst0  0x1000      /* AMBA BURST[0:1] pins set on ARM 9xxT */
#define acc_burst1  0x2000        
#define acc_burst2  0x4000      /* AMBA AHB HBURST[2]; HBURST[0:1] uses acc_burst{0,1} */
#define acc_burst3  0x8000      /* ARM9EJ-S uses BURST[0:3] for burst lengthin LDM/STM. */
#define ACC_BURST_POS   12      /* Bit position of burst access type */
#define acc_dmore   0x10000      /* DMORE pin on the ARM9TDMI */
#define acc_Nareq   0x20000      /* No AMBA request - ASB and AHB processors */
#define acc_Ntrans   0x200
#define acc_nTRANS(w)   ((w) & acc_Ntrans)

/* AMBA ASB Burst encodings */
#define acc_BURST_TYPE(w)   (w & (acc_burst0 | acc_burst1))
#define acc_burst_none      0
#define acc_burst_4word     acc_burst0
#define acc_burst_8word     acc_burst1
#define acc_burst_reserved  (acc_burst0 | acc_burst1)  
#define acc_burst_tlb_walk  (acc_burst0 | acc_burst1)
#define acc_burst_buffered  (acc_burst0 | acc_burst1)

/* AMBA AHB Burst encodings */
#define acc_AHB_BURST_TYPE(w)   (w & (acc_burst0 | acc_burst1 | acc_burst2))
#define acc_ahb_burst_single    0
#define acc_ahb_burst_incr      acc_burst0
#define acc_ahb_burst_wrap4     acc_burst1
#define acc_ahb_burst_incr4     (acc_burst0 | acc_burst1)
#define acc_ahb_burst_wrap8     acc_burst2
#define acc_ahb_burst_incr8     (acc_burst2 | acc_burst0)
#define acc_ahb_burst_wrap16    (acc_burst2 | acc_burst1)
#define acc_ahb_burst_incr16    (acc_burst2 | acc_burst1 | acc_burst0)

/* AMBA AHB HPROT[3:2] are used to encode cacheable and bufferable accesses */

#define acc_cacheable   0x40000
#define acc_bufferable  0x80000

#ifndef FAST
#define acc_physical   0x100000
#define acc_spec       0x200000 /* speculative instruction fetch (ARM810) */
#define acc_new        0x400000 /* a new request (StrongARM) */
#endif

#define acc_CACHEABLE(w)    ((w) & acc_cacheable)
#define acc_BUFFERABLE(w)   ((w) & acc_bufferable)

#define acc_BURST0(w)       ((w) & acc_burst0)
#define acc_BURST1(w)       ((w) & acc_burst1)
#define acc_BURST2(w)       ((w) & acc_burst2)

#define acc_DMORE(w)   ((w) & acc_dmore)

#define acc_NAREQ(w)   ((w) & acc_Nareq)
#define acc_AREQ(w)    (!acc_NAREQ(w))

#define acc_WRITE(w)   ((w) & acc_Nrw)
#define acc_SEQ(w)     ((w) & acc_seq)
#define acc_nMREQ(w)   ((w) & acc_Nmreq)
#define acc_nOPC(w)    ((w) & acc_Nopc)
#define acc_LOCK(w)    ((w) & acc_rlw)
/* #define acc_nTRANS(w)  ((w) & acc_Ntrans) */
#define acc_SPEC(w)    ((w) & acc_spec)
#define acc_NEW(w)     ((w) & acc_new)
#define acc_KILL(w)    ((w) & acc_kill)

#define acc_READ(w)    (!acc_WRITE(w))
#define acc_nSEQ(w)    (!acc_SEQ(w))
#define acc_MREQ(w)    (!acc_nMREQ(w))
#define acc_OPC(w)     (!acc_nOPC(w))
#define acc_nLOCK(w)   (!acc_LOCK(w))
/* #define acc_TRANS(w)   (!acc_nTRANS(w)) */
#define acc_nSPEC(w)   (!acc_SPEC(w))
#define acc_nNEW(w)    (!acc_NEW(w))

#define acc_typeN  0
#define acc_typeS  acc_seq
#define acc_typeI  acc_Nmreq
#define acc_typeC  (acc_Nmreq | acc_seq)

#define acc_CYCLE(w)  (w & (acc_Nmreq | acc_seq))

/*
 * The bottom four bits of the access word define the width of the access.
 * The number used is LOG_2(number-of-bits)
 */

#define BITS_8  3
#define BITS_16 4
#define BITS_32 5
#define BITS_64 6
#define WIDTH_MASK 0x0f

#define acc_WIDTH(w) ((w) & WIDTH_MASK)



#define acc_byte_0 0x1
#define acc_byte_1 0x2
#define acc_byte_2 0x4
#define acc_byte_3 0x8
#define BYTELANE_MASK 0xf

#define acc_BYTELANE(w) ((w) & BYTELANE_MASK)


#define acc_BYTE0(acc) ((acc) & acc_byte_0)
#define acc_BYTE1(acc) ((acc) & acc_byte_1)
#define acc_BYTE2(acc) ((acc) & acc_byte_2)
#define acc_BYTE3(acc) ((acc) & acc_byte_3)

#define acc_LoadInstrS    (BITS_32 | acc_typeS)
#define acc_LoadInstrS2   (BITS_64 | acc_typeS)
#define acc_LoadInstrS2Spec   (BITS_64 | acc_typeS | acc_spec)
#define acc_LoadInstrN    (BITS_32 | acc_typeN)
#define acc_LoadInstrN2   (BITS_64 | acc_typeN)
#define acc_LoadInstrN2Spec   (BITS_64 | acc_typeN | acc_spec)
#define acc_LoadInstr16S  (BITS_16 | acc_typeS)
#define acc_LoadInstr16N  (BITS_16 | acc_typeN)
#define acc_LoadWordS     (BITS_32 | acc_typeS | acc_Nopc)
#define acc_LoadWordS2    (BITS_64 | acc_typeS | acc_Nopc)
#define acc_LoadWordN     (BITS_32 | acc_typeN | acc_Nopc)
#define acc_LoadWordN2    (BITS_64 | acc_typeN | acc_Nopc)
#define acc_LoadByte      (BITS_8  | acc_typeN | acc_Nopc)
#define acc_LoadHalfWord  (BITS_16 | acc_typeN | acc_Nopc)

#define acc_StoreWordS    (BITS_32 | acc_typeS | acc_Nrw | acc_Nopc)
#define acc_StoreWordN    (BITS_32 | acc_typeN | acc_Nrw | acc_Nopc)
#define acc_StoreByte     (BITS_8  | acc_typeN | acc_Nrw | acc_Nopc)
#define acc_StoreHalfWord (BITS_16 | acc_typeN | acc_Nrw | acc_Nopc)

#define acc_NoFetch (acc_typeI)

#define acc_Icycle (acc_typeI | acc_Nopc)
#define acc_Ccycle (acc_typeC | acc_Nopc)

#define acc_NotAccount    0x0080


#define acc_nACCOUNT(x)   ((x) & acc_NotAccount)
#define acc_ACCOUNT(x)    (!acc_nACCOUNT(x))

#define acc_DontAccount(X)  ((X) | acc_NotAccount)

#define acc_ReadWord      acc_DontAccount(acc_LoadWordN)
#define acc_ReadHalfWord  acc_DontAccount(acc_LoadHalfWord)
#define acc_ReadByte      acc_DontAccount(acc_LoadByte)

#define acc_WriteWord     acc_DontAccount(acc_StoreWordN)
#define acc_WriteHalfWord acc_DontAccount(acc_StoreHalfWord)
#define acc_WriteByte     acc_DontAccount(acc_StoreByte)

/*
 * Definitions of things in the memory interface
 */

typedef struct armul_meminterface ARMul_MemInterface;

/*
 * There are many different revisions of the memory interface,
 * relating to different processor types
 */

typedef enum {
  ARMul_MemType_Uninitialised=0,
  /*
   * Basic memory interface - one access per cycle
   */
  /* Basic:
   *   N/S/I/C cycles
   *   instr fetch seq/non-seq
   *   word read/write seq/non-seq
   *   byte read/write non-seq
   *   word/byte locked read/write non-seq */
  ARMul_MemType_Basic,          /* 1 only does word/byte accesses */
  /* 16Bit: as Basic, but adds:
   *   half-word read/write non-seq */
  ARMul_MemType_16Bit,          /* 2 can also do 16-bit data accesses */
  /* Thumb: as 16Bit, but adds:
   *   half-word instr fetch seq/non-seq */
  ARMul_MemType_Thumb,          /* 3 can also do 16-bit instr accesses */

  /*
   * Cached memory interface
   */
  /* Some cached processors (940T on AHB) only ever do "cycle" and 
   * "not a cycle", and never perform 'N' cycles.
   * 946E-S is cached, but can emit N cycles onto the AHB.
   */
  ARMul_MemType_AHB,           /* 4 AHB */
/* For compatibility with old source-code :-( */
#define ARMul_MemType_BasicCached ARMul_MemType_AHB
  ARMul_MemType_16BitCached,   /* 5 AMBA */
  ARMul_MemType_ThumbCached,   /* 6 AMBA */


  /*
   * Pipelined memory interfaces for Advanced Core Simulator,
   * targetted at pipeline-accurate EDA products.
   */
  ARMul_MemType_PipelinedBasic,/* 7 */
  ARMul_MemType_PipelinedAMBA, /* 8 */
  ARMul_MemType_PipelinedARM9, /* 9 */


  /*
   * The ARM8 core can request two sequential accesses per cycle
   */
  /* ARM8: as 16Bit, but adds:
   *   instr fetch, double bandwidth, seq/non-seq
   *   word read, double bandwidth, seq/non-seq */
  ARMul_MemType_ARM8,          /* 10 double bandwidth */ /* 10 */

  /*
   * StrongARM makes one or two seperate requests per cycle, the last
   * one of which will have nOPC low.
   */
  ARMul_MemType_StrongARM,      /* 11 StrongARM interface */

  /*
   * The StrongARM's external memory interface supports byte-lanes.
   * In such a model the bottom four bits of "acc" are a mask, rather
   * than a width specifier, and the bottom two bits of the address
   * are assumed to be zero, and the word written/read will have the
   * data to be written in the specified bytes of the word.
   * However the access function is the same.
   *
   * This is (a) not extensible
   * (b) only used when Validate is false :-(
   * (c) only output in digital/armmmuSA.c, from cpu SA-110.
   */
  ARMul_MemType_ByteLanes,      /* 12 */

  /*
   * ARM9 Harvard
   */
  ARMul_MemType_ARM9,           /* 13 */

  /*
   * ARM10 and XScale : Can handle 8..64 bit requests.
   * NB it is unlikely that future models will use ARMul_MemType_ARMissCache.
   */
  ARMul_MemType_ARMissCache,    /* 14 */
  ARMul_MemType_ARMissAHB,      /* 15 */

  /*
   * (more) Pipelined memory interfaces for Advanced Core Simulator,
   * targetted at pipeline-accurate EDA products.
   */
  ARMul_MemType_PipelinedMultiLayerAHB, /* 16 */




  ARMul_MemType_Last
} ARMul_MemType;

/* All revisions share an init function type.
 * Purpose: Fill in (at init-time) the client's interface so it can call
 *          you (at run time) to access memory.
 *          E.g. the core calls this for caches (if used), 
 *          and caches call this for leaf memories.
 *
 * Change 2000-10-17: variant is passed in in interf->memtype.
 */
struct ARMul_State;
typedef ARMul_Error armul_MemInit(struct ARMul_State *state,
                                  ARMul_MemInterface *interf,
                                  /* ARMul_MemType variant, */
                                  toolconf your_config,
                                  toolconf core_config);

/*
 * These are the functions that can be defined in a given memory interface
 * revision
 */

/*
 * This describes a (memory) property-ID.
 */
typedef struct ARMul_PropID
{
    uint32 ap_id;
    /* format chars are...
     * u<n> : unsigned n-byte integer, n in {1,2,4,8}.
     * s<n> : signed n-byte integer.
     * p : pointer
     */
    char const *ap_format; /* For debugging, including asserts. */
} ARMul_PropID;




/*
 * Function to query and set memory properties.
 * Note that the callee usually knows what size the property is,
 * and is unlikely to handle variations.
 * Returns: RDIError_Ok or RDIError_UnimplementedMessage.
 */
#define MemPropertyID_TotalCycles 1
#define MemPropertyID_CycleSpeed  2
#define MemPropertyID_BaseMemoryRef  3
#define MemPropertyID_BaseMemoryEnable  4
/* Data-pointer is an ARMul_Bus* for this. */
#define MemPropertyID_Bus 5

/* Q: Why not separate GET and SET?
 * If it deletes an entire parameter, it could be correct?
 * Q: Where do the index(es) go?
 * E.g inside ()'s in the format?
 */
typedef unsigned armul_InfoProc(void *handle, unsigned type, ARMword *pID,
                                uint64 *data);

/*
 * Properties that a bus-master (core or cache) may expose to a downstream
 * memory model (cache or armflat).
 * Note that there is an assumption that any arbiter will pass the call
 * back up to all its bus-masters.
 */
/* Deprecated. Not used yet, but reserved for asynch memories? */
#define BMPropertyID_DataAbort 1
/* data == NULL means flush nano-cache. */
#define BMPropertyID_NanoCache 2
/* *data is address of new bus : write-only. */
#define BMPropertyID_NewBusConnected 3



/*
 * Function to return a "time elapsed" counter in microseconds.
 * If NULL, then Unix "clock()" is used.
 */

typedef ARMTime armul_ReadClock(void *handle);

/*
 * Function to return a cycle count. Returns NULL if not available,
 * or may be assigned to be NULL.
 * On some processors cycle types may overlap (e.g. F cycles
 * and I cycles overlap on cached processors, or additional wait-state
 * cycles on a standard memory model). Hence there is a "total"
 * field, which should be filled in as the total number of cycles on the
 * model's primary clock.
 */

typedef struct {
    /* This is incremented once per call to ARMflat or Mapfile. */
  ARMTime Total;  /* Bus-Cycles, used for ARMul_Time() */

    ARMTime ac_counts[8]; /* 4..7 are non-accounted */
# define NumNcycles ac_counts[0]
# define NumScycles ac_counts[1]
# define NumIcycles ac_counts[2]
# define NumCcycles ac_counts[3]
# define COUNTCYCLES(c,acc) c.ac_counts[ (acc & 0xE0) >> 5 ]++

  ARMTime ac_IWaits, ac_DWaits; /* 8,9 */
  ARMTime ac_MergedIS; /* 10 */

  ARMTime NumFcycles; /* Internal cycles not corrseponding to any of the above.
                       */
  ARMTime CoreCycles; /* #Times the core's pipeline has advanced
                       * as counted by ARM9/SA1's Fetch stage.*/

} ARMul_Cycles;

typedef const ARMul_Cycles *armul_ReadCycles(void *handle);

/*
 * Function to return the number of core cycles in an individual memory
 * access.
 * On cached processors where the number of core cycles in a given memory
 * cycle can vary this function is used to keep track of the number
 * of core cycles required before the next external memory access.
 */
typedef unsigned long armul_ReadDeltaCycles(void *handle);
typedef unsigned long armul_MultiLayerReadDeltaCycles(void *handle, unsigned int layer);

/* For Multi-Layer AHB systems the following function is used to
 * register a layer. The value returned should be used when the
 * layer is accessed via other Multi-Layer access functions.
 * The hint parameter can be used by a memory model for labeling
 * purposes.
 */
typedef unsigned int armul_MultiLayerRegister(void *handle, tag_t hint);


/*
 * For synchronous memory interfaces, this function returns the duration
 * of a cycle, in tenths of a nanosecond. All calls to "MemAccess" are
 * assumed to take this long by the caller. To insert wait states, return
 * '0' from MemAccess, or use an asynchronous memory interface (not yet
 * supported)
 */
typedef unsigned long armul_GetCycleLength(void *handle);

/*
 * The MemAccess functions return the number of datums read, or -1 for an abort
 *  (N.B. 0 signifies "wait", so a successful idle cycle returns 1)
 *  - MemAccess2 can return up to two words
 *  - MemAccAsync fills in a counter for the time til the next memory cycle
 */
typedef int armul_MemAccess(void *handle,
                            ARMword address,
                            ARMword *data,
                            ARMul_acc access_type);

typedef int armul_MultiLayerMemAccess(void *handle,
                                      unsigned int layer,
                                      ARMword address,
                                      ARMword *data,
                                      ARMul_acc access_type);

typedef void armul_MemBurstCount(void *handle, unsigned Count, unsigned IsWrite);


/* Used by cycle based models which include two clock
 * domains - eg , core clock and bus clock - to advance
 * the bus clock one cycle
 */  
typedef void armul_AdvanceBus(void *handle, ARMword phase);

/* This call provides information on the next cycle type, which enables
 * the ARMulator to be used in logic simulation environments where
 * pipelined address are required.
 * LOCK & nTRANS are provided via callbacks - these APIs are
 * called a cycle in advance when a pipelined ARMulator is used.
 */
typedef void armul_NextCycle(void *handle,
                             ARMword address,
                             ARMul_acc access_type);

typedef void armul_NextDataCycle(void *handle,
                             ARMword address,
                             ARMword data, 
                             ARMul_acc access_type);

typedef void armul_MultiLayerNext(void *handle,
                                  unsigned int layer,
                                  ARMword address,
                                  ARMword data,
                                  ARMul_acc access_type);

/* DRS 2000-09-24 MemAccess2 only needs one data-pointer.
 * This makes it the same as a normal access-fn */

typedef int armul_MemAccess2(void *,ARMword,ARMword *,ARMul_acc);

/* Used for ARMISS core calling cache. */
typedef int armul_MemAccAsync(void *handle, ARMword address, ARMword *data,
                              ARMul_acc acc, ARMTime *abs_time);


/* Memory access function used for true Harvard models, where both busses */
/* present the required access parameters in the same function call.      */
typedef void armul_HarvardMemAccess(void *, ARMword, ARMword *, ARMul_acc, int *,
                                            ARMword, ARMword *, ARMul_acc, int *);


typedef int armul_BytelanesMemAccess(void *,ARMword *addr, ARMword *data,
                                     ARMword *lanes,
                                     ARMul_acc);


/*
 * When the core takes an exception - e.g. an access to the vectors in a
 * 26-bit mode, on a processor which supports exporting such exceptions
 * (i.e. an ARM8) this function is called if supplied. penc is the priority
 * encoding for the exception in question.
 */
typedef void armul_CoreException(void *handle,ARMword address,
                                 ARMword penc);

/*
 * StrongARM has a tightly coupled data-cache, where the pipeline needs
 * to know if the data cache is busy. This function provides that
 * functionality.
 */

typedef unsigned int armul_DataCacheBusy(void *handle);

/*
 * Function to return the grant status of the AMBA bus in cycle-based
 * simulation (1=granted, 0=not-granted). This only applies to pipelined
 * AMBA processors and will determine whether an access proceeds or not.
 *
 */
typedef unsigned long armul_ArbitrateBus(void *handle, ARMword request);
typedef unsigned long armul_MultiLayerArbitrateBus(void *handle, 
                                                   unsigned int layer, 
                                                   ARMword request);

/*
 * Function to indicate that the previous prediction has proven to be invalid
 * as a result of data returned by the current cycle. Typically used for late
 * kill signals on the bus of the ARM9 integer core.
 */
typedef void armul_KillAccess(void* handle, ARMword data_addr, ARMul_acc data_acc,
                              ARMword instr_addr, ARMul_acc instr_acc);

typedef struct armul_meminterface_ref {
    /* This points to where a memory-model's armul_meminterface has
     * been installed or moved to. */
    struct armul_meminterface *mif;
    /* This allows a memory to tell the cache or core something. */
    void *handle;
    armul_InfoProc *master_info;
} ARMul_MemInterfaceRef;

/* QQQ: Why isn't armul_MemAccess *[debug_]access;
 * also outside the union?
 */
struct armul_meminterface {
    void *handle;
    /* armul_MemAccess *debug_access; */
    
    /* 
     * This allows any memory object to keep track of where its
     * interface has been moved to (so long as whatever moves it
     * remembers to update this link). NB Only objects which switch themselves
     * in and out need to do this.
     */
    struct armul_meminterface_ref *mem_link;
    /*
     * This allows a watcher to know what type of veneer to insert
     */
  ARMul_MemType memtype;

    /* This allows a client to query or inform its memory. */
  armul_InfoProc *mem_info;

  armul_ReadClock *read_clock;
  armul_ReadCycles *read_cycles;
  armul_GetCycleLength *get_cycle_length;

  union {
    struct {
      armul_MemAccess *access;
      armul_NextCycle *next;
      armul_ReadDeltaCycles *delta_cycles;
      /* Cycle-based models have arbitrate */
      armul_ArbitrateBus *arbitrate;
    } pipe;
    struct {
      armul_MemAccess *debug_access;
      armul_CoreException *core_exception;
      armul_HarvardMemAccess  *harvard_access;
      armul_NextDataCycle *dside_next;
      armul_NextCycle *iside_next;
      /* ARM10 has delta_cycles */
      armul_ReadDeltaCycles *delta_cycles;
        /* These ifdef's are required to avoid expanding the size of
         * this structure between ADS1.1 and ADS1.2, which causes
         * pain when trying to build memory-models which work in either. */
/* #ifdef PIPELINED */
      /* Cycle-based models have advance_bus and kill_access*/
      armul_AdvanceBus *advance_bus;
      armul_KillAccess *kill_access;
/* #endif */
    } pipe_arm9;
    struct {
      armul_MemAccess *debug_access;
      armul_CoreException *core_exception;
      armul_MultiLayerRegister  *register_layer;
      armul_MultiLayerMemAccess *current_access;
      armul_MultiLayerNext      *next_access;
      armul_MultiLayerReadDeltaCycles *delta_cycles;
/* #ifdef PIPELINED */
      /* Cycle-based models have arbitrate */
      armul_MultiLayerArbitrateBus *arbitrate;
/* #endif */
    } pipe_multi_layer;
    struct {
      armul_MemAccess *access;
    } basic;
    struct {
      armul_MemAccess *access;
      armul_CoreException *core_exception;
      armul_MemAccess2 *access2;
    } arm8;
    struct {
      armul_MemAccess *access;
      armul_CoreException *core_exception;
      armul_DataCacheBusy *data_cache_busy;
    } strongarm;
    struct {
      armul_MemAccess *access;
      armul_CoreException *core_exception;
      armul_DataCacheBusy *data_cache_busy;
      armul_BytelanesMemAccess *bytelanes_access;
    } bytelanes;
    struct {
      armul_MemAccess *access;
      armul_CoreException *core_exception;
      armul_HarvardMemAccess *harvard_access;
        armul_DataCacheBusy *data_cache_busy_obsolete; /* Never called */
      armul_MemBurstCount *burst_count;
    } arm9;
      /* ARM10+XScale core->cache. */
    struct {
      armul_MemAccess *debug_access;
      armul_MemAccAsync *i_access;
      armul_MemAccAsync *d_read;
      armul_MemAccAsync *d_write;
    } armiss_cache;
      /* This structure is the same as basic, but the access-function
       * must be capable of 64-bit transfers.
       * Also, ICycles will not be generated.
       */
    struct {
      armul_MemAccess *access;
    } armiss_ahb;
  } x;
};


/* 
 * Purpose: Given an existing interface connection,
 * Insert a new one, copying the old one to 'newChild'.
 *
 * Parameters:
 * Out:
 *       *newChild: memory owned by the caller. This call will fill that in
 *                  with the function pointers of the downstream memory
 *                  (copied from *mem).
 * InOut:
 *       *mem:      memory owned by someone soon to be upstream of
 *                  the caller. Caller will write function-pointers into
 *                  this after this call.
 * In:
 * *preparedEarlier: A set of function-pointers belonging to the caller
 *                  to be given to the upstream owner of *mem
 *                  (copied to *mem).
 *       newLink:   Address of 'leash' to mem
 *                  (newLink->mif will be updated if anyone
 *                   plugs themselves in upstream of us).
 *                  Note that only newLink->mem_link is changed - the other
 *                  fields of *newLink are unchanged.
 *               (These are expected to be a callback for newChild
 *                  to call through.)
 */
void ARMul_InsertMemInterface(ARMul_MemInterface *mem,
                              ARMul_MemInterface *newChild,
                              ARMul_MemInterfaceRef *newLink,
                              ARMul_MemInterface *preparedEarlier);
/* This is like the above, but also swaps the upwards ("master") memory-info
 *  callbacks at *newLink and *mem->mem_link.
 */
void ARMul_InsertMemInterface2(ARMul_MemInterface *mem,
                               ARMul_MemInterface *newChild,
                               ARMul_MemInterfaceRef *newLink,
                               ARMul_MemInterface *preparedEarlier);


/*
 * This is called by the owner of the function-pointers,
 * rather than the owner of the memory where they currently
 * reside.
 * Parameters:
 *      memref:   Address of pointer to set of function-pointers the
 *                 caller owns.
 *      child:    Address of downstream set of function-pointers.
 * NB: If the function-pointers at *memref are not saved, they are lost.
 */
void ARMul_RemoveMemInterface(ARMul_MemInterfaceRef *memref,
                              ARMul_MemInterface *child);

/* This is like the above, but also swaps the upwards ("master") memory-info
 *  callbacks at *newLink and *mem->mem_link.
 */
void ARMul_RemoveMemInterface2(ARMul_MemInterfaceRef *memref,
                               ARMul_MemInterface *child);


void ARMul_SwapMasterInfo(ARMul_MemInterfaceRef *my_ref,
                          ARMul_MemInterfaceRef *child_ref);


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif


#endif /* armul_mem__h */
