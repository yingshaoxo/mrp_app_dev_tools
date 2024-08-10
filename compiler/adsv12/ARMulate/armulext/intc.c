/* intc.c - Memory veneer that implements RPS interrupt controller.
 * Copyright (C) Advanced RISC Machines Limited, 1998-1999.
 * Copyright (C) ARM Limited, 1999 - 2001. All rights reserved.
 *
 * RCS $Revision: 1.5.4.6 $
 * Checkin $Date: 2001/08/24 12:57:48 $
 * Revising $Author: lmacgreg $
 */

#include <string.h>             /* for memset */
#define ModelName(IntCtrl)
#include "minperip.h"
#include "armul_bus.h" 




/* Debugging verbosity */
#if !defined(NDEBUG)
# if 1
# else
#  define VERBOSE
#  define VERBOSE_ACCESS
# endif
#endif

/* **************************** */
/* Static function declarations */
/* **************************** */

static ARMul_BusPeripAccessFunc IntCtrl_Access;

/* 
 * Interrupt controller peripheral register/signal structure 
 * Refer to ARM Specification ARM DDI 0062D for HW details.
*/
typedef struct {
  unsigned long  IRQStatus;
  unsigned long  IRQRawStatus;
  unsigned long  IRQEnable;
  unsigned long  IRQEnableClear;
  unsigned long  IRQSoft;
  unsigned long  FIQStatus;
  unsigned long  FIQRawStatus;
  unsigned long  FIQEnable;
  unsigned long  FIQEnableClear;
  int nIRQ;
  int nFIQ;
} intc;


/* 
 * Interrupt controller local state structure.
 * This allows multiple instantiations of a peripheral to be made
 * if necessary.
*/
BEGIN_STATE_DECL(IntCtrl)
  intc  myIntc;                 /* Reference to intc struct above */
  bool warn;                    /* Warn users about invalid register accesses*/
  int waits;                    /* Number of idle cycles before access */
  int waitStates;               /* stored number of wait states */
  int accessState;              /* Current access state */
    ARMul_BusPeripAccessRegistration my_bpar;
END_STATE_DECL(IntCtrl)


static GenericAccessFunc SignalInterruptChange;

static void InitialiseState(IntCtrlState *intcs);
static void FIQUpdate(IntCtrlState *intcs);
static void IRQUpdate(IntCtrlState *intcs);


#ifndef ARMulCnf_Warn
# define ARMulCnf_Warn (tag_t)"WARN"
#endif
#ifndef ARMulCnf_Waits
# define ARMulCnf_Waits (tag_t)"WAITS"
#endif

static unsigned Intc_ConfigEvents(void *handle, void *data)
{
  IntCtrlState *top=(IntCtrlState *)handle;  
  ARMul_Event *evt = (ARMul_Event *)data;
  if (evt->event == ConfigEvent_Reset)
  {
        InitialiseState(top);
        IRQUpdate(top);
        FIQUpdate(top);
  }
     
  return FALSE;

}


BEGIN_INIT(IntCtrl)
{
    Hostif_PrettyPrint(state->hostif, state->config, ", IntCtrl");
    state->warn =  ToolConf_DLookupBool(config, ARMulCnf_Warn, FALSE);
    state->waitStates = ToolConf_DLookupUInt(config, ARMulCnf_Waits, 0 /*1*/);

    if (state->warn)
    {
        Hostif_PrettyPrint(state->hostif,state->config," (warn on)");
    }

    if ( state->waitStates < 0   || state->waitStates > 30)
    {
        Hostif_PrettyPrint(state->hostif,state->config, 
                            "(Intc Error: Invalid wait state value "
                            "- defaulting to zero waits)");
        state->waitStates = 0;
    }

    /*
     * Attach it to the core so that other peripherals can
     * find and use the intc.
     */
    if (ARMulif_InstallNewInterruptController(&state->coredesc,
                                            SignalInterruptChange,
                                            state )
        == NULL)
    {
        Hostif_RaiseError(state->hostif,
                          "CORE FAILED TO REGISTER INTERRUPT-CONTROLLER\n");
    }
      /*
       * NOTE: The intc must currently be given a lower Plugin number
       * than its clients.
       * Later, the clients should connect at ColdBoot time,
       * or check that they have an interrupt-controller.
       */

      /*
       * The interrupt-handler used in ARMulator is purely
       * a reset-handler.
       */
    
    /* Ask for the default bus. */
 
    ARMulif_ReadBusRange(&state->coredesc, state->hostif,
                           ToolConf_FlatChild(config, (tag_t)"RANGE"),
                           &state->my_bpar,  
                           0x0a000000,0x110,"");
      {
          ARMul_Bus *bus = state->my_bpar.bus;
          ARMul_BusPeripAccessRegistration *regn = &state->my_bpar;
          regn->access_func = IntCtrl_Access;
          regn->access_handle = state;
          regn->capabilities = PeripAccessCapability_Minimum;
          (void)bus->bus_registerPeripFunc(BusRegAct_Insert, regn);
      }
      
}
{
   
    /* Initialise the intc structure */
    InitialiseState(state);
    ARMulif_InstallEventHandler(&state->coredesc,
                                ConfigEventSel,
                                Intc_ConfigEvents, state);
}
END_INIT(IntCtrl)

BEGIN_EXIT(IntCtrl)
END_EXIT(IntCtrl)


 


/*
 * ARMulator callbacks
 */



 
/* 
Function Name: SignalInterruptChange
Parameters: void *handle - This should be the IntcState handle from MemInit
         This handle is available from the interrupt controller interface
        structure, which itself can be obtained using 
        ARMul_GetInterruptControllerIF
        int sourceNumber - The incoming interrupt signal line number.
                        See ARM DDI 0062D Table 3-1 Interrupt Controller
                        Defined Bits for predefined connections.
        int value - The state of the interrupt signal. Active HIGH.
Return: Always returns 1 to indicate success - deliberately left open in case 
        future usage changes.
Description: Function to receive incoming interrupt signal changes.
        sourceNumber = 0 always indicates an FIQ, although this may be
        remapped in software to an IRQ.
        On receiving an interrupt signal the FIQ/IRQRawStatus is updated,
        the FIQ/IRQStatus is then updated ( w.r.t FIQ/IRQEnable )
        Finally the nIrq and nFiq signals are set according to the
        FIQ/IRQStatus register values.
*/

static unsigned  SignalInterruptChange(GenericAccessCallback *myCB,
                                       ARMword address, ARMword *data,
                                       unsigned type)
{
    IntCtrlState *intcs = (IntCtrlState*)myCB->handle;
    ARMword value = *data;
    int sourceNumber = address;
    int intState = value ? 1 : 0;

    (void)type;

    /* Note that in the RPS Spec ONLY interrupt source 0 may cause a FIQ */
    if ( sourceNumber == 0)
    {
    /* Update the FIQRawStatus register. There is only 1 bit slice for FIQ
    so we can cheat - not worrying about separate setting/clearing code
    as we have to do for IRQ below. If you base a new intc on this code
    then you will have to copy and modify the IRQ setting/clearing code
    for FIQ.*/
        intcs->myIntc.FIQRawStatus = intState; 
 
        /* Update FIQ register chain */  
        FIQUpdate (intcs);    
    }
    else
    {
        /* Update IRQRawStatus register - either setting or clearing a bit */
        if ( intState )
        {
            /* Set the specified bit */
            intcs->myIntc.IRQRawStatus |= (1 << sourceNumber);
        }
        else
        {
            /* Clear the specified bit */
            intcs->myIntc.IRQRawStatus &= ~(1 << sourceNumber);
        }
        /* Update IRQ register chain */
        IRQUpdate(intcs);
    }
    return RDIError_NoError;
}




/*
Function Name: IRQUpdate
Parameters:  ARMul_State *state - This has to be the original ARMul_State.
             IntcState *intcs - This HAS to be the IntcState - interrupt 
             controller interface structure, which itself can be obtained using 
             ARMul_GetInterruptControllerIF
Return: void
Description: This function is called whenever any of the IRQ registers 
             are changed or whenever a new IRQ signal has arrived.
             We have to do two things;
             1. Make the IRQStatus using IRQRawStatus & Enable.
             2. Set the nIrq signal
*/
void IRQUpdate(IntCtrlState *intcs)
{
  /* Make the IRQStatus using IRQRawStatus & IRQEnable */
    intcs->myIntc.IRQStatus = intcs->myIntc.IRQRawStatus & intcs->myIntc.IRQEnable;
#ifdef VERBOSE
    printf("\n**** Sending SignalIRQ = %lx *****\n",intcs->myIntc.IRQStatus);
#endif
    ARMulif_SetSignal(&intcs->coredesc, RDIPropID_ARMSignal_IRQ,
                    intcs->myIntc.IRQStatus ? Signal_On : Signal_Off);
}



