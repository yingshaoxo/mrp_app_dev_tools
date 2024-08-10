/* -*-C-*-
 *
 * $Revision: 1.4.2.1 $
 *   $Author: lmacgreg $
 *     $Date: 2001/06/14 16:13:58 $
 *
 * Copyright (c) ARM Limited 2000.  All Rights Reserved.
 *
 * rditools.h
 */

#ifndef rditools_h
#define rditools_h

#include "rdi.h" 
#include "sordi.h"
#include "toolconf.h"
#include "rdi_sdm.h"

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif


/*
 * Typedef: RDILib_Handle
 * Purpose: A handle for the RDI library
 */
struct RDI_LibraryData; /* An Opaque type */
typedef struct RDI_LibraryData *RDILib_Handle;

/*
 *        Function: RDILib_Initialise
 *         Purpose: Initialise the RDI library
 *
 *  Pre-conditions: none
 *
 *          Params: none
 *
 *         Returns: a handle to the initialised library, or NULL if failure
 *
 * Post-conditions: library initialised
 */
RDILib_Handle RDILib_Initialise(void);


/*
 *        Function: RDILib_Finalise
 *         Purpose: Finalise (shut down) the RDI library.  Unloads any
 *                  open SORDI targets
 *
 *  Pre-conditions: a valid RDILib_Handle
 *
 *          Params:
 *                   In: handle
 *
 *         Returns: none
 *
 * Post-conditions: library shut down; handle invalid.
 */
void RDILib_Finalise(RDILib_Handle handle);


/*
 *        Function: RDILib_LoadSORDI
 *         Purpose: Load and intialise a SORDI target, if it has not already
 *                  been loaded and initialised, and return the RDI procvec
 *
 *  Pre-conditions: a valid RDILib_Handle
 *
 *          Params:
 *                   In: handle
 *                   In: filename - The filename of the SORDI target
 *                   In: config - The toolconf to pass to SORDI_Initialise
 *                  Out: errstring - In the event of failure, (*errstring)
 *                                   points to an explanatory note (unless
 *                                   errstring == NULL)
 *
 *         Returns: The proc vec of the loaded library, or NULL if it
 *                  failed to load
 *
 * Post-conditions: library loaded (or not) as per return code
 */
RDI_ProcVec const *RDILib_LoadSORDI(RDILib_Handle handle, char const *filename,
                                    toolconf config, char const **errstring);


/*
 *        Function: RDILib_GetSORDIProcVec
 *         Purpose: Get SORDI interface for an already loaded target
 *
 *  Pre-conditions: a valid RDILib_Handle, and rdi procvec.  The procvec
 *                  should correspond to a SORDI target that was loaded
 *                  using RDILib_LoadSORDI
 *
 *          Params:
 *                   In: handle
 *                   In: rdi - The procvec of the target
 *
 *         Returns: none
 *
 */
SORDI_ProcVec const *RDILib_GetSORDIProcVec(RDILib_Handle handle,
                                      RDI_ProcVec const *rdi);


/*
 *        Function: RDILib_UnloadSORDI
 *         Purpose: Unload a SORDI target
 *
 *  Pre-conditions: a valid RDILib_Handle, and rdi procvec.  The procvec
 *                  should correspond to a SORDI target that was loaded
 *                  using RDILib_LoadSORDI
 *
 *          Params:
 *                   In: handle
 *                   In: rdi - The procvec of the target to be unloaded
 *
 *         Returns: none
 *
 * Post-conditions: The SORDI target and its RDI vector are invalid
 */
void RDILib_UnloadSORDI(RDILib_Handle handle, RDI_ProcVec const *rdi);



/*
 *        Function: RDILib_RegisterYieldCallback
 *         Purpose: Take a yield callback, and propogate it to any SORDI
 *                  targets that have been loaded.  Additionally, when new
 *                  SORDI targets are loaded, this callback will be registered
 *                  with them.
 *
 *  Pre-conditions: none
 *
 *          Params:
 *                   In: handle 
 *                   In: yieldproc - Callback function
 *                   In: yieldarg - Callback handle
 *
 *         Returns: none
 *
 * Post-conditions: YieldCallback registered with all SORDI targets
 *                                  
 */
void RDILib_RegisterYieldCallback(RDILib_Handle handle,
                                  SORDI_YieldProc *yieldproc,
                                  SORDI_YieldArg  *yieldarg);


/*
 *        Function: RDILib_CreateETMRegisterMap
 *         Purpose: Return a description of an ETM, for servicing
 *                  RDIInfo_DescribeRegisterMap
 *
 *  Pre-conditions: none
 *
 *          Params:
 *                   In: config_code - ETM config register (bank 0, reg 1)
 *                  Out: *numRegs = Number of entries in the register map
 *
 *         Returns: NULL if failure, else a pointer to a register array
 *
 * Post-conditions: none
 *                                  
 */
RDIRegister* RDILib_CreateETMRegisterMap(uint32 config_code, uint32 *numRegs);

/*
 *        Function: RDILib_DestroyETMRegisterMap
 *         Purpose: Destroy a register map
 *
 *  Pre-conditions: regmap should be valid
 *
 *          Params:
 *                   In: regmap - Map to destroy
 *
 *         Returns: Nothing
 *
 * Post-conditions: Map destroyed
 */
void RDILib_DestroyETMRegisterMap(RDIRegister *regmap);


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /* !def rditools_h */

/* EOF rditools.h */
