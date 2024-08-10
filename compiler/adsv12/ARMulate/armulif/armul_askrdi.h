/*
 * armul_askrdi.h
 * Functions to allow a peripheral to query its core.
 * Copyright (C) 1999 - 2001 ARM Ltd.  All rights reserved.
 *
 * RCS $Revision: 1.38.2.20 $
 * Checkin $Date: 2001/10/26 10:16:15 $
 * Revising $Author: dsinclai $
 */

#ifndef armul_askrdi__h
#define armul_askrdi__h

#include "rdi.h"
#include "armsignal.h" /* for ARMSignalType */
#include "simplelinks.h"
#include "armul_types.h" /* for typedef uint64 ARMTime; */
#include "rdi_armul.h"

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif


/*----------- CORE REGISTER ACCESS ---------*/
/*
 * Parameters:
 *        In: mdesc -- module description for the core.
 */
ARMword ARMulif_GetCPSR(RDI_ModuleDesc *mdesc);
ARMword ARMulif_GetSPSR(RDI_ModuleDesc *mdesc, ARMword mode);
unsigned ARMulif_GetMode(RDI_ModuleDesc *mdesc);
/*
 * Parameters:
 *        InOut: mdesc -- module description for the core.
 *           In: value -- word to be written to CPSR.
 */
void ARMulif_SetCPSR(RDI_ModuleDesc *mdesc, ARMword value);
void ARMulif_SetSPSR(RDI_ModuleDesc *mdesc, ARMword mode, ARMword value);

/* Beware!
 * Some models fail to flush the instruction-pipeline if you set R15 with this
 * call - use SetPC instead.
 */
void ARMulif_SetReg(RDI_ModuleDesc *mdesc, ARMword mode, unsigned reg,
                   ARMword value);
/* (This calls rdi CPUwrite with 1<<16 as the mask.) */
void ARMulif_SetPC(RDI_ModuleDesc *mdesc, ARMword value);

ARMword ARMulif_GetReg(RDI_ModuleDesc *mdesc, ARMword mode, unsigned reg);
ARMword ARMulif_GetPC(RDI_ModuleDesc *mdesc);

/* We no longer support 26-bit mode, so R15 and PC have the same values
 * (but see warning for SetReg above). */
#define ARMulif_SetR15 ARMulif_SetPC
#define ARMulif_GetR15 ARMulif_GetPC


/* -- coprocessor register access --
 *
 * WARNING - These routines may transfer more than one word, because
 * they see registers the same way as ADW and armsd do.
 */
int ARMulif_CPWrite(RDI_ModuleDesc *mdesc, unsigned cpnum, unsigned reg,
                    ARMword *data);

int ARMulif_CPRead(RDI_ModuleDesc *mdesc, unsigned cpnum, unsigned reg,
                   ARMword *data);






/* Deprecated, because slow.
 * Returns: 1 if priviledged mode, else 0.
 */
unsigned ARMulif_IsPrivileged(RDI_ModuleDesc *mdesc);


void ARMulif_StopExecution(RDI_ModuleDesc *mdesc, unsigned reason);

unsigned ARMulif_EndCondition(RDI_ModuleDesc *mdesc);

/*------------ MEMORY ACCESS VIA CORE -------------*/

/* Access functions for the memory system */
ARMword ARMulif_ReadWord(RDI_ModuleDesc *mdesc, ARMword address);
ARMword ARMulif_ReadHalfWord(RDI_ModuleDesc *mdesc, ARMword address);
ARMword ARMulif_ReadByte(RDI_ModuleDesc *mdesc, ARMword address);
/* Returns: RDIError. */
int ARMulif_WriteWord(RDI_ModuleDesc *mdesc, ARMword address, ARMword data);
int ARMulif_WriteHalfWord(RDI_ModuleDesc *mdesc, ARMword address, 
                          ARMword data);
int ARMulif_WriteByte(RDI_ModuleDesc *mdesc, ARMword address, ARMword data);

/*
 * SetSignal and GetProperty are "opposites".  Both access RDI_Properties, but
 * readable properties are "Properties", and writable properties are "Signals"
 */
void ARMulif_SetSignal(RDI_ModuleDesc *mdesc, ARMSignalType sigType,
                       SignalState sigState);

/* Value is output as an (ARMword *) to allow for properties > 32 bits */
void ARMulif_GetProperty(RDI_ModuleDesc *mdesc,
                         ARMSignalType id, ARMword *value);

/* Set a whole heap of CP15 Signals at once.
 * N.B. it is nearly always better to change the individual bits
 * using SetSignal.
 */
void ARMulif_SetConfig(RDI_ModuleDesc *mdesc,
                          ARMword bitsToChange, ARMword newValue);

ARMword ARMulif_GetConfig(RDI_ModuleDesc *mdesc,
                          ARMword bitsToRead);


void ARMulif_RaiseEvent(RDI_ModuleDesc *mdesc, ARMword event, 
                        ARMword data1, ARMword data2);

/*------ Oddments ----- */


/* Returns BUS cycle-length in tenths of a nanosecond, or 0 on error.
 * Uses RDIPropID_ARMulProp_CycleLength.
 */
uint32 ARMulif_GetCycleLength(RDI_ModuleDesc *mdesc);


/*------------- CYCLE-COUNTS ---------------*/



/* Returns the time in BUS-clock-cycles.
 * (I.e. in units of MCCFG/CPUSPEED if there is a cache, else 1/CPUSPEED.)
 */
ARMTime ARMulif_Time(RDI_ModuleDesc *mdesc);
/* Returns the number of times the core has been clocked
 * (ARM9 only so far) */
ARMTime ARMulif_CoreCycles(RDI_ModuleDesc *mdesc);
ARMTime ARMulif_CpuCycles(RDI_ModuleDesc *mdesc);

/* Returns CPUSPEED in Hertz. */
ARMTime ARMulif_GetCoreClockFreq(RDI_ModuleDesc *mdesc);


/* ----------------- BUS ------------------ */

struct ARMul_Bus;
unsigned ARMulif_QueryBus(struct RDI_ModuleDesc *mdesc, char const *name, 
                          struct ARMul_Bus **pBus);


/*------------- CALLBACK-REGISTRATION -----------------*/

void *ARMulif_InstallAccessCallback(RDI_ModuleDesc *mdesc, uint32 *ID,
                                    GenericAccessFunc func,
                                    void *handle);

bool ARMulif_RemoveAccessCallback(RDI_ModuleDesc *mdesc, uint32 *ID,
                                  void *killhandle);


/*
 * Schedule a timed callback - time is in bus-cycles.
 * Returns: A handle by which the callback may be deregistered.
 *    (This may be a copy of tcb, but may not be.)
 */
void *ARMulif_ScheduleTimedFunction(RDI_ModuleDesc *mdesc,
                                    ARMul_TimedCallback *tcb);

unsigned ARMulif_DescheduleTimedFunction(RDI_ModuleDesc *mdesc,
                                         void *tcb);

void *ARMulif_ScheduleFunctionCoreCycles(RDI_ModuleDesc *mdesc,
                                         ARMul_TimedCallback *tcb);


unsigned ARMulif_DescheduleFunctionCoreCycles(RDI_ModuleDesc *mdesc,
                                              ARMul_TimedCallback *tcb);

/*
 * Returns: A handle by which the callback may be deregistered.
 */
void *ARMulif_ScheduleNewTimedCallback(
    RDI_ModuleDesc *mdesc,  ARMul_TimedCallBackProc *func,
    void *handle, ARMTime when, ARMTime period);

void *ARMulif_ScheduleNewTimedCallback_CoreCycles(
    RDI_ModuleDesc *mdesc,  ARMul_TimedCallBackProc *func,
    void *handle, ARMTime when, ARMTime period);


/*
 * New and more general (but experimental) time callback system.
 ***************************************************************
 */
struct ARMulif_TimedCallback; /* opaque */

/* Params:
 *  In:   Which - A CallbackID telling which of the counters
 *                to shedule a callback on.
 *                E.g. ARMulCallbackID_ScheduleFunctionEvery.
 *     Relative - a bitmap of features. Bit 0 means 'when' is added
 *                to current count for selected counter.
 *         when - time (absolute or relative).
 */
struct ARMulif_TimedCallback *ARMulif_ScheduleTimedCallback(
    RDI_ModuleDesc *mdesc, ARMul_TimedCallBackProc *func,
    void *handle, ARMTime when, uint32 Which, uint32 Relative);

/* returns 0 on failure, 1 on success. */
int ARMulif_DescheduleTimedCallback(
        RDI_ModuleDesc *mdesc, struct ARMulif_TimedCallback *handle,
        uint32 Which);




/* 
 * This returns a handle, by which the hourglass-callback may be removed.
 */
void *ARMulif_InstallHourglass(RDI_ModuleDesc *mdesc,
                               armul_Hourglass *newHourglass,
                               void *handle);

int ARMulif_RemoveHourglass(RDI_ModuleDesc *mdesc,
                            void *node);



void ARMulif_ScheduleUnretractable(
    RDI_ModuleDesc *mdesc,  ARMul_TimedCallBackProc *func,
    void *handle, ARMTime when, ARMTime period);

void *ARMulif_InstallEventHandler(RDI_ModuleDesc *mdesc, uint32 events,
                                GenericCallbackFunc *func, void *handle);
int ARMulif_RemoveEventHandler(RDI_ModuleDesc *mdesc, void *node);

/* Returns an RDIError */
int ARMulif_InstallUnkRDIInfoHandler(RDI_ModuleDesc *mdesc,
                                     RDI_InfoProc *func, void *handle);
int ARMulif_RemoveUnkRDIInfoHandler(RDI_ModuleDesc *mdesc,
                                     RDI_InfoProc *func, void *handle);


/* AccessCallbacks */

/* These are used between Eventconverter (in ARM9 core) and ETM extension. */
#ifdef TRACE_CALLBACKS
void *ARMulif_InstallAccessCallback(RDI_ModuleDesc *mdesc,
                                    uint32 *pID,
                                    GenericAccessFunc func, void *handle);
bool ARMulif_RemoveAccessCallback(RDI_ModuleDesc *mdesc,
                                  uint32 *pID,
                                  void *killhandle);

void *ARMulif_InstallTraceCallback(RDI_ModuleDesc *mdesc,
                                    uint32 *pID,
                                    GenericTraceFunc func, void *handle);
bool ARMulif_RemoveTraceCallback(RDI_ModuleDesc *mdesc,
                                  uint32 *pID,
                                  void *killhandle);

unsigned ARMulif_InstallTraceMemoryHandler(RDI_ModuleDesc *mdesc,
                                           GenericTraceCallback *tcb);
unsigned ARMulif_RemoveTraceMemoryHandler(RDI_ModuleDesc *mdesc,
                                          GenericTraceCallback *tcb);


unsigned ARMulif_InstallTraceInstructionHandler(RDI_ModuleDesc *mdesc,
                                                GenericTraceCallback *tcb);
unsigned ARMulif_RemoveTraceInstructionHandler(RDI_ModuleDesc *mdesc,
                                               GenericTraceCallback *tcb);
#endif

unsigned ARMulif_InstallExecChangeHandler(RDI_ModuleDesc *mdesc,
                                          ExecChangeCallback *tcb);

int ARMulif_InstallExceptionHandler(RDI_ModuleDesc *mdesc,
                                    GenericCallbackFunc *func, void *handle);
int ARMulif_RemoveExceptionHandler(RDI_ModuleDesc *mdesc,
                                   GenericCallbackFunc *func, void *handle);

unsigned ARMulif_RemoveExecChangeHandler(RDI_ModuleDesc *mdesc,
                                         ExecChangeCallback *tcb);



GenericAccessCallback *NewGenericAccessCallback(GenericAccessFunc *func,
                                                void *handle);


struct ARMul_CoprocessorV5;
unsigned ARMulif_InstallCoprocessorV5(RDI_ModuleDesc *mdesc, unsigned number,
                                      struct ARMul_CoprocessorV5 *cpv5,
                                      void *data);

GenericAccessCallback *ARMulif_InstallNewCallback(
    RDI_ModuleDesc *mdesc, 
    GenericAccessFunc *func, 
    void *handle, 
    ARMword ID);

