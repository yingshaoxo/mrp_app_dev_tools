/* mapfile.c - - Memory model that supports an "armsd.map" file 
 * Formerly known as armmap.c
 * Copyright (C) Advanced RISC Machines Limited, 1995. All rights reserved.
 * Copyright (C) ARM Limited, 1999-2001. All rights reserved. 
 */
#define MODEL_NAME Mapfile

/*
 * RCS $Revision: 1.16.4.23 $
 * Checkin $Date: 2001/10/19 18:14:09 $
 * Revising $Author: dsinclai $
 */

/* !Todo: Increment bus->bus_BusyUntil when returning "wait".
 * ? Maybe also check for bus-timed-callbacks?
 */

#include <string.h>
#include <ctype.h>

#include "minperip.h"
#include "armul_mem.h" /* Mix'n match */
#include "armul_callbackid.h"
#include "addcounter.h"
#define ACDTEST(FUNC,RET) if (FUNC == RDIError_BufferFull) \
                               RET = RDIError_BufferFull
#include "armul_cyc.h"
#include "armul_lib.h" /* for ARMul_SIRange() */

#include "rdi_conf.h"
#include "armul_cnf.h"

/*
 * In order to allow FPE to work with mapfile, mapfile must
 * enable the base memory and allow all non-aborting accesses to fall
 * through to the base memory
 */
#define USE_BASE_MEMORY 1

#ifndef NDEBUG
# if 1
# else
#  define VERBOSE_Mapfile_MemAccessHarvard
#  define VERBOSE_TIMES
#  define VERBOSE_BUS
#  define VERBOSE_MEMLOOKUP
#  define VERBOSE_DABORT
#  define VERBOSE_ACCESS
#  define VERBOSE_ENDIAN
#  define VERBOSE_CYCLE_DESC
#  define VERBOSE_INIT
#  define VERBOSE_ACCOUNTED_ACCESS
# endif
#endif



#ifdef HOST_ENDIAN_BIG
# define HostEndian 1
#elif defined(HOST_ENDIAN_LITTLE)
# define HostEndian 0
#else
# error "HOST_ENDIAN_UNKNOWN"
#endif


#include "rdi_stat.h"

#include "clxlist.h"

/* Possibly inline some functions */
#ifndef INLINE
# error "INLINE not defined"
#endif

#define ModelName (tag_t)"MapFile"


/* armmap.c */
/* #define ARMulCnf_CountWaitStates (tag_t)"COUNTWAITSTATES" */
#define ARMulCnf_AMBABusCounts (tag_t)"AMBABUSCOUNTS"
#define ARMulCnf_SpotISCycles (tag_t)"SPOTISCYCLES"
#define ARMulCnf_ISTiming (tag_t)"ISTIMING"



typedef struct MemDescr {
  struct MemDescr *next; /* So we can put these in a list */
  RDI_MemDescr desc;
  /* cycle counters */
  /* @@@ N.B. this table RELIES on the bit positions in acc words.
   * for indexing and for it's size.
   */
#if (WIDTH_MASK | acc_Nrw | acc_seq | acc_Nmreq) == 0x7f
  unsigned long access_counts[0x60];   /* number of accesses to this region */
  int counter[0x60];            /* wait states - -ve for "special" */
#else
#  error Code relies on things about ARMul_accs
#endif
#ifdef MAPFILE_IS_NOT_LEAF
    ARMul_BusPeripAccessRegistration *desc_bpar;
#endif
} MemDescr;


#define NUMPAGES 64 * 1024
#define ARMMAP_PAGESIZE 64 * 1024
#define PAGEBITS 16
#define OFFSETBITS 0xffff
#define OFFSETBITS_WORD 0xfffc

typedef struct {
  ARMword memory[ARMMAP_PAGESIZE/4];
} mempage;

typedef struct {
  mempage *page[NUMPAGES];
} memory;

typedef struct {
  ARMTime wait_states;    /* counter for wait states */
  ARMul_acc last_acc;           /* last cycle flag */
  int cnt;                      /* cycle-by-cycle counter */
} Bus;

/* was typedef ... toplevel; */
BEGIN_STATE_DECL(Mapfile)
  unsigned read_bigend, write_bigend;

  ARMword byteAddrXor,hwordAddrXor; /* for reads */

  MemDescr desc;
  ARMul_Cycles cycles;
  ARMul_Cycles saved_cycles;
  unsigned long IS_cycles;      /* counter for IS_cycles */
  unsigned long saved_IS_cycles;      
  Bus i, d, saved_i, saved_d;
  bool harvard_data_flag;
  memory mem;
  unsigned num_regions;

  /* Three values give the clock speed: 
   * clk - clockspeed, as cycle length in us
   * mult/period - integer representation of the same, for determining
   * the waitstates. The clock period is period*mult ns. This is done to
   * try to keep precision in division.
   */
  double clk;
  unsigned long mult,period;

  unsigned long prop;

  bool BaseMemoryEnabled;

  ARMul_MemInterface bus_mem,
                     child;      /* the base memory we override */
  ARMul_MemInterfaceRef mem_ref;
  ARMTime clk_speed;
  bool FailedToInstal;
  char *mf_CycleNames[16];
  int memtype;

  ARMul_Bus *pABus;
  bool pABus_isMine; /* Did we allocate the above? */
END_STATE_DECL(Mapfile)

/* #define MAP_COUNTWAIT     0x0001 */
#define MAP_AMBABUSCOUNTS 0x0002
#define MAP_SPOTISCYCLES  0x0004
#define MAP_HARVARD       0x0008

/*
 * When spotting I-S cycles, a memory controller can either:
 * - speculatively decode all I cycles. This gives 2 cycles to do the I-S
 *   access.
 * - check for nMREQ in the middle of I cycles. This gives 1.5 cycles to do
 *   the access.
 * - process each cycle in turn. This gives only 1 cycle to do the access.
 * armmap.c can model each of these, "SPECULATIVE", "EARLY" or "LATE"
 */
#define MAP_ISMODE        0x0030
#define MAP_IS_SPEC       0x0010
#define MAP_IS_EARLY      0x0020
#define MAP_IS_LATE       0x0030

#define MAP_FIXEDSEX      0x0040 /* memory bytesex is fixed */

static const struct {
  tag_t option;
  unsigned long flag;
} MapOption[] = {
/*  ARMulCnf_CountWaitStates, MAP_COUNTWAIT, */
  ARMulCnf_AMBABusCounts, MAP_AMBABUSCOUNTS,
  ARMulCnf_SpotISCycles, MAP_SPOTISCYCLES,
  NULL, 0
};



static int Mapfile_MemAccess(void *handle, ARMWord address, ARMWord *data,
                             unsigned acc);
static int Mapfile_MemAccessSA(void *handle,ARMWord address, ARMWord *data,
                               unsigned acc);
static void Mapfile_MemAccessHarvard(void *handle,
                       ARMword daddr,ARMword *ddata, ARMul_acc dacc, int *drv,
                       ARMword iaddr,ARMword *idata, ARMul_acc iacc, int *irv);

static const ARMul_Cycles *Mapfile_ReadCycles(void *handle);

#define toplevel MapfileState


static unsigned int Mapfile_DataCacheBusy(void *handle)
{
  UNUSEDARG(handle);
  return FALSE;
}

/* We are a leaf memory, so we don't need to pass this along. */
#define Mapfile_CoreException NULL

static unsigned long Mapfile_GetCycleLength(void *handle)
{
  /* Returns the cycle length in tenths of a nanosecond */
  MapfileState *top=(MapfileState *)handle;
  return (unsigned long)(top->clk*10000.0);
}

/* Needed for MSVC++6 */
static double u64_to_dbl(uint64 v)
{
    int64 v2 = (int64)v;
    return (double)v2;
}


static ARMTime Mapfile_ReadClock(void *handle)
{
  /* returns a us count */
  toplevel *top=(toplevel *)handle;
  double t;
  Mapfile_ReadCycles(handle);
  t=(u64_to_dbl(top->cycles.NumNcycles) +
     u64_to_dbl(top->cycles.NumScycles) +
     u64_to_dbl(top->cycles.NumIcycles) +
     u64_to_dbl(top->cycles.NumCcycles)  +
     u64_to_dbl(top->d.wait_states))*top->clk;  
#ifdef VERBOSE_TIMES
  printf("Mapfile_ReadClock->%f (clk=%f)\n Ncycles = %d\n, scycles = %d\n, Icycles = %d\n, ccycles = %d\n wait states = %d\n",
         t,top->clk, 
         (unsigned int)top->cycles.NumNcycles, 
         (unsigned int)top->cycles.NumScycles,
         (unsigned int)top->cycles.NumIcycles,
         (unsigned int)top->cycles.NumCcycles,
         (unsigned int)top->d.wait_states);
#endif
  return (ARMTime)(int64)t;
}


static unsigned Mapfile_MemInfo(void *handle, unsigned type, ARMword *pID,
                                uint64 *data)
{
    MapfileState *top=(MapfileState *)handle;
    if (ACCESS_IS_READ(type))
    {
        switch (*pID) {
        case MemPropertyID_TotalCycles:
            *data = top->cycles.Total;
            return RDIError_NoError;
        case MemPropertyID_CycleSpeed:
            *data = top->clk_speed;
            return RDIError_NoError;
        case MemPropertyID_BaseMemoryRef:    /* We are a LEAF */
            *(ARMul_MemInterfaceRef*)data = top->mem_ref;
            assert(top->mem_ref.mif->mem_link == &top->mem_ref);
            return RDIError_NoError;
        case MemPropertyID_BaseMemoryEnable: /* We are a LEAF */
            *data = top->BaseMemoryEnabled;
            return RDIError_NoError;

        default:
            return RDIError_UnimplementedMessage;
        }
    }
    else
    {
        switch (*pID) {
        case MemPropertyID_BaseMemoryEnable: /* We are a LEAF */
            top->BaseMemoryEnabled = (bool)*data;
            return RDIError_NoError;
        default:
            return RDIError_UnimplementedMessage;
        }
    }
}

static bool Mapfile_InstallNewBaseMemory(toplevel *ts)
{
    /* First, get the bus */
    /* Then a reference to the start of the (non-cache)memory-chain */
    ARMword *pprop = &ts->prop;

    ARMul_MemInterface *mif;
    uint32 ID[2];
    ID[0] = ARMulBusID_Bus;
    ID[1] = 0;
    mif = ARMulif_QueryMemInterface(&ts->coredesc, &ID[0]);
    if (!mif)
    {
        ID[0] = ARMulBusID_Core;
        mif = ARMulif_QueryMemInterface(&ts->coredesc, &ID[0]);
    }

    /* Ask for the last reference in the chain. */
    if (mif)
    {
        ARMul_MemInterface *mif2 = NULL;        
        uint32 ID[2]; ID[0]=MemPropertyID_BaseMemoryRef; ID[1]=0;
        mif->mem_info(mif->handle,
                      ACCESS_WRITE|ACCESS_WORD,&ID[0],(uint64*)&mif2);
        if (mif2)
        {
            mif = mif2;
        }
    }


#ifdef VERBOSE_BUS
    printf("Mapfile::mif=%p ID:%u\n",mif,(unsigned)ID[0]);
#endif
    if (mif)
    {
        /* Ask the downstream memory for its bus.
         * If that fails (because the bus is now owned by BIU instead
         * of Flatmem) ask the core.
         * !TODO: Ask >upstream< memory.
         */
        {
            uint32 ID[2]; ID[0]=MemPropertyID_Bus; ID[1]=0;
            mif->mem_info(mif->handle,
                          ACCESS_READ|ACCESS_WORD,&ID[0],
                          (uint64*)&ts->pABus);
            if (!ts->pABus)
            {
                ARMulif_QueryBus(&ts->coredesc,"",&ts->pABus);
            }

            if (!ts->pABus)
            {
                Hostif_PrettyPrint(ts->hostif,ts->config,
        "\n** Mapfile got no response to query for MemPropertyID_Bus **\n");
                ts->pABus = calloc(1,sizeof(ARMul_Bus));
                /* Later, construct such a bus? (*/

                ts->FailedToInstal = TRUE;
                return FALSE; /*RDIError_UnableToInitialise;*/
            }
        }

        ts->bus_mem.handle = ts;

        ts->bus_mem.x.basic.access = Mapfile_MemAccess;
        
        /* <Copy info thru. */
        ts->bus_mem.mem_info=Mapfile_MemInfo;
        /* If not emulated time, don't offer one */
        if (mif->read_clock)
            ts->bus_mem.read_clock=Mapfile_ReadClock;
        ts->bus_mem.read_cycles=Mapfile_ReadCycles;
        ts->bus_mem.get_cycle_length = Mapfile_GetCycleLength;
        ts->memtype = mif->memtype;
        switch(ts->memtype) 
        {
        case ARMul_MemType_Basic:
        case ARMul_MemType_16Bit:
        case ARMul_MemType_Thumb:
/*        case ARMul_MemType_BasicCached: */
        case ARMul_MemType_16BitCached:
        case ARMul_MemType_ThumbCached:
        case ARMul_MemType_ARMissAHB:
            break;
        case ARMul_MemType_AHB:
            break;
        case ARMul_MemType_StrongARM:
            *pprop |= MAP_HARVARD;
            ts->bus_mem.x.strongarm.access = Mapfile_MemAccessSA;
            ts->bus_mem.x.strongarm.core_exception = Mapfile_CoreException;
            ts->bus_mem.x.strongarm.data_cache_busy = Mapfile_DataCacheBusy;
            break;
        case ARMul_MemType_ARM8:
            ts->bus_mem.x.arm8.core_exception = Mapfile_CoreException;
            ts->bus_mem.x.arm8.access2 = Mapfile_MemAccess; /* MemAccess2 */
            break;

        case ARMul_MemType_ARM9: /* ARM9 Harvard   */
            *pprop |= MAP_HARVARD;
            ts->bus_mem.x.arm9.harvard_access = Mapfile_MemAccessHarvard;
            break;
        case ARMul_MemType_ARMissCache:
        case ARMul_MemType_ByteLanes:       
        default:
            Hostif_ConsolePrint(ts->hostif,
                     "Mapfile cannot handle this type of MemoryInterface.\n");
            return FALSE;
        }
        ARMul_InsertMemInterface(mif,
                                 &ts->child,
                                 &ts->mem_ref,
                                 &ts->bus_mem);
        return TRUE;
    }
    else
    {
        Hostif_ConsolePrint(ts->hostif,
                            "Core didn't give Mapfile a MemoryInterface.\n");
        return FALSE;
    }
}



/*
 * RDIInfo proc for installing the memory map.
 * Adds to the HEAD of the list
 *
 * Returns 0:Ok else error.
 */

static int InstallMemDescr(void *handle, RDI_MemDescr *md)
{
  MapfileState *top=(MapfileState *)handle;
  long cnt = 1, seq = 1, counter;
  int i, limit;
  unsigned long mult=top->mult,period=top->period;
  MemDescr *list = top->FailedToInstal ? NULL:calloc(1,sizeof(MemDescr));
  if (list==NULL) 
  {
      return (top->FailedToInstal=TRUE);
  }

  if (top->desc.next == NULL) {
    if (Mapfile_InstallNewBaseMemory(top))
        Hostif_ConsolePrint(top->hostif,"Memory map:\n");
    else
    {
        Hostif_ConsolePrint(top->hostif,"Memory map cannot start-up.\n");
        top->FailedToInstal = TRUE;
        return RDIError_Error;
    }
  }

  ++top->num_regions;

  *list = top->desc;
  top->desc.next = list;
  list = &top->desc;

  list->desc=*md;

  list->desc.limit+=list->desc.start-1;
  list->desc.width+=BITS_8;     /* 0->8 bits, 1->16, 2->32 */

  Hostif_ConsolePrint(top->hostif,"\
  %08x..%08x, %.2d-Bit, %c%c%c, wait states:",
                    list->desc.start,list->desc.limit,
                    1<<list->desc.width,
                    (list->desc.access & 4) ? '*' : ' ',
                    (list->desc.access & 2) ? 'w' : '-',
                    (list->desc.access & 1) ? 'r' : '-');



  /*
   * Cycle counts are kept in an array so that the mem_access function only
   * has to do an array lookup to get the cycle count. The work is done
   * here in setting up the array.
   * When spotting I-S cycles, the mem_access function uses an otherwise
   * unused entry (that for an idle cycle) to get the number of wait states.
   */

  limit = (top->prop & MAP_SPOTISCYCLES) ? 0x60 : 0x40;

  /* Many times around this loop correspond to illegal values of
   * acc_WIDTH.
   * Two values -- cnt and seq -- are used to say how many cycles are
   * needed for this type of access (N/S/I-S, Read or Write), cnt being
   * the number of cycles for the access, seq being for any sequential
   * cycles needed when accessing a value wider than the bus.
   * These values are invariant across the different bus widths, so are
   * set up the first time around the loop for a particular access type,
   * when the bus width is the illegal value '0'
   */

  for (i = 0; i < limit; i++) {
    counter=1;
    switch (acc_WIDTH(i)) {
    case BITS_32:
      counter=((list->desc.width==BITS_8) ? (cnt + seq * 3) :
               (list->desc.width==BITS_16) ? (cnt + seq) : cnt);
      if (acc_nSEQ(i) && acc_MREQ(i) && counter &&
          (top->prop & MAP_AMBABUSCOUNTS)) {
        counter++;              /* AMBA decode cycle for N */
      }
      break;

    case BITS_16:
      counter=((list->desc.width==BITS_8) ? (cnt + seq) : cnt);
      if (!acc_SEQ(i) && counter && top->prop & MAP_AMBABUSCOUNTS)
        counter++;              /* AMBA decode cycle for N */
      if (acc_READ(i) && acc_SEQ(i) && (list->desc.access & 4) &&
          counter > 1) {
        /* latched read possibly */
        counter = -counter;
      }
      break;

    default:
    case BITS_8:
      counter=cnt;
      if (!acc_SEQ(i) && counter && top->prop & MAP_AMBABUSCOUNTS)
         counter++;             /* AMBA decode cycle for N */
      break;

    case 0:
      /* First time round for this access type - get the base figures. */
      if (acc_nMREQ(i)) {
        /* Time for I-S cycles, if spotting them */
        if (acc_READ(i)) {
          if ((list->desc.access & 1) != 0) {   /* read okay */
            /* number of ticks needed for a sequential access */
            seq = list->desc.Sread_ns * mult;
            /* now divide by 'period' to get number of cycles, and add one
             * more cycle if there's any remainder */
            seq = (seq / period) + ((seq % period) != 0 || (seq == 0));

            /* cnt is set to the number of ticks needed for a non-seq
             * access. */
            cnt = list->desc.Nread_ns * mult;

            /* The number of wait-states needed for an I-S access depends
             * on the mode being used, which determines how many cycles there
             * are to start with.
             */
            switch (top->prop & MAP_ISMODE) {
            case MAP_IS_SPEC:
              /* For speculative decode, there are 2 cycles to start with */
              /* divide by the period to get the number of cycles, add one
               * more if there's any remainder, then take off the one free
               * we have from the speculative decode */
              cnt = (cnt / period) + ((cnt % period) != 0) - 1;
              break;

            case MAP_IS_EARLY:
              /* For the early decode, there are 1.5 cycles to start with */
              /* divide by the period to get the number of cycles, and add
               * one if the remainder is greater than half-a-cycle. */
              cnt = (cnt / period) + ((cnt % period) >= (period / 2));
              break;

            case MAP_IS_LATE:   /* one + wait cycle */
              /* For the late decode, there's only 1 cycle to start with */
              /* now divide by 'period' to get number of cycles, and add one
               * more cycle if there's any remainder */
              cnt = (cnt / period) + ((cnt % period) != 0);
              break;
            }
            /* Make sure we have at least 1 cycle! */
            if (cnt <= 0) cnt = 1;
          } else {
            seq = cnt = 0;      /* zero cycles signals 'abort' */
          }
        } else {                /* write */
          if ((list->desc.access & 1) != 0) {   /* read okay */
            /* see above */
            seq = list->desc.Swrite_ns * mult;
            seq = (seq / period) + ((seq % period) != 0 || (seq == 0));

            cnt = list->desc.Nwrite_ns * mult;
            switch (top->prop & MAP_ISMODE) {
            case MAP_IS_SPEC:   /* two cycles */
              cnt = (cnt / period) + ((cnt % period) != 0) - 1;
              break;
            case MAP_IS_EARLY:  /* 1.5 cycles */
              cnt = (cnt / period) + ((cnt % period) >= (period / 2));
              break;
            case MAP_IS_LATE:   /* one cycle */
              cnt = (cnt / period) + ((cnt % period) != 0);
              break;
            }
            if (cnt <= 0) cnt = 1;
          } else {
            seq = cnt = 0;
          }
        }
      } else if (acc_READ(i)) { /* read */
        if (list->desc.access & 1) { /* read access okay */
          /* see above */
          seq = list->desc.Sread_ns * mult;
          seq = (seq / period) + ((seq % period) != 0 || (seq == 0));
          if (acc_SEQ(i)) {
            cnt = seq;
          } else {
            /* see above */
            cnt = list->desc.Nread_ns * mult;
            cnt = (cnt / period) + ((cnt % period) != 0 || (cnt == 0));
          }
        } else {
          seq = cnt = 0;
        }
      } else {                  /* write */
        if (list->desc.access & 2) { /* write access okay */
          /* see above */
          seq = list->desc.Swrite_ns * mult;
          seq = (seq / period) + ((seq % period) != 0 || (seq == 0));
          if (acc_SEQ(i)) {
            cnt = seq;
          } else {
            /* see above */
            cnt = list->desc.Nwrite_ns * mult;
            cnt = (cnt / period) + ((cnt % period) != 0 || (cnt == 0));
          }
        } else {
          seq = cnt = 0;
        }
      }

      /* report the wait states */
      Hostif_ConsolePrint(top->hostif, " %c", acc_READ(i) ? 'R' : 'W');
      if (acc_nMREQ(i)) {
        Hostif_ConsolePrint(top->hostif, "IS");
      } else {
        Hostif_ConsolePrint(top->hostif, "%c", acc_SEQ(i) ? 'S' : 'N');
      }

      if (cnt != 0) {
        Hostif_ConsolePrint(top->hostif, "=%d", cnt - 1);
        if ((acc_nSEQ(i) || acc_nMREQ(i)) && seq != cnt) {
          Hostif_ConsolePrint(top->hostif, "/%d", seq - 1);
        }
      } else
        Hostif_ConsolePrint(top->hostif, "=Abt");
    }
    list->access_counts[i]=0;
    /*
     * normally "counter" will be the number of cycles the access takes, so
     *   "counter-1" is the number of wait states.
     * if this would abort, "counter==0", so wait states becomes -ve - i.e. -1.
     * if a 16-bit sequential read to latched memory, then the value is -ve
     *   number of cycles. "counter-1" could only ==-1 iff counter=-0, 
     *   i.e. abort,
     *   so these -ve numbers don't overlap with the "abort" signal. The real
     *   number of wait states is "(-counter)-1" i.e. "-(-counter-1)-2"
     */
    list->counter[i]=counter-1; /* number of wait states, or special */
  }


  /* Ask the host to disable its underlying memory's RDIInfo. */
  if (!top->FailedToInstal)
  { 
      uint64 data = FALSE;
      uint32 ID[2]; ID[0]=MemPropertyID_BaseMemoryEnable; ID[1]=0;
      top->child.mem_info(top->child.handle,
                          ACCESS_WRITE|ACCESS_WORD,&ID[0],&data);
  }

  Hostif_ConsolePrint(top->hostif,"\n");

  return 0;
}

/*
 * Callback for telling about memory stats
 */
static const RDI_MemAccessStats *GetAccessStats(void *handle,
                                                RDI_MemAccessStats *stats,
                                                ARMword s_handle)
{
  MemDescr *desc;
  MapfileState *top=(MapfileState *)handle;

  desc = &top->desc;

  while (desc) {
    if (desc->desc.handle==s_handle) {
      int i;

      stats->Nreads=stats->Nwrites=stats->Sreads=stats->Swrites=0;
      stats->ns=stats->s=0;

      for (i=0;i<0x40;i++) {
        unsigned long count;
        double ns;
        count=desc->access_counts[i];
        if (acc_READ(i)) {
          if (acc_SEQ(i))
            stats->Sreads+=count;
          else
            stats->Nreads+=count;
        } else {
          if (acc_SEQ(i))
            stats->Swrites+=count;
          else
            stats->Nwrites+=count;
        }

        if (desc->counter[i]>=0) {
          ns=((double)(count*(desc->counter[i]+1))*top->clk*1000.0);
        } else {
          ns=((double)(count)*top->clk*1000.0);
        }

        while (ns>1e9) {
          ns-=1e9;
          stats->s++;
        }

        stats->ns+=(unsigned long)ns;
        /*
         * Just in case this goes round more than once
         */
        while (stats->ns>(unsigned long)1e9) {
          stats->ns-=(unsigned long)1e9;
          stats->s++;
        }
      }

      return stats;
    }
    desc=desc->next;
  }

  return NULL;                  /* not found */
}


/*
 * Function to deal with RDI calls related to memory maps
 */

static int RDI_info(void *handle, unsigned type, ARMword *arg1, ARMword *arg2)
{
  toplevel *top = (toplevel *)handle;
  switch (type) {
  case RDIMemory_Access:
    if (GetAccessStats(handle,(RDI_MemAccessStats *)arg1,*arg2))
      return RDIError_NoError;
    else
      return RDIError_NoSuchHandle;
    
  case RDIMemory_Map: {
    int n=(int)*arg2;
    RDI_MemDescr *p=(RDI_MemDescr *)arg1;
    while (--n >= 0 && !top->FailedToInstal)
      if (InstallMemDescr(handle,p++)!=0)
        return RDIError_Error;
  }
    return RDIError_NoError;

  case RDIInfo_Memory_Stats:
    return RDIError_NoError;

  case RDICycles: {
      if(top->num_regions != 0)
      {
          MapfileState *top=(MapfileState *)handle;
          ARMTime idle;
          /* Fix up NumScycles and NumNcycles */
          Mapfile_ReadCycles(handle);
          
          ARMul_AddCounterValue64(NULL, arg1, arg2, top->cycles.NumScycles);
          ARMul_AddCounterValue64(NULL, arg1, arg2, top->cycles.NumNcycles);
          if (!(top->prop & MAP_HARVARD) && (top->prop & MAP_AMBABUSCOUNTS)) {
              idle = top->cycles.NumIcycles + top->cycles.NumCcycles;
              ARMul_AddCounterValue64(NULL, arg1, arg2, idle);
          } else {
              idle = top->cycles.NumIcycles;
              ARMul_AddCounterValue64(NULL, arg1, arg2, top->cycles.NumIcycles);
              ARMul_AddCounterValue64(NULL, arg1, arg2, top->cycles.NumCcycles);
          }
/*           if (top->prop & MAP_COUNTWAIT) */
          {
              ARMul_AddCounterValue64(NULL, arg1, arg2, top->d.wait_states);
              if (top->prop & MAP_HARVARD) {
                  ARMul_AddCounterValue64(NULL, arg1, arg2, top->i.wait_states);
              }
          }
          ARMul_AddCounterValue64(NULL, arg1, arg2, top->cycles.Total);
          if (top->prop & MAP_SPOTISCYCLES) {
              idle -= top->IS_cycles;
              ARMul_AddCounterValue64(NULL, arg1, arg2, idle);
          }
      }
      
      break;
  }
  case RDIRequestCyclesDesc: {
      if(top->num_regions != 0)
      {
          MapfileState *top=(MapfileState *)handle;
          int  retVal = RDIError_UnimplementedMessage;
          top->mf_CycleNames[10]="IS_Cycles";
          if (top->prop & MAP_HARVARD) {
              if( top->memtype == ARMul_MemType_StrongARM) {
                  top->mf_CycleNames[1]="Core_ID";
                  top->mf_CycleNames[0]="Core_IOnly";
                  top->mf_CycleNames[2]="Core_Idles";
                  top->mf_CycleNames[3]="Core_DOnly";
              } else {
                  top->mf_CycleNames[1]="ID_Cycles";
                  top->mf_CycleNames[0]="IBus_Cycles";
                  top->mf_CycleNames[2]="Idle_Cycles";
                  top->mf_CycleNames[3]="DBus_Cycles";
                  ARMulif_ReadCycleNames(&top->mf_CycleNames[0],16,
                                         top->memtype,top->config);
                     
              }
            
              ACDTEST(ARMul_AddCounterDesc(
                          NULL, arg1, arg2, 
                          top->mf_CycleNames[1] /*"ID_Cycles"*/), retVal);
              ACDTEST(ARMul_AddCounterDesc(
                          NULL, arg1, arg2, 
                          top->mf_CycleNames[0]/*"IBus_Cycles"*/), retVal);
              ACDTEST(ARMul_AddCounterDesc(
                          NULL, arg1, arg2,
                          top->mf_CycleNames[2]/*"Idle_Cycles"*/), retVal);
              ACDTEST(ARMul_AddCounterDesc(
                          NULL, arg1, arg2,
                          top->mf_CycleNames[3]/*"DBus_Cycles"*/), retVal);
          } else if (top->prop & MAP_AMBABUSCOUNTS) {
              top->mf_CycleNames[1]="S_Cycles";
              top->mf_CycleNames[0]="N_Cycles";
              top->mf_CycleNames[2]="A_Cycles";
              ARMulif_ReadCycleNames(&top->mf_CycleNames[0],16,
                                     top->memtype,top->config);
              ACDTEST(ARMul_AddCounterDesc(
                          NULL, arg1, arg2,
                          top->mf_CycleNames[1]/*"S_Cycles"*/), retVal);
              ACDTEST(ARMul_AddCounterDesc(
                          NULL, arg1, arg2,
                          top->mf_CycleNames[0]/*"N_Cycles"*/), retVal);
              ACDTEST(ARMul_AddCounterDesc(
                          NULL, arg1, arg2,
                          top->mf_CycleNames[2]/*"A_Cycles"*/), retVal);
          } else {
              top->mf_CycleNames[1]="S_Cycles";
              top->mf_CycleNames[0]="N_Cycles";
              top->mf_CycleNames[2]="I_Cycles";
              top->mf_CycleNames[3]="C_Cycles";
              ARMulif_ReadCycleNames(&top->mf_CycleNames[0],16,
                                     top->memtype,top->config);
              ACDTEST(ARMul_AddCounterDesc(
                          NULL, arg1, arg2,
                          top->mf_CycleNames[1]/*"S_Cycles"*/), retVal);
              ACDTEST(ARMul_AddCounterDesc(
                          NULL, arg1, arg2,
                          top->mf_CycleNames[0]/*"N_Cycles"*/), retVal);
              ACDTEST(ARMul_AddCounterDesc(
                          NULL, arg1, arg2,
                          top->mf_CycleNames[2]/*"I_Cycles"*/), retVal);
              ACDTEST(ARMul_AddCounterDesc(
                          NULL, arg1, arg2,
                          top->mf_CycleNames[3]/*"C_Cycles"*/), retVal);
          }
/*          if (top->prop & MAP_COUNTWAIT)  */
          {              
              if (top->prop & MAP_HARVARD) {
                  ACDTEST(ARMul_AddCounterDesc(NULL, arg1, arg2, "D_Wait_States"), retVal);
                  ACDTEST(ARMul_AddCounterDesc(NULL, arg1, arg2, "I_Wait_States"), retVal);
              } else {
                  ACDTEST(ARMul_AddCounterDesc(NULL, arg1, arg2, "Wait_States"), retVal);
              }
          }
          ACDTEST(ARMul_AddCounterDesc(NULL, arg1, arg2, "Total"), retVal);
          if (top->prop & MAP_SPOTISCYCLES) {
              ACDTEST(ARMul_AddCounterDesc(NULL, arg1, arg2, "True_Idle_Cycles"), retVal);
          }
      } 
  break;
  }

  default:
    /* check for capability messages */
    if (type & RDIInfo_CapabilityRequest)
      switch (type & ~RDIInfo_CapabilityRequest) {
      case RDIMemory_Access:
      case RDIMemory_Map:
      case RDIInfo_Memory_Stats:
        return RDIError_NoError;

      default:
        break;                  /* fall through */
      }
    break;
  }
  return RDIError_UnimplementedMessage;
}


static void Mapfile_Reset(MapfileState *top);

/*
 * Other ARMulator callbacks
 */

/* Listen for EndiannessChanged Event */
static unsigned Mapfile_ConfigEvents(void *handle, void *data)
{
  MapfileState *top=(MapfileState *)handle;  
  ARMul_Event *evt = (ARMul_Event *)data;
  if (evt->event == ConfigEvent_Reset)
      Mapfile_Reset(top);
  if (evt->event == ConfigEvent_EndiannessChanged)
  {
      top->read_bigend = (!!evt->data1 != HostEndian) ? 3 : 0;
      if (!(top->prop & MAP_FIXEDSEX))
          top->write_bigend = top->read_bigend;
#ifdef VERBOSE_ENDIAN
      printf("\nMapfile read_bigend:=%08lx write_bigend:=%08lx "
             "evt->data1:%08lx HostEndian:%08lx\n",
             (unsigned long)top->read_bigend,
             (unsigned long)top->write_bigend,
             (unsigned long)evt->data1,
             (unsigned long)HostEndian);
#endif  
      top->byteAddrXor = ( top->read_bigend != HostEndian ) ? 3 : 0;
      top->hwordAddrXor = ( top->read_bigend != HostEndian ) ? 2 : 0;
  }
  if(evt->event == DebugEvent_RunCodeSequence)
  {
      /* save cycle counts */
      top->saved_cycles = top->cycles;      
      top->saved_IS_cycles = top->IS_cycles;
      top->saved_d = top->d;
      top->saved_i = top->i;
  }
  if(evt->event == DebugEvent_EndCodeSequence)
  {
      /* restore cycle counts */
      top->cycles = top->saved_cycles;
      top->IS_cycles = top->saved_IS_cycles;
  }
    
  return FALSE;

}

static void Mapfile_Reset(MapfileState *top)
{
    MemDescr *desc,*next;
    int i;
    memset(&top->cycles,0,sizeof(top->cycles));
    top->d.wait_states = 0;
    top->d.cnt = 0;
    top->i.wait_states = 0;
    top->i.cnt = 0;
    top->IS_cycles = 0;
    
    /* Clear the memstats ( memory_statistics ) data fields .
       This happens on startup, load, reload. 
       */
    for (desc = &top->desc; desc != NULL; desc=next) 
    {
        next=desc->next;
        for (i=0; i < 0x60; i++)
        {
            desc->access_counts[i] = 0;
        } /* end for i=0 */
    } /* end for desc=top */
}



BEGIN_INIT(Mapfile)
{
  MapfileState *top=state;
  memory *mem=&top->mem;

  unsigned page, i;
  unsigned long prop = 0;

  Hostif_PrettyPrint(hostif, config, ", Mapfile");

  /* Initialise a catch-all for the whole of memory */
  top->desc.desc.handle = RDI_NoHandle;
  top->desc.desc.start = 0l;
  top->desc.desc.limit = 0xffffffffl;
  top->desc.desc.width = 2;
  top->desc.desc.access = 3;
  top->desc.desc.Nread_ns =
    top->desc.desc.Nwrite_ns =
    top->desc.desc.Sread_ns =
    top->desc.desc.Swrite_ns = 0;
  memset(&top->desc.access_counts, 0, sizeof(top->desc.access_counts));
  memset(&top->desc.counter, 0, sizeof(top->desc.counter));
  top->desc.next = NULL;

  top->BaseMemoryEnabled = ToolConf_DLookupBool(config,
                                           (tag_t)"ENABLE_UNMAPPED_MEMORY",
                                               TRUE);


  {
  /* Get cycle-length in 10ths of a nanosecond. */
  uint32 bus_cycle_length =
      ARMulif_GetCycleLength(&state->coredesc); /* !REVERT RENAME! */
  if (!bus_cycle_length)
  {
      uint32 mclk = ARMul_GetMCLK(config);
      if (mclk) {
          bus_cycle_length = (uint32)(0.5 + 100000.0 * 100000.0 / mclk);
      }
  }

  if (!bus_cycle_length)
  {
      Hostif_PrettyPrint(hostif, config,
                     "\n** Mapfile cannot get bus cycle-length from core."
                         "Defaulting to 20MHz.\n");
      bus_cycle_length = 500; /* default to 20MHz */
  }

  top->mult=10; /* Convert ns to 10ths of ns */
  top->period=bus_cycle_length;
  top->clk=(double)bus_cycle_length / 10000.0; /* in microseconds */
  }
  

  for (i = 0; MapOption[i].option != NULL; i++) {
      if (ToolConf_DLookupBool(config, MapOption[i].option,FALSE)) {
          prop |= MapOption[i].flag;
      }
  }

  if (prop & MAP_SPOTISCYCLES) {
    char const *option = ToolConf_Lookup(config, ARMulCnf_ISTiming);
    if (option != NULL && ToolConf_Cmp(option, "SPECULATIVE"))
      prop |= MAP_IS_SPEC;
    else if (option != NULL && ToolConf_Cmp(option, "EARLY"))
      prop |= MAP_IS_EARLY;
    else
      prop |= MAP_IS_LATE;
  }

  top->prop = prop;

  for (page=0; page<NUMPAGES; page++) {
    mem->page[page]=NULL;
  }

  top->cycles.NumNcycles=0;
  top->cycles.NumScycles=0;
  top->cycles.NumIcycles=0;
  top->cycles.NumCcycles=0;
  top->cycles.NumFcycles=0;

  top->d.wait_states=0;
  top->d.cnt=0;

  top->i.wait_states=0;
  top->i.cnt=0;

  top->harvard_data_flag = FALSE;

  top->write_bigend = HostEndian ? 3 : 0; /* writes start littleendian */
  top->read_bigend = top->write_bigend;

  ARMulif_InstallUnkRDIInfoHandler(&state->coredesc,RDI_info,top);
  ARMulif_InstallEventHandler(&state->coredesc,
                              (ConfigEventSel | DebugEventSel),
                              Mapfile_ConfigEvents, state);


}
END_INIT(Mapfile)


#if USE_BASE_MEMORY
#else
static mempage *NewPage(void *handle)
{
  unsigned int i;
  MapfileState *top=(MapfileState *)handle;
  mempage *page=(mempage *)malloc(sizeof(mempage));

  if ( page == NULL)
    {
    /* The malloc failed. we have to do a number of things.
        1. call ARMulRaiseError
        2. Call ARMul_ModelBroken
        3. Return the zero page so that emulation continues
        long enough for the halt to take effect
     */
    return top->mem.page[0];
    }

  /*
   * We fill the new page with data that, if the ARM tried to execute
   * it, it will cause an undefined instruction trap (whether ARM or
   * Thumb)
   */
  for (i=0;i<ARMMAP_PAGESIZE/4;) {
    page->memory[i++]=0xe7ff0010; /* an ARM undefined instruction */
    /* The ARM undefined insruction has been chosen such that the
     * first halfword is an innocuous Thumb instruction (B 0x2)
     */
    page->memory[i++]=0xe800e800; /* a Thumb undefined instruction */
  }
  return page;
}
#endif

