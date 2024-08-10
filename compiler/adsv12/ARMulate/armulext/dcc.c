/* dcc.c - Model of Debug Comms Channel ( co-processor 14 ) */
/* Copyright (C) ARM Limited, 1999.- 2001  All rights reserved.
 *
 * RCS $Revision: 1.4.4.8 $
 * Checkin $Date: 2001/10/04 20:42:50 $
 * Revising $Author: dsinclai $
 */

#define MODEL_NAME DCC

#include "minperip.h"
#include "armul_copro.h"


/* dcc.c */
#define ARMulCnf_IRQOnCommsChannel (tag_t)"IRQONCOMMSCHANNEL"
#define ARMulCnf_CommRXIRQNo (tag_t)"COMMRXIRQNO"
#define ARMulCnf_CommTXIRQNo (tag_t)"COMMTXIRQNO"
#define ARMulCnf_ReadDelay (tag_t)"READDELAY"
#define ARMulCnf_WriteDelay (tag_t)"WRITEDELAY"
/* More general */
#define ARMulCnf_MCCfg (tag_t)"MCCFG"
#define ARMulCnf_Rate (tag_t)"RATE"


/* Options */
#define DCC_IRQ 0x0001


#define BIT(n) ( (ARMword)(instr>>(n))&1)   /* bit n of instruction */
#define BITS(m,n) ( (ARMword)(instr<<(31-(n))) >> ((31-(n))+(m)) ) /* bits m to n of instr */
#define TOPBITS(n) (instr >> (n)) /* bits 31 to n of instr */


static struct {
  tag_t option;
  unsigned long flag;
  char *print;
} DCCOption[] = {
  ARMulCnf_IRQOnCommsChannel, DCC_IRQ, "Generate interrupt",
  NULL, 0, NULL
};

BEGIN_STATE_DECL(DCC)
    ARMword read, write, status; /* the three registers */

    struct {
        RDICCProc_FromHost *fn;
        void *arg;
        ARMword latch;          /* latch of word from debugger */
        bool latched;           /* latch is full */
    } fromhost;                 /* Debugger callback */

    struct {
        RDICCProc_ToHost *fn;
        void *arg;
        ARMword latch;          /* latch of word to debugger */
        bool latched;           /* latch is full */
    } tohost;                   /* Debugger callback */

    unsigned prop;              /* Properties word */
    unsigned poll_interval;     /* Interval to poll debugger at */
    bool polling;               /* Poll loop is active */
    unsigned read_delay;        /* interval to poll debugger for reads (part 2) */
    unsigned write_delay;       /* Interval to poll debugger for writes */
    GenericAccessCallback **intcInterface;
    int txirqno, rxirqno;       /* IRQ controller numbers to use for TX and RX */

    unsigned txirq, rxirq;      /* Our own interrupt controller */
    bool bDebug_acc;
END_STATE_DECL(DCC)

#define RBit 1
#define WBit 2


#ifdef DEBUG
static void ShowState(DCCState *dcc, char const *where)
{
    Hostif_ConsolePrint(dcc->hostif, "DCC: %s\t", where);

    Hostif_ConsolePrint(dcc->hostif, "  %08x %08x %08x\t",
                       dcc->status, dcc->read, dcc->write);

    Hostif_ConsolePrint(dcc->hostif, "  fromhost: %08x %d\t",
                       dcc->fromhost.latch, dcc->fromhost.latched);
    Hostif_ConsolePrint(dcc->hostif, "  tohost: %08x %d\n",
                       dcc->tohost.latch, dcc->tohost.latched);
}
#else
#define ShowState(dcc,where)
#endif

/*
 * Hourglass function - get data from debugger
 */

static void ReadEvent(void *handle);
static void WriteEvent(void *handle);
static void PollEvent(void *handle);




/*
 * Generate an interrupt -- use the interrupt controller if one
 * is specified.
 */
