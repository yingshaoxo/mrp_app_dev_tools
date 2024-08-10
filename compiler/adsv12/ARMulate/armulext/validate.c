/* validate.c - ARM validation suite co-processxor and OS. )
 *
 * Copyright (C) Advanced RISC Machines Limited, 1995-1998 .
 * Copyright (C) 1998-2001  ARM Limited.
 * All rights reserved.
 *
 * RCS $Revision: 1.3.4.8 $
 * Checkin $Date: 2001/08/24 12:57:48 $
 * Revising $Author: lmacgreg $
 */

#include "minperip.h"


#define ARMSignal_IRQ RDIPropID_ARMSignal_IRQ
#define ARMSignal_FIQ RDIPropID_ARMSignal_FIQ
#define ARMSignal_SE RDIPropID_ARMSignal_SE




#ifndef BITS
# define BITS(m,n) ( (ARMword)(instr<<(31-(n))) >> ((31-(n))+(m)) )
#endif

#ifndef BIT
# define BIT(n) ( (instr >> n) & 1 )
#endif
typedef RDI_Error ARMul_Error;



/* Software model of the Validation Suite Coprocessors. Eight    */
/* Coprocessors are defined in the "CPU Validation Test Bench"   */
/* ARM-100-0019-A01. Of these all but CP_PRIVELEGED is supported */

/* ARM7 based models use memory cycle based event scheduling,  */
/* and ARM9 based models use core cycle timings.               */


/* Coprocessor names */
#define CP_BOUNCE_D     0
#define CP_BUSYWAIT_D   1
#define CP_BOUNCE_E     2
#define CP_BUSYWAIT_E   3
#define CP_BUSYWAIT     4
#define CP_TRICKBOX     5
#define CP_PRIVELEGED   6
#define CP_STANDARD     7
#define CP_DTL          8

/* Default Busy_wait count for CP_BUSYWAIT */
#define CP1_WAIT_COUNT  1
#define CP2_WAIT_COUNT  1
#define CP3_WAIT_COUNT  2
#define CP4_WAIT_COUNT  1
#define CP5_WAIT_COUNT  0

typedef struct {
      ARMTime   ulBusyWaitStart;
      int       iBusyWaitDuration;
      int       iBusyWaiting;
} CPBusyWait;

BEGIN_STATE_DECL(Validate)
CPBusyWait CP1Busy /* ={0l,0, FALSE} */;
CPBusyWait CP2Busy /* ={0l,0, FALSE} */;
CPBusyWait CP3Busy /* ={0l,0, FALSE} */;
CPBusyWait CP4Busy /* ={0l,0, FALSE} */;
CPBusyWait CP5Busy /*={0l,0, FALSE} */;
CPBusyWait CP7Busy /*={0l,0, FALSE} */; 
CPBusyWait CP8Busy /*={0l,0, FALSE} */; 


/* All Coprocessors apart from 0 (no registers) and 5 share a common set of registers */ 

 ARMword ValCPStandardReg[16];
#define ValCP1Reg state->ValCPStandardReg
#define ValCP3Reg state->ValCPStandardReg
#define ValCP4Reg state->ValCPStandardReg
#define ValCP7Reg state->ValCPStandardReg
#define ValCP8Reg state->ValCPStandardReg
 ARMword ValCP5Reg[16];

  int ldc_regNum;
  unsigned ldc_words;
  unsigned ldc_wordsToGet;
  int stc_regNum;
  unsigned stc_words;
  unsigned stc_wordsToSend;
  bool bARM9_CoproIF, bARM9_Extensions;

  ARMTime FRTimer;

  ARMul_SchedulerMethod schedule;

END_STATE_DECL(Validate)


static void DoASE(void *handle);
static void DoAFIQ(void *handle);
static void DoAIRQ(void *handle);

static void DoAReset (void *handle);


/* Check whether the Coprocessor returned ARMul_LAST */
/* and return ARMul_INC instead for non ARM9 based   */
/* models.                                           */
static unsigned CheckLast(void *handle, int result)
{
    ValidateState *state=(ValidateState *)handle;

    if ((result == ARMul_CP_LAST) && !state->bARM9_CoproIF) {
        return ARMul_CP_INC;    
    }
    else {
        return result;
    }
}

/* Determine the correct number of busy cycles for the    */
/* Core - ARM9xxE cores always have one extra on external */
/* Coprocessor operations.                                */
static unsigned long CheckBusyCycles(void *handle, unsigned long busy_cycles)
{
    ValidateState *state=(ValidateState *)handle;

    if (state->bARM9_CoproIF && state->bARM9_Extensions)
        /* was (ARMul_Properties(state) & ARM_ARM9Extensions)*/ {
        /* ARM9xxES Core */
        return busy_cycles + 1;    
    }
    else {
        return busy_cycles;
    }
}


/* Checks whether the copro is waiting for a previous instruction */
/* to complete and also whether there is an interrupt.            */
static bool CheckBusyWait(unsigned type, CPBusyWait *CPBusy, 
                          unsigned long busy_cycles, 
                          ARMTime cycle_count, int *result)
{
  ARMTime finish;

  /* If we get an interrupt, regardless of busy wait status then exit */
  if (type == ARMul_CP_INTERRUPT) {
     CPBusy->iBusyWaiting = FALSE; /* Clear busy waiting flag */
     *result = ARMul_CP_DONE;         /* We've been interrupted */
     return TRUE; 
  }

  /* Is this CP currently busy waiting? */
  if (CPBusy->iBusyWaiting) {

    /* Yes it might be, check times */
    finish = CPBusy->ulBusyWaitStart + CPBusy->iBusyWaitDuration;
    if( cycle_count < finish ) {
        /* Yes it is busy waiting still, so return ARMul_CP_BUSY*/
        *result = ARMul_CP_BUSY;
        return TRUE;
    }
    else {
        /* Assert that we aren't busy waiting any more */
        CPBusy->iBusyWaiting = FALSE;
        *result = ARMul_CP_FIRST;
    }
  }
  else if (type == ARMul_CP_FIRST) {
    /* Start a new Busy Wait */
    CPBusy->iBusyWaiting = TRUE;
    CPBusy->ulBusyWaitStart = cycle_count;
    CPBusy->iBusyWaitDuration = busy_cycles;
    *result = ARMul_CP_BUSY;
    return TRUE;
  }
  return FALSE;

}

/* Code to get the current cycle count total, either */ 
/* core cycle type armulators (ARM9) or others.
 * 
 * Since we are a coprocessor, we'd actually like to use
 * the number of clocks on the coprocessor-bus.
 */
static ARMTime GetCycleCount(void *handle)
{
  ValidateState *state=(ValidateState *)handle;
  return ARMulif_Time(&state->coredesc);

}


/* Bouncing Coprocessors (CP0 and CP2) */
static int CP0CDP(void *handle, int type, ARMword instr, ARMword *dummy)
{
    UNUSEDARG(handle);  UNUSEDARG(type);
    UNUSEDARG(instr); (void)dummy;
    
    return ARMul_CP_CANT;
}

static int CP0LDCMCR(void *handle, int type, ARMword instr, ARMword *data)
{
    UNUSEDARG(handle);
    UNUSEDARG(type);
    UNUSEDARG(instr);
    UNUSEDARG(data);    
    return ARMul_CP_CANT;
}


static int CP0STCMRC(void *handle, int type, ARMword instr, ARMword *data)
{
    UNUSEDARG(handle);
    UNUSEDARG(type);
    UNUSEDARG(instr);
    UNUSEDARG(data);    
    return ARMul_CP_CANT;
}

static int CP2CDP(void *handle, int type, ARMword instr, ARMword *dummy)
{
    ValidateState *state=(ValidateState *)handle;
    ARMTime cycle_count = GetCycleCount(handle);
    int result;
    (void)instr;
    (void)dummy;

    if (CP2_WAIT_COUNT && CheckBusyWait(type, &state->CP2Busy, CP4_WAIT_COUNT, cycle_count, &result)) {
        return result; /* Either busy waiting or interrupted */
    }
    
    return ARMul_CP_CANT;
}

static int CP2LDCMCR(void *handle, int type, ARMword instr, ARMword *data)
{
    ValidateState *state=(ValidateState *)handle;
    ARMTime cycle_count = GetCycleCount(handle);
    int result;
    (void)instr;
    (void)data;

    if (CP2_WAIT_COUNT && CheckBusyWait(type, &state->CP2Busy, CP4_WAIT_COUNT, cycle_count, &result)) {
        return result; /* Either busy waiting or interrupted */
    }

    return ARMul_CP_CANT;
}


static int CP2STCMRC(void *handle, int type, ARMword instr, ARMword *data)
{
    ValidateState *state=(ValidateState *)handle;
    ARMTime cycle_count = GetCycleCount(handle);
    int result;
    (void)instr;
    (void)data;

    if (CP2_WAIT_COUNT && CheckBusyWait(type, &state->CP2Busy, CP4_WAIT_COUNT, cycle_count, &result)) {
        return result; /* Either busy waiting or interrupted */
    }

    return ARMul_CP_CANT;
}


static int DoCP4LDC(void *handle, int type, ARMword instr,ARMword data, unsigned long busy_cycles, ARMTime cycle_count)
{
  ValidateState *state=(ValidateState *)handle;

  static int regNum;
  static unsigned words;
  static unsigned wordsToGet;
  int result = type;
 
  if (busy_cycles && CheckBusyWait(type, &state->CP4Busy, busy_cycles, cycle_count, &result)) {
      return result; /* Either busy waiting or interrupted */
  }


  wordsToGet = BIT(22) ? 4 : 1;

  if (type == ARMul_CP_FIRST || result == ARMul_CP_FIRST) {
    regNum = BITS(12,15);  /* Get the register number */
    words = 0;       /* Reset the word fetch counter */
    if (wordsToGet == 1) {
        return ARMul_CP_LAST;
    }
    else {
        return ARMul_CP_INC;
    }
  }

  if (type == ARMul_CP_TRANSFER) {
    if (wordsToGet == 1) {
        return ARMul_CP_LAST;
    }
    else {
        return ARMul_CP_INC;
    }
  }

  if (BIT(22)) {
     /* Its a CPDTL instruction so 4 words to fetch */  
   
     /* Do the data access,  incrementing the register as we go */
     ValCP4Reg[regNum++] = data;
     regNum &= 0xf;

     /* If we've got enough words then return done, else request more ! */

     words++;
     if (words >= wordsToGet) {
        return ARMul_CP_DONE;
     }
     else if ((words+1) == wordsToGet) {
        return ARMul_CP_LAST;
     }
     else {
        return ARMul_CP_INC;
     }
  }
  else {
     /* NOT a long access so get just one word */ 
     ValCP4Reg[BITS(12,15)] = data;
     return ARMul_CP_DONE;
  }
}


static int CP4LDC(void *handle, int type, ARMword instr, ARMword *data)
{
  ARMTime cycle_count = GetCycleCount(handle);
  return CheckLast(handle, DoCP4LDC(handle, type, instr, data ? *data : 0, CheckBusyCycles(handle, CP4_WAIT_COUNT), cycle_count));
}




static int DoCP4STC(void *handle, int type, ARMword instr,
                    ARMword *data, unsigned long busy_cycles, ARMTime cycle_count)
{ 
  ValidateState *state=(ValidateState *)handle;
  static int regNum;
  static unsigned words;
  static unsigned wordsToSend;
  int result = type;
  
  if (busy_cycles && CheckBusyWait(type, &state->CP4Busy, busy_cycles, cycle_count, &result)) {
      return result; /* Either busy waiting or interrupted */
  }
 
  if (type == ARMul_CP_FIRST || result == ARMul_CP_FIRST) {
    regNum = BITS(12,15);  /* Get the register number */
    words = 0;       /* Reset the word fetch counter */
    wordsToSend = 1;  /* just to be safe */
    return ARMul_CP_INC;
  }

  if (BIT(22)) {
     /* it's a long access*/
     /* Do we want 2 or 4 words transferred? */
      
     /* Its a CPDTL instruction so 4 words */   
     wordsToSend = 4;

     /* Now do the data access */
     *data = ValCP4Reg[regNum++];

     /* Store the data in Register[regNum] and increment regNum for the
        next transfer */

     /* If we've got enough words then return done, else request more ! */

     if (++words >= wordsToSend)
       return ARMul_CP_DONE;
     else
       return ARMul_CP_INC;
  }
  else {
    /* get just one word */
    *data = ValCP4Reg[regNum]; 
    return ARMul_CP_DONE;
  }
}


static int CP4STC(void *handle, int type,
                       ARMword instr, ARMword *data)
{
  ARMTime cycle_count = GetCycleCount(handle);
  int result = CheckLast(handle, DoCP4STC(handle, type, instr, data, CheckBusyCycles(handle, CP4_WAIT_COUNT), 
                                          cycle_count));
  return result;
}


