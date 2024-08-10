/* -*-C-*-
 *
 * $Revision: 1.16.2.7 $
 *   $Author: hbullman $
 *     $Date: 2001/07/27 09:34:02 $
 *
 * Copyright (c) ARM Limited 1999-2001.  All Rights Reserved.
 *
 * sordi.h - Shared Object RDI.
 */

#ifndef sordi_h
#define sordi_h

#include "host.h"
#include "toolconf.h"
#include "rdi.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SORDI: "Shared Object RDI"
 * ==========================
 *
 * SORDI defines platform independent C entry points for RDI targets
 * embedded in shared objects of some description (e.g., .so, .sl, .dll)
 * In this file "shared object" refers to a shared object, a shared library,
 * or a DLL.
 *
 * There is a very simple mapping between SORDI and WinRDI, so that on
 * windowing platforms, a platform and environment specific WinRDI wrapper
 * can be supplied.
 *
 * The main reason that we bother to define SORDI_*, rather than reusing
 * WinRDI_*, is because WinRDI is defined to use the windows specific
 * WINAPI calling convention, whereas SORDI uses the standard 'C' calling
 * conventions.
 *
 * WinRDI                          => SORDI
 * ======                             =====
 * WinRDI_Valid_RDI_DLL            => SORDI_Valid_RDI_DLL
 * WinRDI_GetVersion               => SORDI_GetVersion
 * WinRDI_SetVersion               => SORDI_SetVersion
 * WinRDI_GetRDIProcVec            => SORDI_GetRDIProcVec
 * WinRDI_Initialise               => SORDI_Initialise
 * WinRDI_Config                   => no SORDI equivalent
 *                                    WinRDI provides implementation
 * WinRDI_Register_Yield_Callback  => SORDI_RegisterYieldCallback
 * WinRDI_SetProgressFunc          => SORDI_SetProgressFunc
 * WinRDI_ZeroProgressValues       => SORDI_ZeroProgressValues
 */

/*
 * How to use SORDI
 * ================
 *
 * SORDI can be used standalone, to communicate between a non-windowing
 * debug controller and a debug target (Figure 1), and it can be used
 * in conjunction with WinRDI to communicate between a windowing debug
 * controller and a debug target (Figure 2).
 *
 * SORDI Controller   (The debug controller)
 *       |
 *       | SORDI
 *       |
 * SORDI Target       (The debug target)
 *
 * Figure 1
 *
 *
 *
 * WinRDI Controller  (The debug controller)
 *        |
 *        | WinRDI
 *        |
 * WinRDI Target and  (A thin layer providing WinRDI_Config implementation,
 * SORDI Controller    and conduits to SORDI_*)
 *        |
 *        | SORDI
 *        |
 * SORDI Target       (The debug target)
 *
 * Figure 2
 */

/*
 * How to use this file
 * ====================
 *
 * This file defines a type for each SORDI procedure:
 *
 * SORDI_*Proc   : Type definition for entry point
 *
 * This file also defines three things for each SORDI exported symbol:
 *
 * Symbol_*Proc   : Type definition for entry point
 * Symbol         : Prototype for entry point
 * Symbol_*String : String containing name of entry point
 *
 * At present, there is only one SORDI defined exported symbol: QuerySORDI
 *
 *
 * SORDI Target use
 * ================
 *
 * The SORDI target #include's "sordi.h", and then provides implementations
 * for SORDI_*.
 *
 *
 * SORDI Controller use
 * ====================
 *
 * The SORDI controller #include's "sordi.h", and reads the SORDI entry points
 * out of the target, in the following recommended host-specific ways:
 *
 *
 *    Win32
 *    -----
 *
 *       #include <windows.h>
 *
 *       HINSTANCE     *dllHandle;
 *       SORDI_*Proc   *entry;
 *
 *       soHandle = LoadLibrary(...);
 *       entry    = (SORDI_*Proc *)GetProcAddress(dllHandle, SORDI_*String);
 *
 *
 *    Solaris
 *    -------
 *       #include <dlcfn.h>
 *
 *       void        *soHandle;
 *       SORDI_*Proc *entry;
 *
 *       soHandle = dlopen(...);
 *       entry    = (SORDI_*Proc *)dlsym(soHandle, SORDI_*String);
 *
 *
 *    HPUX
 *    ----
 *       #include <dl.h>
 *
 *       shl_t        slHandle;
 *       SORDI_*Proc *entry;
 *
 *       slHandle = shl_load(...);
 *       shl_findsym(&slHandle, SORDI_*String, TYPE_PROCEDURE, (void *)&entry);
 *
 *
 *
 * SORDI Controllers that are also WinRDI Targets
 * ==============================================
 *
 * Aside from the WinRDI_Config call, a complete implementation which should
 * suit all WinRDI/SORDI conversion layers can be found in winsordi.c.  This
 * should be compiled as required by the host WinRDI environment.
 */

/************* Initialization, negotiation and validation ********************/

/*
 * Function: SORDI_Valid_RDI_DLL (required)
 *
 *  Purpose: Identify that this is an SORDI Debug Target
 *
 *  Returns: TRUE if this DLL exports SORDI, FALSE otherwise
 */
typedef bool_int SORDI_Valid_RDI_DLLProc(void);


/*
 * Function: SORDI_SetVersion (optional - multi-lingual targets only)
 *
 *  Purpose: SORDI Controller requests to use this version of RDI
 */
typedef void SORDI_SetVersionProc(int version);


/*
 * Function: SORDI_SetVersion (required)
 *
 *  Purpose: Target informs controller which RDI version it is using
 */
typedef int SORDI_GetVersionProc(void);


/*
 * Function: SORDI_Get_DLL_Description (required)
 *
 *  Purpose: Supply a static message describing the DLL. The string should be
 *           limited to 200 characters in length. This allows the Debug
 *           Controller to estimate the amount of window space required to
 *           display the string.
 */
typedef char *SORDI_Get_DLL_DescriptionProc(void);


/*
 * Function: SORDI_GetRDIProcVec (required)
 *
 *  Purpose: Return a pointer to the RDI_ProcVec exported by this DLL.
 */
typedef RDI_ProcVec *SORDI_GetRDIProcVecProc(void);

/*
 * Function: SORDI_Initialise (optional)
 *
 *  Purpose: To be called before RDI_<anything>
 *           Note: SORDI_Initialise differs from WinRDI_Intialise in that it
 *                 does not accept a windows handle, but it does supply an
 *                 error string, if required.  See winsordi.c for an example.
 *
 *  Returns: TRUE if successful, FALSE otherwise. 
 *           The SORDI controller should display errstring
 *           string in a message box if initialisation was unsuccessful
 */
typedef bool_int SORDI_InitialiseProc(char const **errstring,
                                  RDI_ConfigPointer config);


/************* Yielding Control **********************************************/


typedef struct SORDI_OpaqueYieldArgStr SORDI_YieldArg;
typedef void   SORDI_YieldProc(SORDI_YieldArg *);


/*
 * Function: SORDI_Register_Yield_Callback (required)
 *
 *  Purpose: Supply a callback to yield control to other processes
 *           during Target execution. The callback is passed as a
 *           function/argument closure.
 */
typedef void SORDI_Register_Yield_CallbackProc(SORDI_YieldProc *yieldcallback,
                                               SORDI_YieldArg *hyieldcallback);


/************* Process Indicators ********************************************/


typedef struct SORDI_OpaqueProgressArgStr SORDI_ProgressArg;
typedef struct {
    SORDI_ProgressArg *handle; /* handle passed to SetProgressFunc */
    int nRead;     /* number of bytes read from channel */
    int nWritten;  /* number of bytes written           */
} SORDI_ProgressInfo;

typedef void SORDI_ProgressFuncProc(SORDI_ProgressInfo *info);


/*
 * Function: SORDI_SetProgressFunc (optional)
 *
 *  Purpose: Register a callback for the Debug Target to call
 *           periodically during download. The callback function
 *           receives data (counters) indicating the progress made so
 *           far, and typically displays this in a user-interface
 *           component.
 */
typedef void SORDI_SetProgressFuncProc(SORDI_ProgressFuncProc *func,
                                       SORDI_ProgressArg *handle);


/*
 * Function: SORDI_ZeroProgressValues (optional)
 *
 *  Purpose: Zero the progress counters
 */
typedef void SORDI_ZeroProgressValuesProc(void);

/************* Info **********************************************************/

/*
 *        Function: SORDI_Info
 *         Purpose: A general purpose SORDI call, used to communicate issues
 *                  concerning configuration, initialisation and capabilities
 *
 *  Pre-conditions: Should be called immediately following SORDI_Initialise,
 *                  and before any call to SORDI_Config, or any RDI_* call.
 *
 *          Params:
 *                   In: config - toolconf
 *                       reason - Reason for calling
 *                       arg1 - (void *) argument: meaning defined by reason
 *                       arg2 - (void *) argument: meaning defined by reason
 *
 *    Return value:  As defined by reason, or RDIError_UnimplementedMessage
 *                   if reason is not understood/implemented.
 */
typedef unsigned SORDI_InfoProc(toolconf config, unsigned reason,
                                void *arg1, void *arg2);


/*
 *        Function: SORDI_Info#SORDIInfo_ControllerCapabilities
 *         Purpose: Allows the controller to indicate that it can understand
 *                  certain RDI features.
 *
 *          Params:
 *                   In: reason = SORDIInfo_ControllerCapabilities
 *                   In: *((uint32 *)arg1) = Bitfield describing
 *                                           controller capabilities
 *
 *    Return value: RDIError_NoError - OK
 *                  RDIError_IncompatibleRDILevels - The target can not be
 *                                                   debugged by a controller
 *                                                   with these capabilities
 *                                  
 */