/*
 * Generic memory interface. Just alter this for a memtype memory system
 */

/* Returns a PERIP_ value from minperip.h */
static INLINE int FindMemoryRegion(
    MapfileState *top, ARMword address, ARMword *data, ARMul_acc acc,
    Bus *bus)
{
  /* first cycle of the access */
  MemDescr *desc;
  unsigned long prop = top->prop;

  (void)data;
      
  for (desc = &top->desc; desc; desc = desc->next) {
    if (address >= desc->desc.start &&
        address <= desc->desc.limit) {

      desc->access_counts[acc & (WIDTH_MASK | acc_seq | acc_Nrw)]++;

      /* If we're looking for I-S cycles, then use the N cycle
       * counter if we're on an I-S boundary
       */
      if ((prop & MAP_SPOTISCYCLES) &&
          acc_SEQ(acc) &&
          acc_nMREQ(bus->last_acc)) {
        /* Use the 'this width, idle, read/write' counter */
        bus->cnt = desc->counter[(acc & (WIDTH_MASK | acc_Nrw)) | acc_Nmreq];
        if (prop & MAP_AMBABUSCOUNTS) {
          /* This count includes an extra cycle for AMBA decode. We
           * don't need this for I-S cycles, so we need to remove it.
           */
          if (bus->cnt > 0) bus->cnt--;
        }
        top->IS_cycles++;
      } else
        bus->cnt = desc->counter[acc & (WIDTH_MASK | acc_seq | acc_Nrw)];

      if (bus->cnt) {
        if (bus->cnt < 0) {
          if (bus->cnt == -1) {/* abort */
            bus->cnt = 0;
            bus->last_acc = acc;

#ifdef VERBOSE_DABORT
            printf("Mapfile DABORT addr:%08lx\n", (unsigned long)address);
#endif

            return PERIP_DABORT /*-1*/;
          }
          /*
           * otherwise, this is a 16-bit SEQ read
           * - takes 1 cycle if an even address,
           * - takes -cnt-2 cycles otherwise.
           */
          if (address & 1) bus->cnt = -bus->cnt-2;
          else break;       /* i.e. fall through - 1 cycle */
        }
        bus->wait_states++;
        bus->last_acc = acc;
        return PERIP_BUSY /*0*/;
      }
      break;                /* fall through */
    }
  }
  if ((prop & MAP_AMBABUSCOUNTS) &&
      acc_MREQ(acc) && acc_nSEQ(acc)) {
    /* needs an address decode cycle */
    bus->cnt = 1;
    bus->wait_states++;
    bus->last_acc = acc;
    return PERIP_BUSY /*0*/;
  }

  return PERIP_OK /*1*/;
}

#if USE_BASE_MEMORY
#else
static int MemLookup(MapfileState *top, ARMword address, ARMword *data,
                     ARMul_acc acc)
{
  unsigned int pageno;
  mempage *page;
  ARMword *ptr;
  ARMword offset;

  pageno = address >> PAGEBITS;
  page=top->mem.page[pageno];
  if (page == NULL) {
    top->mem.page[pageno] = page = NewPage(top);
  }
  offset = address & OFFSETBITS_WORD;
  ptr=(ARMword *)((char *)(page->memory)+offset);

#ifdef VERBOSE_MEMLOOKUP
  printf("mapfile:MemLookup(A:%08lx T:%08lx)\n",
         (unsigned long)address,(unsigned long)acc);
#endif

  switch (acc & (ACCESS_SIZE_MASK | ACCESS_IDLE | ACCESS_WRITE) ) 
    {
      case BITS_8:              /* read byte */
        *data = ((unsigned8 *)ptr)[(address ^ top->byteAddrXor) & 3];
        break;
        
      case BITS_16:           /* read half-word */
        *data = *((unsigned16 *)(((char *)ptr)+
                                 ((address ^ top->hwordAddrXor) & 2)));
        return 1;

      case BITS_32:             /* read word */
        *data=*ptr;
        return 1;

      case BITS_64:             /* read dword */
        *data=*ptr;
        if ((offset+4) & OFFSETBITS_WORD) 
        {
            data[1]=ptr[1];
            return 2;
        }
        break;


      case BITS_8 | ACCESS_WRITE:              /* write_byte */
        if (HostEndian!=top->write_bigend) address^=3;
        ((unsigned8 *)ptr)[address & 3]=(unsigned8)(*data);
        break;
        
      case BITS_16 | ACCESS_WRITE:             /* write half-word */
        if (HostEndian!=top->write_bigend) address^=2;
        *((unsigned16 *)(((char *)ptr)+(address & 2))) = (unsigned16)(*data);
        break;

      case BITS_32 | ACCESS_WRITE:             /* write word */
        *ptr=*data;
        return 1;

    case BITS_64 | ACCESS_WRITE:
        *ptr=*data;
        if ((offset+4) & OFFSETBITS_WORD) 
        {
            ptr[1]=data[1];
            return 2;
        }
        break;

      case ACCESS_IDLE:
        return 1;
        
      default:
#ifdef VERBOSE_DABORT
          printf("mapfile:MemLookup -> abort\n");
#endif
          return PERIP_DABORT;

    }
  return PERIP_OK;
}
#endif

static int Mapfile_MemAccess(void *handle, ARMWord address, ARMWord *data,
                             unsigned acc)
{
    MapfileState *top=(MapfileState *)handle;

#ifdef VERBOSE_ACCESS
    /* if (ACCESS_IS_IDLE(acc)) */
    {
        printf("Mapfile_Access AD:%08lx AC:%04lx\n",
               (unsigned long)address, (unsigned long)acc);
    }
#endif

    if (ACCESS_IS_REAL(acc)) 
    {
        ++top->cycles.Total;

#ifdef VERBOSE_ACCOUNTED_ACCESS
        printf("Mapfile_Access! AD:%08lx AC:%04lx\n",
               (unsigned long)address, (unsigned long)acc);
#endif
    if (acc_MREQ(acc)) {
      if (top->d.cnt == 0 || acc != top->d.last_acc) {
        int rv = FindMemoryRegion(top, address, data, acc, &top->d);
        if (rv != PERIP_OK) {
          top->pABus->bus_BusyUntil++;
          return rv;
        }
        /* else fall through */
      } else {
        /* not the first cycle */
        if (--top->d.cnt) {
          top->d.wait_states++;
          top->d.last_acc = acc;
          top->pABus->bus_BusyUntil++;
          return PERIP_BUSY;
        }
        /* else fall through */
      }

    } else {
      if (acc_SEQ(acc)) {
        top->cycles.NumCcycles++;
      } else {
        top->cycles.NumIcycles++;
      }
    }
    top->d.cnt = 0;
    top->d.last_acc = acc;
  } /* endif ACCESS_IS_REAL */
  
#if USE_BASE_MEMORY /* T */
  return top->child.x.basic.access(top->child.handle, address, data, acc);
#else
  return MemLookup(top, address, data, acc);
#endif
}