/* CP5 LDC instruction. Does 2 word accesses */
static unsigned DoCP5LDC(void *handle, unsigned type, ARMword instr, ARMword data, unsigned long busy_cycles, ARMTime cycle_count)
{
  ValidateState *state=(ValidateState *)handle;
  static int regNum;
  static unsigned words;
  static unsigned wordsToGet;
  int result = type;

  if (busy_cycles && CheckBusyWait(type, &state->CP5Busy, busy_cycles, cycle_count, &result)) {
      return result; /* Either busy waiting or interrupted */
  }


  wordsToGet = BIT(22) ? 2 : 1;
 
  if ((type == ARMul_CP_FIRST) || (result == ARMul_CP_FIRST)) {
    regNum = BITS(12,15);  /* Get the register number */
    words = 0;       /* Reset the word fetch counter */
    if (wordsToGet == 1) {
        return ARMul_CP_LAST;
    }
    else {
        return ARMul_CP_INC;
    }
  }

  if (type == ARMul_CP_TRANSFER) {
    if (wordsToGet == 1) {
        return ARMul_CP_LAST;
    }
    else {
        return ARMul_CP_INC;
    }
  }

  if (BIT(22)) {
     /* Its a LDCL instruction so 2 words to fetch */  
     /* Do the data access,  incrementing the register as we go */
     state->ValCP5Reg[regNum++] = data;
     regNum &= 0xf;

     /* If we've got enough words then return done, else request more ! */

     words++;
     if (words >= wordsToGet) {
        return ARMul_CP_DONE;
     }
     else if ((words+1) == wordsToGet) {
        return ARMul_CP_LAST;
     }
     else {
        return ARMul_CP_INC;
     }
  }
  else { 
     /* NOT a long access so get just one word */ 
     state->ValCP5Reg[BITS(12,15)] = data;
     return ARMul_CP_DONE;
  }
}


static int CP5LDC(void *handle, int type, ARMword instr, ARMword *data)
{
  ARMTime cycle_count = GetCycleCount(handle);
  return CheckLast(handle, DoCP5LDC(handle, type, instr, *data, CheckBusyCycles(handle, CP5_WAIT_COUNT), cycle_count));
}



static int DoCP5STC(void *handle, int type, ARMword instr, ARMword *data, unsigned long busy_cycles, ARMTime cycle_count)
{ 
  ValidateState *state=(ValidateState *)handle;
  static int regNum;
  static unsigned words;
  static unsigned wordsToSend;
  int result = type;

  if (busy_cycles && CheckBusyWait(type, &state->CP5Busy, busy_cycles, cycle_count, &result)) {
      return result; /* Either busy waiting or interrupted */
  }

  if ((type == ARMul_CP_FIRST) || (result == ARMul_CP_FIRST)) {
    regNum = BITS(12,15);  /* Get the register number */
    words = 0;       /* Reset the word fetch counter */
    wordsToSend = 1;  /* just to be safe */
    return ARMul_CP_INC;
  }

  if (BIT(22)) { 
     /* it's a long access*/
      
     /* Its a CPDTL instruction so 2 words */   
     wordsToSend = 2;

     /* Now do the data access */
     *data = state->ValCP5Reg[regNum++];
     /* Store the data in Register[regNum] and increment regNum for the
        next transfer */

     /* If we've got enough words then return done, else request more ! */

     if (++words >= wordsToSend)
       return ARMul_CP_DONE;
     else
       return ARMul_CP_INC;
  }
  else {
    /* get just one word */
    *data = state->ValCP5Reg[regNum]; 
    return ARMul_CP_DONE;
  }
}


static int CP5STC(void *handle, int type, ARMword instr, ARMword *data)
{
  ARMTime cycle_count = GetCycleCount(handle);
  return CheckLast(handle, DoCP5STC(handle, type, instr, data, CheckBusyCycles(handle, CP5_WAIT_COUNT), cycle_count));
}




/* CP7 LDC instruction. Does 4 word accesses */
static int DoCP7LDC(void *handle, int type, ARMword instr, ARMword data, unsigned long busy_cycles, ARMTime cycle_count)
{
  ValidateState *state=(ValidateState *)handle;
  static int regNum;
  static unsigned words;
  static unsigned wordsToGet;
  int result = type;

  if (busy_cycles && CheckBusyWait(type, &state->CP7Busy, busy_cycles, cycle_count, &result)) {
      return result; /* Either busy waiting or interrupted */
  }


  wordsToGet = BIT(22) ? 4 : 1;

  if ((type == ARMul_CP_FIRST) || (result == ARMul_CP_FIRST)) {
    regNum = BITS(12,15);  /* Get the register number */
    words = 0;       /* Reset the word fetch counter */
    if (wordsToGet == 1) {
        return ARMul_CP_LAST;
    }
    else {
        return ARMul_CP_INC;
    }
  }
  else if (type == ARMul_CP_TRANSFER) {
    if (wordsToGet == 1) {
        return ARMul_CP_LAST;
    }
    else {
        return ARMul_CP_INC;
    }
  }

  if (BIT(22)) {       
     /* Its a LDCL instruction so 4 words to fetch */  
     /* Do the data access,  incrementing the register as we go */
     ValCP7Reg[regNum++] = data;
     regNum &= 0xf;

     /* If we've got enough words then return done, else request more ! */

     words ++;
     
     if (words >= wordsToGet) {
         return ARMul_CP_DONE;
     }
     else if ((words+1) == wordsToGet) {
         return ARMul_CP_LAST;
     }
     else {
         return ARMul_CP_INC;
     }
  }
  else { 
     /* NOT a long access so get just one word */ 
      ValCP7Reg[BITS(12,15)] = data;
      return ARMul_CP_DONE;
  }
}

static int CP7LDC(void *handle, int type, ARMword instr, ARMword *data)
{
  ARMTime cycle_count = GetCycleCount(handle);
  return CheckLast(handle, DoCP7LDC(handle, type, instr, *data, CheckBusyCycles(handle, 0), cycle_count));
}

/* CP1 and CP3 use the same logic as CP7 but add extra busy-wait cycles */

static int CP3LDC(void *handle, int type, ARMword instr, ARMword *data)
{
  ARMTime cycle_count = GetCycleCount(handle);
  return CheckLast(handle, DoCP7LDC(handle, type, instr, *data, CheckBusyCycles(handle, CP3_WAIT_COUNT), cycle_count));
}


static int CP1LDC(void *handle, int type, ARMword instr, ARMword *data)
{
  ARMTime cycle_count = GetCycleCount(handle);
  return CheckLast(handle, DoCP7LDC(handle, type, instr, *data, CheckBusyCycles(handle, CP1_WAIT_COUNT), cycle_count));
}


static int DoCP7STC(void *handle, int type, ARMword instr, ARMword *data, unsigned long busy_cycles, ARMTime cycle_count)
{ 
  ValidateState *state=(ValidateState *)handle;
  static int regNum;
  static unsigned words;
  static unsigned wordsToSend;
  int result = type;

  if (busy_cycles && CheckBusyWait(type, &state->CP7Busy, busy_cycles, cycle_count, &result)) {
      return result; /* Either busy waiting or interrupted */
  }
  
  if ((type == ARMul_CP_FIRST) || (result == ARMul_CP_FIRST)) {
    regNum = BITS(12,15);  /* Get the register number */
    words = 0;       /* Reset the word fetch counter */
    wordsToSend = 1;  /* just to be safe */
    return ARMul_CP_INC;
  }

  if (BIT(22)) { 
     /* it's a long access*/
      
     /* Its a STCL instruction so 4 words */   
     wordsToSend = 4;


     /* Now do the data access */
     *data = ValCP7Reg[regNum++];
     /* Store the data in Register[regNum] and increment regNum for the
        next transfer */

     /* If we've got enough words then return done, else request more ! */

     if (++words >= wordsToSend)
       return ARMul_CP_DONE;
     else
       return ARMul_CP_INC;
  }
  else {
    /* get just one word */
    *data = ValCP7Reg[regNum]; 
    return ARMul_CP_DONE;
  }
}


