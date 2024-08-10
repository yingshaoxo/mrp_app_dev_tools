/* tracer.c - module to trace memory accesses and instruction execution
 *            in ARMulator.
 * Copyright (c) 1996-1998 Advanced RISC Machines Limited. All Rights Reserved.
 * Copyright (c) 1998-1999  ARM Limited. All Rights Reserved.
 *
 * RCS $Revision: 1.11.2.30 $
 * Checkin $Date: 2001/10/21 18:32:09 $
 * Revising $Author: dsinclai $
 */


#define MODEL_NAME Tracer

#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>       /* for strcpy */
#include "armul_cnf.h"

#include "minperip.h"
#include "rdi_prop.h"    /* for RDIProperty_GetAsNumeric */
#include "armul_callbackid.h"
#define JAVA_EXT_MASK 0x1FF

#include "javadis.h"     /* for disass_java */
#include "disass.h"
#ifdef ETRACE
#include "rdi_rti.h"
#endif

#ifdef SOCKETS
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#include "armul_mem.h"


#ifndef NDEBUG
# if 1
# else
#  define VERBOSE_INIT_CPREG_TRACE
#  define VERBOSE_BUS
#  define VERBOSE_RDI_LOG
# endif
#endif

typedef uint8 byte;


#define CURRENTMODE RDIMode_Curr


/* from armcnf.h */
#define ARMulCnf_TraceInstructions (tag_t)"TRACEINSTRUCTIONS"
#define ARMulCnf_TraceMemory (tag_t)"TRACEMEMORY"
#define ARMulCnf_TraceIdle (tag_t)"TRACEIDLE"
#define ARMulCnf_TraceNonAcc (tag_t)"TRACENONACCOUNTED"
#define ARMulCnf_TraceWaits (tag_t)"TRACEWAITS"
#define ARMulCnf_TraceEvents (tag_t)"TRACEEVENTS"
#define ARMulCnf_TraceRegisters (tag_t)"TRACEREGISTERS"
#define ARMulCnf_Disassemble (tag_t)"DISASSEMBLE"
#define ARMulCnf_Unbuffered (tag_t)"UNBUFFERED"
#define ARMulCnf_StartOn (tag_t)"STARTON"
#define ARMulCnf_File (tag_t)"FILE"
#define ARMulCnf_BinFile (tag_t)"BINFILE"
#define ARMulCnf_Port (tag_t)"PORT"
#define ARMulCnf_Host (tag_t)"HOST"
#define ARMulCnf_Pipe (tag_t)"PIPE"
#define ARMulCnf_EventMask (tag_t)"EVENTMASK"
#define ARMulCnf_Event (tag_t)"EVENT"
#define ARMulCnf_Range (tag_t)"RANGE"
#define ARMulCnf_Sample (tag_t)"SAMPLE"
#define ARMulCnf_TraceBus (tag_t)"TRACEBUS"
#define ARMulCnf_RDILog (tag_t)"RDILOG"
#define ARMulCnf_TimeStamp (tag_t)"TIMESTAMP"
#define ARMulCnf_OpcodeFetch (tag_t)"OPCODEFETCH"
#define ARMulCnf_Arch (tag_t)"TRACEEIS"

/* --- used to be in tracer.h --- */

#define TRACE_INSTR      0x00000001
#define TRACE_MEM        0x00000002
#define TRACE_IDLE       0x00000004
#define TRACE_RANGE      0x00000008
#define TRACE_PIPE       0x00000010     /* use pclose to close */
#define TRACE_TEXT       0x00000020     /* stream is plain text */
#define TRACE_EVENT      0x00000040
#define TRACE_DISASS     0x00000080     /* disassemble instruction stream */
#define TRACE_BUS        0x00000100     /* trace at the bus/core */
#define TRACE_NONACC     0x00000200     /* trace non-accounted accesses */
#define TRACE_WAITS      0x00000400     /* show wait states returned */
#define TRACE_REGISTER   0x00000800     /* trace changes to register values */  
#define TRACE_UNBUFF     0x00001000     /* unbuffered stream output */  
/* !Todo: Allow timestamps of CoreCycles. */
#define TRACE_TIMESTAMP  0x00002000     /* prepend time-stamps. */
#define TRACE_OPCODEFETCH 0x00004000    /* Trace opcode fetches. */
#define TRACE_ARCH       0x00008000     /* "Architectural" trace */

#define TRACE_STARTON    0x80000000     /* tracing always on */
#define TRACE_VERBOSE    0x40000000


/* Functions for standard "dispatch" */
typedef int Tracer_PrintfProc(void *handle, const char *format, ...);
typedef int Tracer_PutSProc(const char *string, void *handle);
typedef int Tracer_PutCProc(char c, void *handle);
typedef int Tracer_WriteProc(void *packet, int size, int n, void *handle);
typedef int Tracer_CloseProc(void *handle);
typedef int Tracer_FlushProc(void *handle);

/* A record describing one coprocessor-register being traced. */
struct CPregTrace {
    unsigned32 mask; /* for asking the core */
    unsigned reg, cpnum;
    unsigned numwords;
    ARMword *old, *new;
};

typedef struct {
  unsigned32 addr;
  ARMul_acc acc;
} Tracer_Pipeline;

enum EPacketType {
    Trace_Instr,                /* instruction execution */
    Trace_MemAccess,            /* memory cycles, includes idles */
    Trace_BusAccess,
    Trace_Event                 /* other misc events */
};

/* We have one of these per MemInterface we trace on. */
typedef struct ATracer {
    struct TracerState *ts;

  /* Pipeline prediction checking */
  Tracer_Pipeline current, advance;
  Tracer_Pipeline dside_current, dside_advance;
  Tracer_Pipeline iside_current, iside_advance;

  unsigned32 memfeatures;       /* features when displaying mem cycles */

  ARMul_MemInterface bus_mem, child;
  ARMul_MemInterfaceRef mem_ref;
  enum EPacketType packet_type;
    /* This one is 'const'. */
  enum EPacketType as_TraceType;

    /* For async2 memory-types */
  int armiss_err;

} ATracer;

BEGIN_STATE_DECL(Tracer)
  unsigned int not_tracing;     /* zero when RDILog_Trace is set */
  unsigned int trace_opened;    /* set to one once Tracer_Open called */
  struct {
    void *handle;               /* usually a FILE * */
    Tracer_PrintfProc *printf;  /* fprintf */
    Tracer_PutSProc *puts;      /* fputs */
    Tracer_PutCProc *putc;      /* fputc */
    Tracer_WriteProc *write;    /* fwrite */
    Tracer_CloseProc *close;    /* fclose */
    Tracer_FlushProc *flush;    /* fflush */
  } output;
  unsigned32 prop;
  uint32 initial_rdi_log;

  ATracer Core_Tracer, Bus_Tracer;

  unsigned32 prev_instr;        /* Thumb 2-instr BL disassembly */
  ARMword range_lo,range_hi;
  unsigned long sample,sample_base;
  unsigned int event_mask,event_set; /* masks for events */
  void *hourglass_kh,*trace_event_kh; /* handles for ARMulator callbacks */

  unsigned32 mainregister[16];
  unsigned32 cpsrregister, spsrregister;

  void *memcallback;  /* so we can deregister it */
  ARMTime LastTime;
  unsigned line_number;
  unsigned break_line;
  CVector cpregs; /* For traceing coprocessor-registers. */
END_STATE_DECL(Tracer)

#define TRACE_MEMFEATURE_AMBA          0x1    /* use AMBA mnemonics */
#define TRACE_MEMFEATURE_PIPELINED     0x2    /* check next cycle info */

enum INSTR_SET { ISET_ARM, ISET_THUMB, ISET_JAVA };

typedef struct {
  enum EPacketType type;
  union {
    struct {
      unsigned32 instr;
      unsigned32 pc;
      unsigned8 executed;       /* 1 if executed, 0 otherwise */
      enum INSTR_SET iset;
    } instr;
    struct {
      ARMul_acc acc;
      unsigned32 addr;
      unsigned32 word1,word2;
      int rv;                   /* return value from mem_access call */
      Tracer_Pipeline predict;
    } mem_access;
    struct {
      unsigned32 addr,addr2;
      unsigned int type;        /* see note in armdefs.h */
    } event;
  } u;
} Trace_Packet;




static struct {
  tag_t option;
  unsigned long flag;
  char *print;
} TraceOption[] = {
 {   ARMulCnf_TraceInstructions, TRACE_INSTR,    "Instructions"},
 {   ARMulCnf_TraceMemory,       TRACE_MEM,      "Memory accesses"},
 {   ARMulCnf_TraceIdle,         TRACE_IDLE,     "Idle cycles"},
 {   ARMulCnf_TraceEvents,       TRACE_EVENT,    "Events"},
 {   ARMulCnf_Disassemble,       TRACE_DISASS,   "Disassemble"},
 {   ARMulCnf_Unbuffered,        TRACE_UNBUFF,   "Unbuffered"},
 {   ARMulCnf_StartOn,           TRACE_STARTON,  "Start on"},
 {   ARMulCnf_TraceBus,          TRACE_BUS,      "Trace bus"},
 {   ARMulCnf_TraceNonAcc,       TRACE_NONACC,   "Non-accounted"},
 {   ARMulCnf_TraceWaits,        TRACE_WAITS,    "Waits"},
 {   ARMulCnf_TraceRegisters,    TRACE_REGISTER, "Trace registers"},
 {   ARMulCnf_TimeStamp,         TRACE_TIMESTAMP, "TimeStamp"},
 {   ARMulCnf_OpcodeFetch,       TRACE_OPCODEFETCH,"Opcode Fetch"},
 {   ARMulCnf_Arch,              TRACE_ARCH,      "EIS-format"},
 {   NULL, 0, NULL}
};

/* other option fields */


/*
 * The trace function generates a stream of packets, which are either
 * logged into a file or sent to a socket.
 */

static char *cb_proc(
    dis_cb_type t, int32 offset, unsigned32 addr, int width, void *arg,
    char *buf)
{
  UNUSEDARG(t); UNUSEDARG(offset); UNUSEDARG(addr); UNUSEDARG(width); UNUSEDARG(arg);
  return buf;
}

static int Tracer_Acc(ATracer *as, ARMul_acc acc, unsigned32 addr,
                      int packet_type)
{
  TracerState *ts = as->ts;
  void *f=ts->output.handle;
  Tracer_PrintfProc *tprintf = ts->output.printf;
  Tracer_PutCProc *tputc = ts->output.putc;
  int width=8;

  switch(packet_type) {
  case Trace_BusAccess:
      tputc('B',f);
      break;
  case Trace_MemAccess:
  default:
      tputc('M',f);
  }
  if (acc_CYCLE(acc) == acc_Icycle)
      {
    /* This is used for 2 purposes:
     *   - SDT 7TDMI ARMulator generates these for 'internal cycles'
     *     Not bus idles because needed to decode merged IS cycles
     *     However SDT 7TDMI does not provide correct address.
     *   - AMBA real bus idle cycles
     */
    tputc('I',f); /* has no relevant address or data */
  } else {
    if (acc_MREQ(acc)) {
      /* These casts are needed to remove warnings under MSVC++ */
      tputc((char)(acc_SEQ(acc) ? 'S' : 'N'), f);
      tputc((char)(acc_READ(acc) ? 'R' : 'W'), f);
      switch (acc_WIDTH(acc)) {
      case BITS_8:  tputc('1',f); width=2; break;
      case BITS_16: tputc('2',f); width=4; break;
      case BITS_32: tputc('4',f); break;
      case BITS_64: tputc('8',f); break;
      default: tputc('r',f); break; /* reserved */
      }
      tputc((char)(acc_OPC(acc) ? 'O' : '_'), f);
      tputc((char)(acc_LOCK(acc) ? 'L' : '_'), f);
      tputc((char)(acc_SPEC(acc) ? 'S' : '_'), f);
      tputc((char)(acc_DMORE(acc) ? 'D' : '_'), f);
    } else {
      if (as->memfeatures & TRACE_MEMFEATURE_AMBA) {
        tputc((char)(acc_SEQ(acc) ? 'r' /* reserved */ : 'A'), f);
        tputc((char)(acc_READ(acc) ? 'R' : 'W'), f);
        switch (acc_WIDTH(acc)) {
        case BITS_8:  tputc('1',f); width=2; break;
        case BITS_16: tputc('2',f); width=4; break;
        case BITS_32: tputc('4',f); break;
        case BITS_64: tputc('8',f); break;
        default: tputc('_',f); break; /* reserved */
      }
      tputc((char)(acc_OPC(acc) ? 'O' : '_'), f);
      tputc((char)(acc_LOCK(acc) ? 'L' : '_'), f);
      tputc((char)(acc_SPEC(acc) ? 'S' : '_'), f);
      tputc((char)(acc_DMORE(acc) ? 'D' : '_'), f);
      } else if (acc_SPEC(acc)) {
        tputc((char)(acc_SEQ(acc) ? 'S' /* reserved */ : 'N'), f);
        tputc((char)(acc_READ(acc) ? 'R' : 'W'), f);
        switch (acc_WIDTH(acc)) {
        case BITS_8:  tputc('1',f); width=2; break;
        case BITS_16: tputc('2',f); width=4; break;
        case BITS_32: tputc('4',f); break;
        case BITS_64: tputc('8',f); break;
        default: tputc('_',f); break; /* reserved */
        }
        tputc((char)(acc_OPC(acc) ? 'O' : '_'), f);
        tputc((char)(acc_LOCK(acc) ? 'L' : '_'), f);
        tputc((char)(acc_SPEC(acc) ? 'S' : '_'), f);
        tputc((char)(acc_DMORE(acc) ? 'D' : '_'), f);
      } else if (acc_KILL(acc)) {
        tputc((char)(acc_SEQ(acc) ? 'S' : 'N'), f);
        tputc((char)(acc_READ(acc) ? 'R' : 'W'), f);
        switch (acc_WIDTH(acc)) {
        case BITS_8:  tputc('1',f); width=2; break;
        case BITS_16: tputc('2',f); width=4; break;
        case BITS_32: tputc('4',f); break;
        case BITS_64: tputc('8',f); break;
        default: tputc('_',f); break; /* reserved */
        }
        tputc((char)(acc_OPC(acc) ? 'O' : '_'), f);
        tputc((char)(acc_LOCK(acc) ? 'L' : '_'), f);
        tputc((char)(acc_KILL(acc) ? 'K' : '_'), f);
        tputc((char)(acc_DMORE(acc) ? 'D' : '_'), f);
      } else {
        tputc((char)(acc_SEQ(acc) ? 'C' : 'I'),f);
        if (acc_OPC(acc)) tputc('O', f);
      }
    }
    if (as->memfeatures & TRACE_MEMFEATURE_AMBA) {
      switch (acc_BURST_TYPE(acc)) {
      case acc_burst_none:  tputc('_',f); break;
      case acc_burst_4word: tputc('4',f); break;
      case acc_burst_8word: tputc('8',f); break;
      default:              tputc('r',f); break; /* reserved */
      }
    }

    if (acc_MREQ(acc) || acc_SPEC(acc) || (as->memfeatures & TRACE_MEMFEATURE_AMBA))
      /* no valid address supplied in ARMulator idle cycles */
      tprintf(f, " %08lX", addr);
  }

  return width;
}

static char const *eventSignalNames[] = {
    "RESET","UNDEF","SWI","PABORT","DABORT","SE","IRQ","FIQ",
    "BreakPt","WatchPt","Stop"
};


/* Purpose: Trigger a breakpoint when on required line. */
static void Tracer_newline(TracerState *ts)
{
    void *f=ts->output.handle;
    Tracer_PutCProc *tputc = ts->output.putc;
    tputc('\n', f);
    if (++ts->line_number == ts->break_line) {
        /* Maybe RDIError_WatchpointAccessed, or RDIError_UserInterrupt? */
        ARMulif_StopExecution(&ts->coredesc,RDIError_BreakpointReached);
    }
}


static void tracer_javadis(uint32 instr, ARMword pc, char *buffer)
{
    byte bytes[3];
    bytes[0]=(byte)instr;
    bytes[1]=(byte)(instr>>16);
    bytes[2]=(byte)(instr>>24);

    /* There is a problem with disassembling some opcodes which take extra
     * words after them, so we special-case those here. */
    switch (bytes[0])
    {
    case 170: sprintf(buffer,"tableswitch");
        break;
    case 171: sprintf(buffer,"lookupswitch");
        break;
    default:
        (void)disass_java(bytes,pc,buffer);
    }
}

static void tracer_disass(TracerState *ts, Trace_Packet *packet,
                          uint32 instr, ARMword pc, char *buffer)
{
    switch(packet->u.instr.iset) 
    {
    case ISET_THUMB:
        /* Complications for Thumb 2-instruction BL...
         * ARM disass API (current instr, next instr) is not ideal
         * for disassembling instructions as they are executed, where
         * want to specify current instr & previous instr.
         */
        if (instr>>11==0x1E) {
            strcpy(buffer, "(1st instr of BL pair)");
        } else if (instr>>11==0x1F) {  /* second instruction */
            /* if prev_instr is invalid, disass_16 shows "???" */
            disass_16(ts->prev_instr,instr,pc-2,buffer,NULL,cb_proc);
        } else {
            disass_16(instr,0,pc,buffer,NULL,cb_proc);
        }
        ts->prev_instr=instr;
        break;
    case ISET_ARM:
        disass(instr,pc,buffer,NULL,cb_proc);
        break;
    case ISET_JAVA:
        tracer_javadis(instr, pc, buffer);
        break;
    default:
        sprintf(buffer,"(UNKNOWN INSTRUCTION SET)");
        break;
    }
}


/* Tracing for Arch mode.*/
#define UNKNOWN_REG ((ARMword)-1)

static void TraceAccess(TracerState *ts, char id[], ARMword address, 
                        ARMword data, ARMword reg,
                        Trace_Packet *packet)
{
    void *f=ts->output.handle;
    Tracer_PrintfProc *tprintf = ts->output.printf;
    Tracer_PutSProc *tputs = ts->output.puts;
    Tracer_PutCProc *tputc = ts->output.putc;
    char localid[4];
    
    localid[0] = id[0];
    localid[1] = id[1];
    localid[2] = id[2];
    localid[3] = '\0';
    if (localid[0] == 'I') { /* instruction */
        switch (localid[1])
        {
        default:
        case 'J': /* JAVA */
        case 'A': /* ARM */
            tprintf(f,"%3s %08x %08x ", localid, (unsigned)address,
                        (unsigned)data);
            break;
        case 'T': /* Thumb */
            tprintf(f,"%3s %08x %04x ", localid, (unsigned)address,
                    (unsigned)data);
            break;
        }

        if (ts->prop & TRACE_DISASS) {
            ARMword instr = data; char buffer[256];
            ARMword pc = address;

            tracer_disass(ts, packet, instr, pc, buffer);
            if (!(id[2]=='P')/*executed*/) {
                char *p;
                for (p=buffer; *p; p++)
                    if (isupper((int)*p)) *p=(char)tolower((int)*p);
            }
            tputc(' ', f);
            tputs(buffer, f);
        }
    }
    else
    {   /* not Instruction */
        if (reg != UNKNOWN_REG)
            tprintf(f,"%3s %08x %08x %1x", localid, (unsigned)address,
                    (unsigned)data,(unsigned)reg);
        else
            tprintf(f,"%3s %08x %08x", localid, (unsigned)address,
                    (unsigned)data);
    }
    Tracer_newline(ts);
}

