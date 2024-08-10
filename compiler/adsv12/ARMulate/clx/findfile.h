/* findfile.h - clx function to find a file from the PATH etc. */
/* Copyright (C) Advanced RISC Machines Limited, 1995. All rights reserved. */

/*
 * RCS $Revision: 1.5 $
 * Checkin $Date: 1999/11/30 19:30:16 $
 * Revising $Author: sdouglas $
 */

#ifndef findfile_h
#define findfile_h

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

/* Find a file relating to <tool> with extension <extn>. Searches a variety of
 * paths, including <path> - which should be argv[0] - and $<xpath> if set. If
 * <extn> is null, it searches for just <tool> - so <tool> could be a complete
 * filename.
 *
 * e.g. to find the error file do clx_findfile(argv[0],"ARMERR","armsd","err");
 *                             or clx_findfile(argv[0],"ARMERR","armsd.err",NULL);
 *
 * The value of argv[0] is cached and used on future calls where argv0==NULL
 * Returns NULL (after caching argv[0]) if tool==NULL. So to cache an argv[0] do
 *    (void)clx_findfile(argv[0],NULL,NULL,NULL);
 */
const char *clx_findfile(const char *argv0,const char *xpath,
                         const char *tool,const char *extn);

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif
