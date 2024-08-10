/* -*-C-*-
 *
 * $Revision: 1.27.2.25 $
 *   $Author: dsinclai $
 *     $Date: 2001/10/21 18:12:10 $
 *
 * OS.c - ARMulator III Operating System Interface.
 *
 * Copyright (c) 1999 ARM P
 * All Rights Reserved.
 */

#define MODEL_NAME OS

/*
 * This file contains a model of ARM's Debug Monitors, including
 * all the SWI's required to support the C library.
 */

/* needed to get EBADF=9 and EMFILE=24 from errno.h on HP/UX */
#define _POSIX_SOURCE 1
/* needed to get tempname() from stdio.h */
#define _XOPEN_SOURCE
/* Get the functional form of errno from erro.h on Solaris,
 * to cope with AXD's threading/linkage. */
#define _REENTRANT

#include <time.h>

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "toolconf.h"
#include "minperip.h"
#include "armul_hostif.h"
#include "armul_askrdi.h"
#include "armul_cnf.h"    /* for ARMulCnf_Clock */
#include "armsignal.h"
#include "rdi_conf.h" /* for Dbg_Cnf_CPUSpeed */
#include "rdi_prop.h"  /* for RDIProperty_SetAsNumeric */
#include "semihost.h"
#include "armul_propid.h" /* ARMul_RDIPropertyDesc */


/* --- Debugging verbosity --- */

#ifndef NDEBUG
# if 1
# else
#  define VERBOSE_RESET
#  define VERBOSE_SYS_FILE
#  define VERBOSE_SET_ENDIAN
#  define VERBOSE_GetCS
#  define VERBOSE_OS_INFO
#  define VERBOSE_EXCEPTION
#  define VERBOSE_CLI
#  define VERBOSE_EVENTS
#  define VERBOSE_CMDLINE
#  define VERBOSE_DEMON_EXIT
#  define VERBOSE_ANGEL_EVENTS
#  define VERBOSE_INIT
# endif
#endif


/* Design decisions */

#define CMDLINE_LENGTH_EXCLUDES_NULL 



static RDI_InfoProc OS_HandleUnkRDI;
static GenericCallbackFunc OS_HandleException;

#ifndef __STDC__
#define remove(s) unlink(s)
#endif


#ifndef NO_DEMON
#define CURRENTMODE RDIMode_Curr
/*
 * Mode Constants - obsolete names
 */

#define USER32MODE    ARM_M_USER32
#define FIQ32MODE     ARM_M_FIQ32
#define IRQ32MODE     ARM_M_IRQ32
#define SVC32MODE     ARM_M_SVC32
#define ABORT32MODE   ARM_M_ABORT32
#define UNDEF32MODE   ARM_M_UNDEF32
#define SYSTEM32MODE  ARM_M_SYSTEM32
#endif




/***************************************************************************\
*                   (demon) SWI numbers + 0x100                            *
\***************************************************************************/

#define SWI_WriteC                 0x100 + 0x0
#define SWI_Write0                 0x100 + 0x2
#define SWI_ReadC                  0x100 + 0x4
#define SWI_CLI                    0x100 + 0x5
#define SWI_GetEnv                 0x100 + 0x10
#define SWI_Exit                   0x100 + 0x11
#define SWI_EnterOS                0x100 + 0x16
#define SWI_WriteHex               0x100 + 0x17

#define SWI_GetErrno               0x100 + 0x60
#define SWI_Clock                  0x100 + 0x61
#define SWI_Time                   0x100 + 0x63
#define SWI_Remove                 0x100 + 0x64
#define SWI_Rename                 0x100 + 0x65
#define SWI_Open                   0x100 + 0x66

#define SWI_Close                  0x100 + 0x68
#define SWI_Write                  0x100 + 0x69
#define SWI_Read                   0x100 + 0x6a
#define SWI_Seek                   0x100 + 0x6b
#define SWI_Flen                   0x100 + 0x6c

#define SWI_IsTTY                  0x100 + 0x6e
#define SWI_TmpNam                 0x100 + 0x6f
#define SWI_InstallHandler         0x100 + 0x70
#define SWI_GenerateError          0x100 + 0x71


/***************************************************************************\
*                          OS private Information                           *
\***************************************************************************/
/* Configuration-names */
#define ARMulCnf_AddrSuperStack "ADDRSUPERSTACK"
#define ARMulCnf_AddrAbortStack "ADDRABORTSTACK"
#define ARMulCnf_AddrUndefStack "ADDRUNDEFSTACK"
#define ARMulCnf_AddrIRQStack "ADDRIRQSTACK"
#define ARMulCnf_AddrFIQStack "ADDRFIQSTACK"
#define ARMulCnf_AddrUserStack "ADDRUSERSTACK"
#define ARMulCnf_AddrSoftVectors "ADDRSOFTVECTORS"
#define ARMulCnf_AddrCmdLine "ADDRCMDLINE"
#define ARMulCnf_AddrsOfHandlers "ADDRSOFHANDLERS"
#define ARMulCnf_SoftVectorCode "SOFTVECTORCODE"

#define ARMulCnf_AngelSWIARM "ANGELSWIARM"
#define ARMulCnf_AngelSWIThumb "ANGELSWITHUMB"
#define ARMulCnf_HeapBase "HEAPBASE"
#define ARMulCnf_HeapLimit "HEAPLIMIT"
#define ARMulCnf_StackBase "STACKBASE"
#define ARMulCnf_StackLimit "STACKLIMIT"

#define ARMulCnf_Angel "ANGEL"
#define ARMulCnf_Demon "DEMON"

/***************************************************************************\
*                   Define the initial layout of memory                     *
\***************************************************************************/

#define DEMON_ADDRSUPERSTACK       0xA00L   /* supervisor stack space */
#define DEMON_ADDRABORTSTACK       0x800L   /* abort stack space */
#define DEMON_ADDRUNDEFSTACK       0x700L   /* undef stack space */
#define DEMON_ADDRIRQSTACK         0x500L   /* IRQ stack space */
#define DEMON_ADDRFIQSTACK         0x400L   /* FIQ stack space */
#define DEMON_ADDRUSERSTACK        0x80000L /* default user stack start */
#define DEMON_ADDRCMDLINE          0xf00L   /* command line is here after a SWI GetEnv */




/* Sizes of shamefully fixed-size arrays. */
#define UNIQUETEMPS 256
#define BUFFERSIZE 4096
#define NONHANDLE -1

#ifndef FOPEN_MAX
#define FOPEN_MAX 64
#endif


BEGIN_STATE_DECL(OS)
   ARMword ErrorNo;
   char *CommandLine ;
   FILE *FileTable[FOPEN_MAX];
   char FileFlags[FOPEN_MAX];
   unsigned FileCount[FOPEN_MAX];
   char OpenMode[FOPEN_MAX];
   char *tempnames[UNIQUETEMPS];

   bool Debugger_Bigend;
   bool Set_Endian;
   bool_int os_verbose;
    
   uint32 semihostingvector, 
          semihostingstate; /* from debugger */

   unsigned AngelEnabled ;
   unsigned DemonEnabled ;
   bool ExitSwisEnabled;
   unsigned os_HaltOnUnknownAngelSwi; /* bool */

   uint32 os_CPUSpeed, os_BusSpeed;

   ARMword usersp;             /* user stack pointer */
   ARMword cmdlineaddr;        /* where the command line is placed */

   ARMword AngelSWIARM;
   ARMword AngelSWIThumb;
   ARMword heap_base,heap_limit;
   ARMword stack_base,stack_limit;
  struct {
    struct {
      ARMword svc,abt,undef;
      ARMword irq,fiq,user;
    } sp;                       /* Stack pointers */
    ARMword soft_vectors;       /* where the soft vectors start */
    ARMword cmd_line;           /* where the command line is placed */
    ARMword handlers;           /* addr/workspace for handlers */
    ARMword soft_vec_code;      /* default handlers */
  } map;

  uint32 vector_catch;
  FILE *os_file_log;

  bool bStop_Received;
 /*
  *   Saved state for resuming sys_read
  */
  bool bResume_Read;
  char saved_buffer[BUFFERSIZE];
unsigned saved_size, saved_type;
ARMword saved_addr;
  bool_int os_bUseRealTime;
END_STATE_DECL(OS)

/* for semihostingstate */
#define SEMIHOSTSTATE_ENABLED 1

/* Returns the time in bus-clocks. */
static ARMTime Semihost_GetTime(OSState *osstate)
{
    return ARMulif_Time(&osstate->coredesc);
}


static void LOG_FILE_OP(OSState *state, char *fmt, ...)
{
    if (state->os_file_log)
    {
        va_list ap;
        char buf[1000];
        va_start(ap, fmt);
        vsprintf(buf, fmt, ap);
        fputs(buf,state->os_file_log);
        va_end(ap);
    }
}


/* Return centiseconds of real or emulated time.
 * Real time is from the host's clock(),
 * Emulated time is 
 *  coreclocks * centiSecondsPerSecond [cs*Hz] / coreClockRate[Hz] 
 *
 */
static ARMTime OS_GetCentiseconds(OSState *osstate)
{
    /* First see if the core supports RDIRead_Clock */
    ARMword secs,ns;
    if (osstate->os_bUseRealTime)
    {
        return
#ifdef CLOCKS_PER_SEC
            ( (CLOCKS_PER_SEC >= 100)
              ? (ARMword) (clock() / (CLOCKS_PER_SEC / 100))
              : (ARMword) ((clock() * 100) / CLOCKS_PER_SEC)
                )
#else
        /* Assume unix... clock() returns microseconds */
        (ARMword) (clock() / 10000)
#endif
            ;
    }
    else if (osstate->coredesc.rdi->info(osstate->coredesc.handle,
                                 RDIRead_Clock,&ns,&secs) == RDIError_NoError)
    {
        ARMTime t = (ARMTime)secs * 100 + (ns / 10000) / 1000;
#ifdef VERBOSE_GetCS
        printf("GetCS secs:%lu ns:%lu -> %lu\n",
               (unsigned long)secs,(unsigned long)ns, (unsigned long)t);
#endif
        return t;
    }
    else
    {
        assert(osstate->os_BusSpeed != 0);
        return (ARMword)(ARMulif_Time(&osstate->coredesc) * 100 /
                         osstate->os_BusSpeed);

    }
}


/* Leaf node - no need to call next->func. */
static unsigned Time_Access(GenericAccessCallback *gacb,
                            ARMword address,
                            ARMword *data,
                            unsigned access_type)
{
    OSState *osstate = (OSState *)gacb->handle;
    (void)address;
    switch (access_type & (ACCESS_WRITE | 0xF))
    {
    case (ACCESS_READ | ACCESS_WORD):
        *data = (uint32)Semihost_GetTime(osstate);
        return RDIError_NoError;
    }
    return RDIError_NoSuchHandle;
}

static unsigned Clock_Access(GenericAccessCallback *gacb,
                            ARMword address,
                            ARMword *data,
                            unsigned access_type)
{
    OSState *osstate = (OSState *)gacb->handle;
    (void)address;
    switch (access_type & (ACCESS_WRITE | 0xF))
    {
    case (ACCESS_READ | ACCESS_WORD):
        *data = (uint32)OS_GetCentiseconds(osstate);
        return RDIError_NoError;
    }
    return RDIError_NoSuchHandle;
}


static ARMul_RDIPropertyDesc Time_PropDesc = 
{
    /* */
    {
        "cputime", /* name */
        "Time in units of core CPUSPEED", /* description */
        0,                  /* ID */
        FALSE,              /* AsString */
        0,                  /* maxStrLen */
        TRUE,               /* AsNumeric */
        FALSE,              /* IsSigned */
        32,                 /* Width */
        TRUE,               /* readOnly */
        FALSE,              /* monotonicIncreasing */
        FALSE,              /* traceable */
        RDIProperty_PDT_Standard
    },

    {
        NULL,
        Time_Access,
        NULL
    }

};


static ARMul_RDIPropertyDesc Clock_PropDesc = 
{
    /* */
    {
        "sys_clock", /* name */
        "Time in centiseconds as read by semihosting SYS_CLOCK", /* description */
        0,                  /* ID */
        FALSE,              /* AsString */
        0,                  /* maxStrLen */
        TRUE,               /* AsNumeric */
        FALSE,              /* IsSigned */
        32,                 /* Width */
        TRUE,               /* readOnly */
        FALSE,              /* monotonicIncreasing */
        FALSE,              /* traceable */
        RDIProperty_PDT_Standard
    },

    {
        NULL,
        Clock_Access,
        NULL
    }

};



/* fileflags */
#define NOOP 0
#define BINARY 1
#define READOP 2
#define WRITEOP 4

RDI150_OpenAgentProc OS_Init;
RDI150_CloseAgentProc OS_Exit;
static unsigned Angel_ConfigEvents(void *handle, void *data);

BEGIN_INIT(OS)
{   ARMword i ;
    OSState *osstate = (OSState *)state;
   if (osstate == NULL) {
      Hostif_RaiseError(hostif,"OS_Init can't allocate state\n");
      return RDIError_OutOfStore;
      }

#ifdef VERBOSE_INIT
   printf("\n* OS_Init, osstate = %p, coredesc.rdi=%p type:%u\n", osstate, 
          osstate->coredesc.rdi, type);
#endif
   
   osstate->ErrorNo = 0;
   osstate->CommandLine = NULL ;

   osstate->os_verbose = ToolConf_DLookupBool(
       config, (tag_t)"VERBOSE", TRUE);

   { char const *option = ToolConf_Lookup(config,
                                          (tag_t)"SEMIHOST_FILEOP_LOG");
   if (option&& option[0])
       state->os_file_log = fopen(option,"w");
   }

       osstate->semihostingstate = SEMIHOSTSTATE_ENABLED;

       osstate->Debugger_Bigend = 
           ((type & RDIOpen_ByteSexMask) == RDIOpen_BigEndian);
       osstate->Set_Endian = 
           ((type & RDIOpen_ByteSexMask) != RDIOpen_DontCareEndian);

       if (!osstate->Set_Endian)
       {
           char const *str_bytesex = ToolConf_Lookup(config, Dbg_Cnf_ByteSex);
           osstate->Debugger_Bigend = (str_bytesex && str_bytesex[0] == 'B') ?
               RDISex_Big : RDISex_Little;
           osstate->Set_Endian = (str_bytesex && (str_bytesex[0] == 'B' ||
                                                  str_bytesex[0] == 'L'));
       }

       /* Only set the endianness if we don't have HWENDIAN=True */

       osstate->Set_Endian &= !ToolConf_DLookupBool(
           config,Dbg_Cnf_TargetHWEndian,FALSE);
       osstate->usersp = 0x80000000 ;
       osstate->map.soft_vectors = ToolConf_DLookupUInt(
           osstate->config, (tag_t)"AddrSoftVectors", 0xA40L) ;
       osstate->map.soft_vec_code = ToolConf_DLookupUInt(
           osstate->config, (tag_t)"SoftVectorCode", 0xB80L) ;
       osstate->map.handlers = ToolConf_DLookupUInt(
           osstate->config, (tag_t)"AddrsOfHandlers", 0xad0L) ;
       osstate->map.cmd_line = ToolConf_DLookupUInt(
           osstate->config, (tag_t)"AddrCmdLine", 0xf00L) ;

       osstate->AngelEnabled = ToolConf_DLookupBool(osstate->config, 
                                                (tag_t)ARMulCnf_Angel, FALSE);
       osstate->DemonEnabled = ToolConf_DLookupBool(osstate->config, 
                                               (tag_t)ARMulCnf_Demon, FALSE);
       osstate->ExitSwisEnabled = osstate->DemonEnabled ||
                               ToolConf_DLookupBool(osstate->config, 
                                            (tag_t)"EXIT_SWIS", FALSE);
       osstate->os_HaltOnUnknownAngelSwi = ToolConf_DLookupBool(config, 
                                (tag_t)"HALTONUNKNOWNSYSNUMBER", FALSE);

       osstate->os_CPUSpeed = ARMul_GetFCLK(osstate->config);
       osstate->os_BusSpeed = ARMul_GetMCLK(osstate->config);

       if (osstate->AngelEnabled || osstate->DemonEnabled || 
           osstate->ExitSwisEnabled)
       {
           ARMulif_InstallUnkRDIInfoHandler(&osstate->coredesc,
                                            OS_HandleUnkRDI,osstate);

           ARMulif_InstallExceptionHandler(
               &osstate->coredesc, OS_HandleException, osstate);

       }
      
       if (osstate->AngelEnabled) {
           osstate->AngelSWIARM = ToolConf_DLookupUInt(osstate->config, (tag_t)ARMulCnf_AngelSWIARM, ANGEL_SWI_ARM) ;
           osstate->AngelSWIThumb = ToolConf_DLookupUInt(osstate->config, (tag_t)ARMulCnf_AngelSWIThumb, ANGEL_SWI_THUMB) ;
           osstate->heap_base = ToolConf_DLookupUInt(osstate->config, (tag_t)ARMulCnf_HeapBase,      ANGEL_HEAPBASE) ;
           osstate->heap_limit = ToolConf_DLookupUInt(osstate->config, (tag_t)ARMulCnf_HeapLimit,     ANGEL_HEAPLIMIT) ;
           osstate->stack_base = ToolConf_DLookupUInt(osstate->config, (tag_t)ARMulCnf_StackBase,     ANGEL_STACKBASE) ;
           osstate->stack_limit = ToolConf_DLookupUInt(osstate->config, (tag_t)ARMulCnf_StackLimit,    ANGEL_STACKLIMIT) ;
           Hostif_PrettyPrint(osstate->hostif, osstate->config, ", Semihosting");
           if (osstate->os_verbose)
           {
               if (osstate->DemonEnabled)
                   Hostif_PrettyPrint(osstate->hostif, osstate->config,
                                      "+DEMON");
               if (osstate->AngelEnabled)
                   Hostif_PrettyPrint(osstate->hostif, osstate->config,
                                      "+ANGEL");
           }
       }
       
       if (osstate->DemonEnabled) {
           ARMword value ;
           
           value = ToolConf_DLookupUInt(osstate->config, (tag_t)ARMulCnf_AddrSuperStack,  DEMON_ADDRSUPERSTACK) ; /* supervisor stack space */
           ARMulif_SetReg(&osstate->coredesc, ARMulif_GetCPSR(&osstate->coredesc),
                          13, value) ; 
           ARMulif_SetReg(&osstate->coredesc, ARM_MODE_V5_SVC32,   13, value) ;
           value = ToolConf_DLookupUInt(osstate->config, (tag_t)ARMulCnf_AddrAbortStack,  DEMON_ADDRABORTSTACK) ; /* abort stack space */
           ARMulif_SetReg(&osstate->coredesc, ARM_MODE_V5_ABORT32, 13, value) ;
           value = ToolConf_DLookupUInt(osstate->config, (tag_t)ARMulCnf_AddrUndefStack,  DEMON_ADDRUNDEFSTACK); /* undef stack space */
           ARMulif_SetReg(&osstate->coredesc, ARM_MODE_V5_UNDEF32, 13, value) ; 
           value = ToolConf_DLookupUInt(osstate->config, (tag_t)ARMulCnf_AddrIRQStack,    DEMON_ADDRIRQSTACK) ; /* IRQ stack space */
           ARMulif_SetReg(&osstate->coredesc, ARM_MODE_V5_IRQ32,   13, value) ;
           value = ToolConf_DLookupUInt(osstate->config, (tag_t)ARMulCnf_AddrFIQStack,    DEMON_ADDRFIQSTACK) ; /* FIQ stack space */
           ARMulif_SetReg(&osstate->coredesc, ARM_MODE_V5_FIQ32,   13, value) ;
           osstate->usersp = ToolConf_DLookupUInt(osstate->config, (tag_t)ARMulCnf_AddrUserStack, DEMON_ADDRUSERSTACK) ; /* default user stack start */
           osstate->cmdlineaddr = ToolConf_DLookupUInt(osstate->config, (tag_t)ARMulCnf_AddrCmdLine,   DEMON_ADDRCMDLINE) ; /* command line is here after a SWI GetEnv */
           Hostif_PrettyPrint(osstate->hostif, osstate->config, ", Demon");
       }
       ARMulif_InstallEventHandler(&state->coredesc,
                                   ConfigEventSel,
                                   Angel_ConfigEvents, osstate);

       Time_PropDesc.gacb.handle = state;
       ARMulif_AddProperty(&state->coredesc, RDIPropertyGroup_SuperGroup,
                           &Time_PropDesc);
       Clock_PropDesc.gacb.handle = state;
       ARMulif_AddProperty(&state->coredesc, RDIPropertyGroup_SuperGroup,
                           &Clock_PropDesc);

   {
     state->os_bUseRealTime = !ARMul_ClockIsEmulated(config);
     if (osstate->os_verbose && state->os_bUseRealTime)
     {
       Hostif_PrettyPrint(osstate->hostif, osstate->config, " Clock=Real Time");
     }
   }

   for (i = 0; i < FOPEN_MAX; i++) {
      osstate->FileTable[i] = NULL;
      osstate->FileCount[i] = 0;
      }

   for (i = 0; i < UNIQUETEMPS; i++)
      osstate->tempnames[i] = NULL;
}
END_INIT(OS)

