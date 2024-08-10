/* timer.c - Peripheral extension model of RPS timer counter.
 * Copyright (C) ARM Limited, 1998-2001 . All rights reserved.
 *
 * RCS $Revision: 1.11.4.12 $
 * Checkin $Date: 2001/10/02 17:26:23 $
 * Revising $Author: dsinclai $
 */



#define MODEL_NAME Timer 
 
#include "minperip.h"

#if !defined(NDEBUG)
# if 1
# else
#  define VERBOSE_KICKOFF
#  define VERBOSE_NOINTC
#  define VERBOSE_TIMEOUT
#  define VERBOSE_GETNOW
#  define VERBOSE_RUNAGAIN
#  define VERBOSE_ACCESS
# endif
#endif

#include "armul_agent.h" /* for ARMul_ShowModuleDesc */

#ifndef ARMulCnf_CLK
# define ARMulCnf_CLK (tag_t)"CLK"
#endif

#ifndef ARMulCnf_IntOne
# define ARMulCnf_IntOne (tag_t)"INT1"
#endif

#ifndef ARMulCnf_IntTwo
# define ARMulCnf_IntTwo (tag_t)"INT2"
#endif

#ifndef ARMulCnf_Warn
# define ARMulCnf_Warn (tag_t)"WARN"
#endif

#ifndef ARMulCnf_Waits
# define ARMulCnf_Waits (tag_t)"WAITS"
#endif



typedef struct {
  unsigned long  TimerLoad;             /* Timer load register */
  unsigned long  TimerValue;            /* Current Timer value register */
  unsigned long  TimerControl;          /* Timer Control register */
  unsigned long  TimerControlChangeMask; /* internal use change mask register */
  int enabled;  /* mirror of TimerControl register bit 7 */
  int mode;     /* mirror of TimerControl register bit 6 */
  int prescale; /* mirror of TimerControl register bits 2..3 */
  int oldPrescale; /* Previous prescale - not used currently */
  unsigned long clockRate; /* The clock speed, read from armul.cnf in MemInit */
  int interruptNumber;  /* The interrupt signal number for this timer */
  ARMTime nextEventTime;  /* In case we need to remove it!! */
    struct ARMulif_TimedCallback *timedCallback;

    ARMul_TimedCallBackProc *fn;          /* ptr to timeout function  */
  unsigned TimerNumber; /* 1 or 2, for debugging */
    unsigned dbgIndex; /* Number of timeouts, for debugging. */
} timer_state;

static ARMul_BusPeripAccessFunc Timer_Access;


static ARMul_TimedCallBackProc Timer1Timeout;
static ARMul_TimedCallBackProc Timer2Timeout;


BEGIN_STATE_DECL(Timer)
    GenericAccessCallback **intcInterface;

    ARMul_BusPeripAccessRegistration my_bpar;

  timer_state  timer1;          /* timer 1 structure */
  timer_state  timer2;
  int  useCoreEventSystem;      /* Use memory or core cycle event system */
  int warn;                     /* Warn users about invalid register accesses*/
  int waits;                    /* Number of idle cycles before access */
  int waitStates;               /* stored number of wait states */
  int accessState;              /* Current access state */
END_STATE_DECL(Timer) /*} tic_state; */

static RDI_InfoProc TimerRDIInfo2;

NamedMethodFunc TimerInterfaceFuncs[] = {
        {"RDI_InfoProc",(GenericMethodFunc*)TimerRDIInfo2},
        {NULL,NULL}
};


static int TimerRDIInfo2(void *handle, unsigned subcode,
                         ARMword *arg1, ARMword *arg2)
{
    TimerState *state = (TimerState*)handle;
#ifdef VERBOSE_TimerRDIInfo2
    Hostif_PrettyPrint(state->hostif,state->config,
                       "\n*** Timer RDIInfo2:0x%04x ***\n", subcode);
#endif
    switch (subcode)
    {
    case RDIInfo_QueryMethod:
#ifdef VERBOSE_TimerRDIInfo2
        Hostif_PrettyPrint(state->hostif,state->config,"\n*** Timer FindMethodInterface ***\n");
#endif
        return FindMethodInterface((GenericMethod *)arg2,(char const *)arg1,
                                   state,&TimerInterfaceFuncs[0]);
    case RDIInfo_RouteLinks:
        /* Q: Do we want fake-time, or any other static scheduling scheme,
         *    during makelinks? */
        
        
        /* !Todo: Form a (faster) link to the interrupt-controller here. */
        /* (1) Request an InterfaceSource for Intc. */
        /* (2) Ask that for an interrupt-input. */
        /* (3) Throw away the old callback, if we got one. */
#ifdef VERBOSE_TimerRDIInfo2
        Hostif_PrettyPrint(state->hostif,state->config,"\n*** Timer RouteLinks ***\n"); 
#endif
        return RDIError_UnimplementedMessage; /* Carry on! */
    default:
        return RDIError_UnimplementedMessage;
    }
}


BEGIN_INIT(Timer)
    Hostif_PrettyPrint(state->hostif, config, ", Timer");
{
  unsigned int TIC_Clk = ToolConf_DLookupUInt(config, ARMulCnf_CLK,
                                                20000000);
  unsigned int tic1IntNum =  ToolConf_DLookupUInt(config, ARMulCnf_IntOne, 4);
  unsigned int tic2IntNum =  ToolConf_DLookupUInt(config, ARMulCnf_IntTwo, 5);
  int warn =        ToolConf_DLookupBool(config, ARMulCnf_Warn, FALSE);
  int waits =       ToolConf_DLookupUInt(config, ARMulCnf_Waits, 0 /*1*/);
 
  if ( waits < 0   || waits > 30)
  {
      /*ARMul_PrettyPrint*/
      Hostif_ConsolePrint(state->hostif, "(Timer Error: Invalid "
                          "wait state value - defaulting to zero waits)");
      waits = 0;
  }
  {
      TimerState *ts = state;
      ts->timer1.clockRate=TIC_Clk; /* as read from armul.cnf */
      ts->timer1.interruptNumber = tic1IntNum; /* as read from armul.cnf */
      ts->timer1.nextEventTime=0l;  /* In case we need to remove it!! */
      ts->timer1.fn = Timer1Timeout; /* The timeout function to call */
      ts->timer1.TimerNumber = 1;
      
      ts->timer2.clockRate =TIC_Clk; /* as read from armul.cnf */
      ts->timer2.interruptNumber = tic2IntNum; /* as read from armul.cnf */
      ts->timer2.nextEventTime=0l;  /* In case we need to remove it!! */
      ts->timer2.fn = Timer2Timeout; /* The timeout function to call */
      ts->timer2.TimerNumber = 2;

      ts->warn = warn;
      ts->waitStates = waits;

      /* Get the Interrupt controller interface structure pointer.
       */
      ts->intcInterface = ARMulif_GetInterruptController(&state->coredesc);
  }


}

    { unsigned err = RDIError_NoError;
    /* Provide access-callback */
    state->my_bpar.access_func = Timer_Access;
    state->my_bpar.access_handle = state;
    state->my_bpar.capabilities = PeripAccessCapability_Minimum;
    err = ARMulif_ReadBusRange(&state->coredesc, state->hostif,
                         ToolConf_FlatChild(config, (tag_t)"RANGE"),
                         &state->my_bpar,  
                         0x0a800000,0x40,"");
    if (err)
    {
        Hostif_PrettyPrint(hostif,config,
                           "\nTimer got err=%i from ReadBusRange\n", err);
        return err;
    }

    if (!err)
    {
        err = state->my_bpar.bus->bus_registerPeripFunc(BusRegAct_Insert, 
                                                        &state->my_bpar);
    }
    if (err)
    {
        Hostif_PrettyPrint(hostif,config,
                           "\nTimer got err=%i from bus_registerPeripFunc\n",
                           err);
        return err;
    }
    }
    ADD_INTERFACE_SOURCE(TimerInterfaceFuncs)
END_INIT(Timer)

BEGIN_EXIT(Timer)
END_EXIT(Timer)



/* ******************* */
/* Static declarations */
/* ******************* */

static void KickOffTimer(TimerState *ts, timer_state *myTimer,
                         unsigned long alternateLoadValue,
                         bool bRepeating);
