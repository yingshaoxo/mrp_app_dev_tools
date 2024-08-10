/* flatmem.c - Fast ARMulator memory interface.
 * (formerly known as armflat.c.)
 * Copyright (C) Advanced RISC Machines Limited, 1995-1998. 
 * Copyright (C) 1998-2001  ARM Limited.
 * All rights reserved.
 *
 * RCS $Revision: 1.1.2.30 $
 * Checkin $Date: 2001/10/22 09:36:59 $
 * Revising $Author: ljameson $
 */

#include <string.h>             /* for memset */
#include <ctype.h>              /* for toupper */
#include "rdi.h"
#include "rdi_priv.h"
#include "rdi_prop.h"

#include "simplelinks.h"
#include "armul_callbackid.h"
#include "perip2rdi.h"         /* Perip2Memret */
#include "minperip.h"
#include "addcounter.h"
#include "armul_lib.h"         /* for ARMul_SIRange() */
#include "armul_agent.h"       /* for ARMulAgent_* */
#define ACDTEST(FUNC,RET) if (FUNC == RDIError_BufferFull) \
                               RET = RDIError_BufferFull

#include "armul_mem.h" /* for ARMul_Cycles ... */
#include "rdi_conf.h"
#include "armul_cnf.h"
#include "clxlist.h"
#include "armul_cyc.h"
#define NO_BYTELANES

/* NUL_BIU owns ABus now. */
/* #define FLAT_OWNS_ABUS */

/* debug options */
#ifndef NDEBUG
# if 1
#  define VERBOSE_NewLeafPageAllocated
# else
#  define VERBOSE_BUS
#  define VERBOSE_PERIPHERAL_ICYCLES
#  define VERBOSE_MEM_TYPE
#  define VERBOSE_CYCLE_DESC
# endif
#endif

#ifdef VERBOSE_DABORT
# define return_PERIP_DABORT \
    printf("\n*** Flatmem returns PERIP_DABORT ***\n"); \
    return_PERIP_DABORT
#else
# define return_PERIP_DABORT return PERIP_DABORT
#endif

#ifdef HOST_ENDIAN_BIG
# define HostEndian 1
#elif defined(HOST_ENDIAN_LITTLE)
# define HostEndian 0
#else
# error "HOST_ENDIAN_UNKNOWN"
#endif


#define NUMPAGES 64 * 1024
#define ARMFLAT_PAGESIZE 64 * 1024
#define PAGEBITS 16
#define OFFSETBITS_WORD 0xfffc

typedef struct {
  ARMword memory[ARMFLAT_PAGESIZE/4];
} mempage;

typedef struct {
  mempage *page[NUMPAGES];
} memory;

BEGIN_STATE_DECL(Flatmem)
  /* things that need to be accessed quickly at the start */
  unsigned read_bigend, write_bigend;
  unsigned int sa_memacc_flag;
  ARMul_Cycles cycles;
  ARMul_Cycles saved_cycles;
  ARMTime saved_bus_BusyUntil;
  ARMword LowestPeripheralAddress, HighestPeripheralAddress;


  ARMword byteAddrXor,hwordAddrXor; /* for reads */

#ifdef FLAT_OWNS_ABUS /*F*/
 /* Was ARMul_State->as_ABus */
  ARMul_Bus fm_ABus;
#endif
  ARMul_Bus *p_ABus;
  ARMul_MemInterface bus_mem;
  ARMul_MemInterfaceRef mem_ref;


  memory mem;                 /* big data structure in the middle */
                              /* things that are rarely accessed at the end */
  ARMul_MemType memtype;
  bool memory_byte_sex;         /* memory bytesex is fixed */
    bool BaseMemoryEnabled;
double clk;

    ARMTime clk_speed;

    struct GenericAccessCallback *NewPageAllocated;

    RDI_AgentHandle agent;
    bool bI_Cycles;
  char *mf_CycleNames[16];
END_STATE_DECL(Flatmem) /*  toplevel; */

#define toplevel FlatmemState

/*
 * ARMulator callbacks
 */
static unsigned ARMFlat_DebugEvents(void *handle, void *data)
{
  toplevel *top=(toplevel *)handle;  
  ARMul_Event *evt = (ARMul_Event *)data;
  if(evt->event == DebugEvent_RunCodeSequence)
  {
      /* save cycle counts */
      top->saved_bus_BusyUntil = top->p_ABus->bus_BusyUntil;
      top->saved_cycles = top->cycles;
  }
  if(evt->event == DebugEvent_EndCodeSequence)
  {
      /* restore cycle counts */
      top->p_ABus->bus_BusyUntil = top->saved_bus_BusyUntil;
      top->cycles = top->saved_cycles;
  }
    
  return FALSE;
}

static void ConfigChange(void *handle, unsigned bNewBigend)
{
  toplevel *top=(toplevel *)handle;
  /* bigendian swapping for reads is ALWAYS done by the memory model */
  top->read_bigend = bNewBigend; /* ((newValue & ARM_MMU_B) != 0); */

  top->byteAddrXor = ( top->read_bigend != HostEndian ) ? 3 : 0;
  top->hwordAddrXor = ( top->read_bigend != HostEndian ) ? 2 : 0;

#ifdef VERBOSE_CONFIGCHANGE
  printf("Flatmem:ConfigChange bNewBigend:%u\n",(unsigned)bNewBigend);
#endif


  /* If we have "fixed" bigend for the memory, then swapping for writes is
   * fixed, otherwise it is the same as for reads
   */
  if (top->memory_byte_sex == FALSE)
    top->write_bigend = top->read_bigend;
}

/* static void Interrupt(void *handle,unsigned int which) */
static unsigned Flatmem_ConfigEvents(void *handle, void *data)
{
  toplevel *top=(toplevel *)handle;
  ARMul_Event *evt = (ARMul_Event *)data;

  switch (evt->event)
  {
  case ConfigEvent_Reset:
    memset(&top->cycles,0,sizeof(top->cycles));
    top->p_ABus->bus_BusyUntil = 0;
    ARMul_BusLimits(top->p_ABus, &top->LowestPeripheralAddress,
                    &top->HighestPeripheralAddress, 1);

        break;
  case ConfigEvent_EndiannessChanged:
          ConfigChange(handle, evt->data1);
          break;
  default:
          return ARMFlat_DebugEvents(handle, data);
  }
  return FALSE;
}


static void ARMFlat_SetBaseMemoryEnable(toplevel *top, uint32 value)
{
    top->BaseMemoryEnabled = value;
}

#define GET_PRIMARY_ID(argx) (*(unsigned *)argx) 
#define GET_INDEXED_ID(argx,ndex) (((unsigned *)argx)[ndex])


