/* -*-C-*-
 *
 * $Revision: 1.9 $
 *   $Author: sdouglas $
 *     $Date: 2000/06/05 09:34:32 $
 *
 * Copyright (c) ARM Limited 1999.  All Rights Reserved.
 *
 * dynlink.h - Non host-specific interface for loading and interrogating
 *             dynamic link libraries and shared objects (etc)
 */

/*
 * Conventions
 * -----------
 *
 * This interface assumes that an exported function FuncName has an
 * associated typedef FuncName_Proc
 */

/*
 * Interface overview
 * ------------------
 *
 * Basics:
 *    Dynlink_LoadLibrary: loads a dynamic library into the current process
 *    Dynlink_GetProcAddress: Retrieves an entry point from a loaded library
 *    Dynlink_UnloadLibrary: Releases the shard library.
 * Additional:
 *    Dynlink_GetDLLExt: Return a string with the host specific DLL extension
 *
 * This interface also defines Win32 specific variants.  These are for use when
 * working with Win32 under unix (e.g., Wind/U), where the base functions are
 * the native versions, but Win32 equivalents may be also required.
 * Win32 versions are implemented in dlinkw32.c
 */

#ifndef dynlink_h
#define dynlink_h

#include "host.h"

/* @@@ need to consider extern "C" vs extern "C++" proc */
#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

#ifdef COMPILING_ON_SEMI
#  define NULL_DLL_SUPPORT
#endif

/*
 *         Typedef: Dynlink_Instance
 *         Purpose: Host independent type for a handle to a loaded library
 */
typedef void *Dynlink_Instance;

/*
 *           Macro: Dynlink_GetProcAddress
 *         Purpose: Retrieve address of a procedure in a DLL/SO/SL.
 *                  Implemented as macro to allow variable return type
 *
 *  Pre-conditions: Library loaded with Dynlink_LoadLibrary (See below)
 *                  Procedure "FuncName" must be of type "FuncName_Proc".
 *
 *          Params:
 *                   In: Dynlink_Instance dll:
 *                                       As returned from Dynlink_LoadLibrary
 *                   In: pr: Procedure name
 *
 *         Returns: Pointer to procedure, or NULL if not in library
 *
 * Post-conditions: None
 *
 *     For example: Dynlink_Instance dll = Dynlink_LoadLibrary(...);
 *                  EntryName_Proc *pointer_to_func =
 *                                      Dynlink_GetProcAddress(dll, EntryName);
 */
