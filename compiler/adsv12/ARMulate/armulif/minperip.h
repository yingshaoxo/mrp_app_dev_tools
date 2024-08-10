/*
 * minperip.h   - minimal peripheral interface for ARMISS/ARMulator.
 * Copyright (C) 1996-2000 ARM Limited. All rights reserved. 
 *
 * RCS $Revision: 1.24.2.8 $
 * Checkin $Date: 2001/09/06 17:26:18 $
 * Revising $Author: dsinclai $
 */

#ifndef amulif_minperip__h
#define amulif_minperip__h

#include <assert.h>       /* used in BEGIN_INIT */
#include "host.h"         /* from clx, for uint64 */
#include "armul_types.h"  /* for ARMTime */
#include "rdi.h"          /* for RDI_DbgStateForSubAgent (rdi150.h) */
#include "armul_hostif.h" /* for Hostif_RaiseError etc. */
#include "simplelinks.h"  /* for GenericAccessFunc */
#include "armul_askrdi.h" /* for ARMulif_InstallNewBusAccessHandle */
#include "armul_bus.h"    /* for ARMul_Bus */
#include "armul_callbackid.h" /* because most extensions need this. */
#include "armul_method.h"
typedef uint32 ARMWord;

/* Bus Interface */
#include "armul_access.h"

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

/* Returns CPU clock speed in Hz. */
uint32 ARMul_GetFCLK(toolconf config);
/* Returns Bus clock speed in Hz. */
uint32 ARMul_GetMCLK(toolconf config);

/* Returns true iff if finds CLOCK=EMULATED in config. */
bool_int ARMul_ClockIsEmulated(toolconf config);

/* Returns TRUE iff "META_" is a prefix of tag */
bool ARMul_TagStartsMeta(tag_t tag);


/* If there is a child of config called "RANGE",
 * extracts BASE,SIZE<MASK from that child to
 * the relevant fields of range (using the defaults
 * for missing ones). MASK defaults to 0xFFFFFFFF.
 * Otherwise, we just use the defaults. */
void ReadRange(toolconf config, AddressRange *range,
               ARMword default_lo, ARMword default_hi);


/*
 * Some #defines to abstract some function-calls.
 * ==============================================
 */

#define BEGIN_INIT(modelname) \
RDI150_OpenAgentProc  modelname##_Init; \
int modelname##_Init(RDI_AgentHandle *agent, unsigned type, \
             RDI_ConfigPointer config, \
             struct RDI_HostosInterface const *hostif, \
             RDI_DbgState *dbg_state) \
{ \
    RDI_DbgStateForSubAgent *dsa = (RDI_DbgStateForSubAgent *)dbg_state; \
    bool coldboot = ((type & RDIOpen_BootLevelMask) == RDIOpen_ColdBoot);\
    modelname##State *state = (modelname##State *) ( coldboot ?\
                                calloc(1,sizeof(modelname##State)) : *agent);\
    assert(agent != NULL); assert(dsa != NULL); \
    *agent = (RDI_AgentHandle)state; \
    if (!state) { \
        Hostif_RaiseError(hostif,#modelname "model could not allocate memory") ;\
        return RDIError_OutOfStore;\
    } \
    assert(dsa->parent_desc != NULL); assert(dsa->agent_desc != NULL); \
    state->coredesc = *dsa->parent_desc; \
    state->agentdesc = *dsa->agent_desc; \
    state->hostif = hostif; state->config = config;\
    if (!coldboot) { Hostif_PrettyPrint(hostif,config,"\n***WARMBOOT***\n"); }\
    if (coldboot) {

        /* Plugins don't get warmboot - but we don't want to reinit if they do! 
         */

#define END_INIT(modelname) \
    }\
    return RDIError_NoError; \
}



#define BEGIN_EXIT(modelname) \
RDI150_CloseAgentProc  modelname##_Exit; \
int modelname##_Exit(RDI_AgentHandle agent) \
{\
    modelname##State *state = (modelname##State *)agent;

#define END_EXIT(modelname) \
    free(state); \
    return RDIError_NoError; \
}

typedef struct {
    /* These must be in the same order as the fields of
     * simplelinks.h:GenericMethod. */
    RDI_InfoProc *func;
    void *handle;
} RDIInfoMethod;

#define DEFAULT_STATE_MEMBERS \
    toolconf config; \
    const struct RDI_HostosInterface *hostif;\
    RDI_ModuleDesc coredesc, agentdesc;\
    RDIInfoMethod unk_rdiinfo;


#define BEGIN_STATE_DECL(modelname) \
typedef struct modelname##State \
{

#define END_STATE_DECL(modelname) \
    DEFAULT_STATE_MEMBERS \
} modelname##State;


/* To be called between BEGIN_INIT and END_INIT.
 * Scans the function-list for one called "RDI_InfoProc", and fills
 * in the callback if it is found.
 * So far, only used in armulext/timer.c
 */
#define ADD_INTERFACE_SOURCE(funclist) \
    FindMethodInterface((GenericMethod*)&state->unk_rdiinfo,"RDI_InfoProc", \
                                                state,&funclist[0]);



/* ---- coprocessor helpers ---- */

#include "armul_copro.h"

typedef int PeripRet;
# define PERIP_OK2 2
# define PERIP_OK 1
# define PERIP_DABORT -1
# define PERIP_BUSY 0
# define PERIP_NODECODE -2
/* Peripherals are unlikely to address-abort : That's
 * normally an MMU's job. */


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /*ndef amulif_minperip__h*/
/* EOF minperip.h */
