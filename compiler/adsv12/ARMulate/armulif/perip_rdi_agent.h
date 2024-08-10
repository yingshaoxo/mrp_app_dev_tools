/* -*-C-*-
 *
 * $Revision: 1.3.10.3 $
 *   $Author: lmacgreg $
 *     $Date: 2001/08/24 12:57:50 $
 *
 * Copyright (c) ARM Limited 1999.- 2001  All Rights Reserved.
 *
 * perip_rdi_agent.h - Interface to Agent for RDI interface to
 *                      ARMulator3 peripheral
 *
 */

#ifndef perip_rdi_agent__h
#define perip_rdi_agent__h

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

#define IMPLEMENT_AGENT_PROCS_NOEXE_NOMODULE(agent_name) \
static int agent_name##_OpenAgent(RDI_AgentHandle *agent, unsigned type,\
                         RDI_ConfigPointer config,\
                         struct RDI_HostosInterface const *hostif,\
                         RDI_DbgState *dbg_state)\
{\
    return agent_name##_Init(agent, type, config, hostif,dbg_state);\
}\
\
\
static int agent_name##_CloseAgent(RDI_AgentHandle agent)\
{\
    return agent_name##_Exit(agent);\
}\
\
\
static RDI_NameList const *agent_name##_CPUNames(RDI_AgentHandle agent)\
{\
    (void)agent;\
    return (RDI_NameList const *)NULL;\
}\
\
\
static int agent_name##Agent_Info(void *handle, unsigned subcode,\
                                  ARMword *arg1, ARMword *arg2)\
{\
    agent_name##State *state = (agent_name##State*)handle;\
    if (handle == NULL)\
        return RDIError_NotInitialised;\
    if (state->unk_rdiinfo.func != NULL) {\
        RDI_InfoProc *info = (RDI_InfoProc *)state->unk_rdiinfo.func;\
        int rv = info(state->unk_rdiinfo.handle, subcode,arg1, arg2);\
        if (rv!=RDIError_UnimplementedMessage)\
            return rv;\
    }\
\
    switch (subcode)\
    {\
      case RDIInfo_Modules:\
          *((unsigned *)arg1) = 0;\
          (void)arg2;  (void)handle;\
          return RDIError_NoError;\
      case RDIInfo_Target:\
          *((unsigned *)arg1) = RDITarget_NonExecutableProcessor;\
          return RDIError_NoError;\
      default:\
          return RDIError_UnimplementedMessage;\
    }\
}



/* Here is the minimal Agent's procvec... */
/* Parameters:
 * agent_name is max 12 chars, e.g. ARMISS
 */
#define IMPLEMENT_AGENT_PROCVEC_NOEXE(agent_name) \
struct RDI_ProcVec agent_name##_AgentRDI = \
{\
    #agent_name, \
    agent_name##_OpenAgent, \
    agent_name##_CloseAgent, \
    0, /* module_name##_Open,*/  \
    0, /* module_name##_Close,*/ \
    0,0,0,0,0, \
    0,0,0,0,0, \
    0, /* module_name##_Execute,*/ /* !!! AGENT CANNOT EXECUTE YET */ \
    0, /* ARM10Agent_Step, */ /* Surely it should be able to? */ \
    agent_name##Agent_Info, \
    0,                                          /* PointInquiry */ \
    0,                                          /* AddConfig */ \
    0,                                          /* LoadConfigData */ \
    0,                                          /* SelectConfig */ \
    0,                                          /* DriverNames */ \
    agent_name##_CPUNames, \
    0,                                          /* ErrMess */ \
    0,                                          /* LoadAgent */ \
    0                                           /* TargetIsDead */ \
};


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /*ndef perip_rdi_agent__h*/

/* EOF perip_rdi_agent.h */








