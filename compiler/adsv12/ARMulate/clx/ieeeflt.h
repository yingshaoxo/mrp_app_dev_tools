/*
 * ieeeflt.h: interface to host-arithmetic-independent IEEE fp package.
 * Copyright (C) Codemist Ltd., 1994.
 * Copyright (C) Advanced RISC Machines Limited, 1994.
 */

/*
 * RCS $Revision: 1.15.8.1 $
 * Checkin $Date: 2000/12/21 15:12:43 $
 * Revising $Author: vkorstan $
 */

#ifndef ieeeflt_h_LOADED
#define ieeeflt_h_LOADED

/*
 * The following types describe the representation of floating-point
 * values by the compiler in both binary and source-related forms.
 * The order of fields in DbleBin in is exploited only in object-code
 * formatters and assembly code generators.
 */

#include "host.h"

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

typedef struct DbleBin {
  int32 lsd,msd;
} DbleBin;

typedef struct FloatBin {
  int32 val;
} FloatBin;

#define flt_inexact (-1)
#define flt_partial_ufl (-2)           /* partial underflow (i.e. not to 0) */
#define flt_diffsame (-3)              /* (x-x): sign of 0 depends on rmode */
#define flt_ok 0
#define flt_very_small 1
#define flt_very_big 2
#define flt_big_single 3
#define flt_small_single 4
#define flt_negative 5
#define flt_divide_by_zero 6
#define flt_invalidop 7 /* inf - int, inf / inf, anything involving NaNs */
#define flt_bad 8       /* invalid string for stod */
#define flt_unordered 128 /* only from flt_compare */
#define flt_unordered_signalling 129 /* only from flt_compare */

extern int fltrep_narrow(DbleBin const *d, FloatBin *e);
extern int fltrep_narrow_round(DbleBin const *d, FloatBin *e);
extern int fltrep_narrow_quiet(DbleBin const *d, FloatBin *e);
extern void fltrep_widen(FloatBin const *e, DbleBin *d);
extern void fltrep_widen_quiet(FloatBin const *e, DbleBin *d);

extern int flt_add(DbleBin *a, DbleBin const *b, DbleBin const *c);
extern int flt_subtract(DbleBin *a, DbleBin const *b, DbleBin const *c);
extern int flt_multiply(DbleBin *a, DbleBin const *b, DbleBin const *c);
extern int flt_divide(DbleBin *a, DbleBin const *b, DbleBin const *c);
extern int flt_compare(DbleBin const *b, DbleBin const *c);
/* flt_compare returns
 *    -1 iff a < b
 *     0 iff a == b
 *    +1 iff a > b
 *    flt_unordered iff a <unordered> b and
 *                      neither a nor b is a signalling NaN
 *    flt_unordered_signalling otherwise (i.e.
 *                      iff a <unordered> b and at least one of them
 *                          is a signalling NaN)
 *
 */
extern int flt_move(DbleBin *a, DbleBin const *b);
extern int flt_negate(DbleBin *a, DbleBin const *b);

extern int flt_fadd(FloatBin *a, FloatBin const *b, FloatBin const *c);
extern int flt_fsubtract(FloatBin *a, FloatBin const *b, FloatBin const *c);
extern int flt_fmultiply(FloatBin *a, FloatBin const *b, FloatBin const *c);
extern int flt_fdivide(FloatBin *a, FloatBin const *b, FloatBin const *c);
extern int flt_fcompare(FloatBin const *b, FloatBin const *c);
extern int flt_fmove(FloatBin *a, FloatBin const *b);
extern int flt_fnegate(FloatBin *a, FloatBin const *b);

extern bool flt_equal(DbleBin const *b, DbleBin const *c);
extern int flt_abs(DbleBin *a, DbleBin const *b);

extern int flt_ftoi(int32 *n, FloatBin const *a);


extern int flt_dtoi(int32 *n, DbleBin const *a);
extern int flt_dtou(unsigned32 *u, DbleBin const *a);
extern int flt_dtoi64(int64 *n, DbleBin const *a);
extern int flt_dtou64(uint64 *n, DbleBin const *a);

extern int flt_itod(DbleBin *a, int32 n);
extern int flt_utod(DbleBin *a, unsigned32 n);
extern int flt_i64tod(DbleBin *a, int64 n);
extern int flt_u64tod(DbleBin *a, uint64 n);

extern int flt_i64tof(FloatBin *a, int64 n);
extern int flt_u64tof(FloatBin *a, uint64 n);

void flt_frexp(DbleBin *res, DbleBin const *dp, int *lvn);
void flt_ldexp(DbleBin *res, DbleBin const *dp, int n);

/* defined in ieeebtod.c */
extern int fltrep_stod(const char *s, DbleBin *p, char **endp);

/* defined in dtos.c */
int fltrep_dtos(char *p, int flags, int ch, int precision, int width,
                const char *prefix, DbleBin const *d);
extern void fltrep_sprintf(char *b, char const *fmt, DbleBin const *dp);

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif
