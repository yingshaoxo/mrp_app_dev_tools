/***************************************************************************
 *
 * rw/math.h - Standard Library vs math.h exception conflict hack.
 *
 * $Id: math.h,v 1.2 1996/08/28 01:30:20 smithey Exp $
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-1999 Rogue Wave Software, Inc.  All Rights Reserved.
 *
 * This computer software is owned by Rogue Wave Software, Inc. and is
 * protected by U.S. copyright laws and other laws and by international
 * treaties.  This computer software is furnished by Rogue Wave Software,
 * Inc. pursuant to a written license agreement and may be used, copied,
 * transmitted, and stored only in accordance with the terms of such
 * license and with the inclusion of the above copyright notice.  This
 * computer software or any other copies thereof may not be provided or
 * otherwise made available to any other person.
 *
 * U.S. Government Restricted Rights.  This computer software is provided
 * with Restricted Rights.  Use, duplication, or disclosure by the
 * Government is subject to restrictions as set forth in subparagraph (c)
 * (1) (ii) of The Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013 or subparagraphs (c) (1) and (2) of the
 * Commercial Computer Software – Restricted Rights at 48 CFR 52.227-19,
 * as applicable.  Manufacturer is Rogue Wave Software, Inc., 5500
 * Flatiron Parkway, Boulder, Colorado 80301 USA.
 *
 ***************************************************************************
 *
 *  Many compilers provide a structure named "exception" in math.h, used to
 *  report errors from a floating point unit.  Some Microsoft compilers also
 *  #define complex as a synonym for a structure named _complex.  To prevent
 *  collisions with the Standard Library exception and complex classes, this
 *  header causes the old exception class to be renamed "math_exception" and
 *  removes the Microsoft complex symbol.
 *
 *  If you want to use math.h in conjunction with the Rogue Wave Standard
 *  library, always #include <rw/math.h> rather than <math.h>!
 *
 **************************************************************************/

#ifndef __STD_RW_MATH_H
#define __STD_RW_MATH_H

#ifndef __STD_RWCOMPILER_H__
#include <stdcomp.h>
#endif

//
// Avoid a conflicting exception structure
//
#if !defined (_MSC_VER)
#define exception math_exception
#endif

#ifndef _RWSTD_NO_NEW_HEADER
#include <cmath>
#else
#include <math.h>
#endif

//
// Undefine exception macro now
//
#ifdef exception
#undef exception
#endif

//
// MSVC provides it's own complex macro too
//
#ifdef _MSC_VER
#ifdef complex
#undef complex
#endif
#endif

#endif /* __STD_RW_MATH_H */

