/*
 * ARM RDI : rdi_stat.h
 * Copyright (C) 1998 Advanced Risc Machines Ltd. All rights reserved.
 * Copyright (c) ARM Limited 1999-2001.  All Rights Reserved.
 */

/*
 * RCS $Revision: 1.5.6.2 $
 * Checkin $Date: 2001/08/24 12:41:31 $
 * Revising $Author: lmacgreg $
 */

#ifndef rdi_stat_h
#define rdi_stat_h

#include "armtypes.h"           /* for ARMword */


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

typedef struct RDI_CycleDesc {
  char name[32];                /* length byte, followed by string */
} RDI_CycleDesc;

typedef struct RDI_MemDescr {
    ARMword handle;
    ARMword start, limit;  /* start & limit of this region */
    unsigned char width;   /* memory width 0,1,2 => 8,16,32 bit */
    unsigned char access;  /* Bit 0 => read access */
                           /* Bit 1 => write access */
                           /* Bit 2 => latched 32 bit memory */
    unsigned char d1, d2;  /* ensure padding */
             /* Access times for R/W N/S cycles */
    unsigned long Nread_ns, Nwrite_ns, Sread_ns, Swrite_ns;
} RDI_MemDescr;

typedef struct RDI_MemAccessStats {
    ARMword Nreads,     /* Counts for R/W N/S cycles */
            Nwrites,
            Sreads,
            Swrites;
    ARMword ns,         /* Time (nsec, sec) */
            s;
} RDI_MemAccessStats;


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /* rdi_stat_h */
