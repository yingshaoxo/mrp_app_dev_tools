/* -*-C-*-
 *
 * $Revision: 1.8.6.2 $
 *   $Author: lmacgreg $
 *     $Date: 2001/08/24 12:57:50 $
 *
 * Copyright (c) ARM Limited 1999.- 2001  All Rights Reserved.
 *
 * perip_sordi_if.h - SORDI interface to ARMulator Peripheral
 *
 * This is the interface for the perip_sordi.h implementation.
 */
#ifndef perip_sordi_if__h
#define perip_sordi_if__h


#include "sordi.h"
#include "rditools.h"


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

#define ARMulCnf_ControllerCapETM (tag_t)"TARGET_CONTROLLER_CAPABILITY_ETM"
#define ARMulCnf_ControllerCapSDM (tag_t)"TARGET_CONTROLLER_CAPABILITY_SDM"
#define ARMulCnf_ControllerCaps   (tag_t)"TARGET_CONTROLLER_CAPABILITIES"

SORDI_ProcVec const *QuerySORDI(void);

toolconf GetSORDIConfig(void);

void SetSORDIConfig(toolconf config);

extern void armsd_hourglass(void);

typedef void peripsordi_libregisteryieldfunc(RDILib_Handle handle,
                                             SORDI_YieldProc *yieldproc,
                                             SORDI_YieldArg  *yieldarg);
extern void peripsordi_setlibhandle(RDILib_Handle libhandle,
                                    peripsordi_libregisteryieldfunc *func);
void peripsordi_shutdown(void);

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif
/* EOF perip_sordi_if.h    */