static int iCheckIntcIf(TimerState *ts);
static void PrescaleValueChanged(TimerState *ts, timer_state *myTimer);



    /*
Function Name: RunTimerAgain
Parameters: 
        TimerState *ts - pointer to TimerState
        timer_state *myTimer - pointer to timer_state
Return: void
Description: Called when Timeout occurs. The purpose of the function
        is to determine if the timer needs to be restarted and if so
        whether it should be as periodic or free running.
        Free running reloads the timer with 0xFFFF on timeout
        Periodic reloads the timer with the Load value on timeout.
        
*/
static void RunTimerAgain(TimerState *ts, timer_state *myTimer)
{

/* 
   Depending on the mode we reload and start again - or we
   just let the timer free run.
*/

#ifdef VERBOSE_RUNAGAIN
    printf("RunTimerAgain.\n");
#endif

if ( myTimer->enabled == 1 )
 {
  /* Timer is enabled - this check is really belt and braces - the
     chances of a timeout occuring AND the timer being disabled
     at the same time are relatively slim and the consequent 
     possible timer restart could be attributed to race conditions.
     However it is relatively inexpensive to do the check - so why not.
  */
  if ( myTimer->mode ==0)
    {
    /* If timer is in free running mode then the timer will continue to
       decrement from its maximum value. Mode bit=0
    */
     KickOffTimer(ts,myTimer, 0xffff, TRUE); /* Use maximum run time */

    }
  else
    {
    /* If timer is in periodic mode then the timer will reload from the load
       register and continue to decrement. Mode bit =1
    */ 
    KickOffTimer(ts,myTimer,0, TRUE); /* Use Load value */

    }


 } /* if timer->enable ==1 */
}


/*
 * Return: 1 for failure, 0 for success 
 */
static int iCheckIntcIf(TimerState *ts)
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
            return 1;
        }
  }
  return 0;
}


/* 
Function Name: Timer1Timeout.
Parameters: void *handle - This is void * because we are re-using the
                        armul_Event typedef for pointers to event functions
                        and this specified that a void* is used instead
                        of a specific type.
Return: unsigned - always return 0; 
Description: This function is registered as the function to call when timer 1
                times out.

Notes:  Note that we check that the intcInterface has been installed correctly
        by MemInit - if it hasn't then we do a late installation.
        The check is prudent for pointers to functions/handles.
        The late installation is an alternative to bombing out - this
        is really a protection against user who don't read the documentation
        and ignore the warnings that intc MUST be the first peripheral
        in the peripheral list in armul.cnf. - or for users who have simply
        left out the intc from their armul.cnf.  These checks could be removed
        to slightly improve performace for very timer intensive systems.
*/
static void Timer1Timeout(void *handle)
{
    TimerState *ts = (TimerState*)handle;
    timer_state *myTimer = &ts->timer1;
    
#ifdef VERBOSE_TIMEOUT
    printf("Timer:1:Timeout.\n");
#endif
        myTimer->timedCallback = NULL;
    /* Check that we have the intc interface installed */
    if (iCheckIntcIf(ts))
    {
#ifdef VERBOSE_NOINTC
        printf("Timer:1: no interrupt controller available.\n");
#endif
        return;
    } 
    {
        GenericAccessCallback *cb1 = *ts->intcInterface;
        ARMword data = 1;
        cb1->func(cb1,myTimer->interruptNumber,&data,ACCESS_WRITE_WORD);
    }
    
    /* See if the timer should be restarted - and if so in what mode */
    RunTimerAgain( ts, myTimer);
}


/* See TimerTimeout for function comment block */
static void Timer2Timeout(void *handle)
{
    TimerState *ts = (TimerState*)handle;
    timer_state *myTimer = &ts->timer2;

#ifdef VERBOSE_TIMEOUT
    printf("Timer:2:Timeout.\n");
#endif
        myTimer->timedCallback = NULL;
    /* Check that we have the intc interface installed */ 
    if (iCheckIntcIf(ts))
    {
#ifdef VERBOSE_NOINTC
        printf("Timer:2: no interrupt controller available.\n");
#endif
        return;
    }
    {
        GenericAccessCallback *cb1 = *ts->intcInterface;
        ARMword data = 1;
        cb1->func(cb1,myTimer->interruptNumber,&data,ACCESS_WRITE_WORD);
    }
    RunTimerAgain( ts,  myTimer);
}





