/*
 * simplelink.h
 *
 * RCS $Revision: 1.2.2.1 $
 * Checkin $Date: 2001/06/14 16:13:55 $
 * Revising $Author: lmacgreg $
 */

#ifndef perip2rdi__h
#define perip2rdi__h

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif


#define Perip2Memret(a) (a)
#define Memret2Perip(a) (a)



int Perip2RDIError(int perip_retval);
int RDIError2Perip(int rdi_error);

int RDIAccess2Perip(int acc);

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif
