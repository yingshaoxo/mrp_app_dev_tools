/* codeseq.c - module to allow RDI code sequences to be run on ARMulator 
 * Copyright (c) 2001  ARM Limited. All Rights Reserved.
 *
 * RCS $Revision: 1.2.2.20 $
 * Checkin $Date: 2001/10/29 18:34:33 $
 * Revising $Author: dsinclai $
 */


#define MODEL_NAME Codeseq

#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>       /* for strcpy */
#include "armul_cnf.h"

#include "minperip.h"

#include "armul_callbackid.h"
#include "armul_agent.h"
#include "disass.h"

#ifdef SOCKETS
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#include "armul_mem.h"
#include "rdi_sdm.h"

#ifndef NDEBUG
# if 1
# else
#  define VERBOSE_BUS
#  define VERBOSE_RDI_LOG
#  define VERBOSE_SDM
# endif
#endif


#define CURRENTMODE RDIMode_Curr

/* MOV Rd,Rm; Rd in bits 12..15, Rm in bits 0..3 */
#define ARM_NOP_INSTR 0xE1A00000
/* ADDS Rd, Rn, #0; Rd in bits 0..2, Rn in bits 3..5. */
#define THUMB_NOP_INSTR 0x1C00

/* static ARMword nop_instr[5] = {
   0,0,THUMB_NOP_INSTR,0,ARM_NOP_INSTR }; */

BEGIN_STATE_DECL(Codeseq)
    struct RDICodeSequence_Data CodeSequenceToRun;
ARMword RegsSavedForRunCodeSequence[16];
ARMul_MemInterface bus_mem, child;
ARMul_MemInterfaceRef mem_ref;
ARMword last_address;
    unsigned instr_size; /* 4 for ARM, 2 for Thumb */
    ARMword base_address;
    ARMword stop_pc; /* When to stop! */
    bool_int verbose;
END_STATE_DECL(Codeseq)

static unsigned CodeseqMemInfo(void *handle, unsigned type, ARMword *pID,
                               uint64 *data)
{
    CodeseqState *mem = (CodeseqState *)handle;
    if (mem->child.mem_info)
    {
        return mem->child.mem_info(mem->child.handle,type,pID,data);
    }
    else
    {
        return RDIError_UnimplementedMessage;
    }
}
static int Codeseq_MemAccess(void *handle, ARMWord address, ARMWord *data,
                             unsigned acc);
static int Codeseq_MemAccessSA(void *handle,ARMWord address, ARMWord *data,
                               unsigned acc);
static void Codeseq_MemAccessHarvard(void *handle,
                       ARMword daddr,ARMword *ddata, ARMul_acc dacc, int *drv,
                       ARMword iaddr,ARMword *idata, ARMul_acc iacc, int *irv);



/* Aims to return a value in microseconds */
static ARMTime CodeseqReadClock(void *handle)
{
    CodeseqState *mem = (CodeseqState *)handle;
    if (mem->child.read_clock)
    {
        return mem->child.read_clock(mem->child.handle);
    }
    else
    {
        return 0L;
    }    
}


static const ARMul_Cycles *CodeseqReadCycles(void *handle)
{
    CodeseqState *mem = (CodeseqState *)handle;
    if (mem->child.read_cycles)
    {
        return mem->child.read_cycles(mem->child.handle);
    }
    else
    {
        return NULL;
    }
}


static uint32 CodeseqGetCycleLength(void *handle)
{
    CodeseqState *mem = (CodeseqState *)handle;
    if (mem->child.get_cycle_length)
        return mem->child.get_cycle_length(mem->child.handle);
    return 0;
}


static void CodeseqCoreException(void *handle,ARMword address,
                        ARMword penc)
{
    CodeseqState *cs = (CodeseqState *)handle;
    cs->child.x.arm8.core_exception(cs->child.handle,address,penc);
}

static unsigned int CodeseqDataCacheBusy(void *handle)
{
    CodeseqState *cs = (CodeseqState *)handle;
    return cs->child.x.strongarm.data_cache_busy(cs->child.handle);
}

static int Codeseq_MemAccess2(void *handle,
                             ARMword address,
                             ARMword *data,
                             ARMul_acc access_type)
{
    CodeseqState *state = (CodeseqState *)handle;
    if(!(acc_nOPC(access_type))) 
    {
        return Codeseq_MemAccess(handle, address, data, access_type);
    }
    else
    {
        int err = 
            state->child.x.arm8.access2(state->child.handle,
                                        address,data,access_type);
        return err;
    }
}

/* This is only for ARM7 (and ARM6) cores. */
static int Codeseq_MemAccess(void *handle,
                             ARMword address,
                             ARMword *data,
                             ARMul_acc access_type)
{
 
    CodeseqState *state = (CodeseqState *)handle;
    armul_MemAccess *mem_access=state->child.x.basic.access;
    RDICodeSequence_Data *cs = &state->CodeSequenceToRun;
    UNUSEDARG(address);
    UNUSEDARG(data);
    UNUSEDARG(access_type);
    if (access_type & ACCESS_IDLE)
        return PERIP_OK;

    if(!(acc_nOPC(access_type))) 
    {
        switch(acc_WIDTH(access_type))
        {
        case BITS_16:
        {
            uint16* ip2 = (uint16*)cs->instructions;
            unsigned offset = (address - state->base_address)>>1;
            if (offset < cs->numInstructions)
            {
                *data = ip2[offset];
            }
            else
            {
                *data = THUMB_NOP_INSTR;
            }
            return PERIP_OK;
        }
        default:
# ifndef NDEBUG
                Hostif_ConsolePrint(state->hostif,"Codeseq : Unexpected access-size. %u\n", acc_WIDTH(access_type));
                /* fallthru */
# endif
        case BITS_32:
        {
            unsigned offset = (address - state->base_address)>>2;
            if (offset < cs->numInstructions)
            {
                *data = cs->instructions[offset];
            }
            else
            {
                unsigned Reg = ((offset - cs->numInstructions) & 7);
                *data = ARM_NOP_INSTR + Reg + (Reg << 12);
            }
#ifdef VERBOSE_SDM
            Hostif_ConsolePrint(state->hostif,"SDM A:%08x, I:%08x\n",
                                (unsigned)address,
                                (unsigned)*data);
#endif
            return PERIP_OK;
        }
        case BITS_64: /* E.g. ARM8 */
        {
            int err1 = Codeseq_MemAccess(handle,address,data,
                                         access_type - 1);
            int err2 = Codeseq_MemAccess(handle,address+4,data+1,
                                         access_type - 1);
            (void)err1; (void)err2;
            return PERIP_OK2;
        }
        }
#  ifdef VERBOSE_CODESEQ_END
        Hostif_ConsolePrint(state->hostif,"Addr:%08x, LAST:%08x\n",
                            (unsigned)address,
                            (unsigned)state->last_address);
#  endif
    }
    /*
     * Need to pass data access on to mem system.
     * NB data-aborts are bad news here.
     */
    return mem_access(state->child.handle,address,data,access_type);
}

static int Codeseq_MemAccessSA(void *handle,
                               ARMword address,
                               ARMword *data,
                               ARMul_acc access_type)
{
 
    CodeseqState *state = (CodeseqState *)handle;
    RDICodeSequence_Data *cs = &state->CodeSequenceToRun;

    UNUSEDARG(address);
    UNUSEDARG(data);
    UNUSEDARG(access_type);
  
    if(!(acc_nOPC(access_type)))
    {
        if (cs->numInstructions > 0)
        {
            --cs->numInstructions;
            if (acc_WIDTH(access_type) == BITS_16)
            {
                uint16* ip2 = (uint16*)cs->instructions;
                ARMWord rv = *(ip2++);
                cs->instructions = (uint32*)ip2;
                *data = rv; 
                state->last_address = address;
                return PERIP_OK;
            }
            else
            {
                uint32 rv = *(cs->instructions++);
                *data = rv; 
                state->last_address = address;
                return PERIP_OK;
            }
        }
        return (unsigned)PERIP_DABORT; /* Ouch! */
    }
    return
        state->child.x.strongarm.access(state->child.handle,address,data,access_type);
}

static void Codeseq_MemAccessHarvard(void *handle,
                       ARMword daddr,ARMword *ddata, ARMul_acc dacc, int *drv,
                       ARMword iaddr,ARMword *idata, ARMul_acc iacc, int *irv)
{
    CodeseqState *state = (CodeseqState *)handle;

    ARMWord dummy_idata = 0;
    ARMul_acc dummy_iacc = ACCESS_IDLE + ACCESS_INSTR;
    int dummy_irv = 0;

    /* It is a requirement on the core-models that they clear *irv when
     * they want a new opcode. (This test isn't needed while we don't
     * generate D-side waits, but it's in here while we remember.)
     */
    if (!*irv) /* Only return opcode when requested */
    {
        if(!(acc_nMREQ(iacc)))
        {
            *irv = Codeseq_MemAccess(state,iaddr,idata,iacc);
        }
        else
            *irv =  PERIP_OK;
    }
    state->child.x.pipe_arm9.harvard_access(
        state->child.handle,
        daddr, ddata, dacc, drv,
        iaddr, &dummy_idata, dummy_iacc, &dummy_irv);
#ifndef NDEBUG
    if (state->verbose)
    {
        if (*irv!=PERIP_OK) Hostif_ConsolePrint(state->hostif,"Warning, FPE i returns %d\n",*irv);
        if (*drv!=PERIP_OK) Hostif_ConsolePrint(state->hostif,"Warning, FPE d returns %d\n",*drv);
    }
#endif
    return;
}