GenericAccessCallback *ARMulif_InstallNewInterruptController(
    RDI_ModuleDesc *mdesc,
    GenericAccessFunc *func,
    void *handle);

GenericAccessCallback **ARMulif_GetInterruptController(RDI_ModuleDesc *mdesc);

/* Preconds: (ID < 0xFFFF0000)
 */
GenericAccessCallback **ARMulif_QueryCallback(RDI_ModuleDesc *mdesc,
                                              ARMword ID);

/*
 * This doesn't technically belong here, but nor does it deserve its own
 * unit.................................................................
 */
void
deleteCallbackFromList(GenericAccessCallback **head,
                       GenericAccessCallback *arg2);

/*
 * This is an element in a list of references to generic callbacks.
 */
typedef struct GenericCallBackReference
{
    struct GenericCallBackReference *next;
    GenericAccessCallback *cb;
    ARMword ID[1];
} GenericCallBackReference;

unsigned ARMulif_GetIDSize(ARMword *pID);

/*
 * Parameters
 *   In        arg1 - pointer to callback ID.
 */
unsigned ARMulif_RegisterUnknownCallback(
                    GenericCallBackReference **pUnknownCallbacks,
                    ARMword *arg1,
                    GenericAccessCallback *arg2);


/*
 * Parameters:
 *          InOut: pUnknownCallbacks - address of list of
 *                   GenericCallBackReference's.
 *             In:  arg1 - points to callback-ID.
 *            Out: *arg2 - address of pointer to first callback in list.
 */
unsigned ARMulif_QueryUnknownCallback(
    GenericCallBackReference **pUnknownCallbacks, ARMword *arg1,
    GenericAccessCallback ***arg2);
/*
 * Parameters:
 *          InOut: pUnknownCallbacks - address of list of
 *                   GenericCallBackReference's.
 *             In: arg1 - points to callback-ID.
 *         Unused: arg2 - should be NULL.
 * Returns: RDIError
 */
unsigned ARMulif_DeregisterUnknownCallback(
    GenericCallBackReference **pUnknownCallbacks, ARMword *arg1,
    GenericAccessCallback *arg2);


/*
 * Returns: Handle by which callback can be deregistered.
 */
void *ARMulif_RegisterNewAccessCallback(RDI_ModuleDesc *mdesc, uint32 *pID,
                                        GenericAccessFunc *func, void *handle);

/*
 * Returns: RDIError
 */
unsigned ARMulif_DeregisterAccessCallback(RDI_ModuleDesc *mdesc, uint32 *pID,
                                          void *killhandle);
                                  

/* Memory interface Query.
 * pID points at ARMulBusID_Core or ARMulBusID_Bus.
 * Returns NULL on failure.
 */
struct armul_meminterface;
struct armul_meminterface *ARMulif_QueryMemInterface(RDI_ModuleDesc *mdesc, 
                                                     uint32 *pID);


/* Performance Monitor Counters
 * ---------------------------- */

void ARMulif_RemovePerfMonCtr(RDI_ModuleDesc *mdesc,
                              unsigned index,
                              ARMul_PerfMonCounter *pmc);

ARMul_PerfMonCounter *ARMulif_AddPerfMonCtr(RDI_ModuleDesc *mdesc,
                                            unsigned index,
                                            ARMul_PMCRolloverProc *rollProc,
                                            void *handle);

/* RDI Properties 
 * ==============
 */

/* 
 * This must be called during initialisation, as debuggers are
 * unlikely to notice changes.
 * ARMulator only supports GroupID=RDIPropertyGroup_SuperGroup so far.
 */
struct ARMul_RDIPropertyDesc;
RDI_Error ARMulif_AddProperty(RDI_ModuleDesc *mdesc, uint32 GroupID,
                              struct ARMul_RDIPropertyDesc *propdesc);




int ARMulif_Execute(RDI_ModuleDesc *mdesc, bool_int stop_others );
/* Asks the core "mdesc" to step just itself (if possible),
 * by one cycle (if it can) or one instruction otherwise.
 * Returns an RDIError value.
 */
int ARMulif_Step1(RDI_ModuleDesc *mdesc);

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /*ndef armul_askrdi__h */

/* EOF armul_askrdi.c */









