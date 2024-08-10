/* millisec.c - Peripheral Model that implements a
 * simple millisecond timer counter.
 * Copyright (C) ARM Limited, 1998-2001 . All rights reserved.
 *
 * RCS $Revision: 1.6.2.10 $
 * Checkin $Date: 2001/09/18 14:25:16 $
 * Revising $Author: lmacgreg $
 */


#include "minperip.h"
#include "armul_mem.h" 


#ifndef ARMulCnf_Waits
# define ARMulCnf_Waits (tag_t)"WAITS"
#endif

#ifndef NDEBUG
# if 1
# else
#  define VERBOSE_CONFIG
#  define VERBOSE
# endif
#endif



BEGIN_STATE_DECL(Millisec)
   uint32 ncycles_lo, ncycles_hi;
   double cycles_per_millisecond;
/* Parts for connection as a peripheral */
   ARMul_BusPeripAccessRegistration my_bpar;

  int waits;                    /* Number of idle cycles before access */
  int waitStates;               /* stored number of wait states */
  int accessState;              /* Current access state */
  bool bigendSig; 
END_STATE_DECL(Millisec)




/* ******************* */
/* Static declarations */
/* ******************* */



static ARMword MaskToSize(ARMword value, bool bigend, ARMul_acc acc,
                          ARMword addr)
{
    if (bigend) addr ^= 3;
    switch(ACCESS_SIZE(acc))
    {
    case BITS_8:
        return (value >> (addr & 3)) & 0xff;
    case BITS_16:
        return (value >> (addr & 2)) & 0xffff;
    default:
    case BITS_32:
        return value;
    }
}



static ARMword GuessMilliseconds(MillisecState *ts)
{
        double ncycles = (double)(int64)ts->my_bpar.bus->bus_BusyUntil;
    {
        /* Needed by VC++5 */
        double twoE16 = 65536.0;
        double twoE32 = twoE16 * twoE16;
        double cycr = (double)ncycles + 
            twoE32 * (double)ts->ncycles_hi;
        cycr /= ts->cycles_per_millisecond;
        return (ARMword)cycr;
    }
}

/*
Function Name: TICRegisterAccess
Parameters:     MillisecState *ts,  -MillisecState pointer
                ARMword addr,   - access address
                ARMword *word,  - pointer to data word
                ARMul_acc acc   - access type information
Return: int  1 = success, -2 = failed to decode address.
Description: Called from MemAccess.  This meory model is called from the switch
        because partial address decoding identified this as a memory that could
        fully decode the address. This function continues the address decoding
        trying to identify individual registers.  For each decoded address
        an action is carried out. For any addressed that aren't decoded -2 is
        returned indicating failure to decode. In the case of failure to decode
        then the access is passed on to the next level of memory by switch.
*/
static int TICRegisterAccess(MillisecState *ts,ARMword addr,ARMword *word,
                             unsigned /*ARMul_acc*/ acc)
{
    (void)addr;
    if ( ACCESS_IS_READ(acc) )
    {
        *word = MaskToSize(GuessMilliseconds(ts), ts->bigendSig,
                           acc, addr);
#ifdef VERBOSE
        printf("*** Millisec->%08lx!\n", (unsigned long)*word);
#endif
    }
    else
    {
#ifdef VERBOSE
        printf("*** WRITE TO MILLISEC!\n");
#endif
    }
    return PERIP_OK;
}


/* Bus state machine */

static int BusState(MillisecState *ts, ARMword addr, ARMword *word,
                    unsigned /*ARMul_acc*/ acc)
{
    if (!ACCESS_IS_REAL(acc)) {
        return TICRegisterAccess(ts, addr, word, acc);
    }

  if ( ts->accessState == 0 )
      {
      /* Idle on access */
      if ( ts->waitStates == 0)
          {
          /* No waits - do access now */
          return TICRegisterAccess(ts,addr,word,acc);
          }
      else
         {
         /* First access -but with waits - so decrement  and return 0*/
         ts->waits = ts->waitStates;
         ts->accessState=1; 
         return RDIError_Busy;  /* tell core to wait */
         }
      }
  else /* ts->accessState == 1) */
      {
      /* Waiting state */
      if ( ts->waits == 0)
          {
          /* No waits - do access now */
          ts->accessState = 0; /* restore access state */
          /* Post wait - doing access */
          return TICRegisterAccess(ts,addr,word,acc);
          }
      else
         {
         /* Subsequent access -but with waits - so decrement  and return 0*/
         ts->waits--;
         /* Subsequent wait */
         return RDIError_Busy;  /* tell core to wait */
         }

      }
/* accessState : 0=Idle,  1= Waiting */
}


/* MemAccess functions */

static ARMul_BusPeripAccessFunc Millisec_Access;
static int Millisec_Access(void *handle, 
                           struct ARMul_AccessRequest *req)
{
    ARMWord address = req->req_address[0];
    ARMWord *data = req->req_data;
    unsigned type = req->req_access_type;
    MillisecState *state=(MillisecState *)handle;
   
    assert(address >= state->my_bpar.range[0].lo && \
           address <= state->my_bpar.range[0].hi);

    return BusState(state,address,data,type);
}


BEGIN_INIT(Millisec)
{
  MillisecState *ts = state;
  /* 
   * Look up the TimerCounter  parameters.
   */

  int waits = ToolConf_DLookupUInt(config, ARMulCnf_Waits, 0 /*1*/);
  if ( waits < 0   || waits > 30)
  {
      /* PrettyPrint */
      Hostif_ConsolePrint(state->hostif,"(Timer Error: Invalid wait state "
                          "value - defaulting to zero waits)");
      waits = 0;
  }

  {
      uint32 BusSpeed = ARMul_GetMCLK(state->config);
      ts->cycles_per_millisecond =  BusSpeed / 1000.0;
  }

  Hostif_PrettyPrint(ts->hostif, config, ", Millisecond "
                     "[%g cycles_per_millisecond]",
                     ts->cycles_per_millisecond);

  /* Fill in other entries */ 
  ts->waitStates = waits;
  ts->waits=0;
  ts->accessState=0; /* Reset to idle */

  /* -- connect as a peripheral to the bus -- */
    { unsigned err;
    state->my_bpar.access_func = Millisec_Access;
    state->my_bpar.access_handle = state;
    state->my_bpar.capabilities = PeripAccessCapability_Typical;
    err = ARMulif_ReadBusRange(&state->coredesc, state->hostif,
                         ToolConf_FlatChild(config, (tag_t)"RANGE"),
                         &state->my_bpar,  
                         0x0bfffff0,4,"");/*0x0bfffff0,0x0bfffff3*/
    if (err)
        return err;
    err = state->my_bpar.bus->bus_registerPeripFunc(BusRegAct_Insert, 
                                                    &state->my_bpar);
    if (err)
        return err;
    }


}
END_INIT(Millisec)


BEGIN_EXIT(Millisec)
END_EXIT(Millisec)

/*--- <SORDI STUFF> ---*/

#define SORDI_DLL_NAME_STRING "Millisec"
#define SORDI_DLL_DESCRIPTION_STRING "Millisecond Timer (test only)"
#define SORDI_RDI_PROCVEC Millisec_AgentRDI
#include "perip_sordi.h"

#include "perip_rdi_agent.h"
    IMPLEMENT_AGENT_PROCS_NOEXE_NOMODULE(Millisec)
    IMPLEMENT_AGENT_PROCVEC_NOEXE(Millisec)

/*--- </> ---*/








/* EOF millisec.c */