static void StatusRegisterChanged(DCCState *dcc)
{
    uint32 commrx, commtx;

    /* Do nothing if we're not wired up to an interrupt controller */
    if (!(dcc->prop & DCC_IRQ)) {
        return;
    }

    ShowState(dcc, "StatusRegisterChanged");
    
    /* RBit set means ok to Read */
    commrx = ((dcc->status & RBit) != 0);

    /* WBit clear means OK to Write */
    commtx = ((dcc->status & WBit) == 0);

    if (*dcc->intcInterface != NULL)
    {
        GenericAccessCallback *cb1 = *dcc->intcInterface;
        ARMword dataT = commtx;
        ARMword dataR = commrx;
        cb1->func(cb1,dcc->txirqno,&dataT,ACCESS_WRITE_WORD);
        cb1->func(cb1,dcc->rxirqno,&dataR,ACCESS_WRITE_WORD);
    }
    else
    {
        ARMulif_SetSignal(&dcc->coredesc, RDIPropID_ARMSignal_IRQ,
                          commrx || commtx);
    }
}

static void SchedulePollEvent(DCCState *dcc)
{
    if (!dcc->polling
        && (dcc->fromhost.fn != NULL || dcc->tohost.fn != NULL)
        && (!dcc->fromhost.latched && !dcc->tohost.latched)) {
        (void)ARMulif_ScheduleNewTimedCallback(
            &dcc->coredesc, PollEvent, dcc,
            dcc->poll_interval + ARMulif_Time(&dcc->coredesc), 0
            );
        dcc->polling = TRUE;
    } else {
        dcc->polling = FALSE;
    }
}

static void PollEvent(void *handle)
{
    DCCState *dcc = (DCCState *)handle;

    ShowState(dcc, "PollEvent");

    dcc->polling = FALSE;

    /* Poll - check for a read first */
    if ((dcc->status & RBit) == 0) {
        if (dcc->fromhost.fn != NULL && !dcc->fromhost.latched) {
            int valid;
            dcc->fromhost.fn(dcc->fromhost.arg, &dcc->fromhost.latch, &valid);
            if (valid) {
                dcc->fromhost.latched = TRUE;
                (void)ARMulif_ScheduleNewTimedCallback(
                    &dcc->coredesc, ReadEvent, dcc,
                    dcc->read_delay + ARMulif_Time(&dcc->coredesc), 0
                    );
                return /*0*/;
            }
        }
    }

    /* Then for a write */
    if ((dcc->status & WBit) != 0
        && dcc->tohost.fn != NULL
        && !dcc->tohost.latched) {

        dcc->tohost.latch = dcc->write;
        dcc->tohost.latched = TRUE;

        (void)ARMulif_ScheduleNewTimedCallback(
            &dcc->coredesc, WriteEvent, dcc,
            dcc->write_delay + ARMulif_Time(&dcc->coredesc), 0);
        return /*0*/;
    }

    /* Poll again if neither occured */
    SchedulePollEvent(dcc);

    return /*0*/;
}

/*
 * This is called to transfer data from the read latch to the DCC
 */
static void ReadEvent(void *handle)
{
    DCCState *dcc = (DCCState *)handle;

    ShowState(dcc, "ReadEvent");

    if (dcc->fromhost.latched) {
        /* copy the data from the read latch into the read register &
         * update the status register */
        dcc->read = dcc->fromhost.latch;
        dcc->fromhost.latched = FALSE;
        dcc->status |= RBit;
        StatusRegisterChanged(dcc);
    }

    SchedulePollEvent(dcc);

    return /*0*/;
}

/*
 * This is called to transfer data from the DCC to the host.
 */
static void WriteEvent(void *handle)
{
    DCCState *dcc = (DCCState *)handle;

    ShowState(dcc, "WriteEvent");

    /* check that the write can proceed */
    /* do we need to do this, since ScheduleWriteEvent() has already done it??? */
    if (dcc->tohost.latched) {
        dcc->tohost.fn(dcc->tohost.arg, dcc->tohost.latch);
        dcc->status &= ~WBit;
        dcc->tohost.latched = FALSE;
        StatusRegisterChanged(dcc);
    }

    SchedulePollEvent(dcc);

    return;
}


/*
 * Function to reset the DCC (e.g. on reload)
 */
static unsigned DCC_ConfigEvents(void *handle, void *data)
{
    DCCState *dcc = (DCCState *)handle;
    ARMul_Event *evt = data;

    if (evt->event == ConfigEvent_Reset)
    {
        dcc->status = (1 << 28);
        dcc->read = dcc->write = 0;

        dcc->tohost.latched = FALSE;
        dcc->fromhost.latched = FALSE;
        StatusRegisterChanged(dcc);
        SchedulePollEvent(dcc);
    }
    if(evt->event == DebugEvent_RunCodeSequence)
    {
        dcc->bDebug_acc = TRUE;
    }
    if(evt->event == DebugEvent_EndCodeSequence)
    {
        dcc->bDebug_acc = TRUE;
    }
    return FALSE;
}


/*
 * Function to intercept RDI calls that deal with the DCC
 */
typedef void generic_func(void); 

static int DCCRDIInfo(
    void *handle, unsigned type, ARMword *arg1, ARMword *arg2)
{
  DCCState *dcc = (DCCState *)handle;

  union { generic_func *f; void *v; } u;
  u.v = (void*)arg1;

  switch (type) {
  case RDICommsChannel_ToHost:
    dcc->tohost.fn = (RDICCProc_ToHost *)u.f;
    dcc->tohost.arg = (void *)arg2;
    SchedulePollEvent(dcc);
    return RDIError_NoError;

  case RDICommsChannel_FromHost:
    dcc->fromhost.fn = (RDICCProc_FromHost *)u.f;
    dcc->fromhost.arg = (void *)arg2;
    SchedulePollEvent(dcc);
    return RDIError_NoError;

  case RDIInfo_Target:
    *arg1 |= RDITarget_HasCommsChannel;
    break;                      /* return UnimplementedMessage */
  }

  return RDIError_UnimplementedMessage; /* Pass on the call */
}


/*
 * Coprocessor interface
 */
static int DCCMRC(void *handle, int type, ARMword instr, ARMword *value)
{
  /* MRC - reads from the Data Read channel or the control register */
  DCCState *dcc = (DCCState *)handle;
  ARMword reg = BITS(16,19);

  UNUSEDARG(type);
  switch (reg) {
  case 0:                       /* control register */
      *value = dcc->status;
      return ARMul_CP_DONE;

  case 1:                       /* read register */
    *value = dcc->read;
    if(!dcc->bDebug_acc)
    {
        if (!(dcc->status & RBit)) {
            Hostif_ConsolePrint(dcc->hostif,
                                "Read from DCC when register empty.\n");
        }
        dcc->status &= ~RBit;       /* clear read bit */
        dcc->read = 0xdcc00dcc;     /* next read garbled */
        StatusRegisterChanged(dcc);
    }
    return ARMul_CP_DONE;
  }

  return ARMul_CP_CANT;
}


static int DCCMCR(void *handle, int type, ARMword instr, ARMword *data)
{
  /* MCR - write to the Data Write channel */
  DCCState *dcc = (DCCState *)handle;
  ARMword reg = BITS(16,19); ARMword value = *data;

  UNUSEDARG(type);
  switch (reg) {
  case 0:
      if(dcc->bDebug_acc)
      {
          dcc->status = (value & 3) | (1 << 28);
          StatusRegisterChanged(dcc);
          return  ARMul_CP_DONE;
      }
      else
          break;
  case 1:                       /* write register */
    if ((dcc->status & WBit) != 0) {
        Hostif_ConsolePrint(dcc->hostif,
                           "Write to DCC register before cleared.\n");
        dcc->write = 0xdcc00dcc; /* garbled */
        dcc->status |= WBit;
    } else {
        dcc->write = value;
        dcc->status |= WBit;
    }
    StatusRegisterChanged(dcc);
    return ARMul_CP_DONE;
  }
  return ARMul_CP_CANT;
}


static int DCCRead(void *handle, int type, ARMword reg, ARMword *value)
{
  DCCState *dcc = (DCCState *)handle;
  (void)type;
  switch (reg) {
  case 0:
    *value = dcc->status;
    return TRUE;

  case 1:
    value[0] = dcc->read;
    value[1] = dcc->write;
    return TRUE;
  }

  return FALSE;
}