/*
Function Name: DivisorValue
Parameters: ARMul_State *state - the original ARMul_State
            int prescale - the prescale bits from intc->control shifted right 2 places
Return: int - the Clock Divisor value represented by the prescale bits passed in.
                these will be one of 1, 16, 256 or controversially 65536.
                The 65536 value is returned for the undefined result bits 2..3 == 11 
Description: See Return description.
*/
static int DivisorValue(TimerState *state, int prescale)
{
 int ClockDivisor;

 switch(prescale)
    {
    case 0: ClockDivisor = 1;
            break;
    case 1: ClockDivisor = 16;
            break;
    case 2: ClockDivisor = 256;
            break;
    default: Hostif_ConsolePrint(state->hostif,"TIC Divisor value (%d) "
                    "has undefined behaviour - (using 65536)\n",prescale);
            ClockDivisor = 65536;
            break;
    }
 return ClockDivisor;
}

static ARMTime getNow(TimerState *ts)
{

        /* use BCLK */
        /*  ARMulif_BusTime(&ts->coredesc); */
#ifdef VERBOSE_GETNOW
        printf("getNow->0x%08x=%u bus:%p BU:%p\n", 
                   (unsigned)ts->my_bpar.bus->bus_BusyUntil,
                   (unsigned)ts->my_bpar.bus->bus_BusyUntil,
                   ts->my_bpar.bus, &ts->my_bpar.bus->bus_BusyUntil);
#endif
        return ts->my_bpar.bus->bus_BusyUntil;

}


/*
Function Name: KickOffTimer
Parameters: TimerState *ts - the TimerState pointer
            timer_state *myTimer - the timer_state pointer
            unsigned long alternateLoadValue -  if not 0 then this is the value to use for reload.
Return: void
Description: This function is called to start/restart a timer. 
        It calculates how long from now the timer would timeout
        for the supplied values of load, prescale. An event is 
        scheduled for the 'timer timerout' time. The function 
        to be called at timeout time is from the timer_state 
        structure.

Notes: In normal use the parameter alternateLoadValue should be zero. 
        When this is zero the timer's own load register is used to 
        calculate the timer delay. However if you need to restart the 
        timer with a different load value then you may supply an
        alternate load value. An example of the usage of this is for 
        free running timer restarts where the load value is 0xFFFF 
        for restart.
 */
static void KickOffTimer(TimerState *ts, timer_state *myTimer,
                         unsigned long alternateLoadValue,
                         bool bRepeating)
{
    unsigned long delay;
    int ClockDivisor;
    unsigned long loadValue;
    ARMTime Now;
    
    if ( myTimer->enabled ==0 ) 
    {
        /*  timer disabled - user probably wrote to load value before
         *  timer was enabled
         */
        return;
    }

        Now = getNow(ts);
    
    /* Select the loadValue to use - either the reload value or a supplied
       value ( if it is non zero ) 
       */
    /* Make sure it isn't bigger than the allowable 16 bits */
    alternateLoadValue &= 0xFFFF;
    loadValue = alternateLoadValue ? alternateLoadValue : myTimer->TimerLoad;


 /* Calculate the delay */
 ClockDivisor = DivisorValue(ts, myTimer->prescale); 
 delay =  (ClockDivisor * loadValue);

 /*
  * Correct for the difference between now and when we expected to
  * be called.
  */
 if (myTimer->nextEventTime != 0)
 {
     int delta = (int)(Now - myTimer->nextEventTime);
     assert(delta >=0);
     
     if (delta > 0 && delay > (unsigned long)delta)
     {
         delay -= delta;
     }
 }


 if (bRepeating)
     delay += ClockDivisor; /* Period = 1 + reload-value. */

 
 delay = delay ? delay : 1; /* forbid 0 delays */
 /* Now submit the event and record the time at which it occurs 
    for removal purposes */
 myTimer->nextEventTime = Now + delay;
    myTimer->timedCallback = ARMulif_ScheduleTimedCallback(
        &ts->coredesc, myTimer->fn, ts, myTimer->nextEventTime,
        ARMulCallbackID_ScheduleFunctionEvery /*BCLK */,
        0 /* absolute, UPDATE-time-only.. */
        );
        /* handle, ARMTime when, ARMTime period) */
    assert(myTimer->timedCallback != NULL);

#ifdef VERBOSE_KICKOFF
    myTimer->dbgIndex++;
    printf("KickOffTimer:%u:[%u] when:%lu t=%lu delay=%lu\n", 
           myTimer->TimerNumber,myTimer->dbgIndex,
           (unsigned long)myTimer->nextEventTime,
           (unsigned long)Now, delay);
#endif
 
}


