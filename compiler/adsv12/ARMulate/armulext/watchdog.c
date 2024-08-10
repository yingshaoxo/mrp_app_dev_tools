/* peripheral.c - Memory veneer that implements RPS peripheral counter.
 * Copyright (C) ARM Limited, 1998-1999. All rights reserved.
 *
 * RCS $Revision: 1.5.2.4 $
 * Checkin $Date: 2001/09/18 14:47:07 $
 * Revising $Author: lmacgreg $
 */
 
#include <string.h>             /* for memset */
#include "minperip.h"
#include "armul_cnf.h"
#include "armul_types.h"      /* for ARMTime */
#include "armul_access.h"


#define ARMulCnf_KeyValue (tag_t)"KEYVALUE"
#define ARMulCnf_WatchPeriod (tag_t)"WATCHPERIOD"
#define ARMulCnf_IRQPeriod (tag_t)"IRQPERIOD"
#define ARMulCnf_IntNumber (tag_t)"INTNUMBER"
#define ARMulCnf_StartOnReset (tag_t)"STARTONRESET"
#define ARMulCnf_RunAfterBark (tag_t)"RUNAFTERBARK"

/* for acc_MREQ, acc_READ, acc_ACCOUNT */
#define acc_MREQ(a) (!( (a) & ACCESS_IDLE))
#define acc_READ(a) ACCESS_IS_READ(a)
#define acc_ACCOUNT(a) ACCESS_IS_REAL(a)

typedef ARMul_TimedCallBackProc armul_EventProc;

typedef struct {
  unsigned long  KeyValue;      /* register 0x00 */
  unsigned long  WatchPeriod;   /* register 0x04 */
  unsigned long  WatchPeriod2;  /* register 0x08 */
  ARMTime  nextEventTime;  /* In case we need to remove it!! */
    void *nextEvent_killhandle;
  armul_EventProc *fn;          /* ptr to timeout function  */
  ARMTime  nextIRQEventTime;      /* In case we need to remove IRQ event */
    void *nextIRQEvent_killhandle;
  armul_EventProc *irqfn;         /* ptr to irq timeout function */
  int   continueOnTimeout;      /* Do we continue on timeout */
  int   inCountdown;           /* Are we in normal count(0) or
                                  * IRQ countdown (1) */
  int   runOnStartup;           /* Do we start running on startup */
  int interruptNumber;       /* The interrupt signal number for this timer */
} peripheral;


BEGIN_STATE_DECL(Watchdog) /* was WatchdogState */
    GenericAccessCallback **intcInterface;
    ARMul_BusPeripAccessRegistration my_bpar;

  peripheral  peripheral1;          /* peripheral 1 structure */
  int warn;                     /* Warn users about invalid register accesses*/
  int waits;                    /* Number of idle cycles before access */
  int waitStates;               /* stored number of wait states */
  int accessState;              /* Current access state */
END_STATE_DECL(Watchdog)


static void DO_INTCTRL(GenericAccessCallback **intc1, ARMword srcN,
                       ARMword v2)
{
    if (intc1)
    {
        GenericAccessCallback *intc1a = *intc1;
        if (intc1a)
        {
            intc1a->func(intc1a,srcN,&v2,0);
        }
    }
}


/* See note 4 */

/* ******************* */
/* Static declarations */
/* ******************* */

typedef unsigned ARMul_acc;

static void WatchdogIRQTimeout(void *handle);
static int RDI_info(void *handle, unsigned type, ARMword *arg1, ARMword *arg2);
static int RegisterAccess(WatchdogState *ts,ARMword addr,ARMword *word,
                          unsigned /* was ARMul_acc */ acc);

/*
 * Return: 1 for failure, 0 for success 
 */
static int iCheckIntcIf(WatchdogState *ts)
{
    int endcondition;
    if (ts->intcInterface  == NULL)
    {
        /* No we don't have it installed yet.
           By the time we have started running code the intc WILL have been
           installed if it exists - so we can try again to install it
           */
        ts->intcInterface = ARMulif_GetInterruptController(&ts->coredesc);
        
        if (ts->intcInterface == NULL || *ts->intcInterface == NULL)
        {
            endcondition = RDIError_Reset;

        }
  }
  return 0;
}