static int CP7STC(void *handle, int type, ARMword instr, ARMword *data)
{
  ARMTime cycle_count = GetCycleCount(handle);
  return CheckLast(handle, DoCP7STC(handle, type, instr, data, CheckBusyCycles(handle, 0), cycle_count));
}

/* CP1 and CP3 use the same logic as CP7 but add extra busy-wait cycles */

static int CP1STC(void *handle, int type, ARMword instr, ARMword *data)
{
  ARMTime cycle_count = GetCycleCount(handle);
  return CheckLast(handle, DoCP7STC(handle, type, instr, data, CheckBusyCycles(handle, CP1_WAIT_COUNT), cycle_count));
}

static int CP3STC(void *handle, int type, ARMword instr, ARMword *data)
{
  ARMTime cycle_count = GetCycleCount(handle);
  return CheckLast(handle, DoCP7STC(handle, type, instr, data, CheckBusyCycles(handle, CP3_WAIT_COUNT), cycle_count));
}


/* CP8 LDC instruction. Does 2 word accesses */
static int DoCP8LDC(void *handle, int type, ARMword instr, ARMword data, unsigned long busy_cycles, ARMTime cycle_count)
{
  ValidateState *state=(ValidateState *)handle;
  static int regNum;
  static unsigned words;
  static unsigned wordsToGet;
  int result = type;

  if (busy_cycles && CheckBusyWait(type, &state->CP8Busy, busy_cycles, cycle_count, &result)) {
      return result; /* Either busy waiting or interrupted */
  }


  wordsToGet = BIT(22) ? 2 : 1;

  if ((type == ARMul_CP_FIRST) || (result == ARMul_CP_FIRST)) {
    regNum = BITS(12,15);  /* Get the register number */
    words = 0;       /* Reset the word fetch counter */
    if (wordsToGet == 1) {
        return ARMul_CP_LAST;
    }
    else {
        return ARMul_CP_INC;
    }
  }
  else if (type == ARMul_CP_TRANSFER) {
    if (wordsToGet == 1) {
        return ARMul_CP_LAST;
    }
    else {
        return ARMul_CP_INC;
    }
  }

  if (BIT(22)) {       
     /* Its a LDCL instruction so 2 words to fetch */  
     /* Do the data access,  incrementing the register as we go */
     ValCP8Reg[regNum++] = data;
     regNum &= 0xf;

     /* If we've got enough words then return done, else request more ! */

     words ++;
     
     if (words >= wordsToGet) {
        return ARMul_CP_DONE;
     }
     else if ((words+1) == wordsToGet) {
        return ARMul_CP_LAST;
     }
     else {
         return ARMul_CP_INC;
     }
  }
  else { 
     /* NOT a long access so get just one word */ 
      ValCP8Reg[BITS(12,15)] = data;
     return ARMul_CP_DONE;
  }
}

static int CP8LDC(void *handle, int type, ARMword instr, ARMword *data)
{
  ARMTime cycle_count = GetCycleCount(handle);
  return CheckLast(handle, DoCP8LDC(handle, type, instr, *data, CheckBusyCycles(handle, 0), cycle_count));
}


static int DoCP8STC(void *handle, int type, ARMword instr, ARMword *data, unsigned long busy_cycles, ARMTime cycle_count)
{ 
  ValidateState *state=(ValidateState *)handle;
  static int regNum;
  static unsigned words;
  static unsigned wordsToSend;
  int result = type;

  if (busy_cycles && CheckBusyWait(type, &state->CP8Busy, busy_cycles, cycle_count, &result)) {
      return result; /* Either busy waiting or interrupted */
  }
  
  if ((type == ARMul_CP_FIRST) || (result == ARMul_CP_FIRST)) {
    regNum = BITS(12,15);  /* Get the register number */
    words = 0;       /* Reset the word fetch counter */
    wordsToSend = 1;  /* just to be safe */
    return ARMul_CP_INC;
  }

  if (BIT(22)) { 
     /* it's a long access*/
      
     /* Its a CPDTL instruction so 2 words */   
     wordsToSend = 2;


     /* Now do the data access */
     *data = ValCP8Reg[regNum++];
     /* Store the data in Register[regNum] and increment regNum for the
        next transfer */

     /* If we've got enough words then return done, else request more ! */

     if (++words >= wordsToSend)
       return ARMul_CP_DONE;
     else
       return ARMul_CP_INC;
  }
  else {
    /* get just one word */
    *data = ValCP8Reg[regNum]; 
    return ARMul_CP_DONE;
  }
}

static int CP8STC(void *handle, int type, ARMword instr, ARMword *data)
{
    ARMTime cycle_count = GetCycleCount(handle);
    return CheckLast(handle, DoCP8STC(handle, type, instr, data, CheckBusyCycles(handle, 0), cycle_count));
}


/* MRC instructions for CP 4,5, 7 and 8 -                                 */
/*   there are 8 versions to allow variations between CP's, with a single */
/*   function (DoMRC) which does all the stuff common to all MRC's        */

static int DoMRC(void *handle, int type, ARMword instr, ARMword *value,
                 ARMword reg_val, unsigned long busy_cycles, 
                 CPBusyWait *CPBusy, ARMTime cycle_count)
{
  int result = 0;
    
  UNUSEDARG(instr); (void)handle;
  
  if (busy_cycles && CheckBusyWait(type, CPBusy, busy_cycles, cycle_count, &result)) {
      return result; /* Either busy waiting or interrupted */
  }

  if (type == ARMul_CP_FIRST || result == ARMul_CP_FIRST) {
    *value = reg_val;  
    return ARMul_CP_DONE; /* Complete */
  } 

  return ARMul_CP_BUSY;
}

static int DoMRRC(void *handle, int type, ARMword instr, ARMword *value,
                  ARMword reg_val, ARMword reg_val2, 
                  unsigned long busy_cycles, 
                  CPBusyWait *CPBusy, ARMTime cycle_count)
{
  int result = 0;
    
  UNUSEDARG(instr); (void)handle;
  
  if (busy_cycles && CheckBusyWait(type, CPBusy, busy_cycles, cycle_count, &result)) {
      return result; /* Either busy waiting or interrupted */
  }

  if (type == ARMul_CP_FIRST || result == ARMul_CP_FIRST) {
    value[0] = reg_val;
    value[1] = reg_val2;
    return ARMul_CP_DONE; /* Complete */
  } 

  return ARMul_CP_BUSY;
}

static int CP1MRC(void *handle, int type, ARMword instr,ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  return DoMRC(handle, type, instr, value, ValCP1Reg[BITS(16,19)],
               CheckBusyCycles(handle, CP1_WAIT_COUNT), &state->CP1Busy, cycle_count);
}

static int CP3MRC(void *handle, int type, ARMword instr,ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  return DoMRC(handle, type, instr, value, ValCP3Reg[BITS(16,19)],
               CheckBusyCycles(handle, CP3_WAIT_COUNT), &state->CP3Busy, cycle_count);
}

static int CP4MRC(void *handle, int type, ARMword instr,ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  return DoMRC(handle, type, instr, value, ValCP4Reg[BITS(16,19)],
               CheckBusyCycles(handle, CP4_WAIT_COUNT), &state->CP4Busy, cycle_count);
}


static int CP1MRRC(void *handle, int type, ARMword instr, ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  ARMword reg = BITS(0,3);
  ARMword reg2;

  if(reg < 15)
      reg2 = reg + 1;
  else
      reg2 = 0;
  return DoMRRC(handle, type, instr, value, ValCP1Reg[reg], ValCP1Reg[reg2],
                CheckBusyCycles(handle, CP1_WAIT_COUNT), &state->CP1Busy, cycle_count);
}

static int CP3MRRC(void *handle, int type, ARMword instr, ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  ARMword reg = BITS(0,3);
  ARMword reg2;

  if(reg < 15)
      reg2 = reg + 1;
  else
      reg2 = 0;
  return DoMRRC(handle, type, instr, value, ValCP3Reg[reg], ValCP3Reg[reg2],
                CheckBusyCycles(handle, CP3_WAIT_COUNT), &state->CP3Busy, cycle_count);
}

static int CP4MRRC(void *handle, int type, ARMword instr, ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  ARMword reg = BITS(0,3);
  ARMword reg2;

  if(reg < 15)
      reg2 = reg + 1;
  else
      reg2 = 0;
  return DoMRRC(handle, type, instr, value, ValCP4Reg[reg], ValCP4Reg[reg2],
                CheckBusyCycles(handle, CP4_WAIT_COUNT), &state->CP4Busy, cycle_count);
}

static int CP5MRC(void *handle, int type, ARMword instr,ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  return DoMRC(handle, type, instr, value, state->ValCP5Reg[BITS(16,19)],
               CheckBusyCycles(handle, CP5_WAIT_COUNT), &state->CP5Busy, cycle_count);
}

static int CP7MRC(void *handle, int type, ARMword instr,ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  return DoMRC(handle, type, instr, value, ValCP7Reg[BITS(16,19)],
               CheckBusyCycles(handle, 0), &state->CP7Busy, cycle_count);
}

static int CP7MRRC(void *handle, int type, ARMword instr, ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  ARMword reg = BITS(0,3);
  ARMword reg2;

  if(reg < 15)
      reg2 = reg + 1;
  else
      reg2 = 0;
  return DoMRRC(handle, type, instr, value, ValCP7Reg[reg], ValCP7Reg[reg2],
                CheckBusyCycles(handle, 1), &state->CP7Busy, cycle_count);
}


static int CP8MRC(void *handle, int type, ARMword instr,ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  return DoMRC(handle, type, instr, value, ValCP8Reg[BITS(16,19)], 
               CheckBusyCycles(handle, 0), &state->CP8Busy, cycle_count);
}

static int CP8MRRC(void *handle, int type, ARMword instr, ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  ARMword reg = BITS(0,3);
  ARMword reg2;

  if(reg < 15)
      reg2 = reg + 1;
  else
      reg2 = 0;
  return DoMRRC(handle, type, instr, value, ValCP8Reg[reg], ValCP8Reg[reg2],
                CheckBusyCycles(handle, 1), &state->CP8Busy, cycle_count);
}


/* MCR instructions for CP 1,3,4,5 7 and 8 -                            */ 
/* there are 8 versions to allow variations between CP's, with a single */
/* function (DoMCR) which does all the stuff common to all MCRs         */

static int DoMCR(void *handle, int type, ARMword instr, ARMword value, 
                 ARMword *reg_val,
                 unsigned long busy_cycles, CPBusyWait *CPBusy, 
                 ARMTime cycle_count)
{
  int result;
  UNUSEDARG(instr); (void)handle;

  /* Is this CP currently busy waiting? */

  if (busy_cycles && CheckBusyWait(type, CPBusy, busy_cycles, cycle_count, &result)) {
      return result; /* Either busy waiting or interrupted */
  }

  if (type == ARMul_CP_FIRST || result == ARMul_CP_FIRST) {
     *reg_val = value;
     return ARMul_CP_DONE; /* Complete */
 } 
  
  return ARMul_CP_BUSY;
}

static int DoMCRR(void *handle, int type, ARMword instr, ARMword *value, 
                  ARMword *reg_val0, ARMword *reg_val1,
                  unsigned long busy_cycles, CPBusyWait *CPBusy, 
                  ARMTime cycle_count)
{
    int result;
    UNUSEDARG(instr); (void)handle;

    /* Is this CP currently busy waiting? */

    if (busy_cycles && CheckBusyWait(type, CPBusy, busy_cycles, cycle_count, &result)) {
        return result; /* Either busy waiting or interrupted */
  }
    
    if (type == ARMul_CP_FIRST || result == ARMul_CP_FIRST) {
        *reg_val0 = value[0];
        *reg_val1 = value[1];
        return ARMul_CP_DONE; /* 1st transfer */
    }
    return ARMul_CP_BUSY;
}


static int CP1MCR(void *handle, int type, ARMword instr, ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  return DoMCR(handle, type, instr, *value, &ValCP1Reg[BITS(16,19)], CheckBusyCycles(handle, CP1_WAIT_COUNT), &state->CP1Busy, cycle_count);
}

static int CP3MCR(void *handle, int type, ARMword instr, ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  return DoMCR(handle, type, instr, *value, &ValCP3Reg[BITS(16,19)], CheckBusyCycles(handle, CP3_WAIT_COUNT), &state->CP3Busy, cycle_count);
}

static int CP4MCR(void *handle, int type, ARMword instr, ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  return DoMCR(handle, type, instr, *value, &ValCP4Reg[BITS(16,19)], CheckBusyCycles(handle, CP4_WAIT_COUNT), &state->CP4Busy, cycle_count);
}

static int CP1MCRR(void *handle, int type, ARMword instr, ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  ARMword reg = BITS(0,3);
  ARMword reg2;
  /*
   * This is not part of the validation coprocessor specification. 
   * It is here to test mcrr and mrcc
   * The two coprocessor registers ar Crn and Crn + 1.
   * If CRn == 15 the second register is 0
   */
  if(reg < 15)
      reg2 = reg + 1;
  else
      reg2 = 0;
  return DoMCRR(handle, type, instr, value, &ValCP1Reg[reg],&ValCP1Reg[reg2],
                CheckBusyCycles(handle, CP1_WAIT_COUNT), &state->CP1Busy, cycle_count);
}

static int CP3MCRR(void *handle, int type, ARMword instr, ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  ARMword reg = BITS(0,3);
  ARMword reg2;
  /*
   * This is not part of the validation coprocessor specification. 
   * It is here to test mcrr and mrcc
   * The two coprocessor registers ar Crn and Crn + 1.
   * If CRn == 15 the second register is 0
   */
  if(reg < 15)
      reg2 = reg + 1;
  else
      reg2 = 0;
  return DoMCRR(handle, type, instr, value, &ValCP3Reg[reg],&ValCP3Reg[reg2],
                CheckBusyCycles(handle, CP3_WAIT_COUNT), &state->CP3Busy, cycle_count);
}

static int CP4MCRR(void *handle, int type, ARMword instr, ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  ARMword reg = BITS(0,3);
  ARMword reg2;
  /*
   * This is not part of the validation coprocessor specification. 
   * It is here to test mcrr and mrcc
   * The two coprocessor registers ar Crn and Crn + 1.
   * If CRn == 15 the second register is 0
   */
  if(reg < 15)
      reg2 = reg + 1;
  else
      reg2 = 0;
  return DoMCRR(handle, type, instr, value, &ValCP4Reg[reg],&ValCP4Reg[reg2],
                CheckBusyCycles(handle, 1), &state->CP4Busy, cycle_count);
}

static int CP5MCR(void *handle, int type, ARMword instr, ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  return DoMCR(handle, type, instr, *value, &state->ValCP5Reg[BITS(16,19)], CheckBusyCycles(handle, CP5_WAIT_COUNT), &state->CP5Busy, cycle_count);
}

static int CP7MCR(void *handle, int type, ARMword instr, ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  return DoMCR(handle, type, instr, *value, &ValCP7Reg[BITS(16,19)], CheckBusyCycles(handle, 0), &state->CP7Busy, cycle_count);
}

static int CP7MCRR(void *handle, int type, ARMword instr, ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  ARMword reg = BITS(0,3);
  ARMword reg2;
  /*
   * This is not part of the validation coprocessor specification. 
   * It is here to test mcrr and mrcc
   * The two coprocessor registers ar Crn and Crn + 1.
   * If CRn == 15 the second register is 0
   */
  if(reg < 15)
      reg2 = reg + 1;
  else
      reg2 = 0;
  return DoMCRR(handle, type, instr, value, &ValCP7Reg[reg],&ValCP7Reg[reg2],
                CheckBusyCycles(handle, 1), &state->CP7Busy, cycle_count);
}


static int CP8MCR(void *handle, int type, ARMword instr, ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  return DoMCR(handle, type, instr, *value, &ValCP8Reg[BITS(16,19)], CheckBusyCycles(handle, 0), &state->CP8Busy, cycle_count);
}

static int CP8MCRR(void *handle, int type, ARMword instr, ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  ARMword reg = BITS(0,3);
  ARMword reg2;
  /*
   * This is not part of the validation coprocessor specification. 
   * It is here to test mcrr and mrcc
   * The two coprocessor registers ar Crn and Crn + 1.
   * If CRn == 15 the second register is 0
   */
  if(reg < 15)
      reg2 = reg + 1;
  else
      reg2 = 0;
  return DoMCRR(handle, type, instr, value, &ValCP8Reg[reg],&ValCP8Reg[reg2],
                CheckBusyCycles(handle, 1), &state->CP8Busy, cycle_count);
}


/* CDP co-processor instructions for CP 1,3,4,5,7 and 8 */

static int DoCDP(void *handle, int type, ARMword instr, ARMTime cycle_count, ARMword busy_cycles, CPBusyWait *CPBusy)
{
  int result = type;
  UNUSEDARG(instr); (void)handle;

  if (busy_cycles && CheckBusyWait(type, CPBusy, busy_cycles, cycle_count, &result)) {
      return result; /* Either busy waiting or interrupted */
  }

  return ARMul_CP_DONE;
}


/* The core functionality of CDP instructions has now been placed in DoCDP, which */
/* is used by CP7 and CP8. CP5 is sufficiently different to not use this.         */

static int CP4CDP(void *handle, int type, ARMword instr, ARMword *dummy)
{
  ValidateState *state=(ValidateState *)handle;
  ARMword howlong = ValCP4Reg[BITS(12,13)]; /* See how long we have to busy wait for - if we can */
  ARMTime cycle_count = GetCycleCount(handle);
  (void)dummy;
  return DoCDP(handle, type, instr, cycle_count, howlong, &state->CP4Busy);
}

/* CP5 CDP instruction - behaviour not checked.
 * cdp p5, OP1, CRd, CRn, CRm, OP2
 * BitFields:
 * 23:20 OP1 - determines the operation.
 * 19:16 CRn
 * 15:12 CRd - Which cp reg. For some reason, only 4 registers are supported.
 * 11:8  CPnum (==5)
 * 7:5   OP2
 * 3:0   CRm
 *
 * If OP1==0xE then bits 19..16 are decoded as
 * 19: Reset
 * 18: System Error
 * 17: IRQ
 * 16: FIQ.
 */ 
static int CP5CDP(void *handle, int type, ARMword instr, ARMword *dummy)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime timeNow;
  ARMword howlong  = state->ValCP5Reg[BITS(12,13)];
  ARMTime cycle_count = GetCycleCount(handle);
  unsigned long busy_cycles = CheckBusyCycles(handle, CP5_WAIT_COUNT);
  int result = type;
  (void)dummy;
  if (busy_cycles && CheckBusyWait(type, &state->CP5Busy, busy_cycles, cycle_count, &result)) {
      return result; /* Either busy waiting or interrupted */
  }
  
/* how long is used to store the number of cycles in advance of now
   the required action happens - See Regme in the VHDL */

  /* Bits 23..20 is correct for opcode_1*/
  switch((int)BITS(20,23)) {

  case 1: 
       if (howlong == 0) {
           ARMulif_SetSignal(&state->coredesc, ARMSignal_FIQ, Signal_On);
       }
       else {
        {
            ARMulif_ScheduleUnretractable(&state->coredesc,
                                          DoAFIQ,state,
                                          cycle_count + howlong, 0);
        }

       }
       return ARMul_CP_DONE;

  case 2: 
       if (howlong == 0) {
           ARMulif_SetSignal(&state->coredesc, ARMSignal_IRQ, Signal_On);
       }
       else {
        {
            ARMulif_ScheduleUnretractable(
                &state->coredesc,DoAIRQ,state,cycle_count + howlong,0);
        }
       }
       return ARMul_CP_DONE;
  case 3: 
       ARMulif_SetSignal(&state->coredesc,ARMSignal_FIQ, Signal_Off);
       return ARMul_CP_DONE;
  case 4: 
       ARMulif_SetSignal(&state->coredesc,ARMSignal_IRQ, Signal_Off);
       return ARMul_CP_DONE;
  case 5: /* Reset Timer */
       state->FRTimer = cycle_count;
       return ARMul_CP_DONE;
  case 6: 
       timeNow =  cycle_count;
       if ( state->FRTimer < timeNow ) {
         state->ValCP5Reg[BITS(12,13)] = 
             ((ARMword)(timeNow-state->FRTimer) & 0xFF)-1;
       }
       else {
         /* ARMul time has rolled over */
           state->ValCP5Reg[BITS(12,13)] = 
             (ARMword)(((2^32)-state->FRTimer )+timeNow) &
             0xFF;
       }
       return ARMul_CP_DONE;

  case 7:
       if (howlong == 0) {
           DoASE(state);
       }
       else {
           ARMulif_ScheduleUnretractable(&state->coredesc,
                                         DoASE,state,
                                         cycle_count + howlong, 0);
       }
       return ARMul_CP_DONE;

  case 8:
       ARMulif_SetSignal(&state->coredesc,ARMSignal_SE, Signal_Off);
       return ARMul_CP_DONE;
  case 0xE:
      return ARMul_CP_DONE;


  case 0xf:
       if (howlong == 0) {
         DoAReset(state);
       }
       else {
        {
            ARMulif_ScheduleUnretractable(
                &state->coredesc,DoAReset,state, cycle_count + howlong, 0);
        }

       }
      return ARMul_CP_DONE;
  default:
      return ARMul_CP_CANT;
  }
}

 
/* CP7 CDP instruction - behaviour not checked*/ 
static int CP1CDP(void *handle, int type, ARMword instr, ARMword *dummy)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  (void)dummy;
  return DoCDP(handle, type, instr, cycle_count, 
               CheckBusyCycles(handle, CP1_WAIT_COUNT), &state->CP1Busy);
}

/* CP7 CDP instruction - behaviour not checked*/ 
static int CP3CDP(void *handle, int type, ARMword instr, ARMword *dummy)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  (void)dummy;
  return DoCDP(handle, type, instr, cycle_count, 
               CheckBusyCycles(handle, CP3_WAIT_COUNT), &state->CP3Busy);
}

/* CP7 CDP instruction - behaviour not checked*/ 
static int CP7CDP(void *handle, int type, ARMword instr, ARMword *dummy)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  (void)dummy;
  return DoCDP(handle, type, instr, cycle_count, 
               CheckBusyCycles(handle, 0), &state->CP7Busy);
}

/* CP8 CDP instruction - behaviour not checked*/ 
static int CP8CDP(void *handle, int type, ARMword instr, ARMword *dummy)
{
  ValidateState *state=(ValidateState *)handle;
  ARMTime cycle_count = GetCycleCount(handle);
  (void)dummy;

  return DoCDP(handle, type, instr, cycle_count, 
               CheckBusyCycles(handle, 0), &state->CP8Busy);
}



static void DoASE(void *handle)
{
  ValidateState *state=(ValidateState *)handle;
  ARMulif_SetSignal(&state->coredesc,ARMSignal_SE, Signal_On);
}

static void DoAFIQ(void *handle)
{
  ValidateState *state=(ValidateState *)handle;
  ARMulif_SetSignal(&state->coredesc,ARMSignal_FIQ, Signal_On);
}

static void DoAIRQ(void *handle)
{
    ValidateState *state=(ValidateState *)handle;
  ARMulif_SetSignal(&state->coredesc,ARMSignal_IRQ, Signal_On);
}

static void DoAClearReset(void *handle)
{
  ValidateState *state=(ValidateState *)handle;
    ARMulif_SetSignal(&state->coredesc,RDIPropID_ARMSignal_RESET, Signal_Off);
}

static void DoAReset(void *handle)
{
    ValidateState *state = (ValidateState *)handle;
    ARMulif_SetSignal(&state->coredesc,RDIPropID_ARMSignal_RESET, Signal_On);
    {
        ARMulif_ScheduleUnretractable(
            &state->coredesc, DoAClearReset, state, 
            3 + ARMulif_Time(&state->coredesc), 0);
    }
}



/* CP Register Read write access for RDI */

static int CP1Read(void *handle,int type, ARMword reg,ARMword *value)
{
    ValidateState *state = (ValidateState *)handle;
 (void)type;
 *value = ValCP1Reg[reg & 0x1f];
 return ARMul_CP_DONE;
}

static int CP3Read(void *handle,int type, ARMword reg,ARMword *value)
{
    ValidateState *state = (ValidateState *)handle;
 (void)type;
 *value = ValCP3Reg[reg & 0x1f];
 return ARMul_CP_DONE;
}

static int CP4Read(void *handle,int type, ARMword reg,ARMword *value)
{
    ValidateState *state = (ValidateState *)handle;
 (void)type;
 *value = ValCP4Reg[reg & 0x1f];
 return ARMul_CP_DONE;
}

   
static int CP5Read(void *handle,int type, ARMword reg,ARMword *value)
{
    ValidateState *state = (ValidateState *)handle;
 (void)type;
 *value = state->ValCP5Reg[reg];  
 return ARMul_CP_DONE;
}

static int CP7Read(void *handle,int type, ARMword reg,ARMword *value)
{
    ValidateState *state = (ValidateState *)handle;
 (void)type;
 *value = ValCP7Reg[reg &0x1f];
 return ARMul_CP_DONE;
}
 
static int CP8Read(void *handle,int type, ARMword reg,ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  (void)type;
  *value = ValCP8Reg[reg];  
  return ARMul_CP_DONE;
}

static int CP1Write(void *handle,int type, ARMword reg, ARMword *value)
{
  ValidateState *state = (ValidateState *)handle;
  (void)type;
  ValCP1Reg[reg] = *value; 
  return ARMul_CP_DONE;
}

static int CP3Write(void *handle,int type, ARMword reg, ARMword *value)
{
  ValidateState *state = (ValidateState *)handle;
  (void)type;
  ValCP3Reg[reg] = *value; 
  return ARMul_CP_DONE;
}

static int CP4Write(void *handle,int type, ARMword reg, ARMword *value)
{
  ValidateState *state = (ValidateState *)handle;
  (void)type;
  ValCP4Reg[reg] = *value; 
  return ARMul_CP_DONE;
}

static int CP5Write(void *handle,int type, ARMword reg,ARMword *value)
{
  ValidateState *state = (ValidateState *)handle;
  (void)type;
  state->ValCP5Reg[reg] = *value; 
  return 1;
}

static int CP7Write(void *handle,int type, ARMword reg, ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  (void)type;
  ValCP7Reg[reg] = *value;
  return 1;
}

