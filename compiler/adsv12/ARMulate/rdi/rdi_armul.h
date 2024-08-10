/*
 * ARM RDI - RDIInfo codes: rdi_armul.h
 * Copyright (C) 1994-1999 Advanced RISC Machines Ltd.
 * Copyright (C) 2000 - 2001 ARM Limited. All rights reserved.
 */

/*
 * RCS $Revision: 1.13.2.10 $
 * Checkin $Date: 2001/09/04 12:57:23 $
 * Revising $Author: lmacgreg $
 */

#ifndef rdi_armul__h
#define rdi_armul__h


#ifndef  rdi_info_h
# include "rdi_info.h"  /* for ARMulInfo_Base */
#endif


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

#define ARMulInfo_Base RDIInfo_Reserved_ARMulateSubAgents_FirstInfo



/*
 *        Function: RDIInfo_DescribeCallback
 *         Purpose: Tell a debugger about one or more callbacks.
 *
 *          Handle: Module
 *
 *          Params:
 *                   In: *((RDI_EventType *)arg1): Identifies the set of
 *                       events for which the callback applies.
 *                         If arg1==NULL then list all the callbacks.
 *                  Out: *((RDI_CallbackDesc *)arg2): Description of the
 *                       callback.
 *
 *         Returns: RDIError_NoError: 
 *                  RDIError_UnimplementedMessage: Module doesn't describe
 *                                                 callbacks.
 *                  RDIError_UnimplementedType: Module doesn't describe
 *                                                 this callback.
 */
#define RDIInfo_DescribeCallback        (ARMulInfo_Base+0)
/*
 *        Function: RDIInfo_RegisterCallback
 *         Purpose: Give a callback (method-pointer) to a module to be
 *                  called when an event occurs.
 *
 *          Handle: Module
 *
 *          Params:
 *                   In: *((RDI_EventType *)arg1): Identifies the set of
 *                       events for which the callback applies.
 *                   In: *((RDI_CallbackDesc *)arg2): Description of the
 *                       callback to be registered.
 *
 *         Returns: RDIError_NoError: Callback accepted
 *                  RDIError_UnimplementedMessage: Callback not accepted
 *
 * Post-conditions: If the callback was (successfully) registered, it has
 *                  been added to a list of callbacks for that class of event.
 *                  Note that there is no provision for chained callbacks.
 */
#define RDIInfo_RegisterCallback        (ARMulInfo_Base+1)
#define RDIInfo_DeregisterCallback      (ARMulInfo_Base+2) /* 0x0762 */
#define RDIInfo_QueryCallback           (ARMulInfo_Base+3) /* 0x0763 */
/*
 *        Function: RDIInfo_QueryBus
 *         Purpose: Return a pointer to an ARMul_Bus structure, on which
 *                  a peripheral can register it's access-range(s).
 *
 *          Handle: Module
 *
 *          Params:
 *                   In: (char *)arg1: String which identifies the set of
 *                       events for which the callback applies.
 *                   Out: *(ARMul_Bus**)arg2): Pointer to the ARMul_Bus.
 *
 *         Returns: RDIError_NoError: Callback accepted
 *                  RDIError_UnimplementedMessage: Callback not accepted
 */
#define RDIInfo_QueryBus                (ARMulInfo_Base+4) /* 0x0764 */
/*
 * Purpose: Return a pointer to an ARMul_MemInterface structure.
 * Handle: Module
 *
 * Params:
 *         In: *arg1: ID of the memory-interface, as defined in
 *                    armulif/armul_callbackid.h
 *         Out: *(ARMul_MemInterface**)arg2): Pointer to the interface.
 *
 * Returns: RDIError_NoError: Interface exists.
 *          RDIError_UnimplementedMessage: No such interface.
 */
#define RDIInfo_QueryMemInterface       (ARMulInfo_Base+5) /* 0x0765 */

/* ]]] End of UNDER-CONSTRUCTION */



/*
 *        Function: RDIInfo_RaiseEvent
 *         Purpose: A peripheral can raise (any) event.  It will be
 *                  propagated to those components that have registered
 *                  with the ARMulCallbackID_EventHandler Callback.
 *
 *  Pre-conditions: None
 *
 *          Params:
 *              In: handle - Handle of the processor MODULE that the
 *                           peripheral is attached to
 *                  reason - RDIInfo_RaiseEvent
 *                  arg1   - (struct ARMul_Event *)
 *                  arg2   - NULL
 *
 *         Returns: RDIError_NoError: Event raised
 *                  RDIError_UnimplementedMessage: The module cannot raise
 *                  events (should not happen!)
 *
 * Post-conditions: 
 *                                  
 */