static int Codeseq_RunCodeSequence(CodeseqState *state, 
                                  const RDICodeSequence_Data *cs)
{
    int rv = RDIError_NoError;
    ARMWord OldCPSR = ARMulif_GetCPSR(&state->coredesc);
    ARMWord NewCPSR = OldCPSR;
    ARMword coremode = OldCPSR  & 0x1f ;
    ARMWord OldSPSR = ARMulif_GetSPSR(&state->coredesc,coremode);
    unsigned i, mode = (unsigned)cs->mode;
    ARMul_MemInterface *mif;
    uint32 ID[2];
    if (cs->numInstructions <= 0)
        return RDIError_NoError;
    /* Check we understand ISET */
    if ((unsigned)cs->iset > 3)
    {
        return RDIError_BadCPUStateSetting;
    }
#ifdef VERBOSE_SDM
    Hostif_ConsolePrint(state->hostif,"Begin_Codeseq iset:%u numisntrs:%u",
                        (unsigned)cs->iset,
                        (unsigned)cs->numInstructions);
#endif
    state->instr_size = (cs->iset & 1) ? 2 : 4;
    state->base_address = 0x80000000;
    state->stop_pc = state->base_address + 
        state->instr_size * cs->numInstructions +
        4*7; /* For the "nop" instrs to wait for LDR's to complete. */

    /*
     * Insert mem interface and set up 
     */
    ID[0] = ARMulBusID_Core;
    ID[1] = 0;
    mif = ARMulif_QueryMemInterface(&state->coredesc, &ID[0]);
    
    if (mif)
    { /* start if */
        state->bus_mem.handle = state;
        state->bus_mem.x.basic.access = Codeseq_MemAccess;
        /* <Copy info thru. */
        state->bus_mem.mem_info=CodeseqMemInfo;
        state->bus_mem.read_clock=CodeseqReadClock;
        state->bus_mem.read_cycles=CodeseqReadCycles;
        state->bus_mem.get_cycle_length = CodeseqGetCycleLength;
        /* </> */
        
        switch(mif->memtype) 
        {
        case ARMul_MemType_Basic:
        case ARMul_MemType_16Bit:
        case ARMul_MemType_Thumb:
/*        case ARMul_MemType_BasicCached:*/
        case ARMul_MemType_16BitCached:
        case ARMul_MemType_ThumbCached:
        case ARMul_MemType_ARMissAHB:
            break;
        case ARMul_MemType_AHB:
            break;
            
        case ARMul_MemType_StrongARM:
            state->bus_mem.x.strongarm.access = Codeseq_MemAccessSA;
            state->bus_mem.x.strongarm.core_exception = CodeseqCoreException;
            state->bus_mem.x.strongarm.data_cache_busy = CodeseqDataCacheBusy;
            break;
        case ARMul_MemType_ARM8:
            state->bus_mem.x.arm8.core_exception = CodeseqCoreException;
            state->bus_mem.x.arm8.access2 = Codeseq_MemAccess2;
            break;
            
        case ARMul_MemType_ARMissCache:
            break;
        case ARMul_MemType_ARM9: /* ARM9 Harvard   */
            state->bus_mem.x.arm9.harvard_access = Codeseq_MemAccessHarvard;
            break;
        case ARMul_MemType_ByteLanes:       
        default:
            break;
        }
    
    }
    ARMul_InsertMemInterface(mif,
                             &state->child,
                             &state->mem_ref,
                             &state->bus_mem);
    /*
     * memory model inserted - set up core and run code
     */
  
    /* Compute new mode */
    if (mode > RDICodeSequenceMode_System)
        mode = RDICodeSequenceMode_System;
    NewCPSR = (OldCPSR & ~ARM_M) | mode;
    /* Change to new mode and instruction-set. */
    ARMulif_SetCPSR(&state->coredesc, NewCPSR);
    for (i=0; i<16; i++)
        state->RegsSavedForRunCodeSequence[i] = 
            ARMulif_GetReg(&state->coredesc,coremode, i);
    /* use broadcast event here to get cores + models to save cycle counts */
     ARMulif_RaiseEvent(&state->coredesc,DebugEvent_RunCodeSequence,
                       (cs->iset & 1),0);     
     /* Write PC to a fixed value, for debugging.
      * NB ARMulator ARM9 models distinguish between
      * 15 : R15 and
      * 16 : PC (writing this flushes the Execute stage).
      */
     ARMulif_SetReg(&state->coredesc, RDIMode_Curr, 16, state->base_address);


    /* Write the input registers */
    { unsigned i;
    for (i=0;i<cs->numDataIn;i++)
        ARMulif_SetReg(&state->coredesc, coremode, i, cs->dataIn[i]);
    }
    state->CodeSequenceToRun = *cs;
    {
        ARMword pc;
        int err; 
# ifdef VERBOSE_CODESEQ
        Hostif_ConsolePrint(state->hostif, "Starting codeseq\n");
# endif

#ifdef OldCode        
        err = ARMulif_Execute(&state->coredesc, FALSE); 
#else
        do {
            err = ARMulif_Step1(&state->coredesc);
            pc = ARMulif_GetPC(&state->coredesc);
            
            if (err!=RDIError_NoError) {
                if (state->verbose) {
                    Hostif_ConsolePrint(
                        state->hostif,
                        "Warning, codeseq ARMulif_Step1 returns %d\n",err);
                }
            }
            
        } while (pc >= state->base_address && pc < state->stop_pc);
        if (pc < state->base_address) {
            if (state->verbose) {
                Hostif_ConsolePrint(
                    state->hostif,
                    "Warning, codeseq pc(0x%08x) < base_address(0x%08x)\n",
                    pc,state->base_address);
                Hostif_ConsolePrint(
                    state->hostif,
                    "    state->stop_pc=0x%08x, stored pc=0x%08x\n",
                    state->stop_pc,state->RegsSavedForRunCodeSequence[15]);
            }
            rv = RDIError_Error;
        }
#endif

# ifdef VERBOSE_CODESEQ
        Hostif_ConsolePrint(state->hostif, "Finished codeseq\n");
# endif
    }
    /*
     * Code has been run we now need to restore the core to its 
     * previous state
     */
    
    /* Read data out of the registers */
    for (i=0;i<cs->numDataOut;i++) 
    {
        cs->dataOut[i] = ARMulif_GetReg(&state->coredesc, coremode, i);
    }

    for (i=0; i<16; i++)
    {                   /* for r15, write to 16 which also flushes execute stage on 9s */
        ARMulif_SetReg(&state->coredesc, coremode, (i==15)?16:i, 
                     state->RegsSavedForRunCodeSequence[i]);
    }
    ARMulif_SetCPSR(&state->coredesc, OldCPSR);
    ARMulif_SetSPSR(&state->coredesc, coremode, OldSPSR);
    ARMulif_RaiseEvent(&state->coredesc,DebugEvent_EndCodeSequence,
                       0/* use OldCPSR and T bit */,0);     

    assert(state->mem_ref.mif);
    if (state->mem_ref.mif)
    {
        ARMul_RemoveMemInterface(&state->mem_ref,&state->child);
    }
    return rv; /* RDIError_NoError; */

}