/* Returns RDIError_NoError => data at *data. */
static int tracer_GetCPReg(RDI_ModuleDesc *mdesc,
                           unsigned32 CPnum,
                           unsigned32 mask,
                           unsigned32 *data)
{
    return mdesc->rdi->CPread(mdesc->handle, CPnum, mask, data);
}

/* Return True if words are not the same .*/
static bool tracer_word_diff(uint32 *old, uint32 *new, unsigned numwords)
{
    while (numwords--)
    {
        if (*old++ != *new++)
            return TRUE;
    }
    return FALSE;
}

/**/
static void traceRegsAsText(TracerState *ts)
{
    if (ts->prop & TRACE_REGISTER) {
        void *f=ts->output.handle;
        Tracer_PrintfProc *tprintf = ts->output.printf;
        /* Tracer_PutSProc *tputs = ts->output.puts; */
        /* Tracer_PutCProc *tputc = ts->output.putc; */

        char nextchar = '\n';
        int regnum;
        unsigned32 regvalue;
        /* We miss out r15 because it's not useful, and is displayed
         * differently between 9 and 10 */
        for (regnum=0; regnum<16-1; regnum++) {
            regvalue=ARMulif_GetReg(&ts->coredesc, CURRENTMODE ,regnum);
            if (regvalue != ts->mainregister[regnum]) {
                if (nextchar == '\n') tprintf(f,"R ");
                else tprintf(f,", ");
                tprintf(f, "r%d=%08lx", regnum, regvalue);
                nextchar = ',';
                ts->mainregister[regnum] = regvalue;
            }
        }
        
        regvalue=ARMulif_GetCPSR(&ts->coredesc);
        if (regvalue != ts->cpsrregister) {
            if (nextchar == '\n') tprintf(f,"R ");
            else tprintf(f,", ");
            tprintf(f, "cpsr=%08lx", regvalue);
            nextchar = ',';
            ts->cpsrregister = regvalue;
        }

        regvalue=ARMulif_GetSPSR(&ts->coredesc,CURRENTMODE);
        if (regvalue != ts->spsrregister) {
            if (nextchar == '\n') tprintf(f,"R ");
            else tprintf(f,", ");
            tprintf(f, "spsr=%08lx", regvalue);
            nextchar = ',';
            ts->spsrregister = regvalue;
        }


        if (nextchar==',') Tracer_newline(ts);
    }

    if (CVector_Count(&ts->cpregs)) {
        void *f=ts->output.handle;
        Tracer_PrintfProc *tprintf = ts->output.printf;
        char nextchar = '\n';
        unsigned i;
        /* We miss out r15 because the information is provided elsewhere, 
         *  and is displayed
         * differently between 9 and 10 */
        for (i=0; i < CVector_Count(&ts->cpregs); i++) {
            struct CPregTrace *cpr = CVector_At(&ts->cpregs,i);
            int err = tracer_GetCPReg(&ts->coredesc, cpr->cpnum, cpr->mask,
                                      cpr->new);/* data */
            if (err == RDIError_NoError && cpr->numwords > 0 &&
                tracer_word_diff(cpr->old,cpr->new,cpr->numwords))
            {
                if (nextchar == '\n') tprintf(f,"RCP ");
                else tprintf(f,", ");
                tprintf(f, "p%dc%d=0x%08lx", cpr->cpnum, cpr->reg,
                        cpr->new[0]);
                {unsigned j;
                for (j=1; j< cpr->numwords; j++)
                    tprintf(f, "_%08lx", cpr->new[j]);
                }
                nextchar = ',';
                {void *temp = cpr->old;
                 cpr->old=cpr->new; cpr->new=temp;
                }
            }
        }
        if (nextchar==',') Tracer_newline(ts);
    }
}


static void Tracer_Dispatch(ATracer *as, Trace_Packet *packet)
{
  TracerState *ts = as->ts;
  void *f=ts->output.handle;
  if (ts->prop & TRACE_TEXT) {
    Tracer_PrintfProc *tprintf = ts->output.printf;
    Tracer_PutSProc *tputs = ts->output.puts;
    Tracer_PutCProc *tputc = ts->output.putc;

    if (ts->prop & TRACE_TIMESTAMP)
    {
        ARMTime t1 = ARMulif_Time(&ts->coredesc);
        if (t1 != ts->LastTime)
        {
            tprintf(f,"T %08lX  ", (unsigned long)t1);
            ts->LastTime = t1;
        }
        else
            tprintf(f,"            ");
    }

    if (ts->prop & TRACE_ARCH)
        switch (packet->type) {
        case Trace_Instr:
            /* ID of instruction access is "I[A|T|J][P|F]". */
        {
            char id[4];
            static char const iset2char[4]="ATJX";
            id[0]='I';
            id[1]=iset2char[packet->u.instr.iset];
            id[2]=(char)(packet->u.instr.executed ? 'P' : 'F');
            id[3]='\0';

            TraceAccess(ts,id,packet->u.instr.pc,
                        packet->u.instr.instr,
                        UNKNOWN_REG /* DESTReg not applicable */, packet);
            /* Registers are handy! */
            traceRegsAsText(ts);
            return;
        }
        
        case Trace_BusAccess:
        case Trace_MemAccess:
            /* ID of memory access is "M[R|W][1|2|4]" */
        {
            char id[4];
            ARMul_acc acc=packet->u.mem_access.acc;
            int rv=packet->u.mem_access.rv;
            int width=1 << (acc & 15);
            assert(rv != 0);
            
            id[0]='M';
            id[1]=(char)(acc_READ(acc) ? 'R' : 'W');
            id[2]=(char)('0' + ((width > 32) ? 4 : (width / 8)));
            id[3]='\0';
            
            if (rv < 0 && (acc_CYCLE(acc) != acc_typeI)) {
                id[0] = 'X';
            }
            TraceAccess(ts,id,packet->u.mem_access.addr,
                        packet->u.mem_access.word1,
                        UNKNOWN_REG /* DESTReg unknown */, packet);
            if (width > 32)
                TraceAccess(ts,id,packet->u.mem_access.addr+4,
                            packet->u.mem_access.word2,
                            UNKNOWN_REG /* DESTReg unknown */, packet);
        }
        default: return;
        }
    else
    switch (packet->type) 
    {
    case Trace_Instr: {
      char buffer[256];
      ARMword instr=packet->u.instr.instr,pc=packet->u.instr.pc;
      unsigned executed=packet->u.instr.executed;
      /* T for Taken, S for Skipped. */
      tprintf(f,"I%c %08lX ", executed ? 'T' : 'S', pc);
      switch(packet->u.instr.iset)
      {
      case ISET_THUMB:
          tprintf(f, "%04lx", instr);
          break;
      case ISET_JAVA:
      {    /* Kill the high bytes */
          unsigned extrabytes = (instr >> 12) & 3; /* 0,1,2 */
          static uint32 masks[4] = 
              { 0x00000FFF, 0x00FF0FFF, 0xFFFF0FFF, 0xFFFF0FFF };
          instr &= masks[extrabytes];
      }
      default:
          tprintf(f, "%08lx", instr);
          break;
      }

      if (ts->prop & TRACE_DISASS) {
        tracer_disass(ts, packet, instr, pc, buffer);
        if (!executed) {
          char *p;
          for (p=buffer; *p; p++)
            if (isupper((int)*p)) *p=(char)tolower((int)*p);
        }
        tputc(' ', f);
        tputs(buffer, f);
      }
      Tracer_newline(ts);
      traceRegsAsText(ts);
    }
      break;

    case Trace_BusAccess:
    case Trace_MemAccess:
    {
      ARMul_acc acc=packet->u.mem_access.acc;
      unsigned32 addr=packet->u.mem_access.addr;
      int rv=packet->u.mem_access.rv;
      int width;

      /* all mem_access packets sent are to be displayed */
      width=Tracer_Acc(as, acc, addr,packet->type); /* prints "M" etc */
      if (acc_CYCLE(acc) != acc_typeI /* includes acc_Icycle */) {
        if (rv != 0) {
          tprintf(f," %0*.*lX", width, width, packet->u.mem_access.word1);
          if (acc_WIDTH(acc)==BITS_64)
            tprintf(f," %08lX", packet->u.mem_access.word2);
        }
      }
      if (rv < 0) {
        tputs(" (abort)", f);
      } else if (rv == 0) {
        tputs(" (wait)", f);
      }
      if (!acc_ACCOUNT(acc)) {
        /* display non-accounted accesses */
        if (acc_MREQ(acc))
          tprintf(f, acc_READ(acc) ? " (peek)" : " (poke)");
      }

      Tracer_newline(ts);
    }
    break;

    case Trace_Event: {
        char const *eventname = (packet->u.event.type >= SignalEvent_RESET &&
            packet->u.event.type <= SignalEvent_Stop) ?
            eventSignalNames[packet->u.event.type - SignalEvent_RESET] : "";
        tprintf(f,"E %08lX %08lX %x %s",
              packet->u.event.addr,packet->u.event.addr2,
              packet->u.event.type, eventname);
        Tracer_newline(ts);
        break;
        }
    }
  } else {
    Tracer_WriteProc *twrite = ts->output.write;
    twrite(packet,sizeof(*packet),1,f);
    fflush(f);
  }
}

/*
 * Veneers onto DebugPrint
 */
static int DebugPutS(const char *s, void *handle)
{
  RDI_HostosInterface *hostif = (RDI_HostosInterface *)handle;
  Hostif_DebugPrint(hostif, "%s", s);
  return 0;
}