static int DCCWrite(void *handle, int type, ARMword reg, ARMword *value)
{
  DCCState *dcc = (DCCState *)handle;
  (void)type;
  switch (reg) {
  case 0:
    dcc->status = (*value & 3) | (1 << 28);
    StatusRegisterChanged(dcc);
    return TRUE;

  case 1:
    dcc->read = value[0];
    dcc->write = value[1];
    return TRUE;
  }

  return FALSE;
}


static const unsigned int DCCRegBytes[] = {
  2,
  4, 8
};





static ARMword Properties(toolconf config)
{
  unsigned int i;
  ARMword prop;

  prop = 0;

  for (i = 0; DCCOption[i].option != NULL; i++) {
    const char *option = ToolConf_Lookup(config, DCCOption[i].option);
    if (option != NULL) {
      prop = ToolConf_AddFlag(option, prop, DCCOption[i].flag, TRUE);
    }
  }

  return prop;
}

IMPLEMENT_COPRO_DESC_FN_FROM_ARRAY(DCCDesc,DCCState,DCCRegBytes)

ARMul_CoprocessorV5 dcc_cp = {0 /* CDP */, 
                              DCCMCR /* MCR */, DCCMRC /* MRC */,
                              0,0, /* MCRR, MRRC */
                              0 /* LDC */, 0 /* STC */,
                              DCCDesc,
                              DCCRead, DCCWrite} ;



BEGIN_INIT(DCC)
{
  DCCState *dcc = state;
  unsigned long mccfg;


  /* Get the Interrupt controller interface structure pointer.
   */
  state->intcInterface = ARMulif_GetInterruptController(&state->coredesc);

  dcc->fromhost.fn = NULL;
  dcc->tohost.fn = NULL;

  dcc->status = 1 << 28;
  dcc->read = dcc->write = 0;

  dcc->fromhost.latched = FALSE;
  dcc->tohost.latched = FALSE;

  dcc->prop = Properties(config);

  mccfg = ToolConf_DLookupUInt(config, ARMulCnf_MCCfg, 20);
  dcc->poll_interval = (ToolConf_DLookupUInt(config, ARMulCnf_Rate,       1000)
                        * mccfg);
  dcc->read_delay =    (ToolConf_DLookupUInt(config, ARMulCnf_ReadDelay,  500)
                        * mccfg);
  dcc->write_delay =   (ToolConf_DLookupUInt(config, ARMulCnf_WriteDelay, 750)
                        * mccfg);

  dcc->polling = FALSE;

  /* Attempt to locate an interrupt controller */
  /* This model is initialised after memory, so any interrupt controller
   * must have been installed by now
   */
  dcc->rxirqno = ToolConf_DLookupUInt(config, ARMulCnf_CommRXIRQNo, 2);
  dcc->txirqno = ToolConf_DLookupUInt(config, ARMulCnf_CommTXIRQNo, 3);
  dcc->rxirq = FALSE;
  dcc->txirq = FALSE;

  ARMulif_InstallUnkRDIInfoHandler(&dcc->coredesc, DCCRDIInfo, dcc);
  ARMulif_InstallEventHandler(&state->coredesc,
                              ConfigEventSel,
                              DCC_ConfigEvents, state);
  /* if (ARMul_Properties(state) & ARM_Debug_Prop)... */
  ARMulif_InstallCoprocessorV5(&dcc->coredesc,14,&dcc_cp,(void *)state);
  Hostif_PrettyPrint(dcc->hostif, config, ", Debug Comms Channel");
}
END_INIT(DCC)

BEGIN_EXIT(DCC)
END_EXIT(DCC)


/*--- <SORDI STUFF> ---*/


#define SORDI_DLL_DESCRIPTION_STRING "Debug Comms Channel (a Coprocessor)"
#define SORDI_RDI_PROCVEC DCC_AgentRDI
#include "perip_sordi.h"

#include "perip_rdi_agent.h"
    IMPLEMENT_AGENT_PROCS_NOEXE_NOMODULE(DCC)
    IMPLEMENT_AGENT_PROCVEC_NOEXE(DCC)

/*--- </> ---*/


/* EOF dcc.c */


