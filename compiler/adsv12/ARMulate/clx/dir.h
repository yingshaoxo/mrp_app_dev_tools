/*
 * dir.h - host independent directory manipulations
 * Copyright (C) ARM Limited, 1998. All rights reserved.
 */
/*
 * RCS $Revision: 1.17 $
 * Checkin $Date: 2000/06/05 09:33:55 $
 * Revising $Author: sdouglas $
 */

#ifndef dir_h_LOADED
#define dir_h_LOADED 1

#include "host.h"

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

#if !defined(COMPILING_ON_UNIX) && !defined(COMPILING_ON_WINDOWS)

typedef struct { int x; } DirIterator;
# include <stddef.h>       /* for size_t */

#else

#ifdef COMPILING_ON_WINDOWS
# include <io.h>
# include <stddef.h>       /* for size_t */
#endif

typedef struct DirIterator
{

#ifdef COMPILING_ON_UNIX
    void *dirHandle; /* 'DIR *' really but we don't include dirent.h to avoid namespace
                      * pollution problems (clients would need to define _XOPEN_SOURCE
                      * under HP/UX
                      */
    char const *filePattern;
#else
    struct _finddata_t fileinfo;
    long    dirHandle;
#endif
} DirIterator;

#endif /* ndef COMPILING_ON_SEMI */

 
extern char *dir_findFirst(
        char const *dirName, char const *filePattern, DirIterator *iter);
/*
 * Find the first file name in the directory named by 'dirName'
 * which matches the pattern 'filePattern' (which must remain
 * a valid pointer until dir_finalise() is called).
 *
 * Return a pointer to the found name.
 * Return NULL if no name matches or if the directory cannot be opened.
 *
 * The state of the iteration is cached in *iter.
 *
 * If dirName == NULL, use current directory.
 */

extern char *dir_findNext(DirIterator *iter);
/*
 * Find the the next file name in the directory associated with the
 * directory iterator 'iter' by an earlier call to dir_findFirst
 * which matches the pattern associated with 'iter'.
 *
 * Return a pointer to the found name.
 * Return NULL if no name matches.
 */

extern void dir_finalise(DirIterator *iter);
/* Finalise the iterator 'iter'. */ 


extern int dir_ensurePath(const char *pathName);
/* create a directory if it does not exist 
   can create multiple levels of new directories */

extern char *dir_getcwd(char *buf, size_t size);
/* get the current working directory */

extern char dir_getdirsep(void);
/* returns the directory seperator for the host */

/* Defines an environment variable */
int dir_putenv(const char *ev);

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif

/* end of dir.h */
