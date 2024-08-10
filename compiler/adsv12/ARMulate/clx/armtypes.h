/*
 * armtypes.h - standard toolkit types - things about an ARM.
 * Copyright (C) 1996 Advanced RISC Machines Limited. All Rights Reserved.
 *
 * RCS $Revision: 1.8.2.4 $
 * Checkin $Date: 2001/10/08 14:32:24 $
 * Revising $Author: dsinclai $
 */

#ifndef armtypes_h
#define armtypes_h

#include "host.h"



#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif


typedef unsigned32 ARMaddress;
typedef unsigned32 ARMword;
typedef unsigned16 ARMhword;
typedef unsigned8  ARMbyte;

/*
 * Condition codes
 */
enum { ARM_CC_EQ=0,  ARM_CC_NE,  ARM_CC_CS,  ARM_CC_CC,
       ARM_CC_MI,    ARM_CC_PL,  ARM_CC_VS,  ARM_CC_VC,
       ARM_CC_HI,    ARM_CC_LS,  ARM_CC_GE,  ARM_CC_LT,
       ARM_CC_GT,    ARM_CC_LE,  ARM_CC_AL,  ARM_CC_NV };

/*
 * modes
 */
#define ARM_M           0x1f
#define ARM_M32         0x10
#define ARM_M_USER26    0x00
#define ARM_M_FIQ26     0x01
#define ARM_M_IRQ26     0x02
#define ARM_M_SVC26     0x03
#define ARM_M_USER32    0x10
#define ARM_M_FIQ32     0x11
#define ARM_M_IRQ32     0x12
#define ARM_M_SVC32     0x13
#define ARM_M_ABORT32   0x17
#define ARM_M_UNDEF32   0x1b
#define ARM_M_SYSTEM32  0x1f

/*
 * Bits in the status register/r15
 */
#define ARM_PSR_N_pos       31
#define ARM_PSR_Z_pos       30
#define ARM_PSR_C_pos       29
#define ARM_PSR_V_pos       28
#define ARM_PSR_S_pos       27
#define ARM_PSR_N           0x80000000L
#define ARM_PSR_Z           0x40000000L
#define ARM_PSR_C           0x20000000L
#define ARM_PSR_V           0x10000000L
#define ARM_PSR_Q           0x08000000L
#define ARM_PSR_S ARM_PSR_Q  /* obsolete name */
#define ARM_PSR_CC          0xf8000000L
#define ARM_PSR_I           0x00000080L
#define ARM_PSR_F           0x00000040L
#define ARM_PSR_T           0x00000020L
#ifndef NOJAVA
#define ARM_PSR_J           0x01000000L
#endif
#define ARM_PSR_M           ARM_M
#define ARM_PSR_26_M        0x00000003
#define ARM_PSR_M32         ARM_M32
#define ARM_PSR_M_USER      ARM_M_USER26
#define ARM_PSR_M_FIQ       ARM_M_FIQ26
#define ARM_PSR_M_IRQ       ARM_M_IRQ26
#define ARM_PSR_M_SVC       ARM_M_SVC26
#define ARM_PSR_M_ABORT    (ARM_M_ABORT32 & ~ARM_M32)
#define ARM_PSR_M_UNDEF    (ARM_M_UNDEF32 & ~ARM_M32)
#define ARM_PSR_M_SYSTEM   (ARM_M_SYSTEM32 & ~ARM_M32)
#define ARM_PSR_32_IF      (ARM_PSR_I+ARM_PSR_F)
#define ARM_PSR_26_I       (ARM_PSR_I << 20)
#define ARM_PSR_26_F       (ARM_PSR_F << 20)
#define ARM_PSR_26_IF      (ARM_PSR_32_IF << 20)
#define ARM_PSR_FLAGS      0xfc000003L

/*
 * register names
 */
enum {
  ARM_r_r0,  ARM_r_r1,  ARM_r_r2,  ARM_r_r3,
  ARM_r_r4,  ARM_r_r5,  ARM_r_r6,  ARM_r_r7,
  ARM_r_r8,  ARM_r_r9,  ARM_r_r10, ARM_r_r11,
  ARM_r_r12, ARM_r_r13, ARM_r_r14, ARM_r_r15
};

/* alternate names */
#define ARM_r_a1 ARM_r_r0
#define ARM_r_a2 ARM_r_r1
#define ARM_r_a3 ARM_r_r2
#define ARM_r_a4 ARM_r_r3

#define ARM_r_v1 ARM_r_r4
#define ARM_r_v2 ARM_r_r5
#define ARM_r_v3 ARM_r_r6
#define ARM_r_v4 ARM_r_r7
#define ARM_r_v5 ARM_r_r8
#define ARM_r_v6 ARM_r_r9
#define ARM_r_v7 ARM_r_r10
#define ARM_r_v8 ARM_r_r11

#define ARM_r_sb ARM_r_r9
#define ARM_r_sl ARM_r_r10
#define ARM_r_fp ARM_r_r11
#define ARM_r_ip ARM_r_r12
#define ARM_r_sp ARM_r_r13
#define ARM_r_lr ARM_r_r14
#define ARM_r_pc ARM_r_r15

/*
 * vectors/addresses
 */
