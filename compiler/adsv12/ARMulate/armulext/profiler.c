/* profiler.c - module to do ARMsd profiling
 * Copyright (c) 1996-1998  Advanced RISC Machines Limited. 
 * Copyright (C) 1998-2001  ARM Limited.
 * All Rights Reserved.
 *
 * RCS $Revision: 1.6.4.6 $
 * Checkin $Date: 2001/08/28 13:01:28 $
 * Revising $Author: dsinclai $
 */

#define MODEL_NAME Profiler

#include <stdlib.h>
#include <string.h>


#include "minperip.h"

#include "armul_callbackid.h"
#include "rdi_prop.h"  /* for RDIProperty_SetAsNumeric */


#if 0 /* ndef NDEBUG */
# if 1
#  define VERBOSE
# else
#  define VERBOSE_EVENT
# endif
#endif


#define ARMulCnf_EventMask (tag_t)"EVENTMASK"
#define ARMulCnf_Event (tag_t)"EVENT"
#define ARMulCnf_Verbose (tag_t)"VERBOSE"
#define ARMulCnf_Type (tag_t)"TYPE"
#define ARMulCnf_EventWord (tag_t)"EVENTWORD"

BEGIN_STATE_DECL(Profiler)
  ARMword mapsize;
  ARMword *map;
  ARMword *counts;
  enum {
    PROFILE_usec, PROFILE_cycle,
    PROFILE_event1, PROFILE_event2, PROFILE_eventpc
    } prof_type;
  void *handle;
  union {
    struct {
      unsigned long rate;
      unsigned int ignore;
    } usec;
    struct {
      ARMTime last;
      unsigned long interval, countdown;
    } cycle;
    struct {
      unsigned long mask, value;
    } event;
  } x;
  bool verbose;
END_STATE_DECL(Profiler)

static void count(ProfilerState *p,ARMword pc,unsigned cnt)
{
  ARMword *map = p->map;
  ARMword size = p->mapsize;
  ARMword low = 0, high = size-1;

  /* binary search */
  if (map[low] <= pc && pc < map[high]) {
    ARMword i;
    for (;;) {
      i = (high + low) / 2;
      if (pc >= map[i]) {
        if (pc < map[i+1]) {
          i++; break;
        } else
          low = i;
      } else if (pc >= map[i-1])
        break;
      else
        high = i;
    }
    p->counts[i-1]+=cnt;
  }
}

/*
 * Microsecond-based scheduler
 * This is an ARMul_EventProc
 */
static void ProfileMicrosecond(void *handle)
{
  ProfilerState *p = (ProfilerState *)handle;

  if (p->verbose)
      Hostif_DebugPrint(p->hostif,"ProfileMicrosecond\n");


  if (!p->x.usec.ignore) {
    (void)ARMulif_ScheduleNewTimedCallback(
        &p->coredesc, ProfileMicrosecond, handle, 
        p->x.usec.rate + ARMulif_Time(&p->coredesc), 0);
    count(p, ARMulif_GetPC(&p->coredesc), 1);
  } else {
    p->handle = handle;
    p->x.usec.ignore = FALSE;
  }

  return ;
}

/*
 * Cycles-based profiler
 * This is an hourglass function
 */
static void ProfileCycles(void *handle, ARMword pc, ARMword instr)
{
  ProfilerState *p = (ProfilerState *)handle;
#ifdef VERBOSE_EVENT
  if (p->verbose)
      Hostif_ConsolePrint(p->hostif, "ProfileCycles pc:%08lx\n",
                          (unsigned long)pc);
#endif
  
#ifndef COMPLEX_HOURGLASS
  if (--p->x.cycle.countdown == 0)
  {
#endif
      ARMTime t = ARMulif_Time(&p->coredesc);
      unsigned long delta = (unsigned long)(t - p->x.cycle.last);
      p->x.cycle.last = t;

      count(p, pc, delta);

#ifndef COMPLEX_HOURGLASS
      p->x.cycle.countdown = p->x.cycle.interval;
  }
#endif

  UNUSEDARG(instr);
}

/*
 * Events based profiler
 * This is an EventProc function
 */
