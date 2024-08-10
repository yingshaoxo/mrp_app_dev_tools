/*
 * armul_method.h
 *
 * Copyright (C) 1996-2000 ARM Limited. All rights reserved. 
 * RCS $Revision: 1.1.2.3 $
 * Checkin $Date: 2001/08/24 12:57:50 $
 * Revising $Author: lmacgreg $
 */

#ifndef armul_method__h
#define armul_method__h

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

typedef int GenericMethodFunc(void *handle, void *index, void *data);
typedef struct { GenericMethodFunc *func; void *handle; } GenericMethod;

typedef struct { char const *name; GenericMethodFunc *func;} NamedMethodFunc;

/* Returns 0: Ok else error. */
typedef int InterfaceSourceFunc(void *handle, char const *TypedName, GenericMethod *cb);
/*
 * RDIInfo#QueryInterfaceSource takes an InterfaceID and returns (as *arg2)
 *  a GenericMethod whose function is an InterfaceSourceFunc.
 * An InterfaceID is the name of a port on an instance. (!WIP!)
 */
typedef struct { InterfaceSourceFunc *func; void *handle; } InterfaceSource;

/*
 * Function: FindMethodInterface
 * Purpose: A helper for RDIInfo-functions to implement QueryInterfaceSource,
 *           or for interface-sources themselves.
 * Parameters:
 *      Out: arg2 - has it's handle and func filled in.
 *       In: arg1 - name of interface element.
 *         handle - data handle to fill into arg2.
 *             mf - array of named methods to search in.
 * Returns 0 if Ok, else RDIError_UnimplementedMessage.
 */
extern int FindMethodInterface(GenericMethod *arg2, char const *arg1,
                                                           void *handle,NamedMethodFunc *mf);

/* Purpose: allow, during configuration, a slow generic method-call to be made
 *          though an interface. (Makes configuration code simpler, at the
 *          expense of config-time speed.)
 * Parameters:
 *          arg1,arg2 - arguments for function fetched from interface.
 *       In: 
 */
extern int CallThroughInterfaceSource(InterfaceSource *ifsrc,
                                      char const *TypedName,
                                      void *arg1, void *arg2);

/* These are for use while invoking CALL_IFSRC_FN,
 * to package a set of parameters into one macro-parameters.
 * This relies on the C pre-processor expanding macro-parameters during
 * the macro-expansion, not before.
 * VC6, gccsolrs and the hpux compiler all satisfy that requirement,
 * and, since it's the cheapest way to implement a pre-processor, I
 * shall assume that all the C comnpilers we use do so.
 */
#define P2(x,y) x,y
#define P3(x,y,z) x,y,z
#define P4(x,y,z,t) x,y,z,t


/* Purpose: As CallThroughInterfaceSource, but allows a wider variety of 
 *    function-types to be called, and is more type-safe (but costs more code).
 * Parameters:
 *      Out: rv1 - RDIError if ifsrc cannot give you the required method,
 *                 else 0.
 *           rv2 - return value of the method.
 *       In: id  - name of interface-function, less the type, in quotes.
 *          type - type of interface function.
 *
 * Preconditions: ifsrc != NULL && ifsrc->func != NULL.
 * Notes: If params is more than one parameter,
 *        use one of the Pn macros above.
 */

#define CALL_IFSRC_FN(rv1,rv2,ifsrc,id,type,params) \
  {  GenericMethod cif_gm; \
        if ((rv1 = ifsrc->func(ifsrc->handle, id ":" #type, &cif_gm)) == 0 && \
                cif_gm.func != NULL) \
                rv2 = ((type*)(cif_gm.func))(cif_gm.handle,params);\
    else\
        rv1 = RDIError_UnimplementedMessage;\
   }
/* As above, but function has void return type. */
#define CALL_IFSRC_PROC(rv1,ifsrc,id,type,params) \
  {  GenericMethod cif_gm; \
        if ((rv1 = ifsrc->func(ifsrc->handle, id ":" #type, &cif_gm)) == 0 && \
                cif_gm.func != NULL) \
                ((type*)(cif_gm.func))(cif_gm.handle,params);\
    else\
        rv1 = RDIError_UnimplementedMessage;\
   }



#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)

#endif

#endif /* ndef armul_method__h */
/* EOF armul_method.h */