static int DebugPutC(char c, void *handle)
{
  RDI_HostosInterface *hostif = (RDI_HostosInterface *)handle;
  Hostif_DebugPrint(hostif, "%c", c);
  return 0;
}

   
/*
 * Open the trace stream.
 * Returns 1 on failure.
 */

static int Banner(TracerState *ts)
{    
  time_t now=time(NULL);
  void *f=ts->output.handle;
  Tracer_PrintfProc *tprintf = ts->output.printf;
  Tracer_PutSProc *tputs = ts->output.puts;
  Tracer_PutCProc *tputc = ts->output.putc;

  tprintf(f, "Date: %s", ctime(&now));
  tprintf(f, "Source: Armul");
  Tracer_newline(ts);
  tputs("Options: ", f);

  if (ts->prop & TRACE_INSTR) {
    char nextch = '(';
    tputs("Trace Instructions  ", f);

    if (ts->prop & TRACE_DISASS) {
      tputc(nextch, f);
      tputs("Disassemble", f);
      nextch = ',';
    }
    if (ts->prop & TRACE_REGISTER) {
      tputc(nextch, f);
      tputs("Registers", f);
      nextch = ',';
    }
    if (nextch != '(')
      tputs(")  ", f);
  }   
   
  if (ts->prop & TRACE_MEM) {
    char nextch = '(';
    tputs("Trace Memory Cycles  ", f);
    if (ts->prop & TRACE_IDLE) {
      tputc(nextch, f);
      tputs("Idles", f);
      nextch = ',';
    }
    if (ts->prop & TRACE_NONACC) {
      tputc(nextch, f);
      tputs("Non-accounted", f);
      nextch = ',';
    }
    if (ts->prop & TRACE_WAITS) {
      tputc(nextch, f);
      tputs("Waits", f);
      nextch = ',';
    }
    if (nextch != '(')
      tputs(")  ", f);
  }
  if (ts->prop & TRACE_EVENT)  tputs("Trace Events  ", f);
  Tracer_newline(ts);
  if (ts->prop & TRACE_RANGE)
  {
    tprintf(f, "Range: 0x%08lX -> 0x%08lX",
            ts->range_lo, ts->range_hi);
    Tracer_newline(ts);
  }
  if (ts->sample_base)
  {
    tprintf(f, "Sample: %ld", ts->sample_base);
    Tracer_newline(ts);
  }
  Tracer_newline(ts);
  return 0;
}

/* Returns 0:Ok, else error. */
static unsigned Tracer_Open(TracerState *ts)
{
  const char *option;
  unsigned verbose;

  verbose=ts->prop & TRACE_VERBOSE;

  if (ToolConf_DLookupBool(ts->config, ARMulCnf_RDILog, FALSE)) {
    /* output to the rdi_log window */
    /* talk directly, rather than via DebugPrint, as it should be faster,
     * and DebugPrint checks rdi_log bit 1
     */
      union { void const *c; void *v; } u;
      u.c = ts->hostif;
      ts->output.handle = u.v;
    ts->output.printf = (Tracer_PrintfProc *)Hostif_DebugPrint;
    ts->output.puts = DebugPutS;
    ts->output.putc = DebugPutC;
    ts->prop|=TRACE_TEXT;

    return Banner(ts);
  }

  option=ToolConf_Lookup(ts->config, ARMulCnf_File);
  if (option!=NULL && option[0]) {
    FILE *f = fopen(option,"w");
    if (f == NULL) {
      fprintf(stderr,"Could not open trace file '%s' - abandoning trace.\n",
              option);
      return 1;
    }
    ts->output.handle = f;
    ts->output.printf = (Tracer_PrintfProc *)fprintf;
    ts->output.puts = (Tracer_PutSProc *)fputs;
    ts->output.putc = (Tracer_PutCProc *)fputc;
    ts->output.close = (Tracer_CloseProc *)fclose;
    ts->output.flush = (Tracer_FlushProc *)fflush;

    ts->prop|=TRACE_TEXT;

    return Banner(ts);
  }

  option=ToolConf_Lookup(ts->config, ARMulCnf_BinFile);
  if (option!=NULL && option[0]) {
    FILE *f = fopen(option,"wb");
    if (f == NULL) {
      fprintf(stderr,"Could not open trace file '%s' - abandoning trace.\n",
              option);
      return 1;
    }
    ts->output.handle = f;
    ts->output.write = (Tracer_WriteProc *)fwrite;
    ts->output.close = (Tracer_CloseProc *)fclose;
    ts->output.flush = (Tracer_FlushProc *)fflush;
    return 0;
  }

#ifdef SOCKETS
  option=ToolConf_Lookup(ts->config, ARMulCnf_Port);
  if (option) {
    long port;
      
    port=strtol(option,NULL,0);

    option=ToolConf_Lookup(ts->config, ARMulCnf_Host);

    if (option) {
      struct hostent *host;
      struct sockaddr_in con;
      int sock;
      
      host=gethostbyname(option);

      if (host==NULL) {
        Hostif_ConsolePrint(ts->hostif,"Could not resolve host '%s'\n",
                           option);
        return 1;
      }
        
      if (verbose)
        Hostif_ConsolePrint(ts->hostif,"Tracing to %s:%d\n",option,port);

      sock=socket(AF_INET,SOCK_STREAM,0);
      if (sock==-1) {
        Hostif_ConsolePrint(ts->hostif,"Could not open trace port\n");
        return 1;
      }
        
      memset(&con,'\0',sizeof(con));
      con.sin_family=AF_INET;
      memcpy(&con.sin_addr,host->h_addr,sizeof(con.sin_addr));
      con.sin_port=htons(port & 0xffff);
        
      if (connect(sock,(struct sockaddr *)&con, sizeof(con))!=0) {
        close(sock);
        Hostif_ConsolePrint(ts->hostif,"Failed to open socket\n");
        return 1;
      }

      ts->output.handle=(void *)fdopen(sock,"wb");
      if (ts->output.handle!=NULL) return 0;
      ts->output.write = (Tracer_WriteProc *)fwrite;
      ts->output.close = (Tracer_CloseProc *)fclose;
      ts->output.flush = (Tracer_FlushProc *)fflush;

      Hostif_ConsolePrint(ts->hostif,"Failed to fdopen socket.\n");
      return 1;
    }

    Hostif_ConsolePrint(ts->hostif,"PORT configured with no HOST.\n");
    return 1;
  }
#endif

#ifdef PIPE
  option=ToolConf_Lookup(ts->config, ARMulCnf_Pipe);
  if (option) {
    ts->prop|=TRACE_PIPE;
    ts->output.handle=(void *)popen(option,"w");
    if (ts->output.handle == NULL) {
      Hostif_ConsolePrint(ts->hostif,
                         "Could not open pipe to '%s' - abandoning trace.\n",
                         option);
      return 0;
    }
    ts->output.write = (Tracer_WriteProc *)fwrite;
    ts->output.close = (Tracer_CloseProc *)pclose;
    ts->output.flush = (Tracer_FlushProc *)fflush;
    return 1;
  }
#endif
  
  fprintf(stderr,"No trace file configured - abandoning trace.\n");
  return 1;
}

static void Tracer_Close(TracerState *ts)
{
  if (ts->output.close)
    ts->output.close(ts->output.handle);
}

static void Tracer_Flush(TracerState *ts)
{
  if (ts->output.flush)
    ts->output.flush(ts->output.handle);
}

/*
 * The function is called from the ARMulator when rdi_log & 16 is true.
 * It is used to generate an executed instruction stream trace.
 */
static GenericCallbackFunc Tracer_EventHandler;
static unsigned Tracer_EventHandler(void *handle, void *data)
{
  ARMul_Event *evt = (ARMul_Event *)data;
  TracerState *ts=(TracerState *)handle;

  Trace_Packet packet;
  unsigned int event = evt->event;
  ARMword addr1 = evt->data1; ARMword addr2 = evt->data2;
  /* Mask events */
  if ((event & ts->event_mask)!=ts->event_set)
      return FALSE;

  if ((ts->prop & TRACE_RANGE) && (addr1<ts->range_lo ||
                                   (addr1>=ts->range_hi && ts->range_hi!=0)))
    /* range test fails */
    return FALSE;

  if (ts->sample_base) {
    if (ts->sample--)           /* not this one */
      return FALSE;
    ts->sample=ts->sample_base-1;
  }

  packet.type=Trace_Event;

  packet.u.event.addr=addr1;
  packet.u.event.addr2=addr2;
  packet.u.event.type=event;

  Tracer_Dispatch(&ts->Core_Tracer,&packet);
  if (ts->prop & TRACE_UNBUFF)
      Tracer_Flush(ts);
  return FALSE;
}

/*
 * this function is called for every instruction.
 * Preconds:
 * . ts->Core_Tracer initialised.
 */

static void trace_func(void *handle, ARMword pc, ARMword instr,
                                           ARMword cpsr, ARMword condpassed)
{
  TracerState *ts=(TracerState *)handle;
  Trace_Packet packet;

  if ((ts->prop & TRACE_RANGE) && (pc<ts->range_lo ||
                                   (pc>=ts->range_hi && ts->range_hi!=0)))
    /* range test fails */
    return;
  
  packet.u.instr.iset = ((cpsr >> (24-1)) & 2)  /* J BIT -> bit 1 */
                      | ((cpsr >> 5) & 1);      /* T BIT -> bit 0 */
  if (packet.u.instr.iset == ISET_JAVA)
  {
      /* Don't trace cycles which are not the first for each java-opcode,
       * or other things which are not real java opcodes
       * (e.g. prefetch abort) -
       * the disassembler will not understand. */
      if ((instr & JAVA_EXT_MASK) > 255)
          return;
      /* condpassed is not useful for Java bytecodes. */
      packet.u.instr.executed = TRUE;
  }
  else
  {
      packet.u.instr.executed = (unsigned char)(condpassed & 1);
  }

  if (ts->sample_base) {
    if (ts->sample--)           /* not this one */
      return;
    ts->sample=ts->sample_base-1;
  }


  packet.type=Trace_Instr;
  packet.u.instr.pc=pc;
  packet.u.instr.instr=instr;
  Tracer_Dispatch(&ts->Core_Tracer,&packet);
  if (ts->prop & TRACE_UNBUFF)
      Tracer_Flush(ts);
}