#define ARM_V_RESET      0x00
#define ARM_V_UNDEF      0x04
#define ARM_V_SWI        0x08
#define ARM_V_PREFETCH   0x0c
#define ARM_V_ABORT      0x10
#define ARM_V_ADDREXCEPT 0x14
#define ARM_V_IRQ        0x18
#define ARM_V_FIQ        0x1c

/*
 * standard co-processor numbers
 */
enum { ARM_CP_0,  ARM_CP_1,  ARM_CP_2,  ARM_CP_3,
       ARM_CP_4,  ARM_CP_5,  ARM_CP_6,  ARM_CP_7,
       ARM_CP_8,  ARM_CP_9,  ARM_CP_10, ARM_CP_11,
       ARM_CP_12, ARM_CP_13, ARM_CP_14, ARM_CP_15 };

#define ARM_CP_FP1      ARM_CP_1
#define ARM_CP_FP2      ARM_CP_2
#define ARM_CP_FP       ARM_CP_FP1
#define ARM_CP_Piccolo1 ARM_CP_8
#define ARM_CP_Piccolo2 ARM_CP_9
#define ARM_CP_Piccolo  ARM_CP_Piccolo1
#define ARM_CP_System   ARM_CP_15

/*
 * system control register (CP15 r1)
 */

#define ARM_CP15_ID      0
#define ARM_CP15_CTL     1
#define ARM_CP15_TTB     2
#define ARM_CP15_DAC     3
#define ARM_CP15_FSR     5      /* also flush TLB on Arch 3 */
#define ARM_CP15_FAR     6      /* also flush TLB entry on Arch 3 */
#define ARM_CP15_CacheOp 7
#define ARM_CP15_TLBOp   8      /* flush cache on arch 3 */

#define ARM_MMU_M_pos  0         /* MMU enable */
#define ARM_MMU_A_pos  1         /* Address fault enable */
#define ARM_MMU_C_pos  2         /* Cache enable */
#define ARM_MMU_W_pos  3         /* WB enable */
#define ARM_MMU_P_pos  4         /* 32/26 program mode */
#define ARM_MMU_D_pos  5         /* 32/26 data mode */
#define ARM_MMU_L_pos  6         /* Late abort timing mode */
#define ARM_MMU_B_pos  7         /* Big endian mode */
#define ARM_MMU_S_pos  8         /* System control */
#define ARM_MMU_R_pos  9         /* Other (USR) system control */
#define ARM_MMU_F_pos  10        /*  */
#define ARM_MMU_Z_pos  11        /* Branch-prediction enable */
#define ARM_MMU_I_pos  12        /* I-cache enable */
#define ARM_MMU_V_pos  13        /* Prot unit Exception vector location flag */
#define ARM_MMU_RR_pos 14        /* Round Robin cache victim selection */
#define ARM_MMU_T_pos 15         /* ARMv5 LDR pc, TBIT disable */
#define ARM_MMU_DEN_pos 16
#define ARM_MMU_DLM_pos 17
#define ARM_MMU_IEN_pos 18
#define ARM_MMU_ILM_pos 19
#define ARM_PU_S_pos  24         /* Cache split mode */
#define ARM_PU_L1_pos 25         /* second bit of lock bits for cache */
#define ARM_PU_L2_pos 26         /* first of two lock bits */
#define ARM_PU_F_pos  27         /* flag for partially locked cache */
#define ARM_PU_B1_pos 28         /* second of the two bits setting cache bank */
#define ARM_PU_B2_pos 29         /* first of two bits setting cache bank */

#define ARM_MMU_TB_pos 29        /* ARM9EJ-Sr1 Thumb buffer Control */
#define ARM_MMU_nF_pos 30        /* Software Configurable Clocking mode : Not FastBus  */
#define ARM_MMU_iA_pos 31        /* Software Configurable Clocking mode : Asynchronous */        

#define ARM_MMU_M  0x000000001
#define ARM_MMU_A  0x000000002
#define ARM_MMU_C  0x000000004
#define ARM_MMU_W  0x000000008
#define ARM_MMU_P  0x000000010
#define ARM_MMU_D  0x000000020
#define ARM_MMU_L  0x000000040
#define ARM_MMU_B  0x000000080
#define ARM_MMU_S  0x000000100
#define ARM_MMU_R  0x000000200
#define ARM_MMU_F  0x000000400
#define ARM_MMU_Z  0x000000800
#define ARM_MMU_I  0x000001000
#define ARM_MMU_V  0x000002000
#define ARM_MMU_RR 0x000004000
#define ARM_MMU_T  0x000008000
#define ARM_MMU_DEN 0x00010000
#define ARM_MMU_DLM 0x00020000
#define ARM_MMU_IEN 0x00040000
#define ARM_MMU_ILM 0x00080000

#define ARM_PU_S  0x001000000
#define ARM_PU_L1 0x002000000
#define ARM_PU_L2 0x004000000
#define ARM_PU_F  0x008000000
#define ARM_PU_B1 0x010000000
#define ARM_PU_B2 0x020000000

#define ARM_MMU_TB 0x020000000
#define ARM_MMU_nF 0x040000000
#define ARM_MMU_iA 0x080000000



#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif

/* end of file armtypes.h */