static int UnkRDIInfo(void *handle, unsigned type, ARMword *arg1,ARMword *arg2)
{
  toplevel *top = (toplevel *)handle;
  int  retVal = RDIError_UnimplementedMessage;

  if (!top->BaseMemoryEnabled)
      return retVal;

  switch(type) {
  case RDICycles: 
  {
        void *state = NULL; /* ARMul_State, top->state; */
    ARMTime total;
    ARMul_AddCounterValue64(state, arg1, arg2, top->cycles.NumScycles);
    ARMul_AddCounterValue64(state, arg1, arg2, top->cycles.NumNcycles);
    ARMul_AddCounterValue64(state, arg1, arg2, top->cycles.NumIcycles);
    ARMul_AddCounterValue64(state, arg1, arg2, top->cycles.NumCcycles);
    total = (top->cycles.NumScycles + top->cycles.NumNcycles +
             top->cycles.NumIcycles + top->cycles.NumCcycles);
    ARMul_AddCounterValue64(state, arg1, arg2, total);
    break; /* Allow more */
  }
  case RDIRequestCyclesDesc:
  {
          void *state = NULL;

          if (top->memtype == ARMul_MemType_PipelinedARM9 ||
              top->memtype == ARMul_MemType_StrongARM ||
              top->memtype == ARMul_MemType_ARM9)
          {
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
#ifdef VERBOSE_CYCLE_DESC
              printf("ARMflat RDIRequestCyclesDesc Harvard.\n");
#endif
          } else {
              top->mf_CycleNames[1]="S_Cycles";
              top->mf_CycleNames[0]="N_Cycles";
              top->mf_CycleNames[2]="I_Cycles";
              top->mf_CycleNames[3]="C_Cycles";
              if(!top->bI_Cycles)
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
          ACDTEST(ARMul_AddCounterDesc(state, arg1, arg2, "Total"), retVal);
          break; /* Allow more */
  }

  case RDIProperty_SetAsNumeric:
  { /* Currently, only signals. */
      unsigned ID = (unsigned)arg1;
      switch (ID)
      {
      case RDIPropID_ARMSignal_BaseMemoryEnable:
          ARMFlat_SetBaseMemoryEnable(top,*arg2);
          ARMul_BusLimits(top->p_ABus, &top->LowestPeripheralAddress,
                          &top->HighestPeripheralAddress, 1);

          return RDIError_NoError;

      default: break;
      }
      break;
  }
  case RDIInfo_RegisterCallback:
  {
      unsigned ID = GET_PRIMARY_ID(arg1);
      switch(ID)
      {
      case ARMulCallbackID_NewLeafPageAllocated:
#ifdef VERBOSE_NewLeafPageAllocated
          printf("\nflatmem.c:RDIInfo_RegisterCallback:ARMulCallbackID_NewLeafPageAllocated\n");
#endif
          return ARMulif_InstallGenericAccessCallback(&top->NewPageAllocated,
                                              (GenericAccessCallback*)arg2);
      default: break;
      }
      break;

  }
  case RDIInfo_DeregisterCallback:
  {
      unsigned ID = GET_PRIMARY_ID(arg1);
      switch(ID)
      {
      case ARMulCallbackID_NewLeafPageAllocated:
          return ARMulif_RemoveGenericAccessCallback(&top->NewPageAllocated,
                                                     (void*)arg2);
      default: break;
      }
      break;
  }

  case RDIInfo_QueryMemInterface:
  {
      unsigned ID = GET_PRIMARY_ID(arg1);
      switch (ID) {
      case ARMulBusID_Memory: /* Leaf memory, usually flat */
          *(ARMul_MemInterface**)arg2 = top->mem_ref.mif;
          return RDIError_NoError;
      default:break;
      }
      break;
  }

#ifdef FLAT_OWNS_ABUS
  case RDIInfo_QueryBus:
  {
      void **pBus = (void**)arg2;
      char const *name = (char const *)arg1;
      if (name[0]==(char)0 || name[1]==(char)0) {
          /* Flatmem only uses single-char names only so far. */
          switch(*name) {
          case '\0':
          case 'A':
              *pBus = top->p_ABus;
              return RDIError_NoError;
          default:
              break;
          }
      }
  }
#endif

  default: break;
  }
  return retVal;
}


/*
 * Predeclare the memory access functions so that the initialisation function
 * can fill them in
 */
#ifdef PIPELINED
static void NextCycle(void *,ARMword,ARMul_acc);
static void NextDCycle(void *,ARMword,ARMul_acc);
static void NextICycle(void *,ARMword,ARMul_acc);
#endif

#ifdef OldCode
static armul_MemAccAsync MemAccessAsync;
#endif
static void MemAccessHarvard(void *,ARMword,ARMword *,ARMul_acc, int *,
                                    ARMword,ARMword *,ARMul_acc, int *);
static int MemAccess(void *,ARMword,ARMword *,ARMul_acc);
static int MemAccessCached(void *,ARMword,ARMword *,ARMul_acc);
static int MemAccessThumb(void *,ARMword,ARMword *,ARMul_acc);
/* This expects D,I call-pairs, and accounts like Harvard. */
static int MemAccessSA(void *,ARMword,ARMword *,ARMul_acc);
#ifndef NO_BYTELANES
static int MemAccessBL(void *,ARMword,ARMword *,ARMul_acc);
#endif
#ifdef MEM_ACCESS2_DISTINCT
static int MemAccess2(void *,ARMword,ARMword *,ARMword *,ARMul_acc);
#endif
static unsigned int DataCacheBusy(void *);

static const ARMul_Cycles *Flat_ReadCycles(void *handle);
static unsigned long Flat_GetCycleLength(void *handle);
static unsigned Flat_MemInfo(void *handle, unsigned type, ARMword *pID,
                        uint64 *data);
static GenericAccessFunc flat_newPage;


/* Needed for VC++6 */
static double u64_to_dbl(uint64 v)
{
    int64 v2 = (int64)v;
    return (double)v2;
}



static ARMTime Flat_ReadClock(void *handle)
{
  /* returns a us count */
  toplevel *top=(toplevel *)handle;
  double t=(u64_to_dbl(top->cycles.NumNcycles) +
            u64_to_dbl(top->cycles.NumScycles) +
            u64_to_dbl(top->cycles.NumIcycles) +
            u64_to_dbl(top->cycles.NumCcycles))*top->clk;
#if 0
  printf("Flat_ReadClock->%f (clk=%f)\n",t,top->clk);
#endif
  return (ARMTime)(int64)t;
}

/* armmmu requires log2(0) == 0 */
static int flatmem_log2(unsigned int value)
{
    unsigned short n = 0;
    while( (value >>= 1) != 0 )
    {
        n++;
    }
    return n;
}


static int flatmem_masterInfo(toplevel *top, unsigned type,
                              ARMword *address, uint64 *data)
{
    return top->mem_ref.master_info ?
        top->mem_ref.master_info(top->mem_ref.handle,
                                    type,address,data) :
        RDIError_UnimplementedMessage;
}


BEGIN_INIT(Flatmem)
{
    static uint32 ID[2] = {ARMulBusID_Bus,0};
    ARMul_MemInterface *interf = 
        ARMulif_QueryMemInterface(&state->coredesc, &ID[0]); /* "mif" */
    ARMul_MemType memtype;
    bool_int verbose = ToolConf_DLookupBool(config,(tag_t)"VERBOSE",FALSE);
  memory *mem;
  unsigned page;
  ARMword clk_speed = 0;
  const char *option;
  toplevel *top=(toplevel *)state;
  if (top == NULL) 
          return RDIError_OutOfStore;

  top->bI_Cycles =  ToolConf_DLookupBool(config,(tag_t)"ICycles",FALSE);
#ifdef FLAT_OWNS_ABUS /*F*/
  top->p_ABus = &top->fm_ABus;
#endif


  if (!interf)
  {
      static uint32 ID_CORE[2] = {ARMulBusID_Core,0};
      interf = 
          ARMulif_QueryMemInterface(&state->coredesc, &ID_CORE[0]);
#ifdef VERBOSE /* Not really a failure - we expect this for uncached cores. */
      Hostif_PrettyPrint(hostif,config,
                    "\n Flatmem connecting to CORE having failed to get BUS.\n");
#endif
  }
  if (!interf)
  {
      Hostif_PrettyPrint(hostif,config,
                         "\n Flatmem cannot get memory-interface.\n");
      return RDIError_UnableToInitialise;
  }
#ifdef VERBOSE_GETMIF
  Hostif_PrettyPrint(hostif,config, "Flatmem got memory-interface %p.\n",
                     interf);
#endif


#ifdef NotInADS11
    /* Ask the downstream memory for its bus. */
    {
        uint32 ID[2]; ID[0]=MemPropertyID_Bus; ID[1]=0;
        mif->mem_info(mif->handle,
                      ACCESS_READ|ACCESS_WORD,&ID[0],
                      (uint64*)&ts->pABus);
        if (!ts->pABus)
        {
            Hostif_PrettyPrint(ts->hostif,ts->config,
          "\n** Flatmem got no response to query for MemPropertyID_Bus **\n");
            ts->pABus = calloc(1,sizeof(ARMul_Bus));
            /* Later, construct such a bus? (*/
            return RDIError_UnableToInitialise;
        }
    }
#endif
    ARMulif_QueryBus(&state->coredesc,"A",&state->p_ABus);
    if (!state->p_ABus)
    {
        Hostif_PrettyPrint(hostif,config,"\n** Flatmem failed to get bus from "
                           "core model. **\n");
        return RDIError_UnableToInitialise;
    }



  /* Ask the host's built-in ARMFlat to disable its RDIInfo. */
  if (interf->mem_info)
  {
      uint64 data = FALSE;
      uint32 ID[2]; ID[0]=MemPropertyID_BaseMemoryEnable; ID[1]=0;      
      interf->mem_info(interf->handle,
                       ACCESS_WRITE|ACCESS_WORD,&ID[0],&data);
  }


  state->memtype = memtype = interf->memtype;
  interf->mem_info=Flat_MemInfo;
  interf->read_clock=Flat_ReadClock;
  interf->read_cycles=Flat_ReadCycles;
  interf->get_cycle_length=Flat_GetCycleLength;

#ifdef VERBOSE_MEM_TYPE
  printf("\nArmflat.c MemInit Type 0x%04x=%u\n",
         (unsigned)type,(unsigned)type);
#endif

  /* Fill in my functions */
  switch (memtype) {
#ifdef PIPELINED
  case ARMul_MemType_PipelinedBasic:
    interf->x.pipe.next=NextCycle;
    interf->x.pipe.access=MemAccessCached;
    interf->x.pipe.delta_cycles = NULL;
    break;
  case ARMul_MemType_PipelinedAMBA:
    interf->x.pipe.next=NextCycle;
    interf->x.pipe.access=MemAccessCached;
    interf->x.pipe.delta_cycles = NULL;
    break;
  case ARMul_MemType_PipelinedARM9:
    interf->x.pipe_arm9.dside_next = NextDCycle;
    interf->x.pipe_arm9.iside_next = NextICycle;
    interf->x.pipe_arm9.harvard_access = MemAccessHarvard;
    interf->x.pipe_arm9.debug_access = MemAccess;
    break;
#endif
  case ARMul_MemType_Basic:
  case ARMul_MemType_16Bit:
    interf->x.basic.access=MemAccess;
    state->p_ABus->bus_Type = BT_Endian;
    break;
  case ARMul_MemType_Thumb:
    interf->x.basic.access=MemAccessThumb;
    state->p_ABus->bus_Type = BT_Endian;
    break;
  case ARMul_MemType_AHB:
  case ARMul_MemType_16BitCached:
  case ARMul_MemType_ThumbCached:
  case ARMul_MemType_ARMissAHB:
    interf->x.basic.access=MemAccessCached;
    state->p_ABus->bus_Type = BT_Endian;
    break;
  case ARMul_MemType_ARM8:
    interf->x.arm8.access=MemAccess;
#ifdef MEM_ACCESS2_DISTINCT
    interf->x.arm8.access2=MemAccess2;
#else
    interf->x.arm8.access2=MemAccess;
#endif
    state->p_ABus->bus_Type = BT_Endian;
    Hostif_PrettyPrint(hostif,config,", double-bandwidth");
    break;
  case ARMul_MemType_StrongARM:
    interf->x.strongarm.access=MemAccessSA;
    interf->x.strongarm.data_cache_busy=DataCacheBusy;
    state->p_ABus->bus_Type = BT_Endian | BT_Harvard;
    Hostif_PrettyPrint(hostif,config,", StrongARM dual-ported");
    break;
#ifndef NO_BYTELANES
  case ARMul_MemType_ByteLanes:
    interf->x.basic.access=MemAccessBL;
    Hostif_PrettyPrint(hostif,config,", byte-laned");
    break;
#endif
  case ARMul_MemType_ARM9:

    interf->x.arm9.access = MemAccess; /* used by debugger */
    interf->x.arm9.harvard_access = MemAccessHarvard;
    state->p_ABus->bus_Type = BT_Endian | BT_Harvard;
    break;
#ifdef OldCode
  case ARMul_MemType_ARMissAHB:
      Hostif_PrettyPrint(hostif,config,", AHB");
      interf->x.armiss_ahb.debug_access=MemAccess;
      interf->x.armiss_ahb.ahb_access=MemAccessAsync;
      state->p_ABus->bus_Type = BT_Endian;
      break;
#endif
  case ARMul_MemType_ARMissCache: /* NOT HANDLED */
  default:
      Hostif_PrettyPrint(hostif,config,"\n Cannot handle memory-type 0x%x.\n",
                         memtype);
      return RDIError_UnableToInitialise;
  }
  /* If we were inserting ourselves into the chain, we'd use
   * ARMul_SwapMasterInfo (called by ARMul_InsertMemInterface2).
   */
  top->mem_ref.handle = interf->mem_link->handle;
  top->mem_ref.master_info = interf->mem_link->master_info;

  top->mem_ref.mif = interf;
  interf->mem_link = &top->mem_ref;



   { uint32 busClockDivisor = 
         ToolConf_DLookupUInt(config, ARMulCnf_MCCfg, 0);
   if (0 == busClockDivisor)
   {
       assert(0);
       busClockDivisor = 1;
   }
   ARMul_Bus_Initialise(top->p_ABus,
                        busClockDivisor,
                        ARMul_BusAccess, /* bus_decoderFunc */
                        state, /* bus_DecoderHandle */
                        ARMul_BusRegisterPerip, /* bus_registerPeripFunc */
                        NULL, /* bus_RegisterPeripHandle */
                        NULL
                        );
   }
   top->p_ABus->bus_LogWidth = 
       flatmem_log2(ToolConf_DLookupUInt(config, (tag_t)"BUS_WIDTH", 32));

#ifdef VERBOSE_BUS
   printf("After calling ARMul_Bus_Initialise : LogWidth=%u\n.",
          (unsigned)top->p_ABus->bus_LogWidth);
#endif

  mem=&top->mem;
  top->BaseMemoryEnabled = TRUE;
  option=ToolConf_Lookup(config,ARMulCnf_MCLK);
  if (option != NULL) clk_speed = ToolConf_Power(option,FALSE);
  if (option == NULL || clk_speed == 0) {
      /* We cannot really tell now, but assume that we have a cached core. */
      char const *opt1 = ToolConf_Lookup(config,(tag_t)"FCLK");
      uint32 cpuspeed = opt1 ? ToolConf_Power(opt1,FALSE) : 0;
      uint32 mccfg = ToolConf_DLookupUInt(config,(tag_t)"MCCFG",0);

      if (!cpuspeed)
      {
          option=ToolConf_Lookup(config,(tag_t)"CPUSPEED");
          cpuspeed = option ? ToolConf_Power(option,FALSE) : 0;
      }
      if (!cpuspeed)
      {
          option=ToolConf_Lookup(config,(tag_t)"DEFAULT_CPUSPEED");
          cpuspeed = option ? ToolConf_Power(option,FALSE) : 0;
      }
      clk_speed = (mccfg != 0) ? cpuspeed / mccfg : 0;

      Hostif_PrettyPrint(hostif,config,
                         "\n*** Core failed to define MCLK - guessing %lu.\n",
                         (unsigned long)clk_speed);
  }
  if (clk_speed == 0) {
      top->clk=0.0;
  } else {
      top->clk=1000000.0/clk_speed; /* in microseconds */
  }

  top->clk_speed = clk_speed;

  /* only report the speed if "CPU Speed" has been set in the config */
 
  if (ARMul_ClockIsEmulated(config)) {
    char *fac;
    double clk=ARMul_SIRange(clk_speed,&fac,FALSE);
    if (clk)  /* Don't print this if it is zero */
        Hostif_PrettyPrint(hostif,config,", %.1f%sHz",clk,fac);
  }


  for (page=0; page<NUMPAGES; page++) {
    mem->page[page]=NULL;
  }

  Hostif_PrettyPrint(hostif,config, ", 4GB");

  top->cycles.NumNcycles=0;
  top->cycles.NumScycles=0;
  top->cycles.NumIcycles=0;
  top->cycles.NumCcycles=0;
  top->cycles.NumFcycles=0;

  assert(top->memtype == memtype);

  option = ToolConf_Lookup(config, ARMulCnf_MemoryByteSex);
  if (option == NULL) {
    top->memory_byte_sex = FALSE; /* follow bigendSig on core */
  } else switch (safe_toupper(option[0])) {
  case 'B':
    top->memory_byte_sex = TRUE;
    top->write_bigend = TRUE;   /* writes always bigendian */
    break;
  case 'L':
    top->memory_byte_sex = TRUE;
    top->write_bigend = FALSE;  /* writes always littleendian */
    break;
  default:
    top->memory_byte_sex = FALSE; /* follow bigendSig on core */
    break;
  }

  assert(top == state);

  if (!(type & RDIOpen_ByteSexMask))
  { /* We are being told, rather than asked */
      ConfigChange(state, !!(type & 4));
  }



  /* Also handles ConfigChange and DebugEvents!...*/
  ARMulif_InstallEventHandler(&state->coredesc,
                              (ConfigEventSel | DebugEventSel),
                              Flatmem_ConfigEvents, state);
  ARMulif_InstallUnkRDIInfoHandler(&state->coredesc,UnkRDIInfo,top);

  interf->handle=top;

  top->HighestPeripheralAddress = ~(ARMword)0;


  top->NewPageAllocated = calloc(1,sizeof(GenericAccessCallback));
  top->NewPageAllocated->func = flat_newPage;
  top->NewPageAllocated->handle = top;

#ifdef VERBOSE_INIT
   printf("END_INIT(Flatmem)\n.");
#endif
   /* Tell the Core we provide a Bus.
    * This shall allow it to know when we need to be called.
    * Flatmem does this before loading its children, so they can
    * access this bus.
    */
   { static ARMword ID[2] = {BMPropertyID_NewBusConnected,0};
   void *a_pointer = top->p_ABus;
   flatmem_masterInfo(top,ACCESS_WRITE,ID,(uint64*)&a_pointer);
   }

   assert(dbg_state != NULL);
   assert(((RDI_DbgStateForSubAgent *)dbg_state)->size == sizeof(RDI_DbgStateForSubAgent));
   assert(((RDI_DbgStateForSubAgent *)dbg_state)->lib_handle != NULL);

   ARMulAgent_Create(&state->agent, hostif, config,
                     dbg_state,
                     NULL, /*dll_name*/
                     NULL /*ARMul_RDIProcs*/);
   if (verbose)
   {
       Hostif_PrettyPrint(hostif,config,"\nFLAT LOADS PERIPHERALS...\n");
   }

   {
       toolconf child = ToolConf_Child(config,(tag_t)"PERIPHERALS");

       ARMul_Agent *agent = (ARMul_Agent *)state->agent;
       agent->module_desc = *dsa->agent_desc; /* Core */
       agent->subagent_desc = *dsa->parent_desc; /* Transparent Agent */
       agent->init_type = type; /* ? Can I add this to the constructor? */
       if (child)
       {
           int err = ToolConf_IterateTags(child, ARMulAgent_AddPeripheral,
                                          state->agent);
           if (err != RDIError_NoError)
           {
#ifdef VERBOSE_INIT
               printf("\nARMul_AddComponent returned err=%i.\n", err);
               /* ToolConf_WriteTo(tconf,stderr); */
               /*fprintf(stderr,"\n>>>>\n");*/
#endif
               Hostif_PrettyPrint(hostif,config,
                  "\n A child of Flatmem failed to initialize, error %i.\n",
                         err);
               return(RDIError_UnableToInitialise);
           }
           if (verbose)
           {
               Hostif_PrettyPrint(hostif,config,"\nFLAT LOADED\n");
           }
       }
       else
       {
           if (verbose)
           {
               Hostif_PrettyPrint(hostif,config,"\nFLAT HAS NO PERIPHERALS\n");
           }
       }
   }

#ifdef VERBOSE_INIT
   printf("END_INIT(Flatmem)\n.");
#endif
}
END_INIT(Flatmem)





/* --- BEGIN PERIPHERAL-ACCESS --- */

#include "rdi_err.h"

static int peripheral_access(void *handle,
                             ARMword address,
                             ARMword *data,
                             ARMul_acc acc)
{

    toplevel *ts=(toplevel *)handle;
  /* See if any peripheral registered on ABus, 
   * unless we have an I- or C-cycle. */

#ifdef VERBOSE_PERIPHERAL_ICYCLES
    if (ACCESS_IS_IDLE(acc))
        printf("armflat.c:peripheral_access(A:%08lx T:%08lx)\n",
               (unsigned long)address, (unsigned long)acc);
#endif


#ifdef NO_IDLES_TO_PERIPHERALS
   if (!(acc & acc_Nmreq))
#endif
   {
       ARMul_Bus *bus = ts->p_ABus;

       ARMul_AccessRequest req;
       int err;
       memset(&req,0,sizeof(req));
       req.req_access_type = acc;
       req.req_addr_size = 1;
       req.req_address[0] = address;
       req.req_data = data;
       err = bus->bus_decoderFunc(bus, &req);
       return Perip2Memret(err);
   }
   return -2; /* Pass Idles back to memory. */
}

/* --- END PERIPHERAL-ACCESS --- */






static unsigned flat_newPage(struct GenericAccessCallback *myCB,
                             ARMword address,
                             ARMword*data, unsigned access_type)
{
    toplevel *top=(toplevel *)myCB->handle;
    unsigned i;
    unsigned nw = access_type / sizeof(ARMword);

    assert(access_type == ARMFLAT_PAGESIZE);
    (void)access_type; (void)address; (void)top;

  /*
   * We fill the new page with data that, if the ARM tried to execute
   * it, it will cause an undefined instruction trap (whether ARM or
   * Thumb)
   */
  for (i=0;i<nw;) {
    data[i++]=0xe7ff0010; /* an ARM undefined instruction */
    /* The ARM undefined insruction has been chosen such that the
     * first halfword is an innocuous Thumb instruction (B 0x2)
     */
    data[i++]=0xe800e800; /* a Thumb undefined instruction */
  }
   /* We are the last in the chain, registered first. */
  assert(myCB->next == NULL);

  return 0;
}


static mempage *NewPage(void *handle,ARMword address)
{
  toplevel *top=(toplevel *)handle;
  mempage *page=(mempage *)malloc(sizeof(mempage));
  if ( page == NULL)
  {
    /* The malloc failed. we have to do a number of things.
        1. call ARMulRaiseError
        2. Call ARMul_ModelBroken
        3. Return the zero page so that emulation continues
        long enough for the halt to take effect
     */
      Hostif_DebugPrint(top->hostif, "OUT OF MEMORY\n");
      ARMulif_StopExecution(&top->coredesc, RDIError_OutOfStore);
      return top->mem.page[0];
  }

  top->NewPageAllocated->func(top->NewPageAllocated,address,
                             page->memory,ARMFLAT_PAGESIZE);


  return page;
}


/*
 * Generic memory interface.
 */

/*
 * This is the most basic memory access function - an ARM6/ARM7 interface.
 */
static int MemAccess(void *handle,
                     ARMword address,
                     ARMword *data,
                     ARMul_acc acc)
{
  toplevel *top=(toplevel *)handle;
  unsigned int pageno;
  mempage *page;
  ARMword *ptr;
  ARMword offset;
  int rv;

  if (acc_ACCOUNT(acc)) {
      COUNTCYCLES(top->cycles,acc);
      /* top->p_ABus->bus_BusyUntil++; */
  }

  if ((address >= top->LowestPeripheralAddress) &&
      (address <= top->HighestPeripheralAddress) &&
      (rv = peripheral_access(handle,address,data,acc)) != -2)
  {
      return rv;
  }

  pageno=address>>PAGEBITS;
  page=top->mem.page[pageno];

  if (page==NULL) {
      top->mem.page[pageno]=page=NewPage(top,address);
  }
  offset = address & OFFSETBITS_WORD;
  ptr=(ARMword *)((char *)(page->memory)+offset);


  if (acc==acc_LoadInstrS) {
    *data=*ptr;
    return 1;
  } else if (acc_MREQ(acc)) {
    if (acc_READ(acc)) {
      switch (acc & WIDTH_MASK) {
      case BITS_8:              /* read byte */
        if (HostEndian!=top->read_bigend) address^=3;
        *data = ((unsigned8 *)ptr)[address & 3];
        break;
        
      case BITS_16: {           /* read half-word */
        /* extract half-word */
#ifndef HOST_HAS_NO_16BIT_TYPE
        /*
         * unsigned16 is always a 16-bit type, but if there is no native
         * 16-bit type (e.g. ARM!) then we can do something a bit more
         * cunning.
         */
        if (HostEndian!=top->read_bigend) address^=2;
        *data = *((unsigned16 *)(((char *)ptr)+(address & 2)));
#else
        unsigned32 datum;
        datum=*ptr;
        if (HostEndian!=top->read_bigend) address^=2;
        if (address & 2) datum<<=16;
        *data = (datum>>16);
#endif
      }
        break;
        
      case BITS_32:             /* read word */
        *data=*ptr;
        break;

      case BITS_64:             /* read dword */
        *data=*ptr;
        if ((offset+4) & OFFSETBITS_WORD) 
        {
            data[1]=ptr[1];
            return 2;
        }
        break;

      default:
          return_PERIP_DABORT;
      }
    } else {
      switch (acc & WIDTH_MASK) {
        /* extract byte */
      case BITS_8:              /* write_byte */
        if (HostEndian!=top->write_bigend) address^=3;
        ((unsigned8 *)ptr)[address & 3]=(unsigned8)(*data);
        break;
        
      case BITS_16:             /* write half-word */
        if (HostEndian!=top->write_bigend) address^=2;
        *((unsigned16 *)(((char *)ptr)+(address & 2))) = (unsigned16)(*data);
        break;

      case BITS_32:             /* write word */
        *ptr=*data;
        break;

      case BITS_64:             /* write dword */
        *ptr=*data;
        if ((offset+4) &OFFSETBITS_WORD) 
        {
            ptr[1]=data[1];
            return 2;
        }
        break;

      default:
          return_PERIP_DABORT;
      }
    }                           /* internal cycle */
  }

  return 1;
}



#ifdef PIPELINED
/*
 * This function gets called before every memory access with the address and
 * cycle type for the next cycle.
 */
static void NextCycle(void *handle,
                     ARMword address,
                     ARMul_acc acc)
{
  /* armflat does not make use of pipelined addresses */
  UNUSEDARG(handle); UNUSEDARG(address); UNUSEDARG(acc);
}
#endif /* PIPELINED */


#if 1
# define CHECKTOTAL
#else
      /* ... This only works if we are called for every cycle ... */
# define CHECKTOTAL \
      assert(top->p_ABus->bus_BusyUntil == \
             (top->cycles.NumNcycles + top->cycles.NumScycles +\
              top->cycles.NumIcycles+top->cycles.NumCcycles));
#endif

/*
 * This is the most basic memory access function - an ARM610/ARM710 interface.
 *
 * Optimised for word loads and idle cycles
 */
static int MemAccessCached(void *handle,
                           ARMword address,
                           ARMword *data,
                           ARMul_acc acc)
{
  toplevel *top=(toplevel *)handle;
  unsigned int pageno;
  mempage *page;
  ARMword *ptr;
  ARMword offset;
  int rv;

  if (acc_ACCOUNT(acc)) {
      COUNTCYCLES(top->cycles,acc);
      /* top->p_ABus->bus_BusyUntil++; */
      CHECKTOTAL
  }

  if ((address >= top->LowestPeripheralAddress) &&
      (address <= top->HighestPeripheralAddress) &&
      (rv = peripheral_access(handle,address,data,acc)) != -2)
  {
      return rv;
  }


  if (ACCESS_IS_IDLE(acc))
      return 1;

  pageno=address>>PAGEBITS;
  page=top->mem.page[pageno];

  /* !TODO: if (page==dummyPage && ACCESS_IS_WRITE)... */
  if (page==NULL) 
  {
    top->mem.page[pageno]=page=NewPage(top,address);
  }
  offset = address & OFFSETBITS_WORD;
  ptr=(ARMword *)((char *)(page->memory)+offset);

  if (acc==acc_LoadWordS) {
    *data=*ptr;
    return 1;
  } else if (acc_MREQ(acc)) {
    if (acc_READ(acc)) {
      switch (acc & WIDTH_MASK) {
      case BITS_8:              /* read byte */
        if (HostEndian!=top->read_bigend) address^=3;
        *data = ((unsigned8 *)ptr)[address & 3];
        break;
        
      case BITS_16: {           /* read half-word */
        /* extract half-word */
#ifndef HOST_HAS_NO_16BIT_TYPE
        /*
         * unsigned16 is always a 16-bit type, but if there is no native
         * 16-bit type (e.g. ARM!) then we can do something a bit more
         * cunning.
         */
        if (HostEndian!=top->read_bigend) address^=2;
        *data = *((unsigned16 *)(((char *)ptr)+(address & 2)));
#else
        unsigned32 datum;
        datum=*ptr;
        if (HostEndian!=top->read_bigend) address^=2;
        if (address & 2) datum<<=16;
        *data = (datum>>16);
#endif
      }
        break;
        
      case BITS_32:             /* read word */
        *data=*ptr;
        break;

      case BITS_64:             /* read dword */
        *data=*ptr;
        if ((offset+4) & OFFSETBITS_WORD) 
        {
            data[1]=ptr[1];
            return 2;
        }
        break;

      default:
          return_PERIP_DABORT;
      }
    } else {
      switch (acc & WIDTH_MASK) {
        /* extract byte */
      case BITS_8:              /* write_byte */
        if (HostEndian!=top->write_bigend) address^=3;
        ((unsigned8 *)ptr)[address & 3]=(unsigned8)(*data);
        break;
        
      case BITS_16:             /* write half-word */
        if (HostEndian!=top->write_bigend) address^=2;
        *((unsigned16 *)(((char *)ptr)+(address & 2))) = (unsigned16)(*data);
        break;

      case BITS_32:             /* write word */
        *ptr=*data;
        break;


      case BITS_64:             /* write dword */
        *ptr=*data;
        if ((offset+4) &OFFSETBITS_WORD) 
        {
            ptr[1]=data[1];
            return 2;
        }
        break;

      default:
          return_PERIP_DABORT;
      }
    }                           /* internal cycle */
  }

  return 1;
}

/*
 * Same function, but optimised for Thumb accesses
 */
static int MemAccessThumb(void *handle,
                          ARMword address,
                          ARMword *data,
                          ARMul_acc acc)
{
  toplevel *top=(toplevel *)handle;
  unsigned int pageno;
  mempage *page;
  ARMword *ptr;
  ARMword offset;
  int rv;


  if (acc_ACCOUNT(acc)) {
      COUNTCYCLES(top->cycles,acc);
      /* top->p_ABus->bus_BusyUntil++; */
      CHECKTOTAL
  }

  if ((address >= top->LowestPeripheralAddress) &&
      (address <= top->HighestPeripheralAddress) &&
      (rv = peripheral_access(handle,address,data,acc)) != -2)
  {
      return rv;
  }

  pageno=address>>PAGEBITS;
  page=top->mem.page[pageno];

  if (page==NULL) {
    top->mem.page[pageno]=page=NewPage(top,address);
  }
  offset = address & OFFSETBITS_WORD;
  ptr=(ARMword *)((char *)(page->memory)+offset);
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

        /* This case should not be needed for MemAccessThumb,
        * but I intend to merge that with MemAccessCached. */
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
        break;

      case BITS_64  | ACCESS_WRITE:             /* write dword */
        *ptr=*data;
        if ((offset+4) &OFFSETBITS_WORD) 
        {
            ptr[1]=data[1];
            return 2; /* PERIP_OK2 */
        }
        break;

      case BITS_8 | ACCESS_IDLE:
      case BITS_16 | ACCESS_IDLE:
      case BITS_32 | ACCESS_IDLE:
      case BITS_64 | ACCESS_IDLE:
      case ACCESS_IDLE:
        return 1;
        
      default:
          return_PERIP_DABORT;
    }
  return 1;
}

