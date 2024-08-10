/*
 * armul_agent.h
 * Structures to allow an agent for a core-module
 * (e.g. ARMiss or ARMulator)  to instantiate sub-modules.
 * Copyright (C) 2000 - 2001 ARM Ltd.  All rights reserved.
 *
 * RCS $Revision: 1.18.6.10 $
 * Checkin $Date: 2001/08/24 12:57:49 $
 * Revising $Author: lmacgreg $
 */

#ifndef armul_agent__h
#define armul_agent__h

#include "sordi.h"
#include "rdi_hif.h"  /* for struct RDI_HostosInterface */
                      /* (use rdi_conf.h if you want it typedef'd.) */
#include "rditools.h" /* for RDILib_Handle, _Initialise,_LoadSORDI,_Finalise */
#include "cvector.h"

/* Needed for Dynlink_Instance in PluginNode.
 * Should become removable. */
#include "dynlink.h"

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

typedef struct PluginNode PluginNode;
struct PluginNode {
    PluginNode *next;
    const RDI_ProcVec *pi_procvec;
    RDI_AgentHandle pi_agent_handle; /* from sordi's openagent. */
    toolconf pi_config; 
};


#define ARMUL_AGENT_SIGNIATURE 0xF0000001

/* This is the ARMulator Agent, as used in multirdi.c.
 * It should be used wherever a module can load sub-modules.
 */
typedef struct ARMul_Agent{
    uint32 signiature;

    /* Note that ARMulAgent_Create copies the info into here,
     * rather than expecting it to remain valid after openagent.
     */
    RDI_DbgStateForSubAgent aa_dbgStateFromParent, aa_dbgStateForChildren;

    RDILib_Handle aa_RDILibHandle; /* for SORDI loading utilities */
    bool aa_RDILibHandle_isMine;   /* Self-made, not inherited */
    /* For WinHourglass - functionality (only in core-models) */
    SORDI_YieldProc *yieldproc;
    SORDI_YieldArg  *yieldarg;
    /*
     * For the outermost Agent, this is the toolconf as
     * read from *.dsc and *.ami.
     */
    toolconf aa_base; 
    /*
     * For the outermost Agent, this is the toolconf as
     * used to create each instance - i.e. a nasty temporary. :-(
     */
    toolconf aa_config;

    /* A structure that looks like an RDI_NameList, but that doesn't have all
     * the consts. This avoids compilation errors
     */
    struct NameList {
        int itemmax;
        char **names;
    } aa_processors;
    int aa_default_processor;

    /*
     * A container of RDI_ModuleDesc's built during (multi)OpenAgent.
     */
    CVector all_modules;
    CVector top_modules;   /* The core, or peripheral */
    CVector child_modules; /* Children of the core, or empty */
    /*
     * A container of RDI_ModuleDesc's built during (multi)OpenAgent,
     * not including this main one.
     * Each of these agents may create more than one module
     * (but normally they create just one core, switch or peripheral model).
     */
    CVector agents;
    /* Used while building the above. */
    RDI_ModuleDesc subagent_desc;

    
    /* While constructing modules, some constructor-agents may tell
     * us which endianness they'd like. We note that here. */
    unsigned num_bigend, num_littleend;
    int rdiOpenAgentResult;
    unsigned init_type; /* the 'type' as passed to openagent. */
    struct RDI_HostosInterface const *hostif;
    RDI_DbgState *dbg_state; /* pointer to debug toolbox state */
    /* The module-description for this (multi)Agent, to give to sub-agents. */
    RDI_ModuleDesc module_desc;
    unsigned rdi_log;            /* RDI logging level */
    PluginNode *PluginHandlers;

    bool aa_is_top_level;
#ifndef EXCLUDE_ARM_LIC_MANAGEMENT
    void *license_handle;
    char feature_name[40];
#endif
    char *dll_name; 
    const  RDI_ProcVec * const *ARMul_RDIProcs;

    /*
     * For sending RDI messages to all plugins.
     */
    ARMword iter_infonum, *iter_arg1, *iter_arg2;
    unsigned iterating;  /* Are the above in use? Avoids unwanted info-loops.*/

    CVector as_SuperPropertyDescs_Agent; /* of RDIProperty_GroupDesc */
    CVector as_SuperPropertyAccessors_Agent; /* of GenericAccessCallback */

} ARMul_Agent;

/* -------- functions provided ---------- */


void ARMulAgent_DebugPrint(ARMul_Agent *agent, const char *format, ...);


int ARMulAgent_AddComponent(toolconf cconf, tag_t tag, const char *value,
                            toolconf child, void *arg);

/* For use during initialisation */
void ARMulAgent_ErrorPrint(ARMul_Agent *agent, const char *format, ...);

int ARMulAgent_Create(RDI_AgentHandle *rdi_handle, 
                      struct RDI_HostosInterface const *hostif,
                      toolconf config, 
                      RDI_DbgState *dbg_state,
                      char *dll_name,
                      const  RDI_ProcVec * const *ARMul_RDIProcs);
int ARMulAgent_Destroy(RDI_AgentHandle *rdi_handle);

/* For the Agent's Procvec */
RDI_NameList const *ARMulAgent_RDI_cpunames(RDI_AgentHandle rdi_handle);

/* For OpenAgent and CPUNames */
int ARMulAgent_readconfig(ARMul_Agent *agent, toolconf config,
                          toolconf NamesAddedByTarget);


int ARMul_ExpandVariants(toolconf tconf, tag_t name);

#ifdef OldCode
toolconf ARMul_FindConfRoot(toolconf tcbase);
#else
# define ARMul_FindConfRoot ToolConf_Root
#endif


/*
 * Uses iter_infonum, iter_arg1, iter_arg2.
 * Intended to be called inside a CVector_Iterate.
 *
 * Params: 
 *      In: (RDI_ModuleDesc *)el - which module to do the RDIInfo-call on.
 *         (ARMul_Agent *)st - the agent, whose iter_infonum, iter_arg1, and
 *                            iter_arg2 should be the params to that info-call.
 */
int ARMulAgent_SendToRDI(void *st, void *el);

/* Returns: RDIError */
int ARMulAgent_SendToSubagents(ARMul_Agent *agent, ARMword infonum,
                               ARMword *arg1, ARMword *arg2);


/*
 * These functions are intended to be called inside a CVector_Iterate.
 * They are deprecated specialisations of ARMulAgent_SendToRDI.
 */
int ARMul_Send_AllLoaded(void *st, void *el);
int ARMul_Send_AboutToClose(void *st, void *el);




/* ------------ functions and data required --------------*/
void Target_add_names(toolconf tconf);


toolconf GetSORDIConfig(void); /* E.g. as implemented in "perip_sordi.h" */


int ARMulAgent_AddPeripheral(toolconf cconf, tag_t tag, const char *value,
                             toolconf child, void *arg);

/* Default RDI functions */
int ARMulAgent_RDI_step(RDI_AgentHandle rdi_handle,
                        RDI_ModuleHandle *rdi_handlep,
                        bool_int stop_others, unsigned ninstr,
                        RDI_PointHandle *handle);

int ARMulAgent_RDI_execute(RDI_AgentHandle rdi_handle,
                           RDI_ModuleHandle *rdi_handlep,
                           bool_int stop_others, RDI_PointHandle *handle);





#ifdef __unix
# ifdef COMPILING_ON_HPUX
#  define armul_SO_EXTENSION ".sl"               /* Extension of modules */
# else
#  define armul_SO_EXTENSION ".so"               /* Extension of modules */
# endif
#else /* win32 */
# define armul_SO_EXTENSION ".dll"
#endif

/* debugging only
 * Parameters:
 *        In:  st - NULL or a hostif.
 *             el - pointer to an RDI_ModuleDesc
 */
int ARMul_ShowModuleDesc(const void *st, void *el);


#define armul_MODULEPATH   "ARMDLL"



#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /*ndef armul_agent__h */





/* EOF armul_agent.h */

