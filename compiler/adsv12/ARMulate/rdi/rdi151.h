/* -*-C-*-
 *
 * $Revision: 1.5 $
 *   $Author: rivimey $
 *     $Date: 1999/08/20 15:18:08 $
 *
 * Copyright (c) ARM Limited 1999.  All Rights Reserved.
 *
 * rdi151.h: Definitions for RDI 1.51
 */

#ifndef rdi_151_h
#define rdi_151_h

#if RDI_VERSION == 151

/*
 * RDI 151 is heavily based on RDI 150, so drag in the RDI 150 definitions
 */
#undef RDI_VERSION
#define RDI_VERSION 150

#include "rdi150.h"

#undef RDI_VERSION
#define RDI_VERSION 151

/*
 * RDI 1.51 additions go here.
 */

#else

#include "rdi150.h"

#endif /* RDI_VERSION == 151 */

#endif /* rdi_151_h */