#ifdef MEM_ACCESS2_DISTINCT /* F */
/*
 * This function is used by ARM8. Effectively we model a memory
 * system which can return two words per cycle.
 * The differences between this an a normal 64-bit bus (ARM10, XScale)
 * are
 * (1) 64-bit fetches need not be DWORD aligned.
 * (2) A non-aligned fetch just below a 64k-boundary may return just one word!
 */

static int MemAccess2(void *handle,
                      ARMword address,
                      ARMword *data,ARMword *data2,
                      ARMul_acc acc)
{
  toplevel *top=(toplevel *)handle;
  unsigned int pageno;
  mempage *page;
  ARMword *ptr;
  ARMword offset;
  int words=1;
  int rv;

  if (acc_ACCOUNT(acc)) {
      COUNTCYCLES(top->cycles,acc);
      /* top->p_ABus->bus_BusyUntil++; */
      CHECKTOTAL
  }

  if ((address >= top->LowestPeripheralAddress) &&
      (address <= top->HighestPeripheralAddress) &&
      (rv = peripheral_access(handle,address,data,acc)) != -2)
  {
      return rv;
  }

  pageno=address>>PAGEBITS;
  page=top->mem.page[pageno];

  if (page==NULL) {
      top->mem.page[pageno]=page=NewPage(top,address);
  }
  offset = address & OFFSETBITS_WORD;
  ptr=(ARMword *)((char *)(page->memory)+offset);

  if (acc_MREQ(acc)) {
    if (acc_READ(acc)) {
      switch (acc & WIDTH_MASK) {
      case BITS_8:              /* read byte */
        if (HostEndian!=top->read_bigend) address^=3;
        *data = ((unsigned8 *)ptr)[address & 3];
        break;
        
      case BITS_16: {           /* read half-word */
        /* extract half-word */
        if (HostEndian!=top->read_bigend) address^=2;
        *data = *((unsigned16 *)(((char *)ptr)+(address & 2)));
      }
        break;
        
      case BITS_32:             /* read word */
        *data=*ptr;
        break;

      case BITS_64:             /* read two words */
        *data=*ptr;
        if ((offset+4) & OFFSETBITS_WORD) { *data2=ptr[1]; words=2; }
        break;

      default:
          return_PERIP_DABORT;
      }
    } else {
      switch (acc & WIDTH_MASK) {
        /* extract byte */
      case BITS_8:              /* write_byte */
        if (HostEndian!=top->write_bigend) address^=3;
        ((unsigned8 *)ptr)[address & 3]=(unsigned8)(*data);
        break;
        
      case BITS_16:             /* write half-word */
        if (HostEndian!=top->write_bigend) address^=2;
        *((unsigned16 *)(((char *)ptr)+(address & 2)))=(unsigned16)(*data);
        break;

      case BITS_32:             /* write word */
        *ptr=*data;
        break;

      default:
          return_PERIP_DABORT;
      }
    }                           /* internal cycle */
  }

  return words;
}
#endif


