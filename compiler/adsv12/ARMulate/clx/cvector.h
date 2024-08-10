/*
 * ARMulator : cvector.h simple container in C.
 * Copyright (C) ARM Limited, 1999. All rights reserved.
 */
/*
 * RCS $Revision: 1.9 $
 * Checkin $Date: 2000/08/29 21:01:14 $
 * Revising $Author: dsinclai $
 */


#ifndef cvector_h
#define cvector_h

/* DRS 1999-11-27
 * Bool is not the ARM-standard way to report errors.
 */
#ifdef OldCode
#  include "host.h" /* for bool */
#else
#  include "errcodes.h"  /* for ErrorCode */
#endif

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

typedef struct CVector CVector;

#ifndef OPAQUE_CVECTOR
struct CVector
{
    unsigned count, allocated;
    void *data;
    size_t element_size;
};
#endif

struct CVector *CVector_New(size_t elt_size);
/*
 * These allocate a fresh copy of the data.
 */
ErrorCode CVector_Copy(struct CVector *source, struct CVector *dest);
struct CVector *CVector_Clone(struct CVector *source);



ErrorCode CVector_Init(struct CVector *vec, size_t element_size);
void CVector_Destroy(struct CVector **vecp);

/* Like the above, but doesn't free the CVector. */
void CVector_Clear(struct CVector *vec);

ErrorCode CVector_Realloc(struct CVector *vec, unsigned new_allocn);
ErrorCode CVector_Append(struct CVector *vec, void const *new_element);
ErrorCode CVector_Put(struct CVector *vec, unsigned index, void const *new_element);
void* CVector_At(struct CVector *vec, unsigned index);
ErrorCode CVector_Get(struct CVector *vec, unsigned index, void *dest);

ErrorCode CVector_Insert(struct CVector *vec, unsigned index, void const *new_element);

unsigned CVector_IndexOf(struct CVector *vec, void const *element);

typedef int (*CVector_IterateFunc)(void *state, void *element);
/* Returns first non-zero value returned by the above */
int CVector_Iterate(CVector *vec,CVector_IterateFunc func, void *state);

unsigned CVector_Count(CVector *vec);

/* Remove all identical elements, return number removed */
int CVector_Remove(CVector *vec, void *elt);


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /*ndef cvector_h */

/* EOF cvector.h */
