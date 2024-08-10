/*
 * Some defines that should be the same for any ARMulator,
 * for consistency.
 *
 * RCS $Revision: 1.3.6.4 $
 * Checkin $Date: 2001/08/24 12:57:50 $
 * Revising $Author: lmacgreg $
 * Copyright (C) 1999 - 2001 ARM Ltd.  All rights reserved.
 */

#ifndef armul_defs__h
#define armul_defs__h

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

/*
 * RDI Log bits
 */

#define RDILog_Verbose      0x0001
#define RDILog_CallDebug    0x0004
#define RDILog_NoDebug      0x0008 /* disable DebugPrint calls */
#define RDILog_Trace        0x0010 /* enable tracing */


typedef int ARMul_Error; /* Like RDIError */




/*
 * Types of ARM we know about
 */

/* The bitflags (masks for state->Processor, and return value from
 * ARMulif_Properties). */

#define ARM_Fix26_Prop      0x00001 /* Fixed in 26-bit mode */
#define ARM_Nexec_Prop      0x00002
#define ARM_Abort7_Prop     0x00004 /* Late aborts */
#define ARM_Mult64_Prop     0x00008 /* Long multiplies */
#define ARM_Debug_Prop      0x00010
#define ARM_Isync_Prop      ARM_Debug_Prop
#define ARM_Lock_Prop       0x00020 /* SWP and SWPB */
#define ARM_Halfword_Prop   0x00040 /* Halfword instructions */
#define ARM_Code16_Prop     0x00080 /* Thumb support */
#define ARM_System32_Prop   0x00100 /* System mode */
#define ARM_Cache_Prop      0x00200 /* Cached */
#define ARM_ARM8_Prop       0x00400 /* ARM8 properties */
#define ARM_StrongARM_Prop  0x00800 /* StrongARM properties */
#define ARM_NoLDCSTC_Prop   0x01000 /* LDC/STC bounce to undef vector */
#define ARM_SubPage_Prop    0x02000 /* ARM7 "sub-page" bug */
#define ARM_VectorLoad_Prop 0x04000 /* does vector checking on loads */
#define ARM_Abort8_Prop     0x08000 /* aborts leave LDM base unchanged */
#define ARM_Arch4Undef_Prop 0x10000 /* architecture vsn. 4 extra UITs */
#define ARM_Mult32_Prop     0x20000 /* Short multiplies */
#define ARM_Fix32_Prop      0x40000 /* Fixed in 32-bit modes */
#define ARM_ARM9_Prop              0x00080000 /* ARM 9 Properties */
#define ARM_HighException_Prop     0x00100000 /* ARM X20 exception hi/lo swap. */
#define ARM_EarlySignedMult_Prop   0x00200000 /* Signed multiplies terminate early */

/* ARM 9E extension property */
#define ARM_ARM9Extensions  0x00400000 /* ARM 9 extensions */
#define ARM_CoreEvents_Prop 0x00800000 /* model supports core events */
 
/* MMU and PU property bits - particularly useful for models such as
  pagetables which are otherwise unaware of the processor model
 */
#define ARM_HasMMU_Prop     0x01000000  /* Core has MMU */
#define ARM_HasPU_Prop      0x02000000  /* Core has PU */

/* Properties used by ARM9xxE variants */
#define ARM_HasSRAM_Prop    0x04000000 /* Core has SRAM interface eq ARM966E */
/* Note that this applies to ARM966E-S and now also to ARM946E-S */ 
#define ARM_ARM9OptimizedDAborts_Prop   0x08000000
#define ARM_PExtension_Prop             0x10000000  /* ARM v5 P extensions (LDRD etc) */
#define ARM_Java_Extension_Prop         0x20000000  /* ARM Java (V0) extentions */
#define ARM_Java_V0_Extension_Prop      0x20000000  /* ARM Java (V0) extentions */ 
#define ARM_Java_V1_Extension_Prop      0x40000000  /* ARM Java (V1) extentions */
#define ARM_ARM9OptimizedMemory_Prop    0x80000000  /* ARM9EJ-S/ ARM9E-Sr2 Optimized memory interface */

/* Archtectures */
#define ARM_Arch_1     ARM_Fix26_Prop
#define ARM_Arch_2    (ARM_Arch_1  | ARM_Mult32_Prop)
#define ARM_Arch_2a   (ARM_Arch_2  | ARM_Lock_Prop)
#define ARM_Arch_3    (ARM_Mult32_Prop | ARM_Lock_Prop)
#define ARM_Arch_3G   (ARM_Arch_3  | ARM_Fix32_Prop)
#define ARM_Arch_3M   (ARM_Arch_3  | ARM_Mult64_Prop)
#define ARM_Arch_4    (ARM_Arch_3M | ARM_Arch4Undef_Prop | ARM_Halfword_Prop | \
                       ARM_System32_Prop)
#define ARM_Arch_4T   (ARM_Arch_4  | ARM_Code16_Prop | ARM_Fix32_Prop)
#define ARM_Arch_4TxM (ARM_Arch_4T & ~ARM_Mult64_Prop)



/* ARM6 family */
#define ARM6    (ARM_Lock_Prop)
#define ARM60   ARM6
#define ARM600  (ARM6 | ARM_Cache_Prop)
#define ARM610  ARM600
#define ARM620  ARM600

/* ARM7 family */
#define ARM7base (ARM_Nexec_Prop | ARM_Abort7_Prop)
#define ARM7    (ARM7base | ARM_SubPage_Prop)
#define ARM70   ARM7
#define ARM700  (ARM7 | ARM_Cache_Prop)
#define ARM710  (ARM7 | ARM_Cache_Prop)
#define ARM710a (ARM7 | ARM_Cache_Prop)
#define ARM710T (ARM7 | ARM_Cache_Prop)
#define ARM720T (ARM7 | ARM_Cache_Prop | ARM_HighException_Prop )

/* ...with debug */
#define ARM7d   (ARM7base | ARM_Debug_Prop)
#define ARM70d  ARM7d
/* ...with extended multiply */
#define ARM7dm  (ARM7d | ARM_Mult64_Prop)
#define ARM70dm ARM7dm
/* ...with halfwords and 16 bit instruction set and system mode */
#define ARM7tdm (ARM7dm | ARM_Halfword_Prop | ARM_Code16_Prop | \
                 ARM_System32_Prop)
#define ARM7tdmi ARM7tdm

/* ARM8 family */
#define ARM8    ((ARM7tdm | ARM_ARM8_Prop) & \
                 ~(ARM_Code16_Prop | ARM_Debug_Prop | ARM_Abort7_Prop))
#define ARM810  (ARM8 | ARM_Cache_Prop)

/* ARM9 family */
#define ARM9tdmi ((ARM7tdm | ARM_ARM9_Prop | ARM_CoreEvents_Prop) & \
                  ~(ARM_Abort7_Prop))
#define ARM920t  (ARM9tdmi | ARM_Cache_Prop | ARM_HighException_Prop)
#define ARM940t  (ARM9tdmi | ARM_Cache_Prop)

/* StrongARM family */
#define StrongARM ((ARM8 | ARM_StrongARM_Prop) & ~ARM_ARM8_Prop)
#define StrongARM110 (StrongARM | ARM_Cache_Prop)


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
    }
#endif


#endif /*ndef armul_defs__h */

