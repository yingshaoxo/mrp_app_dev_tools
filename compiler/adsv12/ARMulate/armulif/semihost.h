/* -*-C-*-
 *
 * $Revision: 1.4.6.2 $
 *   $Author: lmacgreg $
 *     $Date: 2001/08/24 12:57:50 $
 *
 * Copyright (c) 1996 Advanced RISC Machines Limited
 * Copyright (C) 1996-2000 ARM Limited. All rights reserved. 
 * All Rights Reserved.
 *
 * semihost.h - renamed from arm.h to avoid conflict with HPUX header file.
 * arm.h - standard ARM processor support. This header file describes the
 * processor in software terms.
 *
 * Preconditions - host.h for uint32.
 */

/*---------------------------------------------------------------------------*/

#ifndef angel_semihost_h
#define angel_semihost_h

#include "angel_reasons.h"

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif
/*
 * APCS register definitions
 */
#define reg_a1      (0)
#define reg_a2      (1)
#define reg_a3      (2)
#define reg_a4      (3)
#define reg_v1      (4)
#define reg_v2      (5)
#define reg_v3      (6)
#define reg_v4      (7)
#define reg_v5      (8)
#define reg_v6      (9)
#define reg_sl      (10)    /* stack limit */
#define reg_fp      (11)    /* frame pointer */
#define reg_ip      (12)    /* intermediate pointer (temporary) */
#define reg_sp      (13)    /* stack pointer */
#define reg_lr      (14)    /* link register */
#define reg_pc      (15)    /* program counter */

#define reg_sb      (reg_v6)    /* static-base or module table pointer */

/*---------------------------------------------------------------------------*/

/*
 * Processor flags:
 */
#define Nbit            (1UL << 31)    /* miNus */
#define Zbit            (1UL << 30)    /* Zero */
#define Cbit            (1UL << 29)    /* Carry */
#define Vbit            (1UL << 28)    /* oVerflow */
#define Ibit            (1UL << 7)     /* processor IRQ enable/disable */
#define Fbit            (1UL << 6)     /* processor FIQ enable/disable */
#define Tbit            (1UL << 5)     /* Thumb / ARM state bit */

#define IRQDisable      (Ibit)
#define FIQDisable      (Fbit)

#define ARM_MODE_BITS            0x1f

#define ARM_MODE_USER32          0x10
#define ARM_MODE_V5_FIQ32        0x11
#define ARM_MODE_V5_IRQ32        0x12
#define ARM_MODE_V5_SVC32        0x13
#define ARM_MODE_V5_ABORT32      0x17
#define ARM_MODE_V5_UNDEF32      0x1b
#define ARM_MODE_V5_SYSTEM32     0x1f



#define InterruptMask   (Ibit | Fbit)
#define ArithmeticMask  (Nbit | Zbit | Cbit | Vbit)
#define PSRMask         (ArithmeticMask | InterruptMask | ModeMask)


/*
 * ARM hardware vectors:
 */
#define ARM_VECTOR_RESET      0x00
#define ARM_VECTOR_UNDEF      0x04
#define ARM_VECTOR_SWI        0x08
#define ARM_VECTOR_PREFETCH   0x0c
#define ARM_VECTOR_ABORT      0x10
#define ARM_VECTOR_ADDREXCEPT 0x14
#define ARM_VECTOR_IRQ        0x18
#define ARM_VECTOR_FIQ        0x1c

/*---------------------------------------------------------------------------*/

/*
 * Angel Undefined Instructions and SWI's
 *
 * Angel requires one SWI and one Undefined Instruction for operation in
 * ARM state (as opposed to Thumb state).  The SWI is used whenever it
 * is possible to pass arguments to Angel - eg. for semihosting support,
 * whereas the Undefined Instruction is used solely for breakpoints.
 *
 * Refer to the Spec for full discussion of this area.
 */

/*
 * We choose a different Undefined Instr depending on whether we are
 * on a little or big endian system.  Great care should be taken
 * before changing the choice of undefined instructions.
 */

/* Since we need a HOST_ENDIAN, make sure host.h gets loaded */
#ifndef host_h_LOADED
# include "host.h"
#endif

#ifdef HOST_ENDIAN_BIG
# define angel_BreakPointInstruction_ARM       (0xE7FFDEFE)
#else
# ifdef HOST_ENDIAN_LITTLE
#   define angel_BreakPointInstruction_ARM     (0xE7FDDEFE)
# else
#   error Must define either HOST_ENDIAN_BIG or HOST_ENDIAN_LITTLE
# endif
#endif

#define angel_BreakPointInstruction_THUMB (0xDEFE)

/*
 * If the SWIs chosen here are inconvenient it may be changed to be any
 * other SWI.
 *
 * We have a SWI for ARM state code and a SWI for Thumb code too
 */
#define angel_SWI_ARM       (0x123456)

#define angel_SWI_THUMB     (0xAB)

/*
 * The reason codes (passed in r0) may also be modified should they
 * be inconvenient.  Note that the exception handler will need to
 * be modified suitably too however.
 */
