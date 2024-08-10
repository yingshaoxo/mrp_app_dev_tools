/*
 * armul_access.h   - peripheral and memory access-types.
 *
 * RCS $Revision: 1.8.6.4 $
 * Checkin $Date: 2001/09/25 13:27:21 $
 * Revising $Author: dsinclai $
 * Copyright (C) 2000 - 2001 ARM Ltd.  All rights reserved.
 */



#ifndef armulif__armul_access__h
#define armulif__armul_access__h

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

/* --- These are the same as in armul_mem.h --- */

/* acc_Nrw */
#define ACCESS_READ              0x0000
#define ACCESS_WRITE             0x0010

#define ACCESS_SEQ               0x0020
# define ACCESS_NSEQ             0x0000

/* acc_Nmreq */
#define ACCESS_IDLE              0x0040

#define ACCESS_CYCLE_MASK        (ACCESS_SEQ|ACCESS_IDLE)
#define ACCESS_IS_IDLE(a)  (((a) &  ACCESS_CYCLE_MASK) == ACCESS_IDLE)

/* acc_NotAccount was (1u << 31) */
#define ACCESS_REAL              0x0000
#define ACCESS_UNREAL            0x0080

/* acc_Nopc must be active-high for armul920/biu920.c */
#define ACCESS_INSTR             0x0000
#define ACCESS_DATA              0x0100
#define ACCESS_IS_DATA(t)       ((t) & ACCESS_DATA)
#define ACCESS_IS_INSTR(t)        (!ACCESS_IS_DATA(t))

/* acc_Ntrans is used in ARM9 models */
#define ACCESS_NTRANS            0x0200

/* acc_rlw */
#define ACCESS_LOCK              0x0400


/* - for debugger-access - */
#define ACCESS_PHYSICAL          0x80000
#define ACCESS_VIRTUAL           0x00000


/* --- These are not the same -- */
#define ACCESS_SIZE_MASK         0x000F
#define ACCESS_BYTE              0x0003
#define ACCESS_HALFWORD          0x0004
#define ACCESS_WORD              0x0005
#define ACCESS_DWORD             0x0006
/* Gives log2(width_in_bits) */
#define ACCESS_LOGWIDTH(at) ((ACCESS_SIZE(at)))
/* inverse of the above */
#define LOGWIDTH2ACCESS(wi) (wi) 

#define ACCESS_NUM_BYTES(at) (1 << ( (ACCESS_SIZE(at)) - ACCESS_BYTE ) )

#define ACCESS_READ_DWORD        (ACCESS_READ | ACCESS_DWORD)
#define ACCESS_READ_WORD         (ACCESS_READ | ACCESS_WORD)
#define ACCESS_READ_HALFWORD     (ACCESS_READ | ACCESS_HALFWORD)
#define ACCESS_READ_BYTE         (ACCESS_READ | ACCESS_BYTE)

#define ACCESS_WRITE_DWORD       (ACCESS_WRITE | ACCESS_DWORD)
#define ACCESS_WRITE_WORD        (ACCESS_WRITE | ACCESS_WORD)
#define ACCESS_WRITE_HALFWORD    (ACCESS_WRITE | ACCESS_HALFWORD)
#define ACCESS_WRITE_BYTE        (ACCESS_WRITE | ACCESS_BYTE)
 
#define ACCESS_REAL_WRITE_WORD        (ACCESS_WRITE_WORD | ACCESS_REAL)
#define ACCESS_REAL_WRITE_HALFWORD    (ACCESS_WRITE_HALFWORD | ACCESS_REAL)
#define ACCESS_REAL_WRITE_BYTE        (ACCESS_WRITE_BYTE | ACCESS_REAL)

#define ACCESS_INSTR_ARM         (ACCESS_INSTR | ACCESS_WORD)
#define ACCESS_INSTR_THUMB       (ACCESS_INSTR | ACCESS_HALFWORD)

#define ACCESS_IS_WRITE(t)       ((t) & ACCESS_WRITE)
#define ACCESS_IS_READ(t)        (!ACCESS_IS_WRITE(t))
#define ACCESS_IS_SEQ(t)         ((t) & ACCESS_SEQ)
#define ACCESS_SIZE(t)           ((t) & ACCESS_SIZE_MASK)
#define ACCESS_SIZE_IN_BYTES(t)  (1 << (ACCESS_SIZE(t) - 3))
#define ACCESS_SIZE_RW(t)        ((t) & (ACCESS_SIZE_MASK | ACCESS_WRITE))
#define ACCESS_IS_UNREAL(t)        ((t) & ACCESS_UNREAL)
#define ACCESS_IS_REAL(t)        (!ACCESS_IS_UNREAL(t))

#define ACCESS_IS_MREQ(t)        ( !((t) & ACCESS_IDLE)  )


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /*ndef armulif__armul_access__h */
/* EOF armul_access.h */