/*
Function Name: FIQUpdate
Parameters: ARMul_State *state - This has to be the original ARMul_State.
            IntcState *intc - This HAS to be the IntcState - interrupt 
        controller interface structure, which itself can be obtained using 
        ARMul_GetInterruptControllerIF
Return: void
Description: This function is called whenever any of the FIQ registers is
         changed or whenever a new IRQ signal has arrived.
   We have to do two things;
   1. Make the FIQStatus using FIQRawStatus & Enable.
   2. Set the nFiq signal
*/

static void FIQUpdate(IntCtrlState *intcs)
{
    /* Make the FIQStatus using FIQRawStatus & FIQEnable */
    intcs->myIntc.FIQStatus = intcs->myIntc.FIQRawStatus & intcs->myIntc.FIQEnable;

    /* Set the nFiq signal */
    if ( intcs->myIntc.FIQStatus )
    {
        /* There is at least ONE active interrupt ( although probably only one)
         * - so trigger FIQ 
         */
        ARMulif_SetSignal(&intcs->coredesc, RDIPropID_ARMSignal_FIQ,
                          Signal_On);
    }
    else
    {
        /* There are no active interrupt sources - clear FIQ source */
        ARMulif_SetSignal(&intcs->coredesc, RDIPropID_ARMSignal_FIQ,
                          Signal_Off);
    }
}




/* 
 ************************************************************************
 * Memory veneer functions                                              *
 * Pass on the call to the real function, then check for the watchpoint.*
 ************************************************************************
*/

 
  
/*
Function Name: INTCRegisterAccess
Parameters: 
        IntcState *intcs - IntcState pointer - in this case the
                           MemAccess handle is the IntcState - but needs to 
                           be cast to IntcState type.
        ARMword addr - the 32 bit address to access
        ARMword *word - pointer to the data word ARMulator want us to
                        fill in/read from depending on acc.
        ARMul_acc acc - the access information ( read/write, seq etc. )

Return: int  0 = Waiting
             1 = success
             -1 = Abort
             -2 = Address not decoded ** This is new for switch/peripherals

Description:  Finished the address decoding that the switch started.
        Resolves an address to a particular register - or rejects the
        access if the decode fails or is to a reserved address.
        For writes to registers FIQ/IRQUpdate is called, this ensures
        that the interrupt registers and signals reflect any changes
        immediately. 

Notes:  It may not be obvious why this code isn't in MemAccess.
        This code is kept in a separate function to ease future
        maintenance should the Memory interface be changed in the future.
*/


static int INTCRegisterAccess(IntCtrlState *intcs, ARMword addr, ARMword *word,
                              unsigned /*ARMul_acc*/ acc)
{
    unsigned long  maskedAddress = addr & 0x000001ff;
#ifdef VERBOSE_ACCESS
    printf("INTCRegisterAccess, addr:%08lx range=(%08lx,%08lx) type:%04x "
           "@data:%08lx \n",
           (unsigned long)addr,
           (unsigned long)intcs->range.lo,
              (unsigned long)intcs->range.hi,
           acc, *(ARMword*)word);
#endif
    if (ACCESS_SIZE(acc) != ACCESS_WORD)
        return PERIP_DABORT;  /* Words are all we know about */
    
    if ( ACCESS_IS_READ(acc) )
    {
    /* Read from registers */
    switch ( maskedAddress)
        {
        case 0x00:       
                *word = intcs->myIntc.IRQStatus;
                break;
        case 0x04:
                *word = intcs->myIntc.IRQRawStatus;
                break;
        case 0x08:
                *word = intcs->myIntc.IRQEnable;
                break;
        case 0x100:
                *word = intcs->myIntc.FIQStatus;
                break;
        case 0x104:
                *word = intcs->myIntc.FIQRawStatus;
                break;
        case 0x108:
                *word = intcs->myIntc.FIQEnable;
                break;
        case 0x0c:
        case 0x10:
        case 0x10c: 
                if (intcs->warn && ACCESS_IS_REAL(acc)) {    
                Hostif_ConsolePrint(intcs->hostif,
                                    "Warning - attempted read from "
                                    "Reserved INTC register address %08x\n",addr);
                }
                /* We accept the read - but always return 0 */
                *word = 0;
                break;
         default:
                return PERIP_NODECODE;  /* Failed to decode address */
        }
    }
    else
    {
    /* Write to registers */
    switch ( maskedAddress)
        {
        case 0x0:
        case 0x04:
        case 0x100:
        case 0x104:
                if (intcs->warn && ACCESS_IS_REAL(acc)) {
                Hostif_ConsolePrint(intcs->hostif,
                                    "Warning - attempted write to "
                                    "Reserved INTC register address %08x\n",addr);
                }
                /* We accept the write - but throw away the data */
                 break;
        case 0x08:
                intcs->myIntc.IRQEnable |= *word; /* IRQEnableSet - add set bits
                                                    to IRQEnable */
                IRQUpdate(intcs);
                break;
        case 0x0c:
                {
                 intcs->myIntc.IRQEnable &=  ~(*word); /* IRQEnableClear - clear
                                                the set bits in IRQEnable reg */
                IRQUpdate(intcs);
                break;
                }
        case 0x10:
            /* IRQSoft - or programmed IRQ interrupt */
            /* Only bit 1 has any effect. */
                intcs->myIntc.IRQRawStatus &= ~2;
                intcs->myIntc.IRQRawStatus |= *word & 2;                

                IRQUpdate(intcs);
                break;
        case 0x108:
                intcs->myIntc.FIQEnable |= *word; /* FIQEnableSet - add set bits
                                        to FIQEnable */
                FIQUpdate(intcs);
                break;
        case 0x10c:
                intcs->myIntc.FIQEnable &= ~(*word); /* FIQEnableClear - clear
                                                the set bits in FIQEnable reg */
                FIQUpdate(intcs);
                break;
        default:
                return PERIP_NODECODE;  /* Failed to decode address */
        }
    }
    return PERIP_OK;
}
    
/* Bus state machine
 */
static int BusState(IntCtrlState *is, ARMword addr, ARMword *word,
                    unsigned /*ARMul_acc*/ acc)
{
#ifdef VERBOSE_ACCESS
    printf("IntCtrl_Access/BusState addr:%08lx range=(%08lx,%08lx) type:%04x "
           "@data:%08lx \n",
           (unsigned long)addr,
           (unsigned long)is->range.lo,
           (unsigned long)is->range.hi,
           acc, *(ARMword*)word);
#endif
    if (!ACCESS_IS_REAL(acc)) {
        return INTCRegisterAccess(is, addr, word, acc);
    }

    if ( is->accessState == 0 )
    {
      /* Idle on access */
      if ( is->waitStates == 0)
          {
          /* No waits - do access now */
          return INTCRegisterAccess(is,addr,word,acc);
          }
      else
         {
         /* First access -but with waits - so decrement  and return 0*/
         is->waits = is->waitStates;
         is->accessState=1; 
         return PERIP_BUSY;  /* tell core to wait */
         }
    }
    else /* ( is->accessState == 1) */
    {
      /* Waiting state */
      if ( is->waits == 0)
          {
          /* No waits - do access now */
          is->accessState = 0; /* restore access state */
          /* Post wait - doing access */
          return INTCRegisterAccess(is,addr,word,acc);
          }
      else
         {
         /* Subsequent access -but with waits - so decrement  and return 0*/
         is->waits--;
         /* Subsequent wait */
         return PERIP_BUSY;  /* tell core to wait */
         }
    }  
/* accessState : 0=Idle,  1= Waiting */
}


/* Memory interface functions */
static int IntCtrl_Access(void *handle, 
                          struct ARMul_AccessRequest *req)
{
  IntCtrlState *is=(IntCtrlState *)handle;
 
  /* We have idenfitied an intc memory access */
  return BusState(is,req->req_address[0],req->req_data,
                  req->req_access_type);
}

/* Reset handler */
static void InitialiseState(IntCtrlState *intcs)
{
    intcs->myIntc.IRQStatus=0;
    intcs->myIntc.IRQRawStatus=0;
    intcs->myIntc.IRQEnable=0;     /*all interrupts disabled */
    intcs->myIntc.IRQEnableClear=0;
    intcs->myIntc.IRQSoft=0;
    intcs->myIntc.FIQStatus=0;
    intcs->myIntc.FIQRawStatus=0;
    intcs->myIntc.FIQEnable=0;
    intcs->myIntc.FIQEnableClear=0;
    intcs->myIntc.nIRQ=1;
    intcs->myIntc.nFIQ=1;
}


/*--- <SORDI STUFF> ---*/


#define SORDI_DLL_DESCRIPTION_STRING "Interrupt Controller "\
    "(a Reference Peripheral)"
#define SORDI_RDI_PROCVEC IntCtrl_AgentRDI
#include "perip_sordi.h"

#include "perip_rdi_agent.h"
    IMPLEMENT_AGENT_PROCS_NOEXE_NOMODULE(IntCtrl)
    IMPLEMENT_AGENT_PROCVEC_NOEXE(IntCtrl)

/*--- </> ---*/



/* EOF intc.c */