/* 
Function Name: ValueRegRead
Parameters: TimerState *ts
            timer_state *myTimer
Return: ARMword - the number of timer counts remaining until timeout.
Description: Work out the numer of counts remaining - this is for
         value register reads
*/
static ARMword ValueRegRead(TimerState *ts, timer_state *myTimer )
{
 int ClockDivisor;
 ARMTime now, diff;
 now = getNow(ts);

 ClockDivisor = DivisorValue(ts, myTimer->prescale); 

 /* Difference between the time the event is scheduled and current time */
 diff = myTimer->nextEventTime - now;

/* 
 Return the 'counts' remaining 
 */
return (ARMword)(diff / ClockDivisor);

}



/* 
Function Name: PrescaleValueChanged
Parameters: TimerState *ts
            timer_state *myTimer
Return: void
Description: When the timer is active and the prescale value is changed
        we have a problem. For the real hardware the prescaled just starts
        dividing at a different rate and the timeout will happen at the
        correct time. Because we don't actually count the clocks, instead
        we calculate the timeout time when the imter is started, we have
        to retract the first event and work out what the *NEW* timeout 
        time will be. Luckily it is only the prescale value that causes
        this problem - the load register is only read when the timer restarts,
        the mode bit is read at timeout only.
*/
static void PrescaleValueChanged(TimerState *ts, timer_state *myTimer)
{
 ARMword remaining;

 /* OH - the prescale has changed
 we have to do the following;
 1. Retract the scheduled timeout event.
 2. Work out what the 16 bit down counter value
   actually was
 3. Recalculate the timeout time and re-schedule
 */      
 if (myTimer->timedCallback != NULL)
     ARMulif_DescheduleTimedCallback(&ts->coredesc, myTimer->timedCallback, 
                            ARMulCallbackID_ScheduleFunctionEvery/*BCLK*/);

 myTimer->timedCallback = NULL;
 myTimer->nextEventTime = 0;
 remaining = ValueRegRead(ts, myTimer);
 KickOffTimer(ts,myTimer, remaining, FALSE);
}



/*
Function Name: TimerClearInterrupt
Paremeters: TimerState *ts
            timer_state *myTimer
Return: void
Description: Clears the interrupt source
*/
static void TimerClearInterrupt(TimerState *ts, timer_state *myTimer)
{
    /* Check that we have the intc interface installed */
    if (! iCheckIntcIf(ts))
    {
        /* Now tell the interrupt controller that our
         * interrupt source has cleared */
        {
            GenericAccessCallback *cb1 = *ts->intcInterface;
            ARMword data = 0; /* Signal_Off */
            cb1->func(cb1,myTimer->interruptNumber,&data,ACCESS_WRITE_WORD);
        }
    }
}