static unsigned ProfileEvents(
    void *handle, void *data)
{
    ARMul_Event *evt = (ARMul_Event *)data;
  ProfilerState *p=(ProfilerState *)handle;
  ARMword addr;
  unsigned int event = evt->event;
  ARMword addr1 = evt->data1; ARMword addr2 = evt->data2;

  /*
   * profiling function that lets you profile by event, e.g. cache misses
   */
  
  if ((event & p->x.event.mask)==p->x.event.value)
  {
  
      switch (p->prof_type) {
      case PROFILE_event1: addr=addr1; break;
      case PROFILE_event2: addr=addr2; break;
      default:
      case PROFILE_eventpc: addr=ARMulif_GetPC(&p->coredesc); break;
      }

      count(p,addr,1);
  }

  return 0; /* Carry on */
}

static int RDI_info(void *handle, unsigned type, ARMword *arg1, ARMword *arg2)
{
    ProfilerState *p=(ProfilerState *)handle;

#ifdef VERBOSE
    if (p->verbose)
        Hostif_DebugPrint(p->hostif,"RDI_info %08lx\n", (unsigned long)type);
#endif

  switch (type) {
  case RDIInfo_Target:
#ifdef VERBOSE
      if (p->verbose)
          Hostif_DebugPrint(p->hostif, "Info_Target\n");
#endif
    /* Add the "we can do profiling" capability */
    *arg1 = (*arg1) | RDITarget_CanProfile;
    /* return Unimplemented because this should be passed on */
    return RDIError_UnimplementedMessage;

  case RDIProfile_Stop:
#ifdef VERBOSE
      if (p->verbose)
          Hostif_DebugPrint(p->hostif, "Profile_Stop\n");
#endif
    if (p->handle != NULL) switch (p->prof_type) {
    case PROFILE_usec:
      p->x.usec.ignore = TRUE;
      break;

    case PROFILE_cycle:
      ARMulif_RemoveHourglass(&p->coredesc, p->handle);
      p->handle = NULL;
      break;

    case PROFILE_event1:
    case PROFILE_event2:
    case PROFILE_eventpc:
        ARMulif_RemoveEventHandler(&p->coredesc, p->handle);
      p->handle = NULL;
      break;
    }
    return RDIError_NoError;
      
  case RDIProfile_Start:
    if (p->handle == NULL) switch (p->prof_type) {
    case PROFILE_usec: {
      /* ARG1 is in microseconds. Convert this to bus-cycles. */
      double rate = (double)*arg1 * 10000.0;
      double cycle_length = (double)ARMulif_GetCycleLength(&p->coredesc);


      p->x.usec.ignore = FALSE;
      if (cycle_length != 0.0) {
        p->x.usec.rate = (unsigned long)(rate / cycle_length);
      } else {
        p->x.usec.rate = *arg1;
      }
      (void) ARMulif_ScheduleNewTimedCallback(
          &p->coredesc, ProfileMicrosecond, (void *)p,
          p->x.usec.rate + ARMulif_Time(&p->coredesc),0
          );

#ifdef VERBOSE
      if (p->verbose)
          Hostif_DebugPrint(p->hostif, "Profile_Start, microsecond "
                            "rate:%08lx\n", (unsigned long)p->x.usec.rate);
#endif
    }
      break;

    case PROFILE_cycle:
      p->handle = ARMulif_InstallHourglass(&p->coredesc, 
                                                           (armul_Hourglass*)ProfileCycles, p); /* !TIP!*/
      p->x.cycle.interval = *arg1;
      p->x.cycle.last = ARMulif_Time(&p->coredesc);

      /* We're called every cycle, so count-down ourselves. */
      p->x.cycle.countdown = p->x.cycle.interval;
#ifdef VERBOSE
      if (p->verbose)
          Hostif_DebugPrint(p->hostif, "Profile_Start, cycle, interval:%lu\n",
                            (unsigned long)p->x.cycle.interval);
#endif
      break;

    case PROFILE_event1:
    case PROFILE_event2:
    case PROFILE_eventpc:
#ifdef VERBOSE
      if (p->verbose)
          Hostif_DebugPrint(p->hostif, "Profile_Start, event\n");
#endif
        p->handle = ARMulif_InstallEventHandler(&p->coredesc, 
                                                ~(uint32)0, 
                                                ProfileEvents, p);
      break;
    }
    return RDIError_NoError;

  case RDIProfile_WriteMap:
    { RDI_ProfileMap *map = (RDI_ProfileMap *)arg1;
      size_t bytes = (size_t)map->len * sizeof(ARMword);

#ifdef VERBOSE
      if (p->verbose)
      {
          unsigned i;
          Hostif_DebugPrint(p->hostif,"RDIProfile_WriteMap len=%u [words]\n",
                            (unsigned)map->len);
          for (i=0; i < map->len; i++)
          {
              Hostif_DebugPrint(p->hostif,"%08lx ",
                                (unsigned long)map->map[i]);
          }
          Hostif_DebugPrint(p->hostif,"\n");
      }
#endif


      if (p->mapsize != map->len) {
        if (p->mapsize != 0) {
          free(p->map);
          free(p->counts);
        }
        p->map = NULL;
      }
      if (p->map == NULL) {
        p->map = (ARMword *)malloc(bytes);
        p->counts = (ARMword *)malloc(bytes);
        if (p->map == NULL || p->counts == NULL)
          return RDIError_OutOfStore;
      }
      p->mapsize = map->len;
      memcpy(p->map, map->map, bytes);
      /* and fall through to clear counts */
    }

  case RDIProfile_ClearCounts:
#ifdef VERBOSE
      if (p->verbose)
          Hostif_DebugPrint(p->hostif,"RDIProfile_ClearMap\n");
#endif
    memset(p->counts, 0, (size_t)p->mapsize * sizeof(ARMword));
    return RDIError_NoError;

  case RDIProfile_ReadMap:
#ifdef VERBOSE
      if (p->verbose)
      {
          unsigned i, n = (unsigned)*arg1;
          Hostif_DebugPrint(p->hostif,"RDIProfile_ReadMap length %lu\n",
                            (unsigned long)n);
          for (i=0; i < n; i++)
          {
              Hostif_DebugPrint(p->hostif,"%08lx ",
                            (unsigned long)p->counts[i]);
          }
          Hostif_DebugPrint(p->hostif,"\n");
      }
#endif
    { ARMword len = *(ARMword *)arg1;
      memcpy(arg2, p->counts, (size_t)len * sizeof(ARMword));
      return RDIError_NoError;
    }



    case RDIProperty_SetAsNumeric:
    {
        unsigned PrID = (unsigned)arg1;
        if (arg2 == NULL)
            return RDIError_UnimplementedMessage;
        switch (PrID)
        {
        case RDIPropID_ARMulProp_Hostif:
            p->hostif = *(struct RDI_HostosInterface const **)arg2;
            return RDIError_UnimplementedMessage;
        default:
            return RDIError_UnimplementedMessage;
        }   
    }


  default:
    if (type & RDIInfo_CapabilityRequest) {
      switch (type & ~RDIInfo_CapabilityRequest) {
      case RDIProfile_Stop:           case RDIProfile_Start:
      case RDIProfile_WriteMap:       case RDIProfile_ClearCounts:
      case RDIProfile_ReadMap:
        return RDIError_NoError;
      }
    }
    break;
  }

  return RDIError_UnimplementedMessage;
}

/*
 * Reset handler -- to deal with ScheduleEvents being removed on
 * reset.
 */
static unsigned Profile_ConfigEvents(void *handle, void *data)
{
    ProfilerState *p = (ProfilerState *)handle;
    ARMul_Event *evt = data;

    if (evt->event == ConfigEvent_Reset)
    {
        if (p->handle != NULL) {
            switch (p->prof_type) {
            case PROFILE_usec:
                p->x.usec.ignore = FALSE;
                break;
                
            case PROFILE_cycle:
                ARMulif_RemoveHourglass(&p->coredesc, p->handle);
                break;

            case PROFILE_event1:
            case PROFILE_event2:
            case PROFILE_eventpc:
                ARMulif_RemoveEventHandler(&p->coredesc, p->handle);
                break;
            }
            p->handle = NULL;
        }
    }
    return 0; /* Continue */
}

BEGIN_INIT(Profiler)
{
  ProfilerState *p = state;
  const char *option;
  int verbose = ToolConf_DLookupBool(config, ARMulCnf_Verbose, FALSE);
  p->verbose = verbose;

  if (verbose)
      Hostif_PrettyPrint(hostif, config, ",\nProfiler ");
  else
      Hostif_PrettyPrint(hostif, config, ", Profiler");

  
  p->prof_type=PROFILE_usec;

  option=ToolConf_Lookup(config,ARMulCnf_Type);
  if (option) {
    if (ToolConf_Cmp(option,"MICROSECOND"))
      p->prof_type=PROFILE_usec;
    else if (ToolConf_Cmp(option,"CYCLE"))
      p->prof_type=PROFILE_cycle;
    else if (ToolConf_Cmp(option,"EVENT"))
      p->prof_type=PROFILE_eventpc;
    else
      Hostif_ConsolePrint(state->hostif,"\n\
Profiling type '%s' not understood (defaulting to \"MICROSECOND\").\n",option);
  }


  switch (p->prof_type) {
  case PROFILE_usec:
    if (verbose) Hostif_PrettyPrint(state->hostif,state->config,"(Microsecond based)");
    p->x.usec.rate = 1;
    break;

  case PROFILE_cycle:
    if (verbose) Hostif_PrettyPrint(state->hostif, state->config,"(Cycle based)");
    break;

  case PROFILE_eventpc:
    if (verbose) Hostif_PrettyPrint(state->hostif,state->config,"(Event");
    option=ToolConf_Lookup(config, ARMulCnf_EventWord);
    if (option) {
      if (ToolConf_Cmp(option,"WORD1") || ToolConf_Cmp(option,"1"))
        p->prof_type=PROFILE_event1;
      else if (ToolConf_Cmp(option,"WORD2") || ToolConf_Cmp(option,"2"))
        p->prof_type=PROFILE_event2;
      else if (ToolConf_Cmp(option,"PC"))
        p->prof_type=PROFILE_eventpc;
      else
        Hostif_ConsolePrint(state->hostif,"\n\
Event profiling type '%s' not understood (defaulting to \"WORD1\").\n",option);
    }
    option=ToolConf_Lookup(config, ARMulCnf_EventMask);

  
    if (option && option[0]) {
      char *q;
      p->x.event.mask = strtoul(option, &q, 0);
      p->x.event.value = q ? strtoul(q+1, NULL, 0) : p->x.event.mask;
      if (verbose) Hostif_PrettyPrint(state->hostif,state->config,
                                      " Mask 0x%08x-0x%08x",
                                      p->x.event.mask,p->x.event.value);
    } else {
      option=ToolConf_Lookup(config, ARMulCnf_Event);
      if (option && option[0]) {
        p->x.event.mask=(unsigned int)~0;
        p->x.event.value=strtoul(option,NULL,0);
        if (verbose) Hostif_PrettyPrint(state->hostif,state->config,
                                        " 0x%08x",p->x.event.value);
      } else {
        p->x.event.mask=p->x.event.value=0;
        if (verbose) Hostif_ConsolePrint(state->hostif,"s");
      }
    }
    break;
  }

  if (verbose) Hostif_ConsolePrint(state->hostif, "\n");

  p->handle=NULL;
  p->mapsize=0;
  p->map=NULL;
  p->counts=NULL;

  ARMulif_InstallUnkRDIInfoHandler(&state->coredesc,RDI_info,p);
  ARMulif_InstallEventHandler(&state->coredesc,
                              ConfigEventSel,
                              Profile_ConfigEvents, p);
}
END_INIT(Profiler)


BEGIN_EXIT(Profiler)
END_EXIT(Profiler)


/*--- <SORDI STUFF> ---*/


#define SORDI_DLL_DESCRIPTION_STRING "Profiler (a Utility)"
#define SORDI_RDI_PROCVEC Profiler_AgentRDI
#include "perip_sordi.h"

#include "perip_rdi_agent.h"
    IMPLEMENT_AGENT_PROCS_NOEXE_NOMODULE(Profiler)
    IMPLEMENT_AGENT_PROCVEC_NOEXE(Profiler)

/*--- </> ---*/



/* EOF profiler.c */