#define OS_State OSState

/*void OS_Exit(void *voidosstate)*/
int OS_Exit(RDI_AgentHandle agent)
{
   OS_State *osstate = (OS_State *)agent;
   unsigned i ;

   if (osstate != NULL) {
      if (osstate->CommandLine != NULL)
         free((char *)(osstate->CommandLine));
      for (i = 0; i < UNIQUETEMPS; i++)
         if (osstate->tempnames[i] != NULL)
            free(osstate->tempnames[i]);
      if (osstate->os_file_log)
      {
          fclose(osstate->os_file_log);
      }
      free((char *)osstate);
   }

   return RDIError_NoError;
}


/* This is an RDI_InfoProc, so must validate its inputs */

static int OS_HandleUnkRDI(void *handle,
                          unsigned subcode,
                          ARMword *arg1, ARMword *arg2)
{
    OS_State *osstate = (OS_State *)handle;
    void *data = (void*)arg1;
    (void)arg2;

#ifdef VERBOSE_OS_INFO
    Hostif_DebugPrint(osstate->hostif,"OS_HandleUnkRDI subcode:0x%08lx=%lu\n",
                      (unsigned long)subcode,(unsigned long)subcode);
#endif
    
    switch (subcode) {

    case RDISemiHosting_GetARMSWI:
        *arg1 = osstate->AngelSWIARM;
    case RDIInfo_SemiHostingGetARMSWI:
        return RDIError_NoError;
    case RDISemiHosting_SetARMSWI:
        osstate->AngelSWIARM = *arg1;
    case RDIInfo_SemiHostingSetARMSWI:
        return RDIError_NoError;
    case RDISemiHosting_GetThumbSWI:
        *arg1 = osstate->AngelSWIThumb;
    case RDIInfo_SemiHostingGetThumbSWI:
        return RDIError_NoError;
    case RDISemiHosting_SetThumbSWI:
        osstate->AngelSWIThumb = *arg1;
    case RDIInfo_SemiHostingSetThumbSWI:
        return RDIError_NoError;
    case RDISemiHosting_SetState: /*0x181*/
        osstate->semihostingstate = *arg1;
        return RDIError_NoError;
    case RDISemiHosting_GetState: /*0x182*/
        *arg1 = osstate->semihostingstate;
        return RDIError_NoError;
    case RDISemiHosting_SetVector: /*0x183*/
        osstate->semihostingvector = *arg1;
        return RDIError_NoError;
    case RDISemiHosting_GetVector: /*0x184*/
        *arg1 = osstate->semihostingvector;
        return RDIError_NoError;

    case RDIVector_Catch:
        osstate->vector_catch = *arg1;
        return (osstate->DemonEnabled) ?
            RDIError_NoError : /* Grab it! */
            RDIError_UnimplementedMessage; /* Let the core handle it */
    case RDIInfo_SemiHosting:
         return RDIError_NoError; /* Yes we do! */
    case RDISet_Cmdline :
#ifdef VERBOSE_CMDLINE
        printf("RDISet_Cmdline [%s]\n",(char *)data);
#endif
          if (osstate != NULL)
            free((char *)(osstate->CommandLine));
 
         osstate->CommandLine = (char *)
             calloc(1,(unsigned)strlen((char *)data) + 1);

         if (osstate->CommandLine != NULL)
             (void)strcpy(osstate->CommandLine, (char *)data);
         /*
          * XXXX We swallow this call - there can be only one OS (shame).
          */
         return RDIError_NoError;

    case RDIErrorP : /* return the last Operating System Error */
         if (osstate->ErrorNo != 0)
             *(ARMword *)data = osstate->ErrorNo;
         return RDIError_NoError;
    case RDIProperty_SetAsNumeric:
    {
        unsigned PrID = (unsigned)arg1;
        if (arg2 == NULL)
            return RDIError_UnimplementedMessage;
        switch (PrID)
        {
        case RDIPropID_ARMulProp_Hostif:
            osstate->hostif = *(RDI_HostosInterface const **)arg2;
#if VERBOSE_PROP_HOSTIF
            printf("\n** Semihost rcv RDIPropID_ARMulProp_Hostif **\n");
#endif
            return RDIError_UnimplementedMessage;
            /* Safer than return RDIError_NoError;
             * in case we have more than 1 listener.
             */
        default:
            return RDIError_UnimplementedMessage;
        }   
    }
    case RDIInfo_Target:
    {
        ARMword arg1val;
        arg1val = RDITarget_HostosSignalAware;  /* Emulator, speed 10**5 IPS */
        *arg1 |= arg1val;
        
        return RDIError_UnimplementedMessage;
    }
    case RDISignal_Stop:
        osstate->bStop_Received = TRUE;
        return RDIError_UnimplementedMessage;
   default:
       return RDIError_UnimplementedMessage;
   }
}

/***************************************************************************\
*                             Get a SWI argument                            *
\***************************************************************************/

static ARMword GetNthWordArg(OS_State *osstate,ARMword arg, unsigned isangel)
{
   ARMword mode = ARMulif_GetCPSR(&osstate->coredesc) & 0x1f ;
   if (isangel)
   {
      ARMword baseAddr = ARMulif_GetReg(&osstate->coredesc,mode,1);
      return ARMulif_ReadWord(&osstate->coredesc, baseAddr + (arg * 4));
   }
   else
      return ARMulif_GetReg(&osstate->coredesc,mode,arg);
}

static char GetNthByteArg(OS_State *osstate,ARMword arg, unsigned isangel)
   {
   ARMword mode = ARMulif_GetCPSR(&osstate->coredesc) & 0x1f ;
   if (isangel)
      return (char)ARMulif_ReadByte(&osstate->coredesc, ARMulif_GetReg(&osstate->coredesc,mode,1) + arg);
   else
      return (char)ARMulif_GetReg(&osstate->coredesc,mode,arg);
   }

/***************************************************************************\
*  Copy a null-terminated string from the debuggee's memory to the host's  *
\***************************************************************************/

static bool GetString(RDI_ModuleDesc *coredesc, ARMword from, char *to,
                      unsigned int BufferSize)
{
    unsigned int i = 0;
    do {
        if (++i > BufferSize) 
        {
            assert(0);
            return FALSE;
        }
        *to = (char)ARMulif_ReadByte(coredesc,from++);
    } while (*to++ != '\0');
    return TRUE;
}



/* ---------- Demon Exception-Exit Stuff ------------ */

/*
 * Macros to extract instruction fields
 */

#define BIT(n) ( (ARMword)(instr>>(n))&1)   /* bit n of instruction */
#define BITS(m,n) ( (ARMword)(instr<<(31-(n))) >> ((31-(n))+(m)) ) /* bits m to n of instr */
#define TOPBITS(n) (instr >> (n)) /* bits 31 to n of instr */


#define UNEG(aval) (~aval + 1)
#define CPSRINSTRUCTIONSETBITPOSN (5)
#define CPSRINSTRUCTIONSETBITS (0x1 << CPSRINSTRUCTIONSETBITPOSN)
#define CPSRINSTRUCTIONSET(r) ((r & CPSRINSTRUCTIONSETBITS) ? INSTRUCTION16 : INSTRUCTION32)
#define CPSRINSTRUCTIONSIZE(r) ((r & CPSRINSTRUCTIONSETBITS) ? INSTRUCTION16SIZE : INSTRUCTION32SIZE)

/* state->Instruction_set values */
#define INSTRUCTION32 (0)
#define INSTRUCTION16 (1)
#define INSTRUCTION16SIZE 2
#define INSTRUCTION32SIZE 4

#define OLDINSTRUCTIONSIZE (CPSRINSTRUCTIONSIZE(ARMulif_GetSPSR(&OSptr->coredesc, ARMulif_GetCPSR(&OSptr->coredesc))))

/*
 * Unwind a data abort
 */

static void UnwindDataAbort(OSState *osstate, ARMword addr, ARMword iset)
{
  if (iset == INSTRUCTION16) {
    ARMword instr = ARMulif_ReadWord(&osstate->coredesc, addr);
    ARMword offset;
    unsigned long regs;
    if (/*state->bigendSig */ 0 ^ ((addr & 2) == 2))
      /* get instruction into low 16 bits */
      instr = instr >> 16;
    switch (BITS(11,15)) {
    case 0x16:
    case 0x17:
      if (BITS(9,10) == 2) { /* push/pop */
        regs = BITS(0, 8);
        offset = 0;
        for (; regs != 0; offset++)
          regs ^= (regs & UNEG(regs));
        /* pop */
        if (BIT(11))
          ARMulif_SetReg(&osstate->coredesc, CURRENTMODE,13,
                       ARMulif_GetReg(&osstate->coredesc, CURRENTMODE,13) - offset*4);
        else
          ARMulif_SetReg(&osstate->coredesc, CURRENTMODE,13,
                       ARMulif_GetReg(&osstate->coredesc, CURRENTMODE,13) + offset*4);
      }
      break;
    case 0x18:
    case 0x19:
      regs = BITS(0,7);
      offset = 0;
      for (; regs != 0; offset++)
        regs ^= (regs & UNEG(regs));
      /* pop */
      if (BITS(11,15) == 0x19) { /* ldmia rb! */
        ARMulif_SetReg(&osstate->coredesc, CURRENTMODE,BITS(8,10),
                     ARMulif_GetReg(&osstate->coredesc, CURRENTMODE,BITS(8,10)) - offset*4);
      } else {          /* stmia rb! */
        ARMulif_SetReg(&osstate->coredesc, CURRENTMODE,BITS(8,10),
                     ARMulif_GetReg(&osstate->coredesc, CURRENTMODE,BITS(8,10)) + offset*4);
      }
      break;
    default:
      break;
    }
  } else
  {
    ARMword instr = ARMulif_ReadWord(&osstate->coredesc, addr);
    ARMword rn = BITS(16, 19);
    ARMword itype = BITS(24, 27);
    ARMword offset;

    if (rn == 15) return;
    if (itype == 8 || itype == 9) {
      /* LDM or STM */
      unsigned long regs = BITS(0, 15);
      offset = 0;
      if (!BIT(21)) return; /* no wb */
      for (; regs != 0; offset++)
        regs ^= (regs & UNEG(regs)); /* (regs & -regs) */
      if (offset == 0) offset = 16;
    } else if (itype == 12 || /* post-indexed CPDT */
               (itype == 13 && BIT(21))) { /* pre_indexed CPDT with WB */
      offset = BITS(0, 7);
    } else
      return;
    
    /* pop */
    if (BIT(23))
      ARMulif_SetReg(&osstate->coredesc,CURRENTMODE,rn,
                   ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,rn) - offset*4);
    else
      ARMulif_SetReg(&osstate->coredesc,CURRENTMODE,rn,
                   ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,rn) + offset*4);
  }
}



/*
 * If demon is enabled, we shall provide semihosting SWI's
 * >= 0x80 for the FPE, which shall implement vector_catch.
 *
 * A typical sequence for an unhandled coprocessor-access-bounce
 * is SWI 0x91 (raise undef) then SWI 0x81 (pass to debugger).
 *
 * A major issue is what value to write to R15.
 * If we halt "immediately" then we must add PCR15DIFFERENCE to the
 * value we write to R15, because this will be subtracted before
 * the debugger gets it.
 * If we halt after refilling the pipeline (as ARM7 + ARM9 cores do,
 * using state->NextInstr & PRIMEPIPE),
 * then we don't want an offset.
 * If we are in ARMISS, and always bias R15 on both reads and writes,
 * then we also don't want to add anything to R15.
 * This leaves StrongARM, which fortunately also does a pipe-refill
 * when an Abort is handled.
 */
static bool handleDemonExitSWIs(OSState *osstate, unsigned number)
{
  OSState *OSptr = osstate;
#ifdef VERBOSE_DEMON_EXIT
    printf("handleDemonExitSWIs enabled:%u number:0x%04x vector_catch:%08lx\n",
           (unsigned)osstate->DemonEnabled,(unsigned)number, 
           (unsigned long)OSptr->vector_catch);
#endif
  if (!osstate->ExitSwisEnabled)
    return FALSE;
  switch(number)
  {
  case SWI_InstallHandler: 
  {
    /*
     * SWI_InstallHandler installs a handler for a hardware exception. On
     * entry r0 contains the exception number; r1 contains a value to pass
     * to the handler when it is eventually invoked; and r2 contains the
     * address of the handler. On return r2 contains the adderss of the
     * previous handler and r1 contains its argument.
     */
    ARMword handlerp = (OSptr->map.handlers + 
                        ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,0) * 8);
    ARMword oldr1 = ARMulif_ReadWord(&osstate->coredesc, handlerp),
    oldr2 = ARMulif_ReadWord(&osstate->coredesc, handlerp + 4);
    ARMulif_WriteWord(&osstate->coredesc, handlerp, 
                      ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,1));
    ARMulif_WriteWord(&osstate->coredesc, handlerp + 4, 
                      ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,2));
    ARMulif_SetReg(&osstate->coredesc,CURRENTMODE,1, oldr1);
    ARMulif_SetReg(&osstate->coredesc,CURRENTMODE,2, oldr2);
    return TRUE;
  }
  case 0x90: {          /* Branch through zero */
    ARMword oldpsr = ARMulif_GetCPSR(&osstate->coredesc);
    ARMulif_SetCPSR(&osstate->coredesc, (oldpsr & 0xffffffc0) | 0x13);
    ARMulif_SetSPSR(&osstate->coredesc, SVC32MODE, oldpsr);
    ARMulif_SetReg(&osstate->coredesc,CURRENTMODE,14, 0);
    goto TidyCommon;
  }
    
  case 0x98: {          /* Error */
    ARMword errorp = ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,0),
    regp = ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,1);
    unsigned i;
    ARMword errorpsr = ARMulif_ReadWord(&osstate->coredesc, regp + 16*4);
    for (i = 0; i < 15; i++)
      ARMulif_SetReg(&osstate->coredesc,(unsigned)errorpsr,i,
                   ARMulif_ReadWord(&osstate->coredesc, regp + i*4L));
    ARMulif_SetReg(&osstate->coredesc,CURRENTMODE,14, ARMulif_ReadWord(&osstate->coredesc, regp + 15*4L));
    ARMulif_SetReg(&osstate->coredesc,CURRENTMODE,10, errorp);
    ARMulif_SetSPSR(&osstate->coredesc,CURRENTMODE,errorpsr);
    OSptr->ErrorNo = errorp; /* ErrorP */
    goto TidyCommon;
  }
    
  case 0x94: {          /* Data abort */
        /* DATA ABORT RETURN ADDRESS FIXUP */
    ARMword addr = (ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,14) - 8);
    ARMword cpsr = ARMulif_GetCPSR(&osstate->coredesc);
    /* QQ: Do we really still support 26-bit mode? I think not. */
    if (ARMulif_GetMode(&osstate->coredesc) < USER32MODE)
      addr = addr & 0x3fffffc;
    ARMulif_SetCPSR(&osstate->coredesc,ARMulif_GetSPSR(&osstate->coredesc,cpsr));
    UnwindDataAbort(osstate, addr,
                 CPSRINSTRUCTIONSET(ARMulif_GetSPSR(&osstate->coredesc,cpsr)));
    ARMulif_SetCPSR(&osstate->coredesc,cpsr);
    
    /* and fall through to correct r14 */
  } 
  case 0x95:                    /* Address Exception */
    ARMulif_SetReg(&osstate->coredesc,CURRENTMODE,14,
                 ARMulif_GetReg(&osstate->coredesc, CURRENTMODE, 14)-8);
  case 0x91:                    /* Undefined instruction */
  case 0x92:                    /* SWI */
  case 0x93:                    /* Prefetch abort */
  case 0x96:                    /* IRQ */
  case 0x97:                    /* FIQ */

 
    ARMulif_SetReg(&osstate->coredesc,CURRENTMODE,14,
                 ARMulif_GetReg(&osstate->coredesc, CURRENTMODE, 14)-4);
#ifdef VERBOSE_DEMON_EXIT
    printf("DemonExitSWI number:0x%04x just reduced R14 to %08lx\n",
           (unsigned)number, 
           (unsigned long)ARMulif_GetReg(&osstate->coredesc, CURRENTMODE, 14));
#endif
  TidyCommon:
    if (OSptr->vector_catch & (1 << (number - 0x90))) {
        /* DRS 2000-08-25
         * Lars agrees that we want to point at the instruction which
         * we are catching, and this makes armsd look right.
         * (OTOH, I'm not sure that we want to apply this
         */
      /* Point at the instruction that cause the vector-catch */
# define DEMON_PC_OFFSET 0

        /* This has no effect while stepping through an UNDEF that
         * isn't handled by the FPE code because it's not executed.
         */
      ARMulif_SetR15(&osstate->coredesc, 
                     (ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,14)+ 4
                           + DEMON_PC_OFFSET ));
      ARMulif_SetCPSR(&osstate->coredesc,
                      ARMulif_GetSPSR(&osstate->coredesc,
                                      ARMulif_GetCPSR(&osstate->coredesc)));
#ifdef VERBOSE_DEMON_EXIT
      printf("Demon Exit SWI STOPPING number:0x%04x vector_catch:%08lx "
             "pc:%08lx\n",
           (unsigned)number, (unsigned long)OSptr->vector_catch,
             (unsigned long)ARMulif_GetR15(&osstate->coredesc));
#endif
      if (number == 0x90) {
#if 0
          printf("b t z\n");
#endif
        ARMulif_StopExecution(&osstate->coredesc, RDIError_BranchThrough0);
      } else {
        ARMulif_StopExecution(&osstate->coredesc,number -  0x8f);
      }
    } else {
      ARMword sp = ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,13);
      ARMulif_WriteWord(&osstate->coredesc, sp - 4, 
                        ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,14));
      ARMulif_WriteWord(&osstate->coredesc, sp - 8, 
                        ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,12));
      ARMulif_WriteWord(&osstate->coredesc, sp - 12,
                        ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,11));
      ARMulif_WriteWord(&osstate->coredesc, sp - 16,
                        ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,10));
      ARMulif_SetReg(&osstate->coredesc,CURRENTMODE,13, sp - 16);
      ARMulif_SetReg(&osstate->coredesc,CURRENTMODE,11, 
                     OSptr->map.handlers + 8 * (number-0x90));
#ifdef VERBOSE_DEMON_EXIT
      printf("DemonExitSWI TidyCommon (stacking, not stopping) number:0x%04x "
             "vector_catch:%08lx stacked R14:%08lx\n",
           (unsigned)number, (unsigned long)OSptr->vector_catch,
             (unsigned long)ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,14));
#endif
    }
    return TRUE;
    
    /* SWI's 0x8x pass an abort of type x to the debugger if a handler returns */
  case 0x80: case 0x81: case 0x82: case 0x83:
  case 0x84: case 0x85: case 0x86: case 0x87: case 0x88: {
    ARMword sp = ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,13);
    ARMulif_SetReg(&osstate->coredesc,CURRENTMODE,10, 
                   ARMulif_ReadWord(&osstate->coredesc, sp));
    ARMulif_SetReg(&osstate->coredesc,CURRENTMODE,11, 
                   ARMulif_ReadWord(&osstate->coredesc, sp + 4));
    ARMulif_SetReg(&osstate->coredesc,CURRENTMODE,12, 
                   ARMulif_ReadWord(&osstate->coredesc, sp + 8));
    ARMulif_SetReg(&osstate->coredesc,CURRENTMODE,14, 
                   ARMulif_ReadWord(&osstate->coredesc, sp + 12));
    ARMulif_SetReg(&osstate->coredesc,CURRENTMODE,13, sp + 16);
    /* the pipelining the the RDI will undo */

    if ( number == 0x84)
    {
        /* Data abort - return address is an extra 4! */
        ARMulif_SetR15(&osstate->coredesc,
                       (ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,14)+
                        4 + DEMON_PC_OFFSET)); /* ? Check this! */
    }
    else
    {
        ARMulif_SetR15(&osstate->coredesc,
                       (ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,14)+
                        DEMON_PC_OFFSET));
    }