static int TraceX(ATracer *as, ARMword addr, uint32 *data, int rv,
                   unsigned acc,
                   int packet_type)
{
  TracerState *ts = as->ts;
  Trace_Packet packet;
  if ((ts->prop & TRACE_RANGE) && (addr<ts->range_lo ||
                                   (addr>=ts->range_hi && ts->range_hi!=0)))
  {
    /* range test fails */
#ifdef VERBOSE_BUS
  printf("!Tracer_MemAccess - out of range!\n");
#endif

    return rv;
  }

  if (ts->sample_base) {
    if (ts->sample--)           /* not this one */
    {
      return rv;
    }
    ts->sample=ts->sample_base-1;
  }

  if (/* (ts->prop & TRACE_MEM) && */
      ((ts->prop & TRACE_WAITS) || (rv != 0)) &&
      ((ts->prop & TRACE_NONACC) || acc_ACCOUNT(acc)) &&
      ((ts->prop & TRACE_IDLE) || !ACCESS_IS_IDLE(acc)) &&
      /* We always care about prefetch-aborts! */
      ((ts->prop & TRACE_OPCODEFETCH) || !acc_OPC(acc) || rv < 0)
      ) 
  {
    packet.type=packet_type; /* E.g. Trace_MemAccess */
    packet.u.mem_access.acc=acc;
    packet.u.mem_access.addr=addr;
    packet.u.mem_access.predict=as->current;
/*    packet.u.mem_access.word1=(acc_MREQ(acc) ? *word : 0);  -- original */
    packet.u.mem_access.word1=(data != NULL) ? *data : 0;
    if (acc_WIDTH(acc) == BITS_64)
        packet.u.mem_access.word2=(data != NULL) ? data[1] : 0;
    packet.u.mem_access.rv=rv;

    Tracer_Dispatch(as,&packet);
    if (ts->prop & TRACE_UNBUFF)
        Tracer_Flush(ts);

    /* Use ts->advance to store predicted cycle information, then
     * delay into ts->current to check prediction against actual. */
    if (acc_ACCOUNT(acc) && rv != 0) {
      as->current=as->advance;
    }
  }
#ifdef VERBOSE_NOSEND
  else
  {
      printf("!Tracer_MemAccess - not sending packet!\n");
  }
#endif
  return rv;
}

#ifdef USE_TRACEMEMEVENT

static unsigned Tracer_MemAccess_X(struct GenericTraceCallback *myCB,
                                   ARMword addr,
                                   ARMword data,
                                   unsigned acc,
                                   int trace_result,
                                   int packet_type)
{
  ATracer *as=(ATracer *)myCB->handle;
  TracerState *ts=as->ts;
  unsigned rv = RDIError_NoError; /* PERIP_OK? */

  if (myCB->next) {
      myCB->next->func(myCB->next,addr,data,acc, trace_result);
  }
  else {
      /*  assume no waits? */
      rv = 1;
  }
  if (ts->not_tracing) return rv;

#ifdef VERBOSE
  printf("!Tracer_MemAccess, tracing!\n");
#endif
  TraceX(as,addr,data,rv,acc, packet_type);

  return rv;
}



static void Tracer_MemAccess_Mem(struct GenericTraceCallback *myCB,
                                 ARMword addr,
                                 ARMword word,
                                 unsigned acc,
                                 int trace_result)
{
    Tracer_MemAccess_X(myCB, addr, word, acc, trace_result, Trace_MemAccess);
}
#endif /*?>>>*/


/* Function called when RDI_LOG changes, so we can control logging
 * memory accesses */

/* <<<< Pass-thru functions */

static void TraceNextCycle(void *handle,ARMword addr,ARMul_acc acc)
{
  ATracer *as = (ATracer *)handle;
  /* TracerState *ts = as->ts; */

  as->child.x.pipe.next(as->child.handle,addr,acc); 

  /* NextCycle info before non-accounted accesses should
   *   - ideally not happen
   *   - be marked as non-accounted if it does happen
   * This enables the NextCycle info from the previous child
   * access to relate to the next child access regardless of
   * intervening non-accounted accesses.
   */
  if (acc_ACCOUNT(acc)) {
    /* store next cycle information */
    as->advance.addr = addr;
    as->advance.acc = acc;
  }
}

static void TraceNextDCycle(void *handle,ARMword addr, ARMword data, ARMul_acc acc)
{
  ATracer *as = (ATracer *)handle;
/*  TracerState *ts = (TracerState *)handle;*/

  as->child.x.pipe_arm9.dside_next(as->child.handle,addr,data,acc); 

/* NextCycle info before non-accounted accesses should
 *   - ideally not happen
 *   - be marked as non-accounted if it does happen
 * This enables the NextCycle info from the previous real
 * access to relate to the next real access regardless of
 * intervening non-accounted accesses.
 */
  if (acc_ACCOUNT(acc)) {
    /* store next cycle information */
    as->dside_advance.addr=addr;
    as->dside_advance.acc=acc;
  }
}

static void TraceNextICycle(void *handle,ARMword addr,ARMul_acc acc)
{
  ATracer *as = (ATracer *)handle;

  as->child.x.pipe_arm9.iside_next(as->child.handle,addr,acc); 

  /* NextCycle info before non-accounted accesses should
   *   - ideally not happen
   *   - be marked as non-accounted if it does happen
   * This enables the NextCycle info from the previous child
   * access to relate to the next child access regardless of
   * intervening non-accounted accesses.
   */
  if (acc_ACCOUNT(acc)) {
    /* store next cycle information */
    as->iside_advance.addr = addr;
    as->iside_advance.acc = acc;
  }
}


/* Pipelined Multi-Layer AHB memory interface functions */

static unsigned int TraceMultiLayerRegister(void *handle, tag_t hint)
{
    /* AHB Layer registration */
    ATracer *as = (ATracer *)handle;
    
    return as->child.x.pipe_multi_layer.register_layer(as->child.handle, hint);
}

static unsigned long TraceMultiLayerDeltaCycles(void *handle, unsigned int layer_id)
{
  ATracer *as = (ATracer *)handle;

  return (as->child.x.pipe_multi_layer.delta_cycles(as->child.handle, layer_id));
}

static void TraceBusMultiLayerNext(void *handle, unsigned int layer_id, ARMword address, ARMword data, ARMul_acc acc)
{
  ATracer *as = (ATracer *)handle;

  as->child.x.pipe_multi_layer.next_access(as->child.handle, layer_id, address, data, acc); 

  /* NextCycle info before non-accounted accesses should
   *   - ideally not happen
   *   - be marked as non-accounted if it does happen
   * This enables the NextCycle info from the previous child
   * access to relate to the next child access regardless of
   * intervening non-accounted accesses.
   */
  if (acc_ACCOUNT(acc)) {
    /* store next cycle information */
    as->advance.addr = address;
    as->advance.acc = acc;
  }
}

static int TraceBusMultiLayerCurrent(void *handle, unsigned int layer_id, ARMword address, 
                                     ARMword *data, ARMul_acc acc)
{
    ATracer *as = (ATracer *)handle;
    int err = as->child.x.pipe_multi_layer.current_access(as->child.handle, layer_id, address, data, acc);

    TraceX(as, address, data, err, acc, as->as_TraceType);
    return err;
}


static unsigned long TraceDeltaCycles(void *handle)
{
  ATracer *as = (ATracer *)handle;

  return (as->child.x.pipe.delta_cycles(as->child.handle));
}

static void TraceCoreException(void *handle,ARMword address,
                               ARMword penc)
{
    ATracer *as = (ATracer *)handle;
    as->child.x.arm8.core_exception(as->child.handle,address,penc);
}

static unsigned int TraceDataCacheBusy(void *handle)
{
    ATracer *as = (ATracer *)handle;
    return as->child.x.strongarm.data_cache_busy(as->child.handle);
}

static void TraceBusMemAccessHarvard(void *handle,
                     ARMword daddr,ARMword *dword, ARMul_acc dacc, int *drv,
                     ARMword iaddr,ARMword *iword, ARMul_acc iacc, int *irv)
{
  ATracer *as=(ATracer *)handle;
  TracerState *ts = as->ts;
  as->child.x.pipe_arm9.harvard_access(as->child.handle,
                                       daddr, dword, dacc, drv,
                                       iaddr, iword, iacc, irv); 

  as->current = as->dside_current;
  /* The checks for *drv may not have the desired effect? */
  if (*drv) { /* Don't bother with all those waits! */
      TraceX(as, daddr, dword, *drv, dacc, as->as_TraceType);
  }
  as->current = as->iside_current;
  if (*irv) { /* Don't trace all those waits! */
      TraceX(as, iaddr, iword, *irv, iacc, as->as_TraceType);
  }

  if (!(ts->prop & TRACE_RANGE) || ((daddr>=ts->range_lo) && (daddr>=ts->range_hi))) {
    if (acc_ACCOUNT(dacc) && (*drv != 0)) {
      as->dside_current=as->dside_advance;  /* delay next cycle information by 1 cycle */
    }
  }

  if (!(ts->prop & TRACE_RANGE) || ((iaddr>=ts->range_lo) && (iaddr>=ts->range_hi))) {
    if (acc_ACCOUNT(iacc) && (*irv != 0)) {
      as->current=as->advance;  /* delay next cycle information by 1 cycle */
    }
  }
}

/* BEGIN ARMISS */

static armul_MemAccAsync TracerARMiss_IAccess;
static armul_MemAccAsync TracerARMiss_DRead;
static armul_MemAccAsync TracerARMiss_DWrite;


int TracerARMiss_IAccess(void *handle ,ARMword address, ARMword *data ,ARMul_acc acc, ARMTime *abs_time)
{
    ATracer *as=(ATracer *)handle;
    int rv;
    as->armiss_err = PERIP_OK;
    rv = as->child.x.armiss_cache.i_access(as->child.handle,address,data,acc,
                                          abs_time);

    TraceX(as, address, data, as->armiss_err, acc, as->as_TraceType);
    return rv;
}
int TracerARMiss_DRead(void *handle ,ARMword address, ARMword *data ,
                       ARMul_acc acc, ARMTime *abs_time)
{
    ATracer *as=(ATracer *)handle;
    int rv;
    as->armiss_err = PERIP_OK;
    rv = as->child.x.armiss_cache.d_read(as->child.handle,address,data,acc,
                                        abs_time);

    TraceX(as, address, data, as->armiss_err, acc, as->as_TraceType);
    return rv;
}
int TracerARMiss_DWrite(void *handle ,ARMword address, ARMword *data ,
                        ARMul_acc acc, ARMTime *abs_time)
{
    ATracer *as=(ATracer *)handle;
    int rv;
    as->armiss_err = PERIP_OK;
    rv = as->child.x.armiss_cache.d_write(as->child.handle,address,data,acc,
                                         abs_time);
    /* NB If caller has memory-aborted, we must set err to -1 */
    TraceX(as, address, data, as->armiss_err, acc, as->as_TraceType);
    return rv;
}

#ifdef OldCode
static armul_MemAccAsync TraceAccessAsync;
static int TraceAccessAsync(void *handle, ARMword address, ARMword *data,
                           ARMul_acc acc, ARMTime *abs_time)
{
    ATracer *as=(ATracer *)handle;
    int rv = as->child.x.armiss_ahb.ahb_access(as->child.handle,address,
                                                data,acc, abs_time);
    TraceX(as, address, data, rv, acc, as->as_TraceType);
    return rv;
}
#endif


/* END ARMISS */


static int TraceBusMemAccess2(void *handle,
                              ARMword address,
                              ARMword *data,
                              ARMul_acc access_type)
{
    ATracer *as=(ATracer *)handle;

    int err = 
        as->child.x.arm8.access2(as->child.handle,address,data,access_type);

    TraceX(as, address, data, err, access_type, as->as_TraceType);
    return err;
}

static int TraceBusMemAccess(void *handle,
                              ARMword address,
                              ARMword *data,
                              ARMul_acc access_type)
{
    ATracer *as=(ATracer *)handle;
    /* TracerState *ts = as->ts; */
    int err = 
        as->child.x.basic.access(as->child.handle,address,data,access_type);

    TraceX(as, address, data, err, access_type, as->as_TraceType);
    return err;
}

static armul_InfoProc tracer_coreMasterInfo;

static unsigned tracer_coreMasterInfo(void *handle, unsigned type,
                                      ARMword *pID, uint64 *data)
{
    ATracer *as=(ATracer *)handle;
#ifdef VERBOSE_MASTERINFO
    printf("tracer_coreMasterInfo:*pID:%08x\n", (unsigned)*pID);
#endif
    switch (*pID)
    {
    case BMPropertyID_DataAbort:
        as->armiss_err = PERIP_DABORT;
        break;
    default:
        break;
    }
    assert(as->mem_ref.handle != as);
    /* Pass up towards core. */
    return as->mem_ref.master_info(as->mem_ref.handle, type, pID, data);
}

static unsigned TraceMemInfo(void *handle, unsigned type, ARMword *pID,
                             uint64 *data)
{
    ATracer *mem=(ATracer *)handle;
/*    TracerState *ts = mem->ts;*/

    if (mem->child.mem_info)
    {
        return mem->child.mem_info(mem->child.handle,type,pID,data);
    }
    else
    {
        return RDIError_UnimplementedMessage;
    }
}

/* Aims to return a value in microseconds */
static ARMTime TraceReadClock(void *handle)
{
    ATracer *mem=(ATracer *)handle;
    if (mem->child.read_clock)
    {
        return mem->child.read_clock(mem->child.handle);
    }
    else
    {
        return 0L;
    }    
}

static const ARMul_Cycles *TraceReadCycles(void *handle)
{
    ATracer *mem = (ATracer *)handle;
    if (mem->child.read_cycles)
    {
        return mem->child.read_cycles(mem->child.handle);
    }
    else
    {
        return NULL;
    }
}


static uint32 TraceGetCycleLength(void *handle)
{
    ATracer *mem = (ATracer *)handle;
    if (mem->child.get_cycle_length)
        return mem->child.get_cycle_length(mem->child.handle);
    return 0;
}


static armul_MemBurstCount Trace_burst_count;
void Trace_burst_count(void *handle, unsigned Count, unsigned IsWrite)
{
    ATracer *mem = (ATracer *)handle;
    ARMul_MemInterface *mi = &mem->child;
    if (mi->x.arm9.burst_count)
        mi->x.arm9.burst_count(mi->handle, Count, IsWrite);
}




/* - end of pass-thru functions - */


