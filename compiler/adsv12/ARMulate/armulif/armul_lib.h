/*
 * armul_lib.h -- ARMulator utility definitions.
 *
 * Copyright (c) 1991-2001 ARM Limited.
 * All Rights Reserved.
 *
 * RCS $Revision: 1.1.8.2 $
 * Checkin $Date: 2001/08/24 12:57:50 $
 * Revising $Author: lmacgreg $
 */

#ifndef armul_lib__h
#define armul_lib__h

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

#ifndef armtypes_h
# include "armtypes.h" /* for ARMword */
#endif

/*
 * Utility function - take a value and return a suitable
 * method for printing it.
 * Returns "k", "M" or "G" for "mult". Reduce "value" to a double.
 */
extern double ARMul_SIRange(ARMword value, char **mult, bool power_of_two);

/*
 * Interface onto disassembler
 */
extern char *ARMul_Disass(ARMword instr, ARMword address, ARMword cpsr);

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /* ndef armul_lib__h */






