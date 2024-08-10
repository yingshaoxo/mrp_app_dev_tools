/* nothing.c - Peripheral Model that implements nothing,
 * so the user can delete unwanted instances with it.
 * Copyright (C) ARM Limited, 1998-2001 . All rights reserved.
 *
 * RCS $Revision: 1.3.4.2 $
 * Checkin $Date: 2001/07/27 14:13:02 $
 * Revising $Author: lmacgreg $
 */

#include "minperip.h"



#ifndef NDEBUG
# if 1
# else
#  define VERBOSE_NOTHING_INIT
# endif
#endif



BEGIN_STATE_DECL(Nothing)
END_STATE_DECL(Nothing)





BEGIN_INIT(Nothing)
{
  NothingState *ts = state;
#ifdef VERBOSE_NOTHING_INIT
  Hostif_PrettyPrint(ts->hostif, ts->config, ", Nothing");
#else
  (void)ts;
#endif
}
END_INIT(Nothing)


BEGIN_EXIT(Nothing)
END_EXIT(Nothing)

/*--- 
+<SORDI STUFF> ---*/

#define SORDI_DLL_NAME_STRING "Nothing"
#define SORDI_DLL_DESCRIPTION_STRING "Nothingond Timer (test only)"
#define SORDI_RDI_PROCVEC Nothing_AgentRDI
#include "perip_sordi.h"

#include "perip_rdi_agent.h"
    IMPLEMENT_AGENT_PROCS_NOEXE_NOMODULE(Nothing)
    IMPLEMENT_AGENT_PROCVEC_NOEXE(Nothing)

/*--- </> ---*/




/* EOF nothing.c */








