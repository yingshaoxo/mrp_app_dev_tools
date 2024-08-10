/*
 * simplelink.h
 * Copyright (C) 1996-2000 ARM Limited. All rights reserved. 
 * RCS $Revision: 1.27.6.8 $
 * Checkin $Date: 2001/08/24 12:57:50 $
 * Revising $Author: lmacgreg $
 */

#ifndef simplelinks__h
#define simplelinks__h

#include "armul_types.h" /* for ARMTime */

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

typedef unsigned (GenericCallbackFunc)(void*handle, void *data);

typedef struct GenericCallback {
    /*
     * This pointer is used by the code in the server that
     * builds the links.
     */
    struct GenericCallback *next;
    /*
     * The callback lives here.
     */
    GenericCallbackFunc *func;
    void *handle;
} GenericCallback;

/* And in ARMulator ... */
typedef GenericCallback ExceptionHandlerNode;
typedef GenericCallback EventHandlerNode;


/*
 * TIMED CALLBACKS
 * ---------------
 *
 */
typedef struct ARMul_TimedCallback ARMul_TimedCallback;

/*
 * Note that timer-callback functions are less likely than generic-access
 * callbacks to need a pointer to the whole structure.
 * Also, unlike access-functions, there is no need to return an error.
 */
typedef void (ARMul_TimedCallBackProc)(void *handle);

struct ARMul_TimedCallback /* An event list node */
{ 
    ARMul_TimedCallback *next;  /* belongs to the core */
    ARMul_TimedCallBackProc *func;
    void *handle;
    ARMTime when;
    ARMTime atc_period;

    /* We just use 'next' in the client's copy. */
};

/* This is not opaque because that would cost speed. */
typedef struct ARMul_TimedCallbackList
{
        ARMul_TimedCallback *tcbs;
        ARMul_TimedCallback *pool;
} ARMul_TimedCallbackList;


/* Returns a handle by which the timed callback may be descheduled. */
typedef void *ARMul_SchedulerFunc(void *shandle,
                                  ARMul_TimedCallBackProc *func,
                                  void *fhandle,
                                  ARMTime when,
                                  ARMTime atc_period); /* Options */
typedef struct {
        ARMul_SchedulerFunc *func;
        void *handle;
} ARMul_SchedulerMethod;

#define SCHEDULER_TXT ":Scheduler"


typedef int ARMul_DeschedulerFunc(void *shandle, void *khandle);

#define DESCHEDULER_TXT ":Descheduler"


/*
 *
 * PERFORMANCE-MONITOR-COUNTERS
 * ----------------------------
 */
typedef struct ARMul_PerfMonCounter ARMul_PerfMonCounter;

typedef void ARMul_PMCRolloverProc(struct ARMul_PerfMonCounter*);

struct ARMul_PerfMonCounter {
    struct ARMul_PerfMonCounter * next;
    ARMul_PMCRolloverProc *pmc_RollProc;
    void *pmc_Handle;
    uint32 pmc_Count;
    uint32 pmc_CountHi;
};

/* Notes...
 * . If RolloverProc requires any extra data,
 *  the registering code should embed the above structure in a larger one.
 * . I have made an exception in pmc_CountHi, even though it's not
 *  needed for a pure 32-bit performance-counter, because it is required
 *  in by far the most commom use of this structure.
 */

/* Example code for arbitrary delta.
 * Params: aperfctr : lvalue of performance counter, e.g. state->perform[0]
 *         avalue   : number to add to the counter.
 */
#define PERFCTR_ADD(aperfctr, delta) \
  { ARMul_PerfMonCounter *p = aperfctr; \
    while (p!=NULL){ \
      uint32 oldD = p->pmc_Count;  uint32 newD = oldD + delta; \
      p->pmc_Count = newD; \
      if (oldD > newD) /* Rollover */ \
        p->pmc_RollProc(p); \
      p = p->next; \
  } }

/* Example code for delta = 1 
 * Params: aperfctr : lvalue of performance counter, e.g. state->perform[0]
*/
#define PERFCTR_INC(aperfctr) \
  { ARMul_PerfMonCounter *p = aperfctr; \
    while (p!=NULL){ \
      if (++(p->pmc_Count) == 0) /* Rollover */ \
        p->pmc_RollProc(p); \
      p = p->next; \
  } }

/* Notes...
 * . The above would rather be inline functions, but C complains if you
 * define an inline function and don't use it. :-( 
 * . Many clients shall define their own macros in terms of the above,
 * so they can be compiled in two variants - with and without performance-
 * counters.
 */

/*
 * CHAINING ACCESS-FUNCTIONS
 * -------------------------
 */
struct GenericAccessCallback; /* forward */
/*
 * Returns: An RDI_Error.
 * Wishes: A more general form of address.
 */
typedef unsigned (GenericAccessFunc)(struct GenericAccessCallback *myCB,
                                     ARMword address,
                                     ARMword *data,
                                     unsigned access_type);

/* Note that this is deliberately the same layout as GenericCallback
 * but the chaining-method is different.
 * GenericAccessFunc's are passed a pointer to the callback,
 * rather than the handle extracted from it. This allows them to call
 * the next one in the list.
 */
typedef struct GenericAccessCallback
{
    struct GenericAccessCallback *next;
    GenericAccessFunc *func;
    void *handle;
} GenericAccessCallback;

#define TRACE_CALLBACKS
#ifdef TRACE_CALLBACKS
/*
 * CHAINING TRACE ACCESS-FUNCTIONS
 * -------------------------------
 */
struct GenericTraceCallback; /* forward */
/*
 * Returns: An RDI_Error.
 * Wishes: A more general form of address.
 */
typedef void (GenericTraceFunc)(struct GenericTraceCallback *myCB,
                                     ARMword address,
                                     ARMword data,
                                     unsigned access_type,
                                     int result);

/* This callback is designed to allow memory acceses to be
 * traced down the chain. Models which register this type of
 * callback cannot effect the actual memory access, only
 * snoop on the request.
 */

/* Note that this is deliberately the same layout as GenericCallback
 * but the chaining-method and ownership are different.
 * GenericAccessCallbacks are owned by whoever registers the callback,
 * and the callback-function is passed a pointer to the callback,
 * rather than the handle extracted from it.
 */
typedef struct GenericTraceCallback
{
    struct GenericTraceCallback *next;
    GenericTraceFunc *func;
    void *handle;
} GenericTraceCallback;

#endif






/*
 * COPROCESSOR-LINKS
 * -----------------
 */

/* The type of a coprocessor-method. It is not chainable.
 */
typedef int NCAccessFunc(void *handle, int type, ARMword iaddr, uint32 *data);
/* This is a function in the core (see tracer for usage example)
 * which despatches (by cpnum) to one of the access-functions in the
 * selected coprocessor.
 */
typedef int CoprocessorAccessFunc(void *handle, uint32 cpnum, int type,
                                  ARMword iaddr, uint32 *data);
typedef struct CoprocessorAccess
{
    /* These must be in the same order as armul_method.h:GenericMethod. */
    CoprocessorAccessFunc *func;
    void *handle;
} CoprocessorAccess;

typedef struct TaggedMethod
{
    uint32 tag;
    NCAccessFunc *func;
} TaggedMethod;

typedef struct GenericObjectCallback
{
    struct GenericObjectCallback *next;
    unsigned index; /* E.g. coprocessor-number */
    void *handle;
    TaggedMethod *methods;
} GenericObjectCallback;


/*
 * Execution state change callbacks
 */
struct ExecChangeCallback; /* forward */

typedef void ExecChangeFunc(struct ExecChangeCallback *myCB,
                            ARMul_ExecChangeEnum estate);

typedef struct ExecChangeCallback
{
    struct ExecChangeCallback *next;
    void *handle;
    ExecChangeFunc *func;
} ExecChangeCallback;


/* ----- Utilities ----- */

/* !Deprecated - use ARMul_InstallGenericAccessCallback instead!
 * Params:
 *     what  - a callback, of which a copy will be installed.
 *     where - address of head of list.
 *
 * On Exit: what->next is the killhandle by which the callback may 
 *          be deregistered.
 * Returns: 
 *      RDIError 
 */
unsigned ARMulif_InstallGenericAccessCallback(GenericAccessCallback**where,
                                              GenericAccessCallback*what);

/*
 * Params:
 *     what  - a callback, of which a copy will be installed.
 *     where - address of head of list.
 *
 * Returns:
 *      the killhandle by which the callback may 
 *      be deregistered, or NULL if we failed.
 */
void *ARMul_InstallGenericAccessCallback(GenericAccessCallback**where,
                                         GenericAccessCallback*what);

/*
 * Returns: 
 *      RDIError 
 */
unsigned ARMulif_RemoveGenericAccessCallback(GenericAccessCallback**where,
                                              void *killhandle);

#ifdef TRACE_CALLBACKS
unsigned ARMulif_InstallGenericTraceCallback(GenericTraceCallback**where,
                                              GenericTraceCallback*what);
unsigned ARMulif_RemoveGenericTraceCallback(GenericTraceCallback**where,
                                              void *killhandle);
#endif

unsigned ARMulif_InstallGenericCallback(GenericCallback**where,
                                              GenericCallback*what);
unsigned ARMulif_RemoveGenericCallback(GenericCallback**where,
                                              void *killhandle);


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /*ndef simplelinks__h */
/* EOF simplelinks.h */