typedef struct ARMul_Event
{
    unsigned event;
    uint32 data1;
    uint32 data2;
} ARMul_Event;

#define RDIInfo_RaiseEvent (ARMulInfo_Base+6) /*0x0766 */


/*
 * List of predefined events (ARMul_Event.event)
 * NB Events are classified by bits 16..20, which define
 * a number from 0x00 to 0x1f.
 * Event-listeners are registered with a bit-mask of classes of
 * events which they care about, so an event-mask of 3 means
 * CoreEvents + MMUEvents.
 *x
 * In order to minimise the total cost of these events, we aim to
 * split them into classes such that most classes either
 * (a) normally have no event-handlers registered, or
 * (b) describe events which are very rare (but may often have many listeners).
 *
 */

/* Core events - these happen fairly often, but not every instruction.
 * In a normal (untraced) system, there should be no listener to these.
 * (Coprocessors which need to know if the core is in a privileged mode
 *  ask the core, on the assumption that such coprocessor-accesses are rarer
 *  than these events.)
 */
#define CoreEvent                0x00000
#define CoreEventSel (1 << (CoreEvent >> 16))


#define CoreEvent_UndefinedInstr 0x00002
#define CoreEvent_SWI            0x00003
#define CoreEvent_PrefetchAbort  0x00004
#define CoreEvent_DataAbort      0x00005
#define CoreEvent_AddrExceptn    0x00006
#define CoreEvent_IRQ            0x00007
#define CoreEvent_FIQ            0x00008
#define CoreEvent_Breakpoint     0x00009 /* processor hit a breakpoint */
#define CoreEvent_Watchpoint     0x0000a /* processor accessed a watchpoint */

/* These are now deprecated: Use SignalEvent's instead */
#if 1
# define CoreEvent_IRQSpotted     0x00017 /* Before masking with "IF" flags */
# define CoreEvent_FIQSpotted     0x00018 /* to see if it will be taken */
#endif

#define CoreEvent_ModeChange     0x00019


/* Events defined by MMUlator: 
 * These are fairly frequent, and will cause slight slowdown.
*/
#define MMUEvent                 0x10000
#define MMUEventSel (1 << (MMUEvent >> 16))
#define MMUEvent_DLineFetch      0x10001 /* or I line fetch in mixed cache */
#define MMUEvent_ILineFetch      0x10002
#define MMUEvent_WBStall         0x10003
#define MMUEvent_DTLBWalk        0x10004 /* or I TLB walk in mixed cache */
#define MMUEvent_ITLBWalk        0x10005
#define MMUEvent_LineWB          0x10006 /* i.e. replacing a dirty line */
#define MMUEvent_DCacheStall     0x10007 /* line fetch had to stall core */
#define MMUEvent_ICacheStall     0x10008

/* This class of events, like the trace events, is very common,
 * so any listener registered will cause a noticable slowdown. */
#define PipeEvent                0x20000
#define PipeEventSel (1 << (PUEvent >> 16))
/* Events from the ARM8 Prefetch Unit */
#define PUEvent                  0x20000
#define PUEventSel (1 << (PUEvent >> 16))
#define PUEvent_Full             0x20001 /* PU fills causing Idle cycles */
#define PUEvent_Mispredict       0x20002 /* PU incorrectly predicts a branch */
#define PUEvent_Empty            0x20003 /* PU empties causing a stall */
/* Events form ARM8, ARM9 */
#define PipeEvent_Dependency     0x20004

/* Debug events */
#define DebugEvent               0x40000
#define DebugEventSel (1 << (DebugEvent >> 16))
#define DebugEvent_InToDebug     0x40001
#define DebugEvent_OutOfDebug    0x40002
#define DebugEvent_DebuggerChangedPC 0x40003
#define DebugEvent_InToSemihost  0x40004
#define DebugEvent_OutOfSemihost 0x40005
#define DebugEvent_TrickboxRead  0x40006
#define DebugEvent_TrickboxWrite 0x40007
#define DebugEvent_RunCodeSequence 0x40008
#define DebugEvent_EndCodeSequence 0x40009
/* These are very rare events, so we don't want to 
 * mix them with CoreEvents. */
#define ConfigEvent              0x50000
#define ConfigEventSel (1 << (ConfigEvent >> 16))
#define ConfigEvent_AllLoaded    0x50001
/* Note that ConfigEvent_Reset is sent to all listeners in a
 * poorly-defined order. Any model which does something to other models
 * (e.g. pagetables) should register an ARMulCallbackID_PrioritisedReset.
 */
#define ConfigEvent_Reset        0x50002
#define ConfigEvent_VectorsLoaded 0x50003 /* from semihost to FPE. */



#define ConfigEvent_EndiannessChanged 0x50005

/* This means the plugins are about to be unloaded (lots of closeagent()
 * calls are about to happen. It can be used by memory-models which
 * want to save their contents before the base-memory-model is removed. */
#define ConfigEvent_AboutToClose    0x50006

#define ConfigEvent_RouteLinks   0x50007  /* data1:0=>make, data1:1=>check. */
#define ConfigEvent_HighVecChanged 0x50008

/* Trace events */
#define TraceEvent               0x60000
#define TraceEventSel (1 << (TraceEvent >> 16))
#define TraceEvent_TraceLine     0x60001

/* Signals, in the same order as RDIPropID_ARMSignal_RESET ...
 * (see rdi_priv.h)
 * NB These are signals into the core.
 */
#define SignalEvent              0x7000
#define SignalEventSel (1 << (SignalEvent >> 16))
#define SignalEvent_RESET        0x7001
#define SignalEvent_UNDEF        0x7002
#define SignalEvent_SWI          0x7003 /
#define SignalEvent_PABORT       0x7004
#define SignalEvent_DABORT       0x7005
#define SignalEvent_SE           0x7006
#define SignalEvent_IRQ          0x7007
#define SignalEvent_FIQ          0x7008
#define SignalEvent_BreakPt      0x7009
#define SignalEvent_WatchPt      0x700A
#define SignalEvent_Stop         0x700B
/* There may be more, later. */




/* User defined events */
#define UserEvent_Base          0x100000
#define UserEvent_Top           0x1fffff



/*
 *        Function: RDIInfo_RegisterRDIInfoExtension
 *         Purpose: Register an "unknown" RDI info handler.
 *
 *          Handle: Module or Agent.
 *
 *          Params:
 *                   In: (RDI_InfoProc *)arg1 - the infoproc.
 *                       (void*)arg2          - the handle.
 *
 *         Returns: RDIError_NoError: Callback registered
 *                  RDIError_UnimplementedMessage: Callback not registered
 */


/*
 *        Function: RDIInfo_AllLoaded
 *         Purpose: Announce that all models have been instantiated,
 *                  and all links have been created,
 *                  and memory can be written by PageTables or Demon.
 *          Handle: Agent or Module.
 *          Params: none.
 *         Returns: RDIError_UnableToInitialise: Fatal error.
 *                  anything else: Ok.
 * NB Each SORDI target contains its own Agent, and it is the responsiblity
 *  of that agent to pass this call on to all sub-agents it has loaded.
 *  An Agent may pass the call on to the modules it created, but for
 *  simple peripherals it services the call itself.
 *  This call is sent as the last action of ARMulator's OpenAgent.
 */
#define RDIInfo_AllLoaded               (ARMulInfo_Base+7) /*0x0767*/

#define RDIInfo_InstallRDIInfoExtension (ARMulInfo_Base+8) /*0x0768*/
#define RDIInfo_RemoveRDIInfoExtension (ARMulInfo_Base+9)  /*0x0769*/
#define RDIInfo_AddARMulPropertyDesc  (ARMulInfo_Base+10)  /*0x076A*/

/*
 * Sent by the top-level agent to all the plugin agents it owns just before
 *   CloseAgent is called on them all.
 * (in armiss/armrdi.c, and armul810/multirdi.c - should it be there?).
 *   Allows them to shut down cleanly before all links become invalid.
 * I don't want plugin-agents to have to register to get this.
 */
#define RDIInfo_AboutToClose  (ARMulInfo_Base+11)  /*0x076B*/

/*
 *        Function: RDIInfo_QueryMethod
 *         Purpose: asks the target module or agent for an ARMul_Method
 *                  (see armulif/armul_method.h).
 *
 *          Handle: Module or Agent.
 *
 *          Params:
 *                In:   (char const *)arg1 - "name:type" of requested method.
 *               Out: *(ARMul_Method*)arg2 - Method.
 *
 *         Returns: RDIError_NoError: *arg2 filled in.
 *                  RDIError_UnimplementedMessage: requested method not found.
 */
#define RDIInfo_QueryMethod (ARMulInfo_Base+12)    /*0x076C*/

/* 
 *        Function: RDIInfo_RouteLinks
 * Called twice before 'AllLoaded' but after all instances created.
 * *arg1 is
 * 0 : Create links.
 * 1 : Check all expected links are created.
 */
#define RDIInfo_RouteLinks (ARMulInfo_Base+13)  /*0x076D*/




/* #define RDIInfo_ */
/* Extra index words are cpnum, coproif, cpreg. */
/* #ARMulProp_CoproRegisterAccess (RDIPropID_ARMulProp_Base+2) */



 


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /* def rdi_armul__h */
/* EOF rdi_armul.h */

