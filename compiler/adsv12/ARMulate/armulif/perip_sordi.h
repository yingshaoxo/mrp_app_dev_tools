/* -*-C-*-
 *
 * $Revision: 1.14.6.6 $
 *   $Author: lmacgreg $
 *     $Date: 2001/08/24 12:57:50 $
 *
 * Copyright (c) ARM Limited 1999. - 2001  All Rights Reserved.
 *
 * perip_sordi.h - SORDI interface to ARMulator Peripheral
 *
 * WARNING - This .H file contains IMPLEMENTATION
 */

/*
 * This header file takes #define'd parameters...
 * - required -
 * SORDI_DLL_DESCRIPTION_STRING - a normal quoted string.
 * SORDI_RDI_PROCVEC - name of the procvec, e.g. armiss_AgentRDI
 * - optional -
 * EXTRA_SORDI_INIT - extra initialisation code.
 * EXTRA_SORDI_INFO - extra info options.
 *
 * It provides implementations of...
 * armsd_hourglass - should be called during slow operations.
 * QuerySORDI - this should be exported from the DLL.
 *
 * If you make use of this file's ability to generate an Agent,
 * then you must provide the Agent's needs...
 * RDI_AGENT_NAME
 *
 *
 */

#undef RDI_VERSION
#define RDI_VERSION 151

#include "rdi.h"
#include "sordi.h"

#include "rdi_conf.h"
#include "toolconf.h"
#include "findfile.h"
#include "dir.h"
#include "rditools.h"


#include "perip_sordi_if.h" /* my interface */


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

#ifndef SORDI_RDI_PROCVEC
/*# error "perip_sordi.h requires SORDI_RDI_PROCVEC defined"*/
/* !WIP! */
# include "perip_rdi_agent.h"
    IMPLEMENT_AGENT_PROCVEC_NOEXE(ARMISS)
#else
extern struct RDI_ProcVec SORDI_RDI_PROCVEC;
#endif


static SORDI_YieldProc *pfnYield = NULL;
static SORDI_YieldArg *hYield = NULL;


static RDILib_Handle lib_handle = NULL;
static peripsordi_libregisteryieldfunc *lib_yieldfunc = NULL;


static toolconf sordi_config;


void peripsordi_setlibhandle(RDILib_Handle handle,
                             peripsordi_libregisteryieldfunc *func)
{
    lib_handle = handle;
    lib_yieldfunc = func;

    if (lib_yieldfunc != NULL && pfnYield != NULL)
        lib_yieldfunc(lib_handle, pfnYield, hYield);
}

void armsd_hourglass(void)
{
    if (pfnYield != NULL)
        pfnYield(hYield);
}

static void peripsordi_Register_Yield_Callback (SORDI_YieldProc *yieldcallback,
                                    SORDI_YieldArg *hyieldcallback)
{
    static bool in_register_yield = 0;
    pfnYield = yieldcallback;
    hYield   = hyieldcallback;

    /*
     * Ensure this is propogated to all SORDI peripherals.
     * Very important to protect against re-entrancy
     */
    if (lib_yieldfunc != NULL && in_register_yield == 0)
    {
        in_register_yield = 1;
        lib_yieldfunc(lib_handle, pfnYield, hYield);
    }

    in_register_yield = 0;

}

static bool_int peripsordi_Valid_RDI_DLL(void)
{
    return TRUE;
}

static char *peripsordi_Get_DLL_Description(void)
{
    /* Limited to around 200 characters */
    static char *msg =
#ifdef SORDI_DLL_DESCRIPTION_STRING
        SORDI_DLL_DESCRIPTION_STRING
#else
        "An ARMulator peripheral model"
#endif
        ;
    
    return msg ;
}

static RDI_ProcVec *peripsordi_GetRDIProcVec(void)
{
    return &SORDI_RDI_PROCVEC;
}

static int peripsordi_GetVersion(void)
{
    return RDI_VERSION;
}


static bool_int peripsordi_Initialise(char const **errstring, toolconf tconf)
{
    char const *targetdir;
    static char dummyargv0[1024];
    
    if (tconf == NULL)
    {
        /*
         * Include the peripherals's name
         */
        *errstring = 
#ifdef SORDI_DLL_NAME_STRING
            SORDI_DLL_NAME_STRING
#else
            "SORDI peripheral"
#endif
            " unable to initialise.  "
            "Debug controller supplied a NULL toolconf";

        return FALSE;
    }

    /*
     * Use the TargetDir to perform a clx_findfile primer - this is required
     * because clx_findfile will store argv0, and use it in later calls.  (In
     * a pre-SORDI world, this was nastily done by armsd
     */
    targetdir = ToolConf_Lookup(tconf, Dbg_Cnf_TargetDir);

    if (targetdir == NULL || targetdir[0] == '\0')
    {
        *errstring = 
#ifdef SORDI_DLL_NAME_STRING
            SORDI_DLL_NAME_STRING
#else
            "SORDI peripheral"
#endif
            " unable to initialise. "
            "Debug controller did not supply TARGET_DIR in toolconf.\n";
        
        return FALSE;
    }
    sprintf(dummyargv0, "%s%carmulate.sdi", targetdir, dir_getdirsep());
    clx_findfile(dummyargv0, NULL, NULL, NULL);

    sordi_config = tconf;

#ifdef EXTRA_SORDI_INIT
    EXTRA_SORDI_INIT
#endif

    return TRUE;
}

static SORDI_InfoProc peripsordi_Info;

static unsigned  peripsordi_Info(toolconf config, unsigned reason,
                                 void *arg1, void *arg2)
{
#ifdef EXTRA_SORDI_INFO
    EXTRA_SORDI_INFO
#else
    (void)config; (void)reason; (void)arg1; (void)arg2;
#endif
    switch (reason)
    {
    case SORDIInfo_ControllerCapabilities:
    {
        uint32 cap =  *(uint32*)arg1;
        ToolConf_AddTyped(config, ARMulCnf_ControllerCaps, tcnf_UInt, cap);
        if (cap & RDIControllerCapability_ETMModules)
        {
            ToolConf_AddTagged(config, ARMulCnf_ControllerCapETM,
                               "True");
        }
        if (cap & RDIControllerCapability_SelfDescribingModules)
        {
            ToolConf_AddTagged(config, ARMulCnf_ControllerCapSDM,
                               "True");
        }
        return RDIError_NoError;
    }
    case SORDIInfo_TargetCapabilities:
    {
        if (arg1 != NULL && arg2 != NULL)
        {
            *(uint32 *)arg1 = RDITargetCapability_TX |
                RDITargetCapability_CallDuringExecute;
            *(uint32 *)arg2 = RDITargetCapability_TX; /* => Maybe */

            return RDIError_NoError;
        }
    }
    default: ;
    }
    /*
     * Returns: RDIError_NoError - OK
     *          RDIError_IncompatibleRDILevels - The target can not be
     *                                    debugged by a controller
     *                                    with these capabilities
     */
     
    return RDIError_UnimplementedMessage;
}


static const SORDI_ProcVec peripsordi =
{
    sizeof(peripsordi),
    0,
    peripsordi_Valid_RDI_DLL,
    NULL, /* We do not support setversion */
    peripsordi_GetVersion,
    peripsordi_Get_DLL_Description,
    peripsordi_GetRDIProcVec,
    peripsordi_Initialise,
    peripsordi_Register_Yield_Callback,
    NULL, /* setprog */
    NULL, /* zeroprog */
    peripsordi_Info /* info XXX - this will be needed. */
};

SORDI_ProcVec const *QuerySORDI(void)
{
    return &peripsordi;
}

toolconf GetSORDIConfig(void)
{
    return sordi_config;
}

void SetSORDIConfig(toolconf config)
{
    sordi_config = config;
}


void peripsordi_shutdown(void)
{
    sordi_config = NULL;

    pfnYield = NULL;
    hYield = NULL;

    lib_handle = NULL;
    lib_yieldfunc = NULL;
}


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

/* EOF perip_sordi.h */





