/*
 * Tube.c - ARMISS-Plugin model of Tube comms channel.
 * Copyright (C) 1999 ARM Limited. All Rights Reserved.
 *
 * RCS $Revision: 1.5.2.2 $
 * Checkin $Date: 2001/03/12 21:37:20 $
 * Revising $Author: dsinclai $
 */

#include "minperip.h"

#include <assert.h>


#define VERBOSE

/*
 * The Tube is a single address peripheral. Writes to that address
 * appear in the console. Reads are ignored.
 *
 * Tube has one configuration-parameter - the address-range.
 */




BEGIN_STATE_DECL(Tube)
      ARMul_BusPeripAccessRegistration my_bpar;
END_STATE_DECL(Tube)

static ARMul_BusPeripAccessFunc Tube_Access;

BEGIN_INIT(Tube)
    Hostif_ConsolePrint(state->hostif, ", Tube");
   /* 
    * Ask for the default bus.
    */
    {   int err;
        ARMul_BusPeripAccessRegistration *regn = &state->my_bpar;

        err = ARMulif_ReadBusRange(&state->coredesc, state->hostif,
                                   ToolConf_FlatChild(config, (tag_t)"RANGE"),
                                   &state->my_bpar,  
                                   0x0d800020,4,"");
        if (err)
            return err;
        regn->access_func = Tube_Access;
        regn->access_handle = state;
        state->my_bpar.capabilities = PeripAccessCapability_Typical;
        err = state->my_bpar.bus->bus_registerPeripFunc(BusRegAct_Insert,
                                                        regn);
        if (err)
            return err;
    }
END_INIT(Tube)


BEGIN_EXIT(Tube)
END_EXIT(Tube)

static int Tube_Access(void *handle, 
                       struct ARMul_AccessRequest *req)
{
    ARMWord address = req->req_address[0];
    ARMWord *data = req->req_data;
    unsigned type = req->req_access_type;
    TubeState *state=(TubeState *)handle;
   
    assert(address >= state->my_bpar.range[0].lo && \
           address <= state->my_bpar.range[0].hi);
    (void)address;

    /* We have idenfitied a TIC memory access */
    if (ACCESS_IS_WRITE(type) && ACCESS_IS_MREQ(type))
    {
        Hostif_ConsolePrint(state->hostif, "%c", (int)( *(ARMword*)data));
    }
    return PERIP_OK;
}


/*--- <SORDI STUFF> ---*/


#define SORDI_DLL_DESCRIPTION_STRING "SemiHosting operating system"
#define SORDI_RDI_PROCVEC Tube_AgentRDI
#include "perip_sordi.h"

#include "perip_rdi_agent.h"
    IMPLEMENT_AGENT_PROCS_NOEXE_NOMODULE(Tube)
    IMPLEMENT_AGENT_PROCVEC_NOEXE(Tube)

/*--- </> ---*/



/* EOF Tube.c */