/*
Function Name: TICControlRegisterWrite
Parameters: TimerState *ts, 
        timer_state *myTimer, 
        ARMword *word   pointer to  data to use for write.
Return: int - always return 0
Description: Breakout function for TICRegisterAccess - the final address decoder - to
        keep the complexity of one function down a little to keep it readable - this
        is hand generated not auto-generated code.
        This function performs the processing for a write to the timer counter control
        registers. It updates the mirror struct members such as enabled/prescale.
        It processes changes to enable and prescale.
*/
static int TICControlRegisterWrite(TimerState *ts, timer_state *myTimer, ARMword *word)
{
 /* Update the internal representation first */
 myTimer->TimerControlChangeMask = (myTimer->TimerControl) ^ (*word) ;
 myTimer->TimerControl = *word ;
 myTimer->enabled = (*word >> 7) & 0x1;
 myTimer->mode =  (*word >> 6) & 0x1;
 myTimer->oldPrescale = myTimer->prescale; /* copy old */
 myTimer->prescale = ( *word >> 2) & 0x3;
 
 /* Now process any changes */


 /* Has the Prescale value changed? */

 if ( ( (myTimer->TimerControlChangeMask >> 2) & 0x3))
     {
 
      /* Yes the prescale has changed
        we have to do the following;
        1. Retract the scheduled timeout event.
        2. Work out what the 16 bit down counter value actually was
        3. Recalculate the timeout time and re-schedule
                   
        If the timer is enabled then we need to do something
        otherwise it is just a value change and can pass unnoticed. 
      */         

     if ( myTimer->enabled )
         {
         ARMTime Now = getNow(ts);
         /* Only ditch the event if one is pending! If one isn't pending then
          * the prescale change will be dealt with when the next event is
          * scheduled. At this point we are only concerned with removing events
          * that we calculated the timeout for using the old prescale value!
          */
 
         if ( Now <= myTimer->nextEventTime)
             {
             PrescaleValueChanged(ts, myTimer);
             }
         }
    }



 /* Has the timer been Enabled or disabled? */

  if ( ( (myTimer->TimerControlChangeMask >> 7) & 0x1))
      {
       /* Enabled bit changed */
      if (myTimer->enabled)
          {
          /* We need to kick off the timer NOW */
          KickOffTimer(ts,myTimer,0, FALSE);
          }
      else
         {
         /* We have been disabled - so we need to grab
            the scheduled timeout event from the queue
         */
             if (myTimer->timedCallback != NULL)
                 ARMulif_DescheduleTimedCallback(&ts->coredesc,
                                                 myTimer->timedCallback, 
                             ARMulCallbackID_ScheduleFunctionEvery/*BCLK*/);
             myTimer->timedCallback = NULL;
             myTimer->nextEventTime = 0;
         }
     }

 /* There is nothing to do for mode changes - these are dealt with
    at timeout time.
 */ 


 return 0;
}   