static int CP8Write(void *handle,int type, ARMword reg,ARMword *value)
{
  ValidateState *state=(ValidateState *)handle;
  (void)type;
  ValCP8Reg[reg] = *value;
  return 1;
}

static unsigned int const RegBytes[17] = {16, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
  
IMPLEMENT_COPRO_DESC_FN_FROM_ARRAY(ValCPDesc, ValidateState, RegBytes)

/* Register CP0 co-processor */
ARMul_CoprocessorV5 CP0_cp = {CP0CDP, 
                              CP0LDCMCR /*MCR*/, CP0STCMRC /* MRC */,
                              NULL, NULL, /* MCRR, MRRC */
                              CP0LDCMCR /* LDC */, CP0STCMRC /* STC */,
                              NULL /* CP0_Desc */,
                              NULL /* ReadReg */, NULL /* WriteReg */} ;

/* Register CP1 co-processor */
static ARMul_CoprocessorV5 CP1_cp = {CP1CDP, 
                              CP1MCR /*MCR*/, CP1MRC /* MRC */,
                              CP1MCRR,CP1MRRC, /* MCRR, MRRC */
                              CP1LDC /* LDC */, CP1STC /* STC */,
                              ValCPDesc /* Desc */,
                              CP1Read, CP1Write};

/* Register CP2 co-processor */
ARMul_CoprocessorV5 CP2_cp = {CP2CDP, 
                              CP2LDCMCR /*MCR*/, CP2STCMRC /* MRC */,
                              NULL, NULL, /* MCRR, MRRC */
                              CP2LDCMCR /* LDC */, CP2STCMRC /* STC */,
                              NULL /* CP2_Desc */,
                              NULL /* ReadReg */, NULL /* WriteReg */} ;

/* Register CP3 co-processor */
static ARMul_CoprocessorV5 CP3_cp = {CP3CDP, 
                              CP3MCR /*MCR*/, CP3MRC /* MRC */,
                              CP3MCRR,CP3MRRC, /* MCRR, MRRC */
                              CP3LDC /* LDC */, CP3STC /* STC */,
                              ValCPDesc /* Desc */,
                              CP3Read, CP3Write};


/* Register CP4 co-processor */
static ARMul_CoprocessorV5 CP4_cp = {CP4CDP, 
                              CP4MCR /*MCR*/, CP4MRC /* MRC */,
                              CP4MCRR,CP4MRRC, /* MCRR, MRRC */
                              CP4LDC /* LDC */, CP4STC /* STC */,
                              ValCPDesc /* Desc */,
                              CP4Read, CP4Write};



/* Register CP5 co-processor */
static ARMul_CoprocessorV5 CP5_cp = {CP5CDP, 
                              CP5MCR /*MCR*/, CP5MRC /* MRC */,
                              NULL, NULL, /* MCRR, MRRC */
                              CP5LDC /* LDC */, CP5STC /* STC */,
                              ValCPDesc /* Desc */,
                              CP5Read, CP5Write};

/* Register CP7 co-processor */
static ARMul_CoprocessorV5 CP7_cp = {CP7CDP, 
                              CP7MCR /*MCR*/, CP7MRC /* MRC */,
                              CP7MCRR,CP7MRRC, /* MCRR, MRRC */
                              CP7LDC /* LDC */, CP7STC /* STC */,
                              ValCPDesc /* Desc */,
                              CP7Read, CP7Write};
/* Register CP8 co-processor */
static ARMul_CoprocessorV5 CP8_cp = {CP8CDP, 
                              CP8MCR /*MCR*/, CP8MRC /* MRC */,
                              CP8MCRR,CP8MRRC, /* MCRR, MRRC */
                              CP8LDC /* LDC */, CP8STC /* STC */,
                              ValCPDesc /* Desc */,
                              CP8Read, CP8Write};


/* Validation suite co-processor model */

#include <time.h>
#include <errno.h>
#include <string.h>
#include "rdi_hif.h"


/*
 * install ourselves as a Coprocessor, without claiming to be an OS model
 */
static ARMul_Error CPInit(ValidateState *state,
                          toolconf config)
{
    ARMul_Error err = RDIError_NoError;

    Hostif_PrettyPrint(state->hostif, config, ", ARM Validation system");


  state->bARM9_CoproIF = ToolConf_DLookupBool(
      config, (tag_t)"ARM9COPROCESSORINTERFACE", FALSE);
  state->bARM9_Extensions = ToolConf_DLookupBool(
      config, (tag_t)"ARM9EXTENSIONS", FALSE);

    {
        Hostif_PrettyPrint(state->hostif, config,
                           ", System cycle event scheduling");
    }
    if (ToolConf_DLookupBool(config,(tag_t)"CP0",FALSE))
    {
        err = ARMulif_InstallCoprocessorV5(
            &state->coredesc, 0, &CP0_cp, state);
    }
    if (!err && ToolConf_DLookupBool(config,(tag_t)"CP1",FALSE)) {
        err = ARMulif_InstallCoprocessorV5(
            &state->coredesc, 1, &CP1_cp, state);
    }
    if (!err && ToolConf_DLookupBool(config,(tag_t)"CP2",FALSE)) {
        err = ARMulif_InstallCoprocessorV5(
            &state->coredesc, 2, &CP2_cp, state);
    }
    if (!err && ToolConf_DLookupBool(config,(tag_t)"CP3",FALSE)) {
        err = ARMulif_InstallCoprocessorV5(
            &state->coredesc, 3, &CP3_cp, state);
    }
    if (!err) {
        err = ARMulif_InstallCoprocessorV5(
            &state->coredesc, 4, &CP4_cp, state);
    }
    if (!err) {
        err = ARMulif_InstallCoprocessorV5(
            &state->coredesc, 5, &CP5_cp, state);
    }
    if (!err) {
        err = ARMulif_InstallCoprocessorV5(
            &state->coredesc, 7, &CP7_cp, state);
    }
    if (!err) {
        err = ARMulif_InstallCoprocessorV5(
            &state->coredesc, 8, &CP8_cp, state);
    }

    return err;
}



/*--------------- RDI STUFF ------------- */

BEGIN_INIT(Validate)
    if (coldboot)
    { int err = CPInit(state, config);
      if (err) return err;
      assert(state->coredesc.rdi != NULL);
    }
END_INIT(Validate)

BEGIN_EXIT(Validate)
END_EXIT(Validate)

/*-------------- endof RDI stuff ----------------*/



#define SORDI_DLL_DESCRIPTION_STRING "Validation Coprocessors"
#define SORDI_RDI_PROCVEC Validate_AgentRDI
#include "perip_sordi.h"

#include "perip_rdi_agent.h"
    IMPLEMENT_AGENT_PROCS_NOEXE_NOMODULE(Validate)
    IMPLEMENT_AGENT_PROCVEC_NOEXE(Validate)



/* EOF validate.c */