/* This function is to be called when the first timer times out.
Its job is to start the second timer - which does nasty shutdowns */

static void WatchdogTimeout(void *handle)
{
    WatchdogState *ts = (WatchdogState*)handle;
    peripheral *myWDog = &ts->peripheral1;
    unsigned long delay;
                
    delay = ts->peripheral1.WatchPeriod2;

 
/* Check that we have the intc interface installed */
    if (iCheckIntcIf(ts))
    {
        return /*0*/;
    }
    Hostif_ConsolePrint(ts->hostif,
                        "Starting second timer and signalling IRQ\n");


    /* Set that we are in countdown to doom */
    myWDog->inCountdown = 1;
    
    /* Start timer */
    { ARMTime Now = ARMulif_Time(&ts->coredesc);
      ARMTime when = Now + delay;
      ts->peripheral1.nextIRQEventTime = when;
    
      ts->peripheral1.nextIRQEvent_killhandle = 
          ARMulif_ScheduleNewTimedCallback(
              &ts->coredesc, WatchdogIRQTimeout, ts, when, 0);
    }


    /* Signal interrupt */
    { uint32 one = 1;
    DO_INTCTRL(ts->intcInterface,myWDog->interruptNumber,one);
    }
}


static void WatchdogIRQTimeout(void *handle)
{
    WatchdogState *ts = (WatchdogState*)handle;
    /* ARMul_State *state = (ARMul_State*)ts->state; */
    peripheral *myWDog = &ts->peripheral1;
 
    int endcondition;

    Hostif_ConsolePrint(
        ts->hostif, "Fatal System Error: Watchdog timed out at %lu!!\n",
        (unsigned long)ARMulif_Time(&ts->coredesc));
    Hostif_ConsolePrint(ts->hostif,"Halting System to allow debug.\n");
  
    /* Make a call to halt execution - indicate that the target has stopped.*/

    if ( myWDog->continueOnTimeout)
    {
        endcondition = RDIError_BreakpointReached;
        ARMulif_StopExecution(&ts->coredesc, endcondition);
        Hostif_PrettyPrint(ts->hostif,ts->config,
                 "Watchdog has halted emulation.\n");
        Hostif_PrettyPrint(ts->hostif,ts->config,
                 "You may continue executing, but watchdog is halted now.\n");
    }
    else
    {
        endcondition = RDIError_Reset;
        ARMulif_StopExecution(&ts->coredesc, endcondition);
        Hostif_PrettyPrint(ts->hostif,ts->config,
                 "Watchdog has halted emulation and reset the target.\n");
    }  
}


/*
 * ARMulator callbacks
 */

 
static int RDI_info(void *handle, unsigned type, ARMword *arg1, ARMword *arg2)
{
  UNUSEDARG(handle);
  UNUSEDARG(arg2);
  UNUSEDARG(arg1);
  UNUSEDARG(type);

  return RDIError_UnimplementedMessage;
}  

 
 
      
   
 

/*
Function Name: RegisterAccess
Parameters:     WatchdogState *ts,  -WatchdogState pointer
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
static int RegisterAccess(WatchdogState *ts, ARMword addr, ARMword *word,
                          ARMul_acc acc)
{
    /* ARMul_State *state = (ARMul_State*)ts->state; */

    unsigned long  maskedAddress = addr & 0x000000FF;

 /* Is it a memory access? */
 if (acc_MREQ(acc) )
  {
  /* Yes it is a memory access - now is it a read or a write? */
  if ( acc_READ(acc) )
    {
    /* Read from registers */
    switch ( maskedAddress)
        {
        case 0x00:       
                 *word = ts->peripheral1.KeyValue;
                break;

        case 0x04:
                *word = ts->peripheral1.WatchPeriod;
                break;
        case 0x08:
                *word = ts->peripheral1.WatchPeriod2;
                break;

        case 0x0c:      /* Reserved */ 
                *word = 0;
                if ( ts->warn == TRUE) {
                Hostif_ConsolePrint(ts->hostif,
                  "Warning - accessed Reserved WDOG register address %08x\n",
                                    addr);
                }
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
/*   >>>>>>>>>>>>Modify Me<<<<<<<<<<<<< */
/* See note 9 */
        case 0x00:  
                /* Key update register written to */    
                if ( ts->peripheral1.KeyValue == *word )
                {
                /* The KEY value written matches the stored check value reset 'bark' */


                unsigned long delay;
                delay = ts->peripheral1.WatchPeriod;

                if (ts->peripheral1.inCountdown == 1)
                    {
                    printf("We are in countdown to doom and we got KEY update \n");
                   ts->peripheral1.inCountdown = 0;
                    printf("Cleared countdown to doom \n");
        
                    /* Clear out the count to doom */
                    ARMulif_DescheduleTimedFunction(&ts->coredesc,
                                    ts->peripheral1.nextIRQEvent_killhandle);
                    /* Clear the interrupt source */
                    /* Check that we have the intc interface installed */
                    if (iCheckIntcIf(ts))
                        {
                        return 0;
                        }
                    { uint32 zero = 0;
                    DO_INTCTRL(ts->intcInterface,
                               ts->peripheral1.interruptNumber,zero);
                    }


                    /* Restart the  main counter */
                 
                    /* Record event time in case we need to remove it
                     * prematurely and schedule the event */
                    { ARMTime Now = ARMulif_Time(&ts->coredesc);
                    ARMTime when = Now + delay;
                    ts->peripheral1.nextEventTime = when;
                    
                    ts->peripheral1.nextEvent_killhandle = 
                        ARMulif_ScheduleNewTimedCallback(
                            &ts->coredesc, WatchdogTimeout, ts, when, 0);
                    }


                    }
                else
                    {
                    /* err - we're not in countdown to doom */
                    Hostif_ConsolePrint(ts->hostif,"Watchdog received KEYVALUE update, BUT NOT expecting it yet.\n");
                    Hostif_ConsolePrint(ts->hostif,"So, Watchdog is ignoring this write.\n");
                    }
                     
                }

                 break;
        case 0x04:
                if ( *word < 5000 )
                   {
                   /* Disallow excessively short watchdog periods */
                   ts->peripheral1.WatchPeriod = 5000;
                  if ( ts->warn == TRUE) {
                     Hostif_ConsolePrint(ts->hostif,"Warning - Attempt to write value < 5000 as watchdog timeout period\n");
                     Hostif_ConsolePrint(ts->hostif,"Defaulted to 5000 as timeout period.\n");
                     }
                   }
                else
                   {
                   /* Was sensible - so allows user period to be written */
                   ts->peripheral1.WatchPeriod = *word;
                   }
                break;

        case 0x08:
              if ( *word < 2000 )
                   {
                   /* Disallow excessively short watchdog periods */
                   ts->peripheral1.WatchPeriod2 = 5000;
                  if ( ts->warn == TRUE) {
                     Hostif_ConsolePrint(ts->hostif,"Warning - Attempt to write value < 2000 as watchdog timeout period 2\n");
                     Hostif_ConsolePrint(ts->hostif,"Defaulted to 5000 as timeout period 2.\n");
                     }
                   }
                else
                   {
                   /* Was sensible - so allows user period to be written */
                   ts->peripheral1.WatchPeriod2 = *word;
                   }
                break;

/*   >>>>>>>>>>>>Modify Me<<<<<<<<<<<<< */
/* See note 10 */
                /* Reserved */
        case 0x0c:
                 if ( ts->warn == TRUE) {
                 Hostif_ConsolePrint(ts->hostif,"Warning - accessed Reserved WDOG register address %08x\n",addr);
                 }
                 break;
        default:
                return PERIP_NODECODE;  /* Failed to decode address */
        }

    }
  }

 return PERIP_OK;  /* Successful */

}
   

/* Bus state machine */

static int BusState(
    WatchdogState *ts, ARMword addr, ARMword *word,ARMul_acc acc)
{
  if (!acc_ACCOUNT(acc)) {
      return RegisterAccess(ts, addr, word, acc);
  }

  if ( ts->accessState == 0 )
      {
      /* Idle on access */
      if ( ts->waitStates == 0)
          {
          /* No waits - do access now */
          return RegisterAccess(ts,addr,word,acc);
          }
      else
         {
         /* First access -but with waits - so decrement  and return 0*/
         ts->waits = ts->waitStates;
         ts->accessState=1; 
         return PERIP_BUSY;  /* tell core to wait */
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
          return RegisterAccess(ts,addr,word,acc);
          }
      else
         {
         /* Subsequent access -but with waits - so decrement  and return 0*/
         ts->waits--;
         /* Subsequent wait */
         return PERIP_BUSY;  /* tell core to wait */
         }

      }

  
/* accessState : 0=Idle,  1= Waiting */
}


/* MemAccess functions */
static int Watchdog_Access(void *handle, 
                           struct ARMul_AccessRequest *req)
{
    ARMWord addr = req->req_address[0];
    ARMWord *data = req->req_data;
    unsigned acctype = req->req_access_type;
  WatchdogState *ts=(WatchdogState *)handle;
   
    assert(addr >= ts->my_bpar.range[0].lo && \
           addr <= ts->my_bpar.range[0].hi);
 
    return BusState(ts,addr,data,acctype);
}



static unsigned Watchdog_ConfigEvents(void *handle, void *data)
{
    WatchdogState *ts = (WatchdogState*)handle;
    ARMul_Event *evt = data;

    if (evt->event == ConfigEvent_Reset)
    {
        /* We are only interested in the Reset event */
        Hostif_PrettyPrint(ts->hostif,ts->config,"Watchdog Reset ");

        /* If there is an event then kill it */
        
        if (  ts->peripheral1.inCountdown ==1 )
        {
            if ( ts->peripheral1.nextIRQEventTime != 0)
            {
                ARMulif_DescheduleTimedFunction(&ts->coredesc,
                                ts->peripheral1.nextIRQEvent_killhandle);

                ts->peripheral1.nextIRQEventTime=0;
                
                /* On reset we are NOT on countdown to doom */
                printf("CLEARING COUNTDOWN TO DOOM ON RESET \n");
                ts->peripheral1.inCountdown =0;
            }

        }
        else
        {
            if ( ts->peripheral1.nextEventTime != 0)
            {
                if( ts->peripheral1.nextIRQEvent_killhandle != NULL)
                    ARMulif_DescheduleTimedFunction(&ts->coredesc,
                                ts->peripheral1.nextIRQEvent_killhandle);
                ts->peripheral1.nextEventTime=0;
            }
        }

        /* Kick off the first wdog event */
    
        if ( ts->peripheral1.runOnStartup == TRUE)
        {
            unsigned long delay;
            
            delay = ts->peripheral1.WatchPeriod;
            /* We must start wdog on startup */
            Hostif_PrettyPrint(ts->hostif,ts->config," (running on startup).");
            
            /*Schedule the event and also record the timeout time
             * so that we can rescehdule */
            { ARMTime Now = ARMulif_Time(&ts->coredesc);
            ARMTime when = Now + delay;
            ts->peripheral1.nextEventTime = when;
            
            ts->peripheral1.nextEvent_killhandle = 
                ARMulif_ScheduleNewTimedCallback(
                    &ts->coredesc, WatchdogTimeout, ts, when, 0);
            }
            
        }
        else
        {
            Hostif_PrettyPrint(ts->hostif,ts->config," (not running).");
        }
    } /* end if which & ARMul_InterruptUpcallReset */
    return FALSE;
}