static int Mapfile_MemAccessSA(void *handle,ARMWord address, ARMWord *data,
                               unsigned acc)
{
    MapfileState *top=(MapfileState *)handle;

  if (ACCESS_IS_REAL(acc)) {
    Bus *bus;
    if (acc_OPC(acc)) {
      ++top->cycles.Total;
      bus = &top->i;
    } else {
      bus = &top->d;
    }
    if (acc_MREQ(acc)) {
      if (bus->cnt == 0 || acc != bus->last_acc) {
        int rv = FindMemoryRegion(top, address, data, acc, bus);
        if (rv != PERIP_OK) {
          top->pABus->bus_BusyUntil++;
          return rv;
        }
        /* else fall through */
      } else {
        /* not the first cycle */
        if (--bus->cnt) {
          bus->wait_states++;
          bus->last_acc = acc;
          top->pABus->bus_BusyUntil++;
          return PERIP_BUSY /*0*/;
        }
        /* else fall through */
      }

      /*
       * On Harvard architectures there are four types of cycle -
       * we'll reuse the four cycle counters for these:
       *
       *  Instruction fetched, No data fetched      N
       *  Instruction fetched, data fetched         S
       *  No instruction fetched, No data fetched   I
       *  No instruction fetched, data fetched      C
       */

      if (acc_OPC(acc)) {
        /* End of cycle - account for access */
        /* This access is either acc_LoadInstrN or acc_NoFetch */
        if (top->harvard_data_flag) {
          /* data fetched */
          top->harvard_data_flag = FALSE;
          top->cycles.NumScycles++; /* instr/data */
        } else {
          /* no data fetched */
          top->cycles.NumNcycles++;/* instr/no data */
        }
      } else {
        /* don't count on data accesses */
        /* data fetched */
        top->harvard_data_flag = TRUE;
      }
    } else {
      if (acc_OPC(acc)) {
        /* End of cycle - account for access */
        /* This access is either acc_LoadInstrN or acc_NoFetch */
        if (top->harvard_data_flag) {
          /* data fetched */
          top->harvard_data_flag = FALSE;
          top->cycles.NumCcycles++; /* no instr/data */
        } else {
          /* no data fetched */
          top->cycles.NumIcycles++;/* no instr/no data */
        }
      }
    }
    bus->cnt = 0;
    bus->last_acc = acc;
  }
#if USE_BASE_MEMORY
  return top->child.x.basic.access(top->child.handle, address, data, acc);
#else
  return MemLookup(top, address, data, acc);
#endif
}

static const ARMul_Cycles *Mapfile_ReadCycles(void *handle)
{
  static ARMul_Cycles cycles;
  MapfileState *top=(MapfileState *)handle;

  if (!(top->prop & MAP_HARVARD)) {
    ARMword n = 0, s = 0;
    MemDescr *d;
    
    for (d = &top->desc; d != NULL; d = d->next) {
      ARMul_acc acc;
      for (acc = 0; acc < 0x60; acc++) {
        if (acc_MREQ(acc)) {
          if (acc_SEQ(acc)) {
            s += d->access_counts[acc];
          } else {
            n += d->access_counts[acc];
          }
        }
      }
    }
    top->cycles.NumNcycles = n;
    top->cycles.NumScycles = s;
  }

/*  if (top->prop & (MAP_COUNTWAIT | MAP_AMBABUSCOUNTS))  */
  {
    cycles.NumNcycles = top->cycles.NumNcycles;
    cycles.NumScycles = top->cycles.NumScycles;
    cycles.NumIcycles = top->cycles.NumIcycles + top->cycles.NumCcycles;
    cycles.NumCcycles = top->d.wait_states + top->i.wait_states;
  }
/*   else  {
 *    cycles = top->cycles;
 * }
 */
  cycles.Total = top->cycles.Total;
  return &cycles;
}



static void Mapfile_MemAccessHarvard(void *handle,
                       ARMword daddr,ARMword *ddata, ARMul_acc dacc, int *drv,
                       ARMword iaddr,ARMword *idata, ARMul_acc iacc, int *irv)
{

#ifdef VERBOSE_Mapfile_MemAccessHarvard
    printf("Mapfile_MemAccessHarvard BEGIN DA:%08lx DT:%04lx "
           " IA:%08lx IT:%04lx    DS:%i IS:%i\n",
           (unsigned long)daddr,(unsigned long)dacc,
           (unsigned long)iaddr,(unsigned long)iacc, *drv, *irv);
#endif
    /* We do D then I because MemAccessSA expects that. */
    if (!*drv)
        *drv = Mapfile_MemAccessSA(handle,daddr,ddata,dacc);
    else
        Mapfile_MemAccessSA(handle,daddr,ddata,ACCESS_IDLE|ACCESS_DATA);
    if (!*irv)
        *irv = Mapfile_MemAccessSA(handle,iaddr,idata,iacc);
    else
        Mapfile_MemAccessSA(handle,iaddr,idata,ACCESS_IDLE|ACCESS_INSTR);
#ifdef VERBOSE_Mapfile_MemAccessHarvard
    printf("Mapfile_MemAccessHarvard END -> DS:%i IS:%i\n",
           *drv, *irv);
#endif
}



BEGIN_EXIT(Mapfile)
{
  ARMword page;
  MapfileState *top=state; /* (MapfileState *)handle */
  memory *mem=&top->mem;
  MemDescr *desc,*next;

  /* free all truly allocated pages */
  for (page=0; page<NUMPAGES; page++) {
    mempage *pageptr= mem->page[page];
    if (pageptr) {
      free((char *)pageptr);
    }
  }

  for (desc = top->desc.next; desc != NULL; desc=next) {
    next=desc->next;
    free(desc);
  }
}
END_EXIT(Mapfile)



/*--- <SORDI STUFF> ---*/
#define SORDI_DLL_NAME_STRING "Mapfile"
#define SORDI_DLL_DESCRIPTION_STRING "Mapfile Timer (test only)"
#define SORDI_RDI_PROCVEC Mapfile_AgentRDI
#include "perip_sordi.h"

#include "perip_rdi_agent.h"
    IMPLEMENT_AGENT_PROCS_NOEXE_NOMODULE(Mapfile)
    IMPLEMENT_AGENT_PROCVEC_NOEXE(Mapfile)

/*--- </> ---*/


/* EOF mapfile.c */