#define Dynlink_GetProcAddress(dll, pr)\
   (pr##_Proc *)Dynlink_GetProcAddressNoCast((dll), #pr)

typedef void Dynlink_AnyFunc(void);

typedef Dynlink_AnyFunc
 *Dynlink_GetProcAddressNoCast_Proc(Dynlink_Instance dll,
                                    char const *procname);

Dynlink_GetProcAddressNoCast_Proc Dynlink_GetProcAddressNoCast;

#ifdef _WIN32
Dynlink_GetProcAddressNoCast_Proc Dynlink_Win32_GetProcAddressNoCast;
#endif


/*
 *      Macro: DYNLINK_MAXPATH
 *    Purpose: Define buffer size required to guarantee that the path will fit
 */
#ifdef COMPILING_ON_WINDOWS
# ifdef _WINDU_SOURCE
#  ifdef COMPILING_ON_HPUX
#   include <limits.h>
#   define DYNLINK_MAXPATH (PATH_MAX)
#  else
#   include <sys/param.h>
#   define DYNLINK_MAXPATH (MAXPATHLEN)
#  endif
# else
#  include <stdarg.h>
#  define DYNLINK_MAXPATH (_MAX_PATH)
# endif
#elif defined COMPILING_ON_HPUX
  /* on HP/UX limits.h does not #define PATH_MAX unless
   * the user asks for it by #defining, e.g., _XOPEN_SOURCE and
   * _XPG4_EXTENDED.  We have a few choices, none very good:
   *
   *  [1] Compile all code on HP with _XOPEN_SOURCE and _XPG4_EXTENDED
   *      (or some equivalent like _HP_SOURCE).  This leads to namespace
   *      pollution problems (e.g. PAGE_SIZE) for files that aren't using
   *      the extra names.
   *  [2] Require files that use this one to define _XOPEN_SOURCE and
   *      _XPG4_EXTENDED or some equivalent.  They have to do it before the
   *      first use of any system header file.
   *  [3] Cheat and define the internal (to the HP/UX headers) macro
   *      _INCLUDE_XOPEN_SOURCE here.  Only users of this file get the
   *      namespace pollution and only the pollution caused by limits.h.
   *      But this must be the first use of limits.h.
   *
   * We're using 3 since it doesn't require each client of this file to do
   * anything special.
   */
  #ifndef _INCLUDE_XOPEN_SOURCE
    #define _INCLUDE_XOPEN_SOURCE 1
  #endif
  #ifndef _XPG4_EXTENDED
    #define _XPG4_EXTENDED 1
  #endif
  #include <limits.h>
  #define DYNLINK_MAXPATH (PATH_MAX)
#elif !defined NULL_DLL_SUPPORT
#include <sys/param.h>
#define DYNLINK_MAXPATH (MAXPATHLEN)
#endif

/*
 *        Function: Dynlink_LoadLibrary
 *         Purpose: Host-independent interface to load a .dll/.so/.sl
 *
 *  Pre-conditions: If argv0 is passed in as NULL, then
 *                  clx_findfile(argv[0], NULL, NULL, NULL) must have already
 *                  been called.
 *
 *          Params:
 *                   In: argv0 - Used by clx_findfile to locate the directory
 *                       of the current executable.  May be NULL, if
 *                       clx_findfile has already been primed with argv0.  If
 *                       NULL and clx_findfile has not already been primed,
 *                       then the search path will be *INCORRECT*.
 *
 *                   In: name - Pointer to filename of library to load.  This
 *                       may contain directory information.  clx_findfile will
 *                       be used to locate the library to be loaded.
 *
 *                       If the file cannot be found, we also search for a file
 *                       called "name.EXT", where EXT is a host-specific shared
 *                       object extension.
 *
 *                  Out: path - Buffer which must be at least
 *                       as large as DYNLINK_MAXPATH.  On output, this will
 *                       contain the directory (but not the filename) of the
 *                       library, without a trailing slash
 *
 *                       If NULL, no path information will be returned.
 *
 *                       *** path and name must not overlap ***
 *
 *         Returns: NULL if the call fails.  This can happen if clx_findfile
 *                  can't find the file, or if the file can't be loaded.
 *                  If the call succeeds, it returns a handle to the module,
 *                  which can be passed to Dynlink_GetProcAddress.
 *
 * Post-conditions: If successful, library has been loaded
 *                                  
 */
typedef Dynlink_Instance Dynlink_LoadLibrary_Proc(char const *argv0,
                                                  char const *name,
                                                  char *path);

Dynlink_LoadLibrary_Proc Dynlink_LoadLibrary;

#ifdef _WIN32
Dynlink_LoadLibrary_Proc Dynlink_Win32_LoadLibrary;
#endif

void Dynlink_UnloadLibrary(Dynlink_Instance dll);
#ifdef _WIN32
void Dynlink_Win32_UnloadLibrary(Dynlink_Instance dll);
#endif

/*
 *        Function: Dynlink_LastError
 *         Purpose: Return the most recent error string (if possible)
 *
 *  Pre-conditions: The previous call to Dynlink_* failed
 *
 *          Params: non
 *
 *         Returns: Either a nul-terminated character string that describes
 *                  the last error, or NULL if there is no error to report
 *
 * Post-conditions: This function may (or may not) clear the error condition,
 *                  meaning that a second call to Dynlink_LastError without any
 *                  further intervening errors may EITHER report the previous
 *                  error OR may report NULL.
 *                                  
 */
char const *Dynlink_LastError(void);

/*
 *        Function: Dynlink_SafeLastError
 *         Purpose: As per Dynlink_LastError, but will return '' instead of
 *                  NULL
 */
char const *Dynlink_SafeLastError(void);

/*
 *        Function: Dynlink_GetDLLExt
 *         Purpose: Returns host-specific DLL extension
 *
 *  Pre-conditions: None
 *
 *          Params: None
 *
 *         Returns: Pointer to a constant string
 *
 * Post-conditions: None
 *                                  
 */
typedef char const *Dynlink_GetDLLExt_Proc(void);

Dynlink_GetDLLExt_Proc Dynlink_GetDLLExt;

#ifdef _WIN32
Dynlink_GetDLLExt_Proc Dynlink_Win32_GetDLLExt;
#endif

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /* !def dynlink_h */

/* EOF dynlink.h */