/*
 * A memory model used by StrongARM. This only accounts
 * for cycles on opcode boundaries - i.e. we allow both a data
 * and an instruction fetch on one cycle.
 */

static int MemAccessSA(void *handle,
                       ARMword address,
                       ARMword *data,
                       ARMul_acc acc)
{
  toplevel *top=(toplevel *)handle;
  unsigned int pageno;
  mempage *page;
  ARMword *ptr;
  ARMword offset;
  int rv;

  /*
   * On StrongARM there are four types of cycle - we'll reuse
   * the four cycle counters for these:
   *
   *  Instruction fetched, No data fetched      N
   *  Instruction fetched, data fetched         S
   *  No instruction fetched, No data fetched   I
   *  No instruction fetched, data fetched      C
   */

  if (acc_ACCOUNT(acc)) {
    if (acc_OPC(acc)) {
      /* End of cycle - account for access */
      /* This access is either acc_LoadInstrN or acc_NoFetch */
      if (top->sa_memacc_flag) {
        /* data fetched */
        top->sa_memacc_flag=0;
        if (acc_MREQ(acc)) top->cycles.NumScycles++;
        else top->cycles.NumCcycles++;
      } else {
        /* no data fetched */
        if (acc_MREQ(acc)) top->cycles.NumNcycles++;
        else top->cycles.NumIcycles++;
      }
    } else if (acc_MREQ(acc)) { /* should be */
      top->sa_memacc_flag=1;    /* flag */
    }
  }

  if ((address >= top->LowestPeripheralAddress) &&
      (address <= top->HighestPeripheralAddress) &&
      (rv = peripheral_access(handle,address,data,acc)) != -2)
  {
      return rv;
  }

  pageno=address>>PAGEBITS;
  page=top->mem.page[pageno];

  if (page==NULL) {
    top->mem.page[pageno]=page=NewPage(top,address);
  }
  offset = address & OFFSETBITS_WORD;
  ptr=(ARMword *)((char *)(page->memory)+offset);

  if (acc_MREQ(acc)) {
    if (acc_READ(acc)) {
      switch (acc & WIDTH_MASK) {
      case BITS_8:              /* read byte */
        if (HostEndian!=top->read_bigend) address^=3;
        *data = ((unsigned8 *)ptr)[address & 3];
        break;
        
      case BITS_16: {           /* read half-word */
        /* extract half-word */
#ifndef HOST_HAS_NO_16BIT_TYPE
        /*
         * unsigned16 is always a 16-bit type, but if there is no native
         * 16-bit type (e.g. ARM!) then we can do something a bit more
         * cunning.
         */
        if (HostEndian!=top->read_bigend) address^=2;
        *data = *((unsigned16 *)(((char *)ptr)+(address & 2)));
#else
        unsigned32 datum;
        datum=*ptr;
        if (HostEndian!=top->read_bigend) address^=2;
        if (address & 2) datum<<=16;
        *data = (datum>>16);
#endif
      }
        break;
        
      case BITS_32:             /* read word */
        *data=*ptr;
        break;

      default:
          return_PERIP_DABORT;
      }
    } else {
      switch (acc & WIDTH_MASK) {
        /* extract byte */
      case BITS_8:              /* write_byte */
        if (HostEndian!=top->write_bigend) address^=3;
        ((unsigned8 *)ptr)[address & 3]=(unsigned8)(*data);
        break;
        
      case BITS_16:             /* write half-word */
        if (HostEndian!=top->write_bigend) address^=2;
        *((unsigned16 *)(((char *)ptr)+(address & 2)))=(unsigned16)(*data);
        break;

      case BITS_32:             /* write word */
        *ptr=*data;
        break;

      default:
          return_PERIP_DABORT;
      }
    }                           /* internal cycle */
  }

  return 1;
}

/*
 * Memory access function that supports byte lanes
 */

#ifndef NO_BYTELANES
static int MemAccessBL(void *handle,
                       ARMword address,
                       ARMword *data,
                       ARMul_acc acc)
{
  toplevel *top=(toplevel *)handle;
  unsigned int pageno;
  mempage *page;
  ARMword *ptr;
  ARMword offset;

  if (acc_ACCOUNT(acc)) {
    if (acc_SEQ(acc)) {
      if (acc_MREQ(acc)) top->cycles.NumScycles++;
      else top->cycles.NumCcycles++;
    } else if (acc_MREQ(acc)) top->cycles.NumNcycles++;
    else top->cycles.NumIcycles++;
  }

  pageno=address>>PAGEBITS;
  page=top->mem.page[pageno];

  if (page==NULL) {
    top->mem.page[pageno]=page=NewPage(top,address);
  }
  offset = address & OFFSETBITS_WORD;
  ptr=(ARMword *)((char *)(page->memory)+offset);

  if (acc_MREQ(acc)) {
    if (acc_BYTELANE(acc)==BYTELANE_MASK) { /* word */
      if (acc_READ(acc)) *data = *ptr;
      else *ptr = *data;
    } else {
      unsigned32 mask;
      static const unsigned32 masks[] = {
        0x00000000, 0x000000ff,
        0x0000ff00, 0x0000ffff,
        0x00ff0000, 0x00ff00ff,
        0x00ffff00, 0x00ffffff,
        0xff000000, 0xff0000ff,
        0xff00ff00, 0xff00ffff,
        0xffff0000, 0xffff00ff,
        0xffffff00, 0xffffffff,
      };
      mask=masks[acc_BYTELANE(acc)];
      if (acc_READ(acc)) *data = *ptr & mask;
      else *ptr = (*ptr & ~mask) | (*data & mask);
    }                           /* internal cycle */
  }

  return 1;
}
#endif

static void MemAccessHarvard(void *handle,
                       ARMword daddr,ARMword *ddata, ARMul_acc dacc, int *drv,
                       ARMword iaddr,ARMword *idata, ARMul_acc iacc, int *irv)
{

    toplevel *top=(toplevel *)handle;
    unsigned int pageno;
    mempage *page;
    ARMword *ptr;
    ARMword offset;

    /* This call doesn't happen for debug accesses, so no need
     * to check acc_ACCOUNT. */
    /* top->p_ABus->bus_BusyUntil++; */
    
    /* Update cycle counters - use the same scheme */
    /* as the StrongARM model (see MemAccessSA()). */
    
    if (acc_MREQ(iacc) && acc_MREQ(dacc)) {
        top->cycles.NumScycles++;
    }
    else if (acc_MREQ(iacc) && acc_nMREQ(dacc)) {
        top->cycles.NumNcycles++;
    }
    else if (acc_nMREQ(iacc) && acc_MREQ(dacc)) {
        top->cycles.NumCcycles++;
    }
    else {
        top->cycles.NumIcycles++;  
    }
    
    /* Deal with Instruction side first */
    
    if (acc_MREQ(iacc)) {
        
        int rv;
        
        if ((iaddr >= top->LowestPeripheralAddress) &&
            (iaddr <= top->HighestPeripheralAddress) &&
            (rv = peripheral_access(handle,iaddr,idata,iacc)) != -2)
        {
            *irv = rv; goto idone;
        }
        
        pageno=iaddr>>PAGEBITS;
        page=top->mem.page[pageno];
        
        if (page==NULL) {
            top->mem.page[pageno]=page=NewPage(top,iaddr);
        }
        offset = iaddr & OFFSETBITS_WORD;
        ptr=(ARMword *)((char *)(page->memory)+offset);
        
        switch (iacc & WIDTH_MASK) {
        case BITS_8: {
            unsigned32 datum = *ptr;
            if (HostEndian!=top->read_bigend) iaddr^=3;
            datum<<=(3 *(iaddr & 3));
            *idata = datum & 0xFF;
        }
        case BITS_16: {           /* read half-word */
            /* extract half-word */
#ifndef HOST_HAS_NO_16BIT_TYPE
            /*
             * unsigned16 is always a 16-bit type, but if there is no native
             * 16-bit type (e.g. ARM!) then we can do something a bit more
             * cunning.
             */
            if (HostEndian!=top->read_bigend) iaddr^=2;
            *idata = *((unsigned16 *)(((char *)ptr)+(iaddr & 2)));
#else
            unsigned32 datum = *ptr;
            if (HostEndian!=top->read_bigend) iaddr^=2;
            if (iaddr & 2) datum<<=16;
            *idata = (datum>>16);
#endif
        }
        break;
        
        case BITS_32:             /* read word */
            *idata=*ptr;
            break;
            
        case BITS_64:             /* read 2 words */
            *idata=*ptr;
            idata[1]=ptr[1];
            break;
            
        }
    }
    *irv = 1;

idone:
  /* Now deal with data side */

  if (acc_MREQ(dacc) /*dacc != acc_Icycle*/) {

      int rv;
      if ((daddr >= top->LowestPeripheralAddress) &&
          (daddr <= top->HighestPeripheralAddress) &&
          (rv = peripheral_access(handle,daddr,ddata,dacc)) != -2)
      {
          *drv = rv; goto ddone;
      }

    pageno=daddr>>PAGEBITS;
    page=top->mem.page[pageno];

    if (page==NULL) {
      top->mem.page[pageno]=page=NewPage(top,daddr);
    }
    offset = daddr & OFFSETBITS_WORD;
    ptr=(ARMword *)((char *)(page->memory)+offset);


    if (acc_READ(dacc)) {
          switch (dacc & WIDTH_MASK) {
          case BITS_8:              /* read byte */
            if (HostEndian!=top->read_bigend) daddr^=3;
            *ddata = ((unsigned8 *)ptr)[daddr & 3];
            break;
        
          case BITS_16: {           /* read half-word */
            /* extract half-word */
#ifndef HOST_HAS_NO_16BIT_TYPE
            /*
             * unsigned16 is always a 16-bit type, but if there is no native
             * 16-bit type (e.g. ARM!) then we can do something a bit more
             * cunning.
             */
            if (HostEndian!=top->read_bigend) daddr^=2;
            *ddata = *((unsigned16 *)(((char *)ptr)+(daddr & 2)));
#else
            unsigned32 datum;
            datum=*ptr;
            if (HostEndian!=top->read_bigend) daddr^=2;
            if (daddr & 2) datum<<=16;
            *ddata = (datum>>16);
#endif
            }
            break;
        
          case BITS_32:             /* read word */
            *ddata=*ptr;
            break;

          case BITS_64:             /* read dword */
            *ddata = *ptr;
            ddata[1] = ptr[1];
            break;

          default:
            break;
          }
    } else {
          switch (dacc & WIDTH_MASK) {
            /* extract byte */
          case BITS_8:              /* write_byte */
            if (HostEndian!=top->write_bigend) daddr^=3;
            ((unsigned8 *)ptr)[daddr & 3]=(unsigned8)(*ddata);
            break;
        
          case BITS_16:             /* write half-word */
            if (HostEndian!=top->write_bigend) daddr^=2;
            *((unsigned16 *)(((char *)ptr)+(daddr & 2))) = (unsigned16)(*ddata);
            break;

          case BITS_32:             /* write word */
            *ptr=*ddata;
            break;

          case BITS_64:             /* write 2 words */
            *ptr = *ddata;
            ptr[1] = ddata[1];
            break;

          default:
            break;
          }
    }
  }

  *drv = 1;
ddone:
  ;
}

#ifdef PIPELINED
static void NextDCycle(void *handle, ARMword addr, ARMul_acc acc)
{
    NextCycle(handle, addr, acc);
}
static void NextICycle(void *handle, ARMword addr, ARMul_acc acc)
{
    NextCycle(handle, addr, acc);
}
#endif

/*
 * Utility functions:
 */


static unsigned Flat_MemInfo(void *handle, unsigned type, ARMword *pID,
                        uint64 *data)
{
    toplevel *top=(toplevel *)handle;
    if (ACCESS_IS_READ(type))
    {
        switch (*pID) {
        case MemPropertyID_TotalCycles:
            *data = (top->cycles.NumNcycles + top->cycles.NumScycles +
                     top->cycles.NumIcycles + top->cycles.NumCcycles);
            return RDIError_NoError;
        case MemPropertyID_CycleSpeed:
            *data = top->clk_speed;
            return RDIError_NoError;
        case MemPropertyID_BaseMemoryRef:    /* We are a LEAF */
            *(ARMul_MemInterface**)data = top->mem_ref.mif;
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



static const ARMul_Cycles *Flat_ReadCycles(void *handle)
{
  toplevel *top=(toplevel *)handle;
  top->cycles.Total=(top->cycles.NumNcycles + top->cycles.NumScycles +
                     top->cycles.NumIcycles + top->cycles.NumCcycles);
  return &(top->cycles);
}

static unsigned int DataCacheBusy(void *handle)
{
  UNUSEDARG(handle);
  return FALSE;
}

static unsigned long Flat_GetCycleLength(void *handle)
{
  /* Returns the cycle length in tenths of a nanosecond */
  toplevel *top=(toplevel *)handle;
  return (unsigned long)(top->clk*10000.0);
}

BEGIN_EXIT(Flatmem)
{
/*
 * Remove the memory interface
 */
  ARMword page;
  toplevel *top=state;
  memory *mem=&top->mem;
  /* free all truly allocated pages */
  for (page=0; page<NUMPAGES; page++) {
    mempage *pageptr= mem->page[page];
    if (pageptr) {
      free((char *)pageptr);
    }
  }

  ClxList_Destroy((ClxList*)&top->NewPageAllocated,NULL);

  ARMulAgent_Destroy(&top->agent);

  /* top-level structure is freed by END_EXIT macro  */
}
END_EXIT(Flatmem)



/*--- <SORDI STUFF> ---*/
#define SORDI_DLL_NAME_STRING "Flatmem"
#define SORDI_DLL_DESCRIPTION_STRING "Flatmem Leaf memory model"
#define SORDI_RDI_PROCVEC Flatmem_AgentRDI
#include "perip_sordi.h"

#include "perip_rdi_agent.h"
    IMPLEMENT_AGENT_PROCS_NOEXE_NOMODULE(Flatmem)
    IMPLEMENT_AGENT_PROCVEC_NOEXE(Flatmem)

/*--- </> ---*/


/* EOF flatmem.c */