#ifdef VERBOSE_DEMON_EXIT
      printf("DemonExitSWI Stopping number:0x%04x "
             "unstacked R14:%08lx -> R15:%08lx\n",
             (unsigned)number, 
             (unsigned long)ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,14),
             (unsigned long)ARMulif_GetReg(&osstate->coredesc,CURRENTMODE,15));
#endif

    ARMulif_SetCPSR(&osstate->coredesc,
                    ARMulif_GetSPSR(&osstate->coredesc,
                                    ARMulif_GetCPSR(&osstate->coredesc)));
    if (number == 0x80)
    {
        ARMulif_StopExecution(&osstate->coredesc, RDIError_BranchThrough0); 
    }    
    else
      ARMulif_StopExecution(&osstate->coredesc,number - 0x7f);
    return TRUE;
  default:
      return FALSE; /* We didn't understand */
  } /* end switch(number) */
}
} /*( ?? )*/



/***************************************************************************\
* The emulator calls this routine when a SWI instruction is encuntered. The *
* parameter passed is the SWI number (lower 24 bits of the instruction).    *
\***************************************************************************/

static INLINE unsigned RaiseOutOfSemihostEventAndReturnTRUE(OS_State *state)
{
   ARMulif_RaiseEvent(&state->coredesc,DebugEvent_OutOfSemihost,0,0);
   return TRUE;
}

#define TRY_NEXT return FALSE
#define DONE  return RaiseOutOfSemihostEventAndReturnTRUE(osstate)

