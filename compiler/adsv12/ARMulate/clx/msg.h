/* msg.h -- interface to the message file and nls system. */
/* Copyright (C) Advanced RISC Machines Limited, 1995. All rights reserved. */

/*
 * RCS $Revision: 1.18 $
 * Checkin $Date: 2000/01/11 11:26:23 $
 * Revising $Author: dsinclai $
 */

#ifndef msg_h
#define msg_h

#include <stdio.h>

#ifdef NLS
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Initialise a tool. Passed in the name of the tool, and it attempts to
 * locate the message file (in a host dependent way). Can be called
 * multiple times for various components of a tool - e.g.:
 *    msg_init("armdbg",NULL); msg_init("armsd",NULL); msg_init("armul",NULL);
 *
 * toolname may be a full pathname to the tool - e.g. argv[0] under DOS,
 * in which case that directory will be searched for the messagefile too.
 */
extern const char *msg_init(const char * /*toolname*/,
                            const char * /*default_name*/);

/* Open and "closes" a message file. Once closed, all attempts to lookup a
 * message will fail. close also frees any internally allocated store.
 */

/* It is possible, at the moment, to call msg_open() multiple times, and the
 * message files will be merged (new entries overwriting old). However any
 * call to msg_close() will destroy the entire hash table, unfortunately.
 */
extern char *msg_open(const char * /*filename*/); /* Returns NULL on error */
extern void msg_close(char * /*value_returned_by_msg_open*/);

/* At the moment if you call msg_lookup(), or any function which in turn
 * calls msg_lookup() will call msg_open("messages") if no msg_open call
 * has yet been called.
 */

#define msg_t  ARM_Ltd_msg_t   /* msg_t used by Ultrix! */
typedef const unsigned char *msg_t;    /* type of message tag */

#define null_msg NULL           /* a null msg_t */

/* printf variants */
extern int msg_printf(msg_t /*tag*/, ...);
extern int msg_sprintf(char *, msg_t /*tag*/, ...);
extern int msg_fprintf(FILE *, msg_t /*tag*/, ...);
extern int msg_vsprintf(char *, msg_t /*tag*/, va_list);
extern int msg_vfprintf(FILE *, msg_t /*tag*/, va_list);

/* Low-level find-a-message-from-a-tag routine */
/* If the lookup fails, it tries the equivalent of returning the result
 * from msg_sprintf("msg_lookup_failed",tag);
 */
extern char *msg_lookup(msg_t /*tag*/);

#ifdef __cplusplus
}
#endif

#else /* not def NLS */

#define msg_t  ARM_Ltd_msg_t   /* msg_t used by Ultrix! */
typedef char const *msg_t;    /* type of message tag */

#define null_msg NULL           /* a null msg_t */

#  define msg_printf printf
#  define msg_sprintf sprintf
#  define msg_fprintf fprintf
#  define msg_vsprintf vsprintf
#  define msg_vfprintf vfprintf

#  define msg_init(X,Y)         NULL /* char * */
#  define msg_open(X)
#  define msg_close(X)

#  define msg_lookup(X) X
#endif /* ifdef NLS */

#include "nls.h"

#endif /* ifndef msg_h */