static void trace_on(TracerState *ts)
{
  unsigned32 prop=ts->prop;
  ts->not_tracing=FALSE;
#ifdef VERBOSE_BUS
  printf("Tracer::trace_on prop=%08lx\n",(unsigned long)prop);
#endif
  
  if (!ts->trace_opened) {
    /* open the tracing file */
    if (Tracer_Open(ts)) return;
    ts->trace_opened=TRUE;
  } 

  ts->Core_Tracer.ts = ts;

  /* install instruction and event tracing functions */
  if (prop & TRACE_INSTR)
  {
    ts->hourglass_kh=ARMulif_InstallHourglass(&ts->coredesc,
                                              trace_func,ts);
  }
  if (prop & TRACE_EVENT)
  {
    ts->trace_event_kh=ARMulif_InstallEventHandler(&ts->coredesc,
                               (1 << (ts->event_set >> 16))/* TraceEventSel*/,
                                                   Tracer_EventHandler, ts);
  }
#ifdef USE_TRACEMEMEVENT
  if (prop & TRACE_MEM) {
    /* Ask the core to call us every memory-cycle.
     * (Currently only core-events, alas)
     */
    { uint32 ID[2];
    ID[0] = ARMulCallbackID_TraceMemoryEventHandler;
    ID[1] = 0;
    ts->memcallback = ARMulif_RegisterNewTraceCallback(
        &ts->coredesc, &ID[0], Tracer_MemAccess_Mem, ts);
    }
  }
#else
  if (prop & TRACE_MEM) {
      ATracer *as = &ts->Core_Tracer;
      ARMul_MemInterface *mif;
      uint32 ID[2];
      ID[0] = ARMulBusID_Core;
      ID[1] = 0;
      mif = ARMulif_QueryMemInterface(&ts->coredesc, &ID[0]);
      as->ts = ts;
      as->as_TraceType = Trace_MemAccess;
      if (mif)
      {
          ARMul_MemInterface *mi = &as->bus_mem;
          as->advance.acc = acc_NotAccount; /* Skip first access */
          as->current.acc = acc_NotAccount; /* Skip first access */
          as->memfeatures = 0;
          mi->handle = as;
          mi->x.basic.access = TraceBusMemAccess;

          /* <Copy info thru. */
          mi->mem_info=TraceMemInfo;
          mi->read_clock=TraceReadClock;
          mi->read_cycles=TraceReadCycles;
          mi->get_cycle_length = TraceGetCycleLength;

          switch(mif->memtype) 
          {
          case ARMul_MemType_PipelinedAMBA:
            as->memfeatures |= TRACE_MEMFEATURE_AMBA;
            /* Fall through */

          case ARMul_MemType_PipelinedBasic:
            as->memfeatures |= TRACE_MEMFEATURE_PIPELINED;
            mi->x.pipe.next = TraceNextCycle;
            if (mif->x.pipe.delta_cycles) {
              mi->x.pipe.delta_cycles = TraceDeltaCycles;
            } else {
              mi->x.pipe.delta_cycles = NULL;
            }
            break;

          case ARMul_MemType_PipelinedARM9:
            as->memfeatures |= TRACE_MEMFEATURE_PIPELINED;
            mi->x.pipe_arm9.dside_next = TraceNextDCycle;
            mi->x.pipe_arm9.iside_next = TraceNextICycle;
            mi->x.pipe_arm9.harvard_access = TraceBusMemAccessHarvard;
            mi->x.pipe_arm9.core_exception = TraceCoreException;
            break;

          case ARMul_MemType_Basic:
          case ARMul_MemType_16Bit:
          case ARMul_MemType_Thumb:
/*          case ARMul_MemType_BasicCached:*/
          case ARMul_MemType_16BitCached:
          case ARMul_MemType_ThumbCached:
          case ARMul_MemType_ARMissAHB:
              break;
          case ARMul_MemType_AHB:
              break;
#ifdef OldCode
          case ARMul_MemType_ARMissAHB:
              mi->x.armiss_ahb.ahb_access = TraceAccessAsync;
              break;
#endif
          case ARMul_MemType_StrongARM:
              mi->x.strongarm.core_exception = TraceCoreException;
              mi->x.strongarm.data_cache_busy = TraceDataCacheBusy;
              break;
          case ARMul_MemType_ARM8:
              mi->x.arm8.core_exception = TraceCoreException;
              mi->x.arm8.access2 = TraceBusMemAccess2;
              break;

          case ARMul_MemType_ARMissCache:
              /*mi->x.armiss_cache.debug_access = */
              mi->x.armiss_cache.i_access = TracerARMiss_IAccess;
              mi->x.armiss_cache.d_read = TracerARMiss_DRead;
              mi->x.armiss_cache.d_write = TracerARMiss_DWrite;

              assert(mif->mem_link->handle != NULL);
              assert(mif->mem_link->master_info != NULL);
              as->mem_ref.handle = as;
              as->mem_ref.master_info = tracer_coreMasterInfo;

              break;
          case ARMul_MemType_ARM9: /* ARM9 Harvard   */
            as->memfeatures |= TRACE_MEMFEATURE_PIPELINED;
            mi->x.arm9.burst_count = Trace_burst_count;
            mi->x.arm9.harvard_access = TraceBusMemAccessHarvard;
            mi->x.arm9.core_exception = TraceCoreException;
            break;

          case ARMul_MemType_ByteLanes:       
          default:
              Hostif_PrettyPrint(ts->hostif,ts->config,
                 "\nFailed to insert Core memory interface -"
                                 " unknown memtype %u.\n", 
                                 (unsigned)mif->memtype);
              ts->prop ^= TRACE_MEM;
              break;
          }
      }
      else
          ts->prop ^= TRACE_MEM;
      if (ts->prop & TRACE_MEM)
      {
          ARMul_InsertMemInterface2(mif,
                                   &as->child, /* DEST & SOURCE */
                                   &as->mem_ref,
                                   &as->bus_mem); /* SOURCE */
      }
  }
#endif
  if (prop & TRACE_BUS) {
      ATracer *as = &ts->Bus_Tracer;
      ARMul_MemInterface *mif;
      uint32 ID[2];
      ID[0] = ARMulBusID_Bus;
      ID[1] = 0;
      mif = ARMulif_QueryMemInterface(&ts->coredesc, &ID[0]);
      if (mif)
      {
          ARMul_MemInterface *mi = &as->bus_mem;
          as->ts = ts;
          as->as_TraceType = Trace_BusAccess;
          as->advance.acc = acc_NotAccount; /* Skip first access */
          as->current.acc = acc_NotAccount; /* Skip first access */
          as->memfeatures = 0;
          mi->handle = as;
          mi->x.basic.access = TraceBusMemAccess;

          /* <Copy info thru. */
          mi->mem_info=TraceMemInfo;
          mi->read_clock=TraceReadClock;
          mi->read_cycles=TraceReadCycles;
          mi->get_cycle_length = TraceGetCycleLength;

          switch(mif->memtype) 
          {
          case ARMul_MemType_PipelinedAMBA:
            as->memfeatures |= TRACE_MEMFEATURE_AMBA;
            /* Fall through */

          case ARMul_MemType_PipelinedBasic:
            as->memfeatures |= TRACE_MEMFEATURE_PIPELINED;
            mi->x.pipe.next = TraceNextCycle;
            if (mif->x.pipe.delta_cycles) {
              mi->x.pipe.delta_cycles = TraceDeltaCycles;
            } else {
              mi->x.pipe.delta_cycles = NULL;
            }
            break;

          case ARMul_MemType_PipelinedARM9:
            as->memfeatures |= TRACE_MEMFEATURE_PIPELINED;
            mi->x.pipe_arm9.dside_next = TraceNextDCycle;
            mi->x.pipe_arm9.iside_next = TraceNextICycle;
            mi->x.pipe_arm9.harvard_access = TraceBusMemAccessHarvard;
            mi->x.pipe_arm9.core_exception = TraceCoreException;
            break;

          case ARMul_MemType_PipelinedMultiLayerAHB:
            as->memfeatures |= (TRACE_MEMFEATURE_PIPELINED | TRACE_MEMFEATURE_AMBA);
            as->bus_mem.x.pipe_multi_layer.register_layer = TraceMultiLayerRegister;
            as->bus_mem.x.pipe_multi_layer.current_access = TraceBusMultiLayerCurrent;
            as->bus_mem.x.pipe_multi_layer.next_access    = TraceBusMultiLayerNext;
            as->bus_mem.x.pipe_multi_layer.debug_access   = TraceBusMemAccess;
            if (mif->x.pipe.delta_cycles) {
               as->bus_mem.x.pipe_multi_layer.delta_cycles = TraceMultiLayerDeltaCycles;
            } else {
               as->bus_mem.x.pipe_multi_layer.delta_cycles = NULL;
            }
            break;
            
          case ARMul_MemType_Basic:
          case ARMul_MemType_16Bit:
          case ARMul_MemType_Thumb:
/*          case ARMul_MemType_BasicCached:*/
          case ARMul_MemType_16BitCached:
          case ARMul_MemType_ThumbCached:
              break;
          case ARMul_MemType_AHB:
              break;
#ifdef OldCode
          case ARMul_MemType_ARMissAHB:
              mi->x.armiss_ahb.ahb_access = TraceAccessAsync;
              break;
#endif
          case ARMul_MemType_StrongARM:
              mi->x.strongarm.core_exception = TraceCoreException;
              mi->x.strongarm.data_cache_busy = TraceDataCacheBusy;
              break;
          case ARMul_MemType_ARM8:
              mi->x.arm8.core_exception = TraceCoreException;
              mi->x.arm8.access2 = TraceBusMemAccess2;
              break;

          case ARMul_MemType_ARMissCache:
          case ARMul_MemType_ARM9: /* ARM9 Harvard   */
          case ARMul_MemType_ByteLanes:       
          default:
              ts->prop ^= TRACE_BUS;
              break;
          }
      }
      else
          ts->prop ^= TRACE_BUS;
      if (ts->prop & TRACE_BUS)
      {
          ARMul_InsertMemInterface(mif,
                                   &as->child,
                                   &as->mem_ref,
                                   &as->bus_mem);
      }
  }  
#ifdef VERBOSE_BUS
  else
      printf("Tracer::TRACE_BUS Off\n");
#endif

}

static void trace_off(TracerState *ts)
{
/*  unsigned32 prop=ts->prop;*/
/*  ARMul_State *state=ts->state; */
  
  ts->not_tracing=TRUE;
  /* remove instruction and event tracing functions */
  if (ts->hourglass_kh) 
  {
      ARMulif_RemoveHourglass(&ts->coredesc,ts->hourglass_kh);
      ts->hourglass_kh = NULL;
  }
  if (ts->trace_event_kh) 
  {
      ARMulif_RemoveEventHandler(&ts->coredesc,ts->trace_event_kh);
      ts->trace_event_kh = NULL;
  }

  if (ts->memcallback) {
      ARMulif_RemoveTraceMemoryHandler(&ts->coredesc, ts->memcallback);
      ts->memcallback = NULL;
  }  
  {
      ATracer *as = &ts->Core_Tracer;
      if (as->mem_ref.mif)
      {
          ARMul_RemoveMemInterface2(&as->mem_ref,&as->child);
      }
  }
  {
      ATracer *as = &ts->Bus_Tracer;
      if (as->mem_ref.mif)
      {
          ARMul_RemoveMemInterface(&as->mem_ref,&as->child);
      }
  }
  Tracer_Flush(ts);
}

static int Tracer_InitCPRegTrace(TracerState *ts,char const *option);


static int RDI_info(void *handle,unsigned type,ARMword *arg1,ARMword *arg2)
{
  TracerState *ts=(TracerState *)handle;
  UNUSEDARG(arg2);
  switch (type) {
  case RDIInfo_RouteLinks:
  {
      unsigned32 a1 = (arg1 ? *arg1 : 0);
      if (!a1)
      {   /* make, rather than check. */
          char const *option=ToolConf_Lookup(ts->config,
                                             (tag_t)"TraceCoproRegisters");
          if (option &&*option)
              Tracer_InitCPRegTrace(ts,option);
          break;
      }
      break;
  }
#ifdef ETRACE
  case RDIInfo_Trace:
    return RDIError_NoError;

  case RDITrace_Format:
    *(RDI_TraceFormat *)arg1 = RDITrace_NoFormat;
    return RDIError_NoError;

  case RDITrace_Control: {
    switch ((RDI_TraceControl)*arg1) {
    case RDITrace_Start:
      if (ts->not_tracing)      /* tracing enable */
        trace_on(ts);
      break;
    case RDITrace_Stop:
      if (!ts->not_tracing)     /* tracing disable */
        trace_off(ts);
      break;
    case RDITrace_Flush:
      break;
    default:
      return RDIError_UnimplementedMessage;
    }
  }
    return RDIError_NoError;

  case RDITrace_BufferExtent: {
    unsigned int this_is_not_useful_behaviour;
    *arg1 = 0;
    *arg2 = 256;
  }
    return RDIError_NoError;

  case RDITrace_GetBlockSize:
    *arg1 = 0;
    return RDIError_NoError;

  case RDITrace_SetTriggerPosition:
    return RDIError_NoError;

  case RDITrace_SetReadyProc:
    return RDIError_NoError;

  case RDITrace_Read: {
    unsigned int this_is_not_useful_behaviour;
    struct off { uint32 size; RDI_TraceIndex start; } *p = (struct off *)arg2;
    char *b = (char *)arg1;
    int n, i;

    for (i = p->start, n = 0; n < p->size; i++, n++) {
      if (i >= p->start && n < p->size) {
        *b++ = "Hello, World!"[i % (sizeof("Hello, World!")-1)];
      }
    }
    p->size = n;
  }
    return RDIError_NoError;
#else
                                /* use RDI log */
  case RDIInfo_SetLog: {
    int newValue=(int)*arg1;
# ifdef VERBOSE_RDI_LOG
    printf("tracer.c:Info#SetLog log:=0x%04x\n",(unsigned)newValue);
# endif


    if (newValue & RDILog_Trace) {
      if (ts->not_tracing)      /* tracing enable */
        trace_on(ts);
    } else {
      if (!ts->not_tracing)     /* tracing disable */
        trace_off(ts);
    }
  }
    break;                      /* pass on */
#endif    
  }

  return RDIError_UnimplementedMessage;
}

