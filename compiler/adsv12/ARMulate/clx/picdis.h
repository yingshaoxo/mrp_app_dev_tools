/*
 * picdis.h - Piccolo disassembler
 * Copyright (c) 1997 Advanced RISC Machines Limited. All rights reserved.
 */

/*
 * RCS $Revision: 1.4 $
 * Checkin $Date: 1999/11/30 19:30:17 $
 * Revising $Author: sdouglas $
 */

#ifndef picdis_h
#define picdis_h

#include "disass.h"

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

extern unsigned32 disass_pic(unsigned32 instr, unsigned32 address, char *o);

/* function to be registered as a co-processor callback */
extern char *piccolo_DisassCP(
    int cpno, Disass_CPOpType type, uint32 instr, uint32 address,
    char *o, char *notes);

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif

/* End of picdis.h */