/* We get called for ConfigEvent_Reset (50002) and CoreEvent_SWI (3) */
static unsigned OS_HandleException(void *handle, 
                              void *data)
{
    ARMul_Event *evt = data;

   OS_State *osstate = (OS_State *)handle;

   ARMword vector = evt->data1;
   ARMword instr = evt->data2;

#ifdef VERBOSE_EXCEPTION
   int dummy = printf("\n* OS_handleException, osstate = %p, "
                      "coredesc.rdi=%p evt=(%08lx,%08lx,%08lx)\n", osstate, 
                      osstate->coredesc.rdi,
                      (unsigned long)evt->event,
                      (unsigned long)evt->data1,
                      (unsigned long)evt->data2
                      );
#endif
#ifdef VERBOSE_RESET /* for making sure the reset event gets sent. */
   if (evt->event == CoreEvent_Reset)
       printf("** Semihost gets Reset **\n");
#endif
   unsigned isangel;
   ARMword mode = ARMulif_GetCPSR(&osstate->coredesc) & 0x1f ;
   ARMword addr, temp;
   char buffer[BUFFERSIZE], *cptr;
   FILE *fptr;
  osstate->bStop_Received = FALSE;
#ifdef VERBOSE_EXCEPTION
   (void)dummy;
#endif

   if (evt->event != CoreEvent_SWI || vector != ARM_VECTOR_SWI)
   { /* we only do SWIs */

     tryNext:
       TRY_NEXT;
   }

   instr = instr & 0x00ffffff ;

   if (instr == osstate->AngelSWIARM || instr == osstate->AngelSWIThumb) {
      if (osstate->AngelEnabled &&
          (osstate->semihostingstate & SEMIHOSTSTATE_ENABLED)
          )
      {
         instr = ARMulif_GetReg(&osstate->coredesc,mode,0) ;
         isangel = TRUE ;
      }
      else
         goto tryNext;
      }   
   else if ( (osstate->DemonEnabled && 
              (osstate->semihostingstate & SEMIHOSTSTATE_ENABLED))
              ||
       (osstate->ExitSwisEnabled && instr >= 0x80 && instr <= 0x9f)) {
      if (instr & 0x100)
         goto tryNext;
      instr |= 0x100 ; /* to make the SWI number distinct from the SYS number */
      isangel = FALSE ;
      }
   else /* not for us */
         goto tryNext;
#ifdef VERBOSE_EXCEPTION
   printf("Semihosted SWI# %02x\n", (unsigned)instr);
#endif

   /*
    * Tell anyone who cares that semihosting has begun.  This is required
    * because semihosting is half-like a debug state, and some models may
    * be confused by the spurious memory accesses
    */
   ARMulif_RaiseEvent(&osstate->coredesc,DebugEvent_InToSemihost,0,0);
   switch (instr) {
      case SWI_WriteC :
      case SYS_WRITEC: /* 0x3 */
         Hostif_ConsoleWriteC(osstate->hostif, GetNthByteArg(osstate,0,isangel));
         osstate->ErrorNo = errno;
         DONE;
       
      case SWI_Write0 :
      case SYS_WRITE0: /* 0x4 */
         addr = ARMulif_GetReg(&osstate->coredesc,mode, isangel ? 1 : 0);
         while ((temp = ARMulif_ReadByte(&osstate->coredesc,addr++)) != 0)
         {
             Hostif_ConsoleWriteC(osstate->hostif, (char)temp );
         }
         osstate->ErrorNo = errno;
         break;

      case SWI_WriteHex:
         addr = ARMulif_GetReg(&osstate->coredesc,mode,0) ;
         Hostif_ConsolePrint(osstate->hostif, "%08lx", (unsigned long)addr);
         break;
     
      case SWI_ReadC :
      case SYS_READC:
      {
         int  temp = Hostif_ConsoleReadC(osstate->hostif);
           if(osstate->bStop_Received == TRUE)
           {
               ARMword pc=ARMulif_GetReg(&osstate->coredesc,mode,15);
               ARMulif_SetReg(&osstate->coredesc,mode,15, pc-8);
               ARMulif_StopExecution(&osstate->coredesc, RDIError_UserInterrupt);
               DONE;
           }
         ARMulif_SetReg(&osstate->coredesc,mode,0,(ARMword)temp);
         osstate->ErrorNo = errno;
         break;
      }
      case SWI_CLI : /* Demon only */
         addr = ARMulif_GetReg(&osstate->coredesc, mode,0);
         if (GetString(&osstate->coredesc,addr,buffer,BUFFERSIZE))
         {
             ARMulif_SetReg(&osstate->coredesc, mode, 0, 
                            (ARMword)system(buffer));
             osstate->ErrorNo = errno;
         }
#ifdef VERBOSE_CLI
         printf("Angel_SWI_CLI[%s] ->%u\n",buffer,(unsigned)osstate->ErrorNo);
#endif
         break;
   
      case SYS_SYSTEM: {
         unsigned int c, len;

         addr = ARMulif_GetReg(&osstate->coredesc, mode,1);
         len = ARMulif_ReadWord(&osstate->coredesc, addr + 4);

         /* memset(buffer,'Z',BUFFERSIZE); -- check that we DO null-terminate
          * by being very bad when we don't. */

         if (ARMulif_ReadWord(&osstate->coredesc, addr) == (unsigned int)NULL
             || len > BUFFERSIZE-1) /* Reserve one byte for the null. */
         {
            /* handle the "check for command processor" case */
            ARMulif_SetReg(&osstate->coredesc, mode, 0, 1);
            DONE;
         }
         addr = ARMulif_ReadWord(&osstate->coredesc, addr); /* Where text is */
         for (c = 0; c < len; c++)
            buffer[c] = (char)ARMulif_ReadByte(&osstate->coredesc, addr + c);
         buffer[c]=0; /* Null-terminate */
         ARMulif_SetReg(&osstate->coredesc, mode, 0, (ARMword)system(buffer));
         osstate->ErrorNo = errno;
         break;
         }

      case SWI_GetEnv :
         ARMulif_SetReg(&osstate->coredesc,mode,0,osstate->cmdlineaddr);
         ARMulif_SetReg(&osstate->coredesc,mode,1,osstate->usersp) ;

         addr = osstate->cmdlineaddr;
         cptr = osstate->CommandLine;
         if (cptr == NULL)
            cptr = "\0";
         do {
            temp = (ARMword)*cptr++;
            ARMulif_WriteByte(&osstate->coredesc,addr++,temp);
            } while (temp != 0);
         break;

      case SYS_GET_CMDLINE: {
         ARMword word2;  /* Buffer size */
         char * ccptr = osstate->CommandLine;
         unsigned cl_length;

         ARMword arg_addr = ARMulif_GetReg(&osstate->coredesc,mode,1);

         if (ccptr != NULL && arg_addr != 0)
         {
             addr = ARMulif_ReadWord(&osstate->coredesc, arg_addr);
             word2 = ARMulif_ReadWord(&osstate->coredesc, 4 + arg_addr);

#ifdef VERBOSE_CMDLINE
             printf("SYS_GET_CMDLINE [%s] "
                    "R1:0x%08lx->{addr:0x%08lx size:0x%08lx}\n",
                    ccptr,
                    (unsigned long)arg_addr,
                    (unsigned long)addr, (unsigned long)word2);
#endif


             if (ccptr == NULL)
                 ccptr = "\0";
             cl_length = strlen(ccptr) + 1; /* Include the \0 */
             /* We need to fill in the length of the string too */
             ARMulif_WriteWord(&osstate->coredesc, 4+arg_addr, cl_length
#ifdef CMDLINE_LENGTH_EXCLUDES_NULL
                               - 1
#endif
                 );
             if ( word2 == 0 || word2 < cl_length ) {
                 /* The buffer size given is too small - do no more. */
                 ARMulif_SetReg(&osstate->coredesc,mode,0, -1);
                 DONE;
             }
             if (addr != 0)
             {
                 do {
                     temp = (ARMword)*ccptr++;
                     ARMulif_WriteByte(&osstate->coredesc,addr++,temp);
                 } while (temp != 0);
             }
             ARMulif_SetReg(&osstate->coredesc,mode,0, 0);
         }
         else
         {
             /* Return the only error-code. */
             ARMulif_SetReg(&osstate->coredesc,mode,0, -1);
         }
         break;
      }
     
      case SYS_ISERROR: {
         ARMword status;

         /* Read the value at the address given in R1 */
         status = ARMulif_ReadWord(&osstate->coredesc, ARMulif_GetReg(&osstate->coredesc,mode,1));
        
         if ((int)status == -1) /* We DO have an error code - so return 1*/
            ARMulif_SetReg(&osstate->coredesc,mode,0, 1);
         else /* No error code - so return 0 */
            ARMulif_SetReg(&osstate->coredesc,mode,0, 0);
         break;
         }
     
      case SWI_GetErrno:
      case SYS_ERRNO:
         ARMulif_SetReg(&osstate->coredesc, mode, 0, osstate->ErrorNo);
         break;

      case SWI_Clock :
      case SYS_CLOCK: {
         ARMulif_SetReg(&osstate->coredesc, mode, 0,
                        (ARMword)OS_GetCentiseconds(osstate));
         osstate->ErrorNo = errno;
         break;
         }

      case SWI_Time:
      case SYS_TIME:
         ARMulif_SetReg(&osstate->coredesc,mode,0, (ARMword)time(NULL));
         osstate->ErrorNo = errno;
         break;

      case SWI_Remove :
      case SYS_REMOVE:
         addr = GetNthWordArg(osstate,0,isangel) ;
         GetString(&osstate->coredesc, addr, buffer,BUFFERSIZE);
         ARMulif_SetReg(&osstate->coredesc,mode,0,remove(buffer));
         osstate->ErrorNo = errno;
         break;
   
      case SWI_Rename :  
      case SYS_RENAME: {
         char buffer2[BUFFERSIZE];
         ARMword addr2;

         if (isangel) {
            ARMword args = ARMulif_GetReg(&osstate->coredesc,mode,1);
            ARMword len1,len2 ;
            addr = ARMulif_ReadWord(&osstate->coredesc, args) ;
            len1 = ARMulif_ReadWord(&osstate->coredesc, args+4) ;
            addr2 = ARMulif_ReadWord(&osstate->coredesc, args+8) ;
            len2 = ARMulif_ReadWord(&osstate->coredesc, args+12) ;
            if (!len1 || !len2)
            {
                ARMulif_SetReg(&osstate->coredesc,mode,0, 1L); /* FAIL */
                break;
            }
         }
         else {
            addr = ARMulif_GetReg(&osstate->coredesc, mode, 0) ;
            addr2 = ARMulif_GetReg(&osstate->coredesc, mode, 1) ;
         }

         GetString(&osstate->coredesc,addr, buffer,BUFFERSIZE);
         GetString(&osstate->coredesc,addr2, buffer2,BUFFERSIZE);

#ifdef VERBOSE_SYS_FILE
         Hostif_ConsolePrint(osstate->hostif,"SYS_RENAME<%s><%s>\n", buffer, buffer2);
#endif


         ARMulif_SetReg(&osstate->coredesc,mode,0, rename(buffer,buffer2));
         osstate->ErrorNo = errno;
         break;
         }

      case SWI_Open :  
      case SYS_OPEN:  /* 0x1 */
      {
         static char* fmode[] = {"r","rb","r+","r+b",
                                 "w","wb","w+","w+b",
                                 "a","ab","a+","a+b",
                                 "r","r","r","r"} /* last 4 are illegal */;
         char type;
         ARMword addr = GetNthWordArg(osstate,0,isangel);
         
         type = (char)(GetNthWordArg(osstate,1,isangel) & 0xf) ;
         GetString(&osstate->coredesc,addr,buffer,BUFFERSIZE);

         LOG_FILE_OP(osstate,"Open('%s','%s')\n",buffer,fmode[(int)type]);

         osstate->ErrorNo = 0;

         if (strcmp(buffer,":tt")==0)
         {
#ifdef VERBOSE_SYS_FILE
             Hostif_ConsolePrint(osstate->hostif,"SYS_OPEN stdio<%s> type:%0x02X\n", buffer, type);
#endif
             if ((type == 0 || type == 1)) /* opening tty "r" */
                 fptr = stdin;
             else if ((type==4 || type==5)) /* opening tty "w" */
                 fptr = stdout;
             else
             {
                 /* Cant openup stdio - fail. */
                 ARMulif_SetReg(&osstate->coredesc, mode, 0, (ARMword)-1L);
                 break;
             }
         }
         else
         {
             fptr = fopen(buffer,fmode[(int)type]);
             osstate->ErrorNo = errno;
         }
#ifdef VERBOSE_SYS_FILE
         Hostif_ConsolePrint(osstate->hostif,"SYS_OPEN ftpr:%p\n", buffer, type, fptr);
#endif
         ARMulif_SetReg(&osstate->coredesc,mode,0,0);
         if (fptr != NULL) {
            for (temp = 0; temp < FOPEN_MAX; temp++)
               if (osstate->FileTable[temp] == NULL || 
                   (osstate->FileTable[temp] == fptr && 
                    (osstate->FileFlags[temp] & 1) == (type & 1))) 
               {
                  osstate->OpenMode[temp] = type;
                  osstate->FileTable[temp] = fptr;
                  osstate->FileFlags[temp] = type & 1; /* preserve the binary bit */
                  osstate->FileCount[temp]++; /* count how many times we open the same file */
                  ARMulif_SetReg(&osstate->coredesc,mode,0, (ARMword)(temp + 1));
#ifdef VERBOSE_SYS_FILE
         Hostif_ConsolePrint(osstate->hostif,"SYS_OPEN<%s> type:0x%02X ftpr:%p ->%u\n", buffer, type, fptr,
                             (unsigned)temp+1);
#endif
                  LOG_FILE_OP(osstate,"  OK -> count:%u filenum:%u\n",
                              (unsigned)osstate->FileCount[temp],
                              (unsigned)temp+1);

                  break;
               }
            if (ARMulif_GetReg(&osstate->coredesc,mode,0) == 0)
               osstate->ErrorNo = EMFILE; /* too many open files */
            else {
                /* osstate->ErrorNo = errno; -- done after fopen. */
            }
            }
         else {
             /* osstate->ErrorNo = errno; -- done after fopen. */
            ARMulif_SetReg(&osstate->coredesc, mode, 0, (ARMword)NONHANDLE);
            }
         break;
         }

      case SYS_CLOSE:
      case SWI_Close :
         temp = GetNthWordArg(osstate,0,isangel) - 1 ;
         LOG_FILE_OP(osstate,"Close(%i)\n",temp+1);
         if ((int)temp == -1 || temp >= FOPEN_MAX || 
             osstate->FileTable[temp] == 0) {

             LOG_FILE_OP(osstate,"  REJECTED\n",temp);

            osstate->ErrorNo = EBADF;
            ARMulif_SetReg(&osstate->coredesc, mode, 0, (ARMword)-1L);
            DONE;
            }

         osstate->ErrorNo = 0;

         if (!(--osstate->FileCount[temp])) 
         {
            fptr = osstate->FileTable[temp];
            if (fptr == stdin || fptr == stdout)
            {
               osstate->ErrorNo = 0;
#ifdef VERBOSE_SYS_FILE
                Hostif_ConsolePrint(osstate->hostif,"SYS_CLOSE ftpr:%p closing stdin/out temp:%u\n", 
                                    fptr,temp);
#endif
               ARMulif_SetReg(&osstate->coredesc,mode,0, 0);
            }
            else
            { 
               ARMword fcres;
#ifdef VERBOSE_SYS_FILE
                Hostif_ConsolePrint(osstate->hostif,"SYS_CLOSE fclose(ftpr:%p) temp:%u\n", fptr, temp);
#endif
               errno = 0;
               fcres = (ARMword)fclose(fptr);
               osstate->ErrorNo = errno;
               ARMulif_SetReg(&osstate->coredesc,mode,0, fcres);
               osstate->FileTable[temp] = NULL;
               LOG_FILE_OP(osstate,"  CLOSED(hostfp:%p)->0x%08x, errno:%i\n",
                           fptr, (unsigned)fcres,
                           osstate->ErrorNo);
            }
         }
         else
         {
#ifdef VERBOSE_SYS_FILE
             Hostif_ConsolePrint(osstate->hostif,"SYS_CLOSE NOT CLOSING temp:%u\n", temp);
#endif
            ARMulif_SetReg(&osstate->coredesc,mode,0, 0);
         }
         break;
     
      case SWI_Write :
      case SYS_WRITE: /* 0x5 */ 
      {
         unsigned size, upto, type;
         char ch;
     
         temp = GetNthWordArg(osstate,0,isangel) - 1;
         size = (unsigned)GetNthWordArg(osstate,2,isangel) ;
         LOG_FILE_OP(osstate,"Write(fh:%d,size:%u)\n",temp+1,size);
         if ((int)temp == -1 || temp >= FOPEN_MAX || 
             osstate->FileTable[temp] == 0) 
         {
            osstate->ErrorNo = EBADF;
            ARMulif_SetReg(&osstate->coredesc, mode, 0, (ARMword)size);
            DONE;
         }

         fptr = osstate->FileTable[temp];
         type = osstate->FileFlags[temp];
         addr = GetNthWordArg(osstate,1,isangel) ;
         if (type & READOP)
         {
            fseek(fptr,0L,SEEK_CUR);
            osstate->ErrorNo = errno;
         }
         osstate->FileFlags[temp] = (type & BINARY) | WRITEOP;;
         while (size > 0) {
            if (size >= BUFFERSIZE)
               upto = BUFFERSIZE;
            else
               upto = size;
            for (cptr = buffer; (cptr - buffer) < (int)upto; cptr++) {
               ch = (char)ARMulif_ReadByte(&osstate->coredesc,(ARMword)addr++);
               *cptr = ch;
               }
            temp = (fptr == stderr || fptr == stdout)
                 ? (unsigned)Hostif_ConsoleWrite(osstate->hostif, buffer, upto)
                 : (unsigned)fwrite(buffer,1,upto,fptr);
            if (temp < upto) {
               ARMulif_SetReg(&osstate->coredesc,mode,0, (ARMword)(size - temp));
               osstate->ErrorNo = errno;
               DONE;
               }
            size -= upto;
        
            }
         ARMulif_SetReg(&osstate->coredesc,mode,0, 0);
         osstate->ErrorNo = errno;
         break;
         }

      case SWI_Read :
      case SYS_READ: {
          /* ARGS:
           *     IN
           * 0 File-handle
           * 1 DestAddress
           * 2 Size
           *     OUT
           * R0=0==NoError
           *    else (Size - BytesMoved).
           */
         unsigned size, upto, type;
         char ch;
         bool first = TRUE;
         ARMword pc=ARMulif_GetReg(&osstate->coredesc,mode,15);
         temp = GetNthWordArg(osstate,0,isangel) - 1;
         size = (unsigned)GetNthWordArg(osstate,2,isangel) ;
         if ((int)temp == -1 || temp >= FOPEN_MAX || 
             osstate->FileTable[temp] == 0 ||
             /* Don't pass read from write-only file to host - 
              * this creates an error (in HPUX's cc)
              * when we try to fclose the file later.
              * D/E 78409. */
             osstate->OpenMode[temp] == 4 || osstate->OpenMode[temp] == 5
             ) {
             osstate->ErrorNo = EBADF;
             ARMulif_SetReg(&osstate->coredesc, mode, 0, (ARMword)size);
             LOG_FILE_OP(osstate,
                         "Read(fh:%d,size:%u) REJECTED\n",
                         temp+1,size);

             DONE;
             }

         fptr = osstate->FileTable[temp];
         type = osstate->FileFlags[temp];
         addr = GetNthWordArg(osstate,1,isangel) ;
         LOG_FILE_OP(osstate,"Read(fh:%d,type:%u,addr:%08x,size:%u)\n",temp+1,type,
                      (unsigned)addr,size);
         if (type & WRITEOP)
            fseek(fptr,0L,SEEK_CUR);
         osstate->FileFlags[temp] = (type & BINARY) | READOP;;
         if(osstate->bResume_Read == TRUE)
         {
             int i = 0;
             osstate->bResume_Read = FALSE;
             for (cptr = osstate->saved_buffer; 
                  (cptr - osstate->saved_buffer) < (int)temp;
                  cptr++) 
             {
                 ch = *cptr;
                 buffer[i++] = ch;
             }
               size = osstate->saved_size ;
               addr = osstate->saved_addr ;
         }
         while (size > 0) {
            if (fptr == stdin || fptr == stdout) {
               upto = (size >= BUFFERSIZE)?BUFFERSIZE:size + 1;
               if (Hostif_ConsoleRead(osstate->hostif, buffer, upto) != 0)
               {
                   temp = strlen(buffer);
                   if(osstate->bStop_Received == TRUE)
                   {
                       osstate->bStop_Received = FALSE;
                       if(first != TRUE)
                       {
                           int i = 0;
                           osstate->bResume_Read = TRUE;
                           for (cptr = buffer; 
                                (cptr - buffer) < (int)temp;
                                cptr++) 
                           {
                               ch = *cptr;
                               osstate->saved_buffer[i++] = ch;
                           }
                           osstate->saved_size = size;
                           osstate->saved_addr = addr;
                      
                       }
                       /*   ARMulif_SetReg(&osstate->coredesc,mode,0, (size - temp)); */ 
                       ARMulif_SetReg(&osstate->coredesc,mode,15, pc-8); 
                       ARMulif_StopExecution(&osstate->coredesc, RDIError_UserInterrupt);
                       DONE;
                   }
                 
               }
               else
               {
                   temp = 0;
                   if(osstate->bStop_Received == TRUE)
                   {
                       osstate->bStop_Received = FALSE;
                       if(first != TRUE)
                       {
                           int i = 0;
                           osstate->bResume_Read = TRUE;
                           for (cptr = buffer; 
                                (cptr - buffer) < (int)temp;
                                cptr++) 
                           {
                               ch = *cptr;
                               osstate->saved_buffer[i++] = ch;
                           }
                           osstate->saved_size = size;
                           osstate->saved_addr = addr;             
                       }
                       ARMulif_StopExecution(&osstate->coredesc, RDIError_UserInterrupt);
                       ARMulif_SetReg(&osstate->coredesc,mode,15, pc-8);
                       DONE;
                   }
                
               upto--;               /* 1 char used for terminating null */
               }
            }
            else {
                /* Neither stdin nor stdout. */
               upto = (size>=BUFFERSIZE)?BUFFERSIZE:size;
               temp = fread(buffer,1,upto,fptr);
               }
               
            first = FALSE;
            for (cptr = buffer; (cptr - buffer) < (int)temp; cptr++) {
               ch = *cptr;
               ARMulif_WriteByte(&osstate->coredesc,(ARMword)addr++,ch);
               }
            if (temp < upto) {
               ARMulif_SetReg(&osstate->coredesc,mode,0, (ARMword)(size - temp));
               osstate->ErrorNo = errno;
               DONE;
               }
            size -= upto;
         }
         ARMulif_SetReg(&osstate->coredesc,mode,0, 0);
         osstate->ErrorNo = errno;
         LOG_FILE_OP(osstate, "  OK\n");
         break;
         }

      case SWI_Seek :
      case SYS_SEEK: {
         temp = GetNthWordArg(osstate,0,isangel) - 1 ;

         if ((int)temp == -1 || temp >= FOPEN_MAX || osstate->FileTable[temp] == 0) {
            osstate->ErrorNo = EBADF;
            ARMulif_SetReg(&osstate->coredesc, mode, 0, (ARMword)-1L);
            DONE;
            }
         fptr = osstate->FileTable[temp];
         ARMulif_SetReg(&osstate->coredesc,mode,0, fseek(fptr,(long)GetNthWordArg(osstate,1,isangel),SEEK_SET));
         osstate->ErrorNo = errno;
         break;
         }

      case SWI_Flen :
      case SYS_FLEN: {
         temp = GetNthWordArg(osstate,0,isangel) - 1 ;
         if ((int)temp == -1 || temp >= FOPEN_MAX || osstate->FileTable[temp] == 0) {
            osstate->ErrorNo = EBADF;
            ARMulif_SetReg(&osstate->coredesc, mode, 0, (ARMword)-1L);
            DONE;
            }
         fptr = osstate->FileTable[temp];
         addr = (ARMword)ftell(fptr);
         if (fseek(fptr,0L,SEEK_END) < 0)
            ARMulif_SetReg(&osstate->coredesc, mode, 0, (ARMword)-1L);
         else {
            ARMulif_SetReg(&osstate->coredesc,mode,0, (ARMword)ftell(fptr));
            (void)fseek(fptr,addr,SEEK_SET);
            }
         osstate->ErrorNo = errno;
         break;
         }   

      case SWI_IsTTY :
      case SYS_ISTTY:
         temp = GetNthWordArg(osstate,0,isangel) - 1;
         if ((int)temp == -1 || temp >= FOPEN_MAX || 
             osstate->FileTable[temp] == 0) 
         {
             osstate->ErrorNo = EBADF;
             ARMulif_SetReg(&osstate->coredesc, mode, 0, (ARMword)-1L);
             DONE;
         }
         fptr = osstate->FileTable[temp];
         ARMulif_SetReg(&osstate->coredesc,mode,0, (fptr == stdin || fptr == stdout));
          /* 0 for a file, 1 for a stdio stream ( stdin/stdout ) */
         osstate->ErrorNo = errno;
         break;
     
      case SWI_TmpNam :
      case SYS_TMPNAM : {
         ARMword size = GetNthWordArg(osstate,2,isangel);
         addr = GetNthWordArg(osstate,0,isangel);
         temp = GetNthWordArg(osstate,1,isangel) & 0xff;
         ARMulif_SetReg(&osstate->coredesc,mode,0, 0); /*IDJ: default to sucess */
         if (osstate->tempnames[temp] == NULL) {
                        char const *tn = 
#  ifdef COMPILING_ON_UNIX
                           tempnam("/var/tmp/","")
#  else
                           _tempnam("C:/tmp","")
#  endif
                           ;
                        size_t asize = strlen(tn) + 1;
            if ((osstate->tempnames[temp] = (char *)calloc(1,asize)) == NULL) {
               ARMulif_SetReg(&osstate->coredesc, mode, 0, (ARMword)-1L);
               /* QQQ: Shouldn't this be RDIError_OutOfStore ? */
               DONE;
               }
            strncpy(osstate->tempnames[temp],tn,asize-1);
                 }
         cptr = osstate->tempnames[temp];

         assert(cptr != NULL);
         assert(strlen(cptr) > 0);

         if (1 + strlen(cptr) > size)
            ARMulif_SetReg(&osstate->coredesc, mode, 0, (ARMword)-1L);
         else
            do {
               ARMulif_WriteByte(&osstate->coredesc,addr++,*cptr);
               } while (*cptr++ != 0);
         osstate->ErrorNo = errno;
         break;
         }

      case SYS_HEAPINFO: {
         ARMword r1=ARMulif_GetReg(&osstate->coredesc,mode,1);
         ARMword base=ARMulif_ReadWord(&osstate->coredesc, r1);
         ARMulif_WriteWord(&osstate->coredesc, base, osstate->heap_base);
         ARMulif_WriteWord(&osstate->coredesc, base+4, osstate->heap_limit);
         ARMulif_WriteWord(&osstate->coredesc, base+8, osstate->stack_base);
         ARMulif_WriteWord(&osstate->coredesc, base+12, osstate->stack_limit);
         break;
         }

      case SWI_Exit :
         ARMulif_StopExecution(&osstate->coredesc, 0);
         break;

      case SWI_EnterOS :
         ARMulif_SetCPSR(&osstate->coredesc, (ARMulif_GetCPSR(&osstate->coredesc) & 0xffffffe0) | 0x13);
         break;

      case angel_SWIreason_ReportException:
         ARMulif_StopExecution(&osstate->coredesc, 0);
         break;

      case SYS_ELAPSED:
      {
         ARMword address = ARMulif_GetReg(&osstate->coredesc,mode,1);
         ARMTime now = ARMulif_Time(&osstate->coredesc);
         ARMulif_WriteWord(&osstate->coredesc, address, (ARMword)now);
         ARMulif_WriteWord(&osstate->coredesc, address + 4, 
                           (ARMword)(now >> 32));

#ifdef VERBOSE_SYS_ELAPSED
         printf("Semihost:SYS_ELAPSED addr:%08lx\n",(unsigned long)address);
#endif
         ARMulif_SetReg(&osstate->coredesc,mode,0, 0);
         /* And leave R0 alone, rather than changing it to -1 */
      }
         break;
        
      case SYS_TICKFREQ:
          {  ARMword t = (ARMword)ARMulif_GetCoreClockFreq(&osstate->coredesc);
          /* Ticks/second reflects CPUSPEED. */
          ARMulif_SetReg(&osstate->coredesc, mode, 0, t ? t :  (ARMword)-1L);
          }
         break;

      case SWI_GenerateError:
        /*
         * On entry r0 points to an error block (containing a 32-bit number,
         * followed by a zero-terminated error string); r1 points to a 17-word
         * block containing the values of the ARM CPU registers at the instant
         * the error occured (the 17th word contains the PSR).
         */
         ARMulif_StopExecution(&osstate->coredesc,9) ;
         break;

         
   case angel_SWIreason_EnterSVC:
       ARMulif_SetCPSR(&osstate->coredesc, 
                      (ARMulif_GetCPSR(&osstate->coredesc) & 0xffffffe0) | 
                      0xD3);
       
       mode = ARMulif_GetCPSR(&osstate->coredesc) & 0x1f;

       ARMulif_SetReg(&osstate->coredesc,mode,0, 0);     
       break;

      case SWI_InstallHandler:
      default :
          if (isangel)
          {
              if (osstate->os_HaltOnUnknownAngelSwi)
              {
                  Hostif_ConsolePrint(osstate->hostif, "ERROR: "
                      "Untrapped semihosting SWI reason code 0x%08x\n", instr);
                  ARMulif_StopExecution(&osstate->coredesc, 
                                        RDIError_SoftwareInterrupt);
              }
              else
              {
                  ARMulif_SetReg(&osstate->coredesc,mode,0, -1L);
              }
              DONE;
          }
          else if (osstate->ExitSwisEnabled &&
                   handleDemonExitSWIs(osstate, instr & ~ 0x100 /*number*/))
          {
              DONE;
          }
          ARMulif_RaiseEvent(&osstate->coredesc,DebugEvent_OutOfSemihost,0,0);

         goto tryNext;
      }
   DONE; /* so break; can do this! */
}





/* ---------- VECTOR-INSTALLATION ----------- */






/*
 * Vectors
 */

#define ARMul_ResetV          ARM_V_RESET
#define ARMul_UndefinedInstrV ARM_V_UNDEF
#define ARMul_SWIV            ARM_V_SWI
#define ARMul_PrefetchAbortV  ARM_V_PREFETCH
#define ARMul_DataAbortV      ARM_V_ABORT
#define ARMul_AddrExceptnV    ARM_V_ADDREXCEPT
#define ARMul_IRQV            ARM_V_IRQ
#define ARMul_FIQV            ARM_V_FIQ

#define INSTRUCTION16SIZE 2
#define INSTRUCTION32SIZE 4


static ARMword softvectorcode[] =
{   /* basic: swi tidyexception + event;
     *        mov lr, pc;
     *        ldmia r11,{r11,pc};
     *        swi generateexception + event
     */
  0xef000090, 0xe1a0e00f, 0xe89b8800, 0xef000080, /*Reset*/
  0xef000091, 0xe1a0e00f, 0xe89b8800, 0xef000081, /*Undef*/
  0xef000092, 0xe1a0e00f, 0xe89b8800, 0xef000082, /*SWI  */
  0xef000093, 0xe1a0e00f, 0xe89b8800, 0xef000083, /*Prefetch abort*/
  0xef000094, 0xe1a0e00f, 0xe89b8800, 0xef000084, /*Data abort*/
  0xef000095, 0xe1a0e00f, 0xe89b8800, 0xef000085, /*Address exception*/
  0xef000096, 0xe1a0e00f, 0xe89b8800, 0xef000086, /*IRQ*/
  0xef000097, 0xe1a0e00f, 0xe89b8800, 0xef000087, /*FIQ*/
  0xef000098, 0xe1a0e00f, 0xe89b8800, 0xef000088, /*Error*/
  0xe1a0f00e /* default handler */
};