/*
Function Name: TICRegisterAccess
Parameters:     TimerState *ts,  -TimerState pointer
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
static int TICRegisterAccess(TimerState *ts,ARMword addr,ARMword *word,
                             unsigned /*ARMul_acc*/ acc)
{
    unsigned long  maskedAddress = addr & 0x0000003F;
#ifdef VERBOSE_ACCESS
    printf("TICRegisterAccess: A:%08lx D:%08lx.\n",(unsigned long)addr,
           (word==NULL)?0UL:(unsigned long)*word);
#endif

    /* Yes it is a memory access - now is it a read or a write? */
    if ( ACCESS_IS_READ(acc) )
    {
        /* Read from registers */
    switch ( maskedAddress)
        {
        case 0x00:      /* Timer1Load */
                /* Only the bottom 16 bits are significant */
                *word = ts->timer1.TimerLoad & 0x0000FFFF;
                break;
        case 0x04:      /* Timer1Value */
                /* We need to work out what this value is at present. */
                *word = ValueRegRead(ts, &ts->timer1);
                break;
        case 0x08:      /* Timer1Control*/
                *word = ts->timer1.TimerControl;
                /* nothing to do apart from give it back */
                break;
        case 0x0c:      /* Reserved */
        case 0x10:
        case 0x2c:
        case 0x30:
                if ( ts->warn == TRUE) {
                Hostif_ConsolePrint(ts->hostif,"Warning - accessed Reserved "
                               "Timer Counter register address %08x\n",addr);
                }
                return PERIP_NODECODE; /*-2*/
                break;
        case 0x20:      
                /*Timer2Load */
                /* Only the bottom 16 bits are significant */
                *word = ts->timer2.TimerLoad & 0x0000FFFF;
                break;
        case 0x24:      
                /*Timer2Value */
                /* We need to work out what this value is at present. */
                *word = ValueRegRead(ts, &ts->timer2);
                break;
        case 0x28:      
                /*Timer2Control */
                *word = ts->timer2.TimerControl;
                /* nothing to do apart from give it back */
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
        case 0x00:      
                /* Timer1Load */
                /* Only the bottom 16 bits are significant */
                 ts->timer1.TimerLoad = *word & 0x0000FFFF;
                /* QQQ We may need to kick off the timer NOW 
                   we may also need to retract an event */
                /* Retract any existing events */
                if ( ts->timer1.nextEventTime != 0)
                {
                    if (ts->timer1.timedCallback != NULL)
                        ARMulif_DescheduleTimedCallback(&ts->coredesc,
                                                ts->timer1.timedCallback, 
                             ARMulCallbackID_ScheduleFunctionEvery/*BCLK*/);
                    /* free(ts->timer1.timedCallback); */
                    ts->timer1.timedCallback = NULL;
                    ts->timer1.nextEventTime=0;
                }
                /* Kick off the timer */
                KickOffTimer(ts,&ts->timer1, 0, FALSE);

                break;
        case 0x08:      
                /* Timer1Control*/
                TICControlRegisterWrite(ts,&ts->timer1, word);
                break;
        case 0x0c:      
                /* Timer1Clear */
                TimerClearInterrupt(ts, &ts->timer1);
                break;
        case 0x4:       /* Reserved */
        case 0x10:
        case 0x24:
        case 0x30:
                if ( ts->warn == TRUE) {
                Hostif_ConsolePrint(ts->hostif,"Warning - accessed Reserved "
                               "Timer Counter register address %08x\n",addr);
                }
                return PERIP_NODECODE;
                break;
        case 0x20:      
                /*Timer2Load */
                /* Only the bottom 16 bits are significant */
                 ts->timer2.TimerLoad = *word & 0x0000FFFF;
                /* Retract any existing events */
                if ( ts->timer2.nextEventTime != 0)
                {
                    if (ts->timer2.timedCallback != NULL)
                        ARMulif_DescheduleTimedCallback(&ts->coredesc,
                                           ts->timer2.timedCallback,
                            ARMulCallbackID_ScheduleFunctionEvery/*BCLK*/);
                    /* free(ts->timer2.timedCallback); */
                    ts->timer2.timedCallback = NULL;
                    ts->timer2.nextEventTime=0;
                }
                /* Kick off the timer */
                KickOffTimer(ts,&ts->timer2, 0, FALSE);
                break;
        case 0x28:      
                /*Timer2Control */
                TICControlRegisterWrite(ts,&ts->timer2, word);
                break;
        case 0x2c:      
                /* Timer2Clear */
                /* Clear the interrupt source */
                TimerClearInterrupt(ts, &ts->timer2);
                break;
        default:
                return PERIP_NODECODE;  /* Failed to decode address */
        }

    }
    return PERIP_OK;  /* Successful */
}


/* Bus state machine */

static int BusState(TimerState *ts, ARMword addr, ARMword *word,
                    unsigned /*ARMul_acc*/ acc)
{
    if (!ACCESS_IS_REAL(acc)) /* acc_ACCOUNT() */
    {
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
            return TICRegisterAccess(ts,addr,word,acc);
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
static int Timer_Access(void *handle, 
                        struct ARMul_AccessRequest *req)
{
    ARMWord address = req->req_address[0];
    ARMWord *data = req->req_data;
    unsigned type = req->req_access_type;
    TimerState *state=(TimerState *)handle;
   
    assert(address >= state->my_bpar.range[0].lo && \
           address <= state->my_bpar.range[0].hi);

    /* We have idenfitied a TIC memory access */
    return BusState(state,address,data,type);
}



/*--- <SORDI STUFF> ---*/


#define SORDI_DLL_DESCRIPTION_STRING "Timer (a Reference Peripheral)"
#define SORDI_RDI_PROCVEC Timer_AgentRDI
#include "perip_sordi.h"

#include "perip_rdi_agent.h"
    IMPLEMENT_AGENT_PROCS_NOEXE_NOMODULE(Timer)
    IMPLEMENT_AGENT_PROCVEC_NOEXE(Timer)

/*--- </> ---*/






/* EOF timer.c */