static ARMword Properties(struct RDI_HostosInterface const *hostif, 
                          toolconf config)
{
  unsigned int i;
  ARMword prop = 0;
  unsigned verbose=ToolConf_DLookupBool(config, ARMulCnf_Verbose, FALSE);


  for (i=0;TraceOption[i].option!=NULL;i++) {
      if (ToolConf_DLookupBool(config,TraceOption[i].option,FALSE)) {
          if (verbose)
              Hostif_PrettyPrint(hostif,config,"%s%s",
                                 prop ? ", " : "\nTracing: ",
                                 TraceOption[i].print);      
          prop |= TraceOption[i].flag;
      }
  }
  if (prop && verbose)
      Hostif_PrettyPrint(hostif,config,"\n");
  if (verbose) 
  {
      prop |= TRACE_VERBOSE;
  }

  return prop;
}

static ARMul_Error CommonInit(TracerState *ts, toolconf config)
{
  const char *option;
  unsigned verbose = ts->prop & TRACE_VERBOSE;

  ts->not_tracing=TRUE;

  ts->output.printf = NULL;
  ts->output.putc = NULL;
  ts->output.puts = NULL;
  ts->output.write = NULL;
  ts->output.close = NULL;
  ts->output.flush = NULL;
  ts->output.handle = NULL;

  ts->trace_opened=FALSE;

  if (ts->prop & TRACE_EVENT) {
    const char *option=ToolConf_Lookup(ts->config, ARMulCnf_EventMask);
    if (option) {
      char *p;
      ts->event_mask = strtoul(option, &p, 0);
      ts->event_set = p ? strtoul(p+1, NULL, 0) : ts->event_mask;
      if (verbose)
        Hostif_ConsolePrint(ts->hostif," Mask 0x%08x-0x%08x",
                           ts->event_mask,ts->event_set);
    } else {
      option=ToolConf_Lookup(config,ARMulCnf_Event);
      if (option) {
        ts->event_mask=(unsigned int)~0;
        ts->event_set=strtoul(option,NULL,0);
        if (verbose)
          Hostif_ConsolePrint(ts->hostif," 0x%08x",ts->event_set);
      } else
        ts->event_mask = ts->event_set = 0;
    }
  }

  { /* Initialize saved registers, so they are not all reported as having
       changed after the first instruction (if TRACE_REGISTER is on) */
    int regnum;
    for (regnum=0; regnum<16; regnum ++) {
      ts->mainregister[regnum] = 0;
    }  
    ts->cpsrregister = 0;
  }


  option=ToolConf_Lookup(ts->config,ARMulCnf_Range);
  if (option) {
    if (sscanf(option,"%li,%li",(long*)&ts->range_lo,(long*)&ts->range_hi)==2) {
      ts->prop|=TRACE_RANGE;
      if (verbose) Hostif_PrettyPrint(ts->hostif,config," Range %08x->%08x",
                                      ts->range_lo,ts->range_hi);
    } else
      Hostif_ConsolePrint(ts->hostif,"TRACER: Did not understand range '%s'\n",
                         option);
  }

  option=ToolConf_Lookup(ts->config,ARMulCnf_Sample);
  if (option) {
    ts->sample_base=strtoul(option,NULL,0);
    ts->sample=0;
    if (verbose) Hostif_PrettyPrint(ts->hostif,config," Sample rate %d",ts->sample_base);
  } else {
    ts->sample_base=0;
  }

  if (ts->prop!=0 && verbose) 
      Hostif_PrettyPrint(ts->hostif,ts->config,"\n");

  ARMulif_InstallUnkRDIInfoHandler(&ts->coredesc,
                                   RDI_info,ts);


  if (ts->prop & TRACE_STARTON)
  {
      if (verbose) 
          Hostif_PrettyPrint(ts->hostif,config," (Start on) ");
      trace_on(ts);
  }

  return RDIError_NoError;
}



/* --- COnfigEvents --- */

static unsigned Tracer_ConfigEvents(void *handle, void *data)
{
    TracerState *ts = handle;
    ARMul_Event *evt = data;
    unsigned boottype = evt->data1;

    if (evt->event != ConfigEvent_Reset)
        return FALSE;
  /* Allow us to power-up ON.
   */
  if (!(boottype & 1)) /* Cold only */
  {
      ARMword newValue = ts->initial_rdi_log;
      if (newValue & RDILog_Trace) {
          if (ts->not_tracing)      /* tracing enable */
              trace_on(ts);
      } else {
          if (!ts->not_tracing)     /* tracing disable */
              trace_off(ts);
      }
  }
  return FALSE; /* Carry on */
}


static unsigned Tracer_GetNumCPWords(TracerState *ts,
                                     unsigned cpnum,
                                     unsigned crnum)
{
    CoprocessorAccess c;
    ARMword data[1];
    int err = ts->coredesc.rdi->info(ts->coredesc.handle,
                                     RDIInfo_QueryMethod,
                                     (ARMword *)"CoprocessorAccess",
                                     (ARMword *)&c);
    if (!err)
    {
        assert(c.func);
        assert(c.handle);
        data[0] = c.func(c.handle, cpnum,ARMul_CP_QUERY_REGSIZE,crnum,NULL);
    }     
    else
    {
        Hostif_PrettyPrint(ts->hostif,ts->config,
                           "\n*** Tracer could not get CoprocessorAccess **\n");
        data[0]=0;
    }
    return data[0];
}

static void Tracer_AddTracedCPReg(TracerState *ts,unsigned cpnum,
                                  unsigned crnum)
{
    struct CPregTrace cprt;
    
    cprt.reg = crnum;
    cprt.mask = 1 << cprt.reg;
    cprt.cpnum = cpnum;
    cprt.numwords = Tracer_GetNumCPWords(ts, cpnum, crnum); /* ? */
    if (cprt.numwords != 0)
    {
        cprt.old = calloc(cprt.numwords,sizeof(ARMWord));
        cprt.new = calloc(cprt.numwords,sizeof(ARMWord));
        if (ts->prop & TRACE_VERBOSE)
        {
            Hostif_PrettyPrint(ts->hostif,ts->config,
                               " P%u C%u words:%u \n",
                               cpnum, crnum, cprt.numwords);
        }
        CVector_Append(&ts->cpregs,&cprt);
    }
}

/* This must not be called until "make-links" time, 
 * in case the VFPv2 has not yet been instantiated. */
static int Tracer_InitCPRegTrace(TracerState *ts,char const *option)
{
#ifdef VERBOSE_INIT_CPREG_TRACE
    printf(" Tracer_InitCPRegTrace<%s>\n", option);
#endif
    /* Check for 'P' */
    while (*option==' ')
        ++option;
    while (*option == 'P' || *option == 'p')
    {
        unsigned cpnum = 0;
        ++option;
        while (isdigit((int)*option))
        {
            cpnum *=10; cpnum += (*option - '0');
            ++option;
        }
        while (*option==' ')
            ++option;
        while (*option == 'c' || *option == 'C')
        {
            unsigned crnum = 0;
            ++option;
            while (isdigit((int)*option))
            {
                crnum *=10; crnum += (*option - '0');
                ++option;
            }
            Tracer_AddTracedCPReg(ts,cpnum,crnum);
        }
        if (*option == ';')
            ++ option;
    }
    return RDIError_NoError;
}



BEGIN_INIT(Tracer)
{
      TracerState *ts = state;
      ARMword prop;
      Hostif_PrettyPrint(ts->hostif, config, ", Tracer");
      prop = Properties(hostif, config);
      ts->prop = prop;
      ts->prev_instr = 0;  /* Thumb 2-instr BL disassembly */
      ts->LastTime = ~(ARMTime)0;
      {
          int err = CommonInit(ts,config);
          if (err) return err;
      }
      ts->break_line = ToolConf_DLookupUInt(config,(tag_t)"BREAK_LINE", 0);
      ts->initial_rdi_log = ToolConf_DLookupUInt(config,(tag_t)"RDI_LOG", 0);

      ARMulif_InstallEventHandler(&state->coredesc,
                                  ConfigEventSel,
                                  Tracer_ConfigEvents, ts);
      CVector_Init(&ts->cpregs, sizeof(struct CPregTrace));
}
END_INIT(Tracer)


BEGIN_EXIT(Tracer)
{
  TracerState *ts=state;

  unsigned i;
  for (i=0; i < ts->cpregs.count; i++)
  {
      struct CPregTrace *rt = CVector_At(&ts->cpregs,i);
      free(rt->old);
      free(rt->new);
  }
  CVector_Clear(&ts->cpregs);

  if (ts->trace_opened) {
    Tracer_Close(ts);
    ts->trace_opened=FALSE;
  }
}
END_EXIT(Tracer)


/*--- <SORDI STUFF> ---*/


#define SORDI_DLL_DESCRIPTION_STRING "Tracer (a Utility)"
#define SORDI_RDI_PROCVEC Tracer_AgentRDI
#include "perip_sordi.h"

#include "perip_rdi_agent.h"
    IMPLEMENT_AGENT_PROCS_NOEXE_NOMODULE(Tracer)
    IMPLEMENT_AGENT_PROCVEC_NOEXE(Tracer)

/*--- </> ---*/

/* EOF tracer.c */











