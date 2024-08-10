/* -*-C-*-
 *
 * int64.h
 *
 * $Revision: 1.23 $
 *   $Author: vkorstan $
 *     $Date: 2000/04/20 17:01:55 $
 *
 * Copyright 1996-1999 ARM Limited.
 * All Rights Reserved.
 *
 */

#ifndef clx_int64_h
#define clx_int64_h

#include "host.h" /* for int64/uint64 */
#include "ieeeflt.h"

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

#define I64_to_int64(x) ( ((int64)((x).hi) << 32) | (x).lo )
#define I64_to_uint64(x) ( ((uint64)((x).hi) << 32) | (x).lo )

/*
 * 990506 KWelton
 *
 * We now use native int64 support - these are the old {,u}int64 data
 * types, allowing code to be gradually ported to the new types.
 */
typedef struct { uint32 lo; int32 hi; } Int64Bin;
typedef struct { uint32 lo, hi; } Uint64Bin;

typedef enum {
  i64_ok,
  i64_overflow,
  i64_negative,
  i64_divide_by_zero,
  i64_overlarge_shift
} I64_Status;

I64_Status I64_SAdd(Int64Bin *res, Int64Bin const *a, Int64Bin const *b);
I64_Status I64_SSub(Int64Bin *res, Int64Bin const *a, Int64Bin const *b);
I64_Status I64_SMul(Int64Bin *res, Int64Bin const *a, Int64Bin const *b);
I64_Status I64_SDiv(Int64Bin *quot, Int64Bin *rem, Int64Bin const *a,
                    Int64Bin const *b);

/*
 * These are the same as the signed set above, except that overflow
 * is set differently
 */
I64_Status I64_UAdd(Uint64Bin *res, Uint64Bin const *a, Uint64Bin const *b);
I64_Status I64_USub(Uint64Bin *res, Uint64Bin const *a, Uint64Bin const *b);
I64_Status I64_UMul(Uint64Bin *res, Uint64Bin const *a, Uint64Bin const *b);
I64_Status I64_UDiv(Uint64Bin *quot, Uint64Bin *rem, Uint64Bin const *a,
                    Uint64Bin const *b);

I64_Status I64_Neg(Int64Bin *res, Int64Bin const *a);

I64_Status I64_SRsh(Int64Bin *res, Int64Bin const *a, unsigned b);
I64_Status I64_URsh(Uint64Bin *res, Uint64Bin const *a, unsigned b);
I64_Status I64_Lsh(Int64Bin *res, Int64Bin const *a, unsigned b);
I64_Status I64_Ror(Int64Bin *res, Int64Bin const *a, unsigned b);

void I64_And(Int64Bin *res, Int64Bin const *a, Int64Bin const *b);
void I64_Or(Int64Bin *res, Int64Bin const *a, Int64Bin const *b);
void I64_Eor(Int64Bin *res, Int64Bin const *a, Int64Bin const *b);
void I64_Not(Int64Bin *res, Int64Bin const *a);

I64_Status I64_SToI(int32 *res, Int64Bin const *a);
I64_Status I64_UToI(uint32 *res, Uint64Bin const *a);
I64_Status I64_IToS(Int64Bin *res, int32 a);
I64_Status I64_IToU(Uint64Bin *res, uint32 a);

I64_Status I64_DToI(Int64Bin *res, DbleBin const *a);
I64_Status I64_DToU(Uint64Bin *res, DbleBin const *a);
I64_Status I64_IToD(DbleBin *res, Int64Bin const *a);
I64_Status I64_UToD(DbleBin *res, Uint64Bin const *a);

int I64_SComp(Int64Bin const *a, Int64Bin const *b);
int I64_UComp(Uint64Bin const *a, Uint64Bin const *b);

bool I64_SEq(Int64Bin const *a, Int64Bin const *b);
bool I64_UEq(Uint64Bin const *a, Uint64Bin const *b);

bool I64_SisSmallConst(Int64Bin const *a, int32  *result);
bool I64_UisSmallConst(Uint64Bin const *a, int32 *result);




int I64_sprintf(char *b, char const *fmt, Int64Bin const *p);

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /* ndef clx_int64_h */

/* End of int64.h */