static void Demon_SetSPs(OSState *osstate)
{
  /* Set up the stack pointers */
  ARMulif_SetReg(&osstate->coredesc, CURRENTMODE, 13, osstate->map.sp.svc);
  ARMulif_SetReg(&osstate->coredesc,(unsigned)SVC32MODE,13,osstate->map.sp.svc);
  ARMulif_SetReg(&osstate->coredesc,(unsigned)ABORT32MODE,13,osstate->map.sp.abt);
  ARMulif_SetReg(&osstate->coredesc,(unsigned)UNDEF32MODE,13,osstate->map.sp.undef);
  ARMulif_SetReg(&osstate->coredesc,(unsigned)IRQ32MODE,13,osstate->map.sp.irq);
  ARMulif_SetReg(&osstate->coredesc,(unsigned)FIQ32MODE,13,osstate->map.sp.fiq);
}


static void Demon_WriteVecs(OSState *osstate)
{
  unsigned i;
  /* load pc from soft vector */
  ARMword instr = 0xe59ff000 | (osstate->map.soft_vectors - 8);

#ifdef VERBOSE_INIT
  printf(" <Demon_WriteVecs> \n");
#endif
  /* dont write over a ROM or flash image loaded at meminit by integrator */
  if (!ToolConf_DLookupBool(osstate->config,(tag_t)"INTE_NOCLOBBER",FALSE)) {
    /* write hardware vectors */
    for (i = ARMul_ResetV; i <= ARMul_FIQV; i += 4)
        ARMulif_WriteWord(&osstate->coredesc, i, instr);

    /* Install the soft vector code */
    for (i = ARMul_ResetV ; i <= ARMul_FIQV + 4 ; i += 4) {
      ARMulif_WriteWord(&osstate->coredesc, osstate->map.soft_vectors + i,
                      osstate->map.soft_vec_code + i * 4);
      ARMulif_WriteWord(&osstate->coredesc, osstate->map.handlers + 2*i + 4L,
                      osstate->map.soft_vec_code + sizeof(softvectorcode) - 4L);
    }
    for (i = 0 ; i < sizeof(softvectorcode) ; i += 4)
      ARMulif_WriteWord(&osstate->coredesc, osstate->map.soft_vec_code + i,
                        softvectorcode[i/4]);
  }
#ifdef FPE_IS_IN_OSSTATE
  if (osstate->install_fpe)
      ARMul_FPEInstall(osstate->state); /* OUCH ! */
#endif
}

static unsigned Angel_ConfigEvents(void *handle, void *data)
{
    OSState *osstate = handle;
    ARMul_Event *evt = data;
    unsigned boottype = evt->data1;
    unsigned highvec = evt->data1;
    if (evt->event == ConfigEvent_HighVecChanged)
    {
        if(highvec && osstate->DemonEnabled)
        {
            Hostif_DebugPrint(osstate->hostif,"High Exception Vectors enabled  - FPE  will not work\n");
        }
        return FALSE;
    }
    if (evt->event != ConfigEvent_Reset)
        return FALSE;

    Demon_SetSPs(osstate);
    if (osstate->Set_Endian)
    {
       ARMword value[8]; /* XScale returns 2 words - allow elbow room */
#ifdef VERBOSE_SET_ENDIAN
       printf("Semihost:Set_Endian:%u\n",(unsigned)osstate->Debugger_Bigend);
#endif
       if (ARMulif_CPRead(&osstate->coredesc,15,1,&value[0]) == RDIError_NoError)
       {
            if (osstate->Debugger_Bigend)
                value[0] |= ARM_MMU_B /*0x80*/;
            else
                value[0] &= ~(uint32)ARM_MMU_B /*0x80*/;
            
            ARMulif_CPWrite(&osstate->coredesc,15,1,&value[0]);
       }
       else
       {
            ARMulif_SetSignal(&osstate->coredesc, RDIPropID_ARMSignal_BigEnd, 
                              osstate->Debugger_Bigend);
       }
    }

#ifdef VERBOSE_ANGEL_EVENTS
    printf("\n *** Angel_Events type=%u ***\n", boottype);
#endif
  

  /* Only write memory during cold-boot, because warm-boot happens
   * after a code-image is loaded. */
  if (boottype & 1)
      return FALSE;


  return FALSE;
}


















/*--- <RDI STUFF> ---*/
/*--------------- RDI STUFF ------------- */

static int OS_OpenAgent(RDI_AgentHandle *agent, unsigned type,
                         RDI_ConfigPointer config,
                         struct RDI_HostosInterface const *hostif,
                         RDI_DbgState *dbg_state)
{
    return OS_Init(agent, type, config, hostif,dbg_state);
}


static int OS_CloseAgent(RDI_AgentHandle agent)
{
    return OS_Exit(agent);
}


static RDI_NameList const *OS_CPUNames(RDI_AgentHandle agent)
{
    (void)agent;
    
    return (RDI_NameList const *)NULL;
}


static int OSAgent_Info(void *handle, unsigned subcode,
                      ARMword *arg1, ARMword *arg2)
{
    OS_State *state = (OS_State *)handle;

    if (state == NULL)
        return RDIError_NotInitialised;

#ifdef VERBOSE_OS_INFO
    Hostif_DebugPrint(state->hostif,"OSAgent_Info subcode:0x%08lx=%lu\n",
                      (unsigned long)subcode,(unsigned long)subcode);
#endif

    switch (subcode)
    {
    case RDIInfo_AllLoaded:
        { uint32 flags = state->DemonEnabled | state->AngelEnabled * 2 |
              state->ExitSwisEnabled * 4;
        if (state->DemonEnabled || state->ExitSwisEnabled)
        {
            Demon_WriteVecs(state);
        }
#ifdef VERBOSE_EVENTS
        printf("*** DSemihost->ConfigEvent_VectorsLoaded %08lx ***\n", 
               (unsigned long)flags);
#endif
        ARMulif_RaiseEvent(&state->coredesc,ConfigEvent_VectorsLoaded,flags,0);
        return RDIError_NoError;
        }
    case RDIInfo_Modules:
          /*
           * DRS 2000-04-07
           * OS doesn't expose any modules yet.
           */
          *((unsigned *)arg1) = 0;
          (void)arg2;
          return RDIError_NoError;
    default:
          return RDIError_UnimplementedMessage;
    }
}



/*-------------- endof RDI stuff ----------------*/



/*--- <SORDI STUFF> ---*/


#define SORDI_DLL_DESCRIPTION_STRING "SemiHosting operating system"
#define SORDI_RDI_PROCVEC OS_AgentRDI
#include "perip_sordi.h"

#include "perip_rdi_agent.h"
    IMPLEMENT_AGENT_PROCVEC_NOEXE(OS)

/*--- </> ---*/