static int CodeSeq_RDI_info(void *handle,unsigned type,ARMword *arg1,
                            ARMword *arg2)
{
    CodeseqState *state = (CodeseqState *)handle;  
    UNUSEDARG(arg2);
    switch (type) {
        
    case RDIInfo_CodeSequenceInquiry:
    {
        return RDIError_NoError;
    }
    case RDIInfo_CodeSequence:
    {
        const RDICodeSequence_Data *cs = (const RDICodeSequence_Data *)arg1;
        return Codeseq_RunCodeSequence(state,cs);
    }
    default:
        break;
    }
    return RDIError_UnimplementedMessage;
    
}

static unsigned Codeseq_ConfigEvents(void *handle, void *data)
{
    CodeseqState *ts = handle;
    ARMul_Event *evt = data;
    UNUSEDARG(ts);
    if (evt->event != ConfigEvent_Reset)
        return FALSE;
    
    return FALSE; /* Carry on */
}

BEGIN_INIT(Codeseq)
{
  CodeseqState *cs = state;
  if (coldboot)
  {
      Hostif_PrettyPrint(hostif, config, ", RDI Codesequences");
      ARMulif_InstallUnkRDIInfoHandler(&cs->coredesc,
                                       CodeSeq_RDI_info, cs);
      ARMulif_InstallEventHandler(&state->coredesc,
                                  ConfigEventSel,
                                  Codeseq_ConfigEvents, cs);
  }
  state->verbose = ToolConf_DLookupBool(config,(tag_t)"VERBOSE",FALSE);
}

END_INIT(Codeseq)


BEGIN_EXIT(Codeseq)
{
  CodeseqState *cs=state;
  UNUSEDARG(cs);

}
END_EXIT(Codeseq)


/*--- <SORDI STUFF> ---*/


#define SORDI_DLL_DESCRIPTION_STRING "CodeSeq (a Utility)"
#define SORDI_RDI_PROCVEC Codeseq_AgentRDI
#include "perip_sordi.h"

#include "perip_rdi_agent.h"
    IMPLEMENT_AGENT_PROCS_NOEXE_NOMODULE(Codeseq)
    IMPLEMENT_AGENT_PROCVEC_NOEXE(Codeseq)

/*--- </> ---*/

/* EOF tracer.c */