#define angel_SWIreasonBase  0x01

/*
 * The following reason codes are used by Angel as part of the C library
 * support.  The C library calls into Angel by using a SWI with the
 * following reason codes.
 */
#define angel_SWIreason_CLibBase  0x01


/*
 * XXX
 *
 * this is needed because SYS_OPEN is also in stdio.h
 */
#ifdef SYS_OPEN
# undef SYS_OPEN
#endif

/*
 * The following are the C Library reason codes ...
 */
#define SYS_OPEN          0x1
#define SYS_CLOSE         0x2
#define SYS_WRITEC        0x3
#define SYS_WRITE0        0x4
#define SYS_WRITE         0x5
#define SYS_READ          0x6
#define SYS_READC         0x7
#define SYS_ISERROR       0x8
#define SYS_ISTTY         0x9
#define SYS_SEEK          0xA
#define SYS_ENSURE        0xB
#define SYS_FLEN          0xC
#define SYS_TMPNAM        0xD
#define SYS_REMOVE        0xE
#define SYS_RENAME        0xF
#define SYS_CLOCK         0x10
#define SYS_TIME          0x11
#define SYS_SYSTEM        0x12
#define SYS_ERRNO         0x13
/* 0x14 was the now defunct SYS_INSTALL_RAISE */
#define SYS_GET_CMDLINE   0x15
#define SYS_HEAPINFO      0x16

#define angel_SWIreason_CLibLimit 0x16

/*
 * The following reason code is used to get the calling code into SVC
 * mode:
 */
#define angel_SWIreason_EnterSVC 0x17

/*
 * The following reason code is used by the C Library support code and
 * / or the C library itself to request that a particular exception is
 * reported to the debugger.  This will only be used when it has been
 * checked that there is no appropriate signal handler installed.  r1
 * must contain one of ADP_Stopped subreason codes which is to be
 * reported.
 */
#define angel_SWIreason_ReportException 0x18

/*
 * The following reason code is used by devappl.c in a full Angel
 * system when the application wants to make use of the device
 * drivers.
 *
 * In addition, values are defined for specifying whether the
 * ApplDevice call is Read, Write or Yield.  These are sub-parameters
 * to angel_SWIreason_ApplDevice
 */
#define angel_SWIreason_ApplDevice 0x19

#define angel_SWIreason_ApplDevice_Read          0x0
#define angel_SWIreason_ApplDevice_Write         0x1
#define angel_SWIreason_ApplDevice_Control       0x2
#define angel_SWIreason_ApplDevice_Yield         0x3
#define angel_SWIreason_ApplDevice_Init          0x4

/*
 * The following reason code is used by the application in a late startup
 * world to indicate that it wants to attach to a debugger now
 */
#define angel_SWIreason_LateStartup 0x20

#define angel_SWIreasonLimit 0x20

/*
 * 24-Apr-98 HGB
 *
 * Two calls (SYS_ELAPSED and SYS_TICKFREQ) added to track target time
 *
 * Instigated by Dominic Symes, approved by Ruth Ivimey-Cook
 */

/*
 * Reason code: SYS_ELAPSED
 *     Purpose: Returns the number of elapsed target 'ticks' since the
 *              support code started executing.  Ticks are defined by
 *              SYS_TICKFREQ.  (If the target cannot define the length of
 *              a tick, it may still supply SYS_ELAPSED.)
 *
 *      Inputs: r0 = 0x30 (reason code)
 *              r1 = pointer to a doubleword in which to put the number of
 *                   elapsed ticks.  The first word in the doubleword is
 *                   the least significant word.  The last word is the
 *                   most significant word.  This follows the convention 
 *                   used by armcc's "long long" data type.
 *
 *     Outputs: r1: If (*(uint64 *)r1) does not contain the number of
 *                  elapsed ticks, r1 is set to -1.
 */
#define SYS_ELAPSED 0x30

/*
 * Reason code: SYS_TICKFREQ
 *     Purpose: Define a tick frequency.  (The natural implementation is
 *              to have a software model specify one core cycle as one tick,
 *              and for Angel to return Timer1 ticks)
 *
 *      Inputs: r0 = 0x31 (reason code)
 *
 *     Outputs: r0 = ticks per second, or -1 if the target does not know the
 *              value of one tick.
 */
#define SYS_TICKFREQ 0x31 

/*---------------------------------------------------------------------------*/

/*
 * Instruction set manifests:
 */

/*
 * Condition codes:
 */
#define cond_EQ         (0x00000000)  /* Z */
#define cond_NE         (0x10000000)  /* Z */
#define cond_CS         (0x20000000)  /* C */
#define cond_HS         (0x20000000)  /* C synonym for CS */
#define cond_CC         (0x30000000)  /* C */
#define cond_LO         (0x30000000)  /* C synonym for CC */
#define cond_MI         (0x40000000)  /* N */
#define cond_PL         (0x50000000)  /* N */
#define cond_VS         (0x60000000)  /* V */
#define cond_VC         (0x70000000)  /* V */
#define cond_HI         (0x80000000)  /* CZ */
#define cond_LS         (0x90000000)  /* CZ */
#define cond_GE         (0xA0000000)  /* NV */
#define cond_LT         (0xB0000000)  /* NV */
#define cond_GT         (0xC0000000)  /* NVZ */
#define cond_LE         (0xD0000000)  /* NVZ */
#define cond_AL         (0xE0000000)  /* always */
#define cond_NV         (0xF0000000)  /* never */

#define cond_mask       (0xF0000000)
#define cond_shift      (28)

/*
 * Branches:
 *
 * offset = ((destination_address - (instruction_address + 8)) >> 2)
 */
#define op_branch       (0x0A000000)    /* branch (no condition code flags) */
#define op_branch_mask  (0x00FFFFFF)    /* branch offset mask */

#define arm_padding     (0xDEADC0DE)    /* Padding word value */

/*---------------------------------------------------------------------------*/

/*
 * Standard co-processor definitions:
 */
#define MMUCP                   (15) /* Memory Management Unit co-proc. no. */

#define MMUIDReg                (0)  /* MMU ID register */
#define MMUControlReg           (1)  /* MMU control register */
#define MMUPageTableBaseReg     (2)  /* MMU Page table base address */
#define MMUDomainAccessReg      (3)  /* MMU Domain access attributes */
#define MMUFlushTLBReg          (5)  /* MMU Flush TLB control */
#define MMUPurgeTLBReg          (6)  /* MMU Purge TLB control */
#define MMUFlushIDCReg          (7)  /* MMU Flush cache control */

#define MMUOn                   (1 << 0) /* 0 = MMU is off */
#define AlignFaultOn            (1 << 1) /* 0 = alignment faults not aborted */
#define CacheOn                 (1 << 2) /* 0 = cache off */
#define WriteBufferOn           (1 << 3) /* 0 = writebuffer off */
#define Config32                (0x30)   /* 0 = 26bit prog/data */
#define ConfigBE                (1 << 7) /* 0 = little-endian */

#define DefaultMMUConfig (Config32) /* 32bit prog/data; little-endian */

/*---------------------------------------------------------------------------*/

/*
 * ARM Identification manifests. These are used by the automatic ARM
 * checking routine. The current definitions allow for a 32bit ARM
 * identification word to be created, where the top 8bits are left
 * free for target/application specific information.
 */
#define ID_unknown          (0x00)      /* unknown processor type */
#define ID_ARM1             (0x01)
#define ID_ARM2             (0x02)
#define ID_ARM3             (0x03)
#define ID_ARM6             (0x06)
#define ID_TYPE_MASK        (0x0F)      /* mask for the above processor
                                         * types */

#define ID_LE               (1 << 8)    /* processor supports little-endian
                                         * mode */
#define ID_BE               (1 << 9)    /* processor supports big-endian
                                         * mode */
#define ID_MUL              (1 << 10)   /* processor has MUL instruction
                                         * support */
#define ID_SWP              (1 << 11)   /* processor has SWP instruction
                                         * support */
#define ID_CACHE            (1 << 12)   /* processor has a cache */
#define ID_MMU              (1 << 13)   /* processor has a MMU */

#define ID_ENDIAN           (1 << 16)   /* reflects current execution
                                         * endianness */

#define ID_CACHE_ON         (1 << 24)   /* set if cache is present and
                                         * enabled */

#define ID_WB_ON            (1 << 25)   /* set if write-buffer is present
                                         * and is enabled */

/*
 * masks for target/application specific flags
 */
#define ID_HWCONFIG_MASK    (0xFF000000)

/*---------------------------------------------------------------------------*/

/*
 * ARM processor state. The following structure offsets are used when
 * storing the complete ARM (software) register state. NOTE: This is
 * independant of any OPTIMISED O/S context saving routines. This
 * structure is used by the Angel exception handling system, and debugos.c
 * when the complete CPU world needs to be saved / restored.
 *
 * Note that this structure has to be kept in line with the offset
 * definitions in except.s
 */
typedef struct ss {
 uint32 user[13];         /* User mode registers R0-R12. */
 uint32 pc;
 uint32 cpsr;
 uint32 fiq[5];           /* FIQ mode regs R8-R12 */
 uint32 banked[6][2];     /* Registers R13-R14 in modes USR,FIQ,IRQ,SVC,ABT,UND
                           respectively. */
 uint32 spsr[5];          /* SPSR in modes FIQ,IRQ,SVC,ABT,UND respectively. */
} ss ;

/*---------------------------------------------------------------------------*/

/* Default values for */

# define ANGEL_HEAPBASE 0x40000
# define ANGEL_HEAPLIMIT 0x50000
# define ANGEL_STACKLIMIT 0x60000
# define ANGEL_STACKBASE 0x70000

#define ANGEL_SWI_ARM       (0x123456)

#define ANGEL_SWI_THUMB     (0xAB)



#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /* !defined(angel_semihost_h) */

/*---------------------------------------------------------------------------*/

/* EOF arm.h */