#define SORDIInfo_ControllerCapabilities 0x1000

#define RDIControllerCapability_ETMModules 1 /* Controller understands ETM */
#define RDIControllerCapability_SDMModules 2 /* Controller understands SDM */
#define RDIControllerCapability_SelfDescribingModules 2 /* old synonym for the above */                                           
#define RDIControllerCapability_Trace 4 /* Controller can use trace (ETM) */
#define RDIControllerCapability_APCModules 8 /* Controller understands APC */

/*
 *        Function: SORDI_Info#SORDIInfo_TargetCapabilities
 *         Purpose: Allows the target to advertise that, if configured with
 *                  the supplied toolconf, it will have a particular set of
 *                  RDI features.
 *
 *  Pre-conditions: Should be called immediately following a call to
 *                  SORDI_TargetCapabilities
 *
 *          Params:
 *                   In: toolconf
 *                  Out: *((uint32 *)arg1 Target capability bitfield
 *                  Out: *((uint32 *)arg2 Level of understanding bitfield
 *
 *                       arg1[n] arg2[n]
 *                         0       0     Feature [n] has no meaning to target
 *                         0       1     Target incapable of feature [n]
 *                         1       0     Target understands [n], but is unable
 *                                       to provide an answer
 *                         1       1     Target capable of feature [n]
 *
 *    Return value: RDIError_NoError
 *
 */
#define SORDIInfo_TargetCapabilities 0x1001

/*
 * HGB 28/03/2000 - Removed spurious #undefs, checked in by plg.  If there is
 * a genuine reason for these #undefs, will plg please get in touch.
 */
#define RDITargetCapability_CallDuringExecute 0x01
                                          /* Can accept calls during execute */
#define RDITargetCapability_RT       0x02 /* Target has RDI 1.5.1rt */
#define RDITargetCapability_TX       0x04 /* Target has RDI 1.5.1tx */
#define RDITargetCapability_TXRun    0x08 /* Target has RDI 1.5.1tx run */
#define RDITargetCapability_TXBuffer 0x10 /* Target has RDI 1.5.1tx buffer */
#define RDITargetCapability_Asynch   0x20 /* Target can use asynch RDI */
#define RDITargetCapability_Synch    0x40 /* Target can use synch RDI */

/*
 * WinRDI uses WinRDI_ConfigInPlace.  SORDI cannot use this (and mustn't
 * reuse the info number...)
 */
#define SORDIInfo_ConfigInPlace 0x1002 /* NOT USED ON SORDI */


#define SORDIInfo_Reserved_0x1003 0x1003 /* This number is reserved */

/*
 *        Function: SORDIInfo_ModuleServerCapabilities
 *         Purpose: Used only by module server, and clients of it.
 *
 *          Params:
 *                  Out: *arg1 = a bitfield of capabilities
 *                       Only one capability at present: RDIReadWrite
 *                       which means that the modsvr will use RDIRead/RDIWrite
 *                       if a memory code sequence fails
 *
 *         Returns: RDIError_NoError (if a module server)
 *                  RDIError_UnimplementedMessage (if otherwise)
 *
 */
#define RDIModuleServerCapability_RDIReadWrite 0x01
#define RDIModuleServerCapability_TargetComponents 0x02

#define SORDIInfo_ModuleServerCapabilities 0x1004

/************* Obtaining the interface ***************************************/

typedef struct SORDI_ProcVec
{
    uint32                             size;     /* = sizeof (SORDI_ProcVec) */
    uint32                             reserved; /* Should be zero */

    SORDI_Valid_RDI_DLLProc           *isvalid;
    SORDI_SetVersionProc              *setversion;
    SORDI_GetVersionProc              *getversion;
    SORDI_Get_DLL_DescriptionProc     *description;
    SORDI_GetRDIProcVecProc           *getrdi;

    SORDI_InitialiseProc              *init;

    SORDI_Register_Yield_CallbackProc *regyield;
    SORDI_SetProgressFuncProc         *setprog;
    SORDI_ZeroProgressValuesProc      *zeroprog;

    SORDI_InfoProc                    *info;

    /*  SORDI_DescribeConfig              *descconfig;*/

} SORDI_ProcVec;

/*
 * Function: QuerySORDI
 *    Notes: This function is exported as a shared symbol
 *
 *  Purpose: Retrieve the SORDI interface
 *
 *  Returns: SORDI_ProcVec
 */
typedef SORDI_ProcVec const *QuerySORDI_Proc(void);
extern                       QuerySORDI_Proc QuerySORDI;
#define                      QuerySORDI_String "QuerySORDI"

#ifdef __cplusplus
};                              /* extern "C" */
#endif

#endif /* !def sordi_h */

/* EOF sordi.h */