BEGIN_INIT(Watchdog)
{
    WatchdogState *ts = state;

  unsigned int value1;  /* Key value local*/
  unsigned int value2;  /* WATCHPERIOD local */
  unsigned int value3;  /* IRQPERIOD local */
  int value4;           /* Interrupt number */
  int  startup, contafter;
  int warn;
  int waits;

  Hostif_PrettyPrint(ts->hostif,ts->config,", Watchdog");
  

  /* 
   Look up the watchdog  parameters from armul.cnf 
  */


  value1 =      ToolConf_DLookupUInt(config, ARMulCnf_KeyValue, 0xA1B2C2D4);
  value2 =      ToolConf_DLookupUInt(config, ARMulCnf_WatchPeriod, 500000);
  value3 =      ToolConf_DLookupUInt(config, ARMulCnf_IRQPeriod,10000);
  value4 =      ToolConf_DLookupUInt(config, ARMulCnf_IntNumber,16);
  startup =     ToolConf_DLookupBool(config, ARMulCnf_StartOnReset, FALSE);
  contafter =   ToolConf_DLookupBool(config, ARMulCnf_RunAfterBark,FALSE); 
  warn =        ToolConf_DLookupBool(config, ARMulCnf_Warn, FALSE);
  waits =       ToolConf_DLookupUInt(config, ARMulCnf_Waits, 1);

  /* If warning is enabled then announce it */
  if ( warn == TRUE) Hostif_PrettyPrint(ts->hostif,ts->config," (warn on)");

  if ( waits < 0   || waits > 30)
      {
      Hostif_PrettyPrint(ts->hostif,ts->config,"(Timer Error: Invalid wait state value - defaulting to zero waits)");
      waits = 0;
      }



 /* Fill in other entries */
 /* record whether we warn about dubious register accesses or not */
  ts->warn = warn;
  ts->waitStates = waits;
  ts->waits=0;
  ts->accessState=0; /* Reset to idle */

  /* Register our peripheral-access-function. */
    { unsigned err = RDIError_NoError;
    state->my_bpar.access_func = Watchdog_Access;
    state->my_bpar.access_handle = state;
    state->my_bpar.capabilities = PeripAccessCapability_Minimum;
    err = ARMulif_ReadBusRange(&state->coredesc, state->hostif,
                         ToolConf_FlatChild(config, (tag_t)"RANGE"),
                         &state->my_bpar,  
                         0xb0000000,0x4,""); /* Defaults from old .cnf */

    err = state->my_bpar.bus->bus_registerPeripFunc(BusRegAct_Insert, 
                                                    &state->my_bpar);
    if (err)
        return err;
    }

  /* Set up the WatchdogState peripheral parameters */
  ts->peripheral1.KeyValue= value1;

  if ( value2 < 5000 )
     {
     ts->peripheral1.WatchPeriod=5000;
     }
  else
    {
    ts->peripheral1.WatchPeriod=value2;
    }

 /* Set up the IRQ timeout period */
 if ( value3 < 1000 )
     {
     ts->peripheral1.WatchPeriod2=1000;
     }
  else
    {
    ts->peripheral1.WatchPeriod2=value3;
    }

  /* Set up the interrupt number */
   ts->peripheral1.interruptNumber = value4;


  /* Record how we behave on timeout bark ;
        we can either halt on say tough - you cannot proceed
        or we can warn, halt but allow to continue
  */
  ts->peripheral1.continueOnTimeout = contafter;
 
  /* Record whether we start running on reset */
  ts->peripheral1.runOnStartup = startup;
 
  /* Get the Interrupt controller interface structure pointer.
     It is important that the interrupt controller is installed
     before any peripherals that use it - otherwise the interface
     will not be available at initialisation time.
     It is possible to install the interface later - but this
     pollutes the peripheral emulation code with initialisation
     functionality.
  */
  ts->intcInterface = ARMulif_GetInterruptController(&ts->coredesc);


  ts->peripheral1.nextEventTime = 0;  /* Should be zero until we have done 1st one */
 
  /* Display appropriate startup banner message */
  if ( ts->peripheral1.runOnStartup == TRUE)
      {
      Hostif_PrettyPrint(ts->hostif,ts->config," (running on startup)");
      }
  else
      {
      Hostif_PrettyPrint(ts->hostif,ts->config," (not running)");
      }

  ARMulif_InstallEventHandler(&state->coredesc,
                              ConfigEventSel,
                              Watchdog_ConfigEvents, state);
  ARMulif_InstallUnkRDIInfoHandler(&state->coredesc,
                                   RDI_info,state);
}
END_INIT(Watchdog)

BEGIN_EXIT(Watchdog)
END_EXIT(Watchdog)



/*--- <SORDI STUFF> ---*/
#define SORDI_DLL_NAME_STRING "Watchdog"
#define SORDI_DLL_DESCRIPTION_STRING "Watchdog Timer (test only)"
#define SORDI_RDI_PROCVEC Watchdog_AgentRDI
#include "perip_sordi.h"

#include "perip_rdi_agent.h"
    IMPLEMENT_AGENT_PROCS_NOEXE_NOMODULE(Watchdog)
    IMPLEMENT_AGENT_PROCVEC_NOEXE(Watchdog)

/*--- </> ---*/


/* EOF watchdog.c */
