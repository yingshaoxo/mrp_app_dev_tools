/*
 * C compiler file host.h
 * Copyright (C) Codemist Ltd., 1988
 * Copyright (C) Acorn Computers Ltd., 1988.
 * Copyright (C) ARM Ltd., 1996-1999.
 */

/*
 * RCS $Revision: 1.95.4.6 $  Codemist 11
 * Checkin $Date: 2001/10/08 14:27:45 $
 * Revising $Author: dsinclai $
 */

/* AM memo, July 1990: in principle there should be no tests of         */
/* COMPILING_ON_<machine>, but only COMPILING_ON_<operating system> or  */
/* COMPILING_ON_<manufacturer> (for special features).                  */
/* Accordingly COMPILING_ON_<machine> is deprecated.                    */

/*
 * This file deals with peculiarities of the host system under which the
 * compiler is compiled AND peculiarities of the host system under which
 * the compiler will run (hence it might need further explication in the
 * unlikely event that we wish to cross-compile the compiler (repeatedly
 * as opposed to once-off bootstrap)).   It is now loaded first and can
 * therefore not depend on TARGET_xxx parameterisations, these are now
 * done in target.h or, if systematic, in mip/defaults.h.
 * The correct mechanism for host->target dependencies (e.g. if compiling
 * on unix then make a unix compiler, else a homebrew object file version)
 * is via the options.h file, along the lines of:
 *   #ifdef COMPILING_ON_UNIX
 *   #  define TARGET_IS_UNIX
 *   #endif
 * The intent is that most of the pecularities should be linked to
 * COMPILING_ON_machine and/or COMPILING_ON_system.  Further NO OTHER FILE
 * should refer to magic names like 'unix', '__arm' etc., but go via
 * the COMPILING_xxx flags defined here in terms of these.
 * The aim is that this file should suffice for all host dependencies
 * and thus all COMPILING_ON_xxx tests outwith are suspect.  However,
 * the #include file munger clearly needs to so depend.
 */

/* The bool problem:
 *   In both C and C++ we want to be able to use to use a type 'bool' and
 *   constants 'true' and 'false' (and the obsolete versions 'TRUE', 'FALSE',
 *   'YES' and 'NO').  Also, when in C++ we'd like 'bool' to mean 'bool' and
 *   'true' and 'false' to be constants of type bool.
 *   The problem is that we need to find a typedef for C that is compatible,
 *   with, i.e. that has same size and alignment as, C++'s 'bool'.  This is
 *   compilcated somewhat by some of the C++ compilers we use not supporting
 *   'bool' (Sparcworks 4.2) or not allowing '#if true' (g++ 2.95.2 and
 *   HP aCC A.01.18).
 *   We also have cases (what cases?) of published C interfaces where
 *   sizeof(bool) == sizeof(int) so we can't really make use of
 *   IMPLEMENT_BOOL_AS_{ENUM,CHAR}_IN_C.  Instead we have to as
 *   IMPLEMENT_BOOL_AS_INT in C++ (which breaks overloading, etc.).
 */

#ifndef host_h_LOADED
#define host_h_LOADED 1

/* The easiest way to ensure that host.h and angel.h cannot clash is
 * to ensure that if angel.h has been included that host.h itself
 * checks this flag and pretends it has already been included.
 * But we're careful to make sure that the '#ifndef host_h_LOADED' still
 * works as, and looks like guard.
 */
#ifndef DO_NOT_INCLUDE_HOST_H

#include <stdio.h>
#include <stddef.h>             /* for xsyn.c (and more?) and offsetof test below */
#include <stdlib.h>             /* for EXIT_FAILURE test below */
#include <time.h>

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C"
{
#endif

#ifndef SEEK_SET
  #define SEEK_SET  0
#endif
#ifndef SEEK_CUR
  #define SEEK_CUR  1
#endif
#ifndef SEEK_END
  #define SEEK_END  2
#endif

#if defined(__STDC__) || defined(__cplusplus)
    #define BELL      '\a'
    typedef void                /* newline to fool the topcc tool */
    *VoidStar;
    typedef const void
    *ConstVoidStar;
#else                           /* ! __STDC__ */
    #define BELL      '\007'
    typedef char *VoidStar;
    #define  ConstVoidStar VoidStar
    /* @@@ # define const *//*nothing */
#endif                          /* ! __STDC__ */

/* The following for the benefit of compiling on SunOS */
#ifndef offsetof
  #define offsetof(T, member)  ((char *)&(((T *)0)->member) - (char *)0)
#endif

/* Set CLOCKS_PER_SEC for non-ANSI compilers */
#ifndef CLOCKS_PER_SEC
  #ifndef CLK_TCK
    /* sunos */
    #define CLK_TCK 1000000
  #endif
  #define CLOCKS_PER_SEC CLK_TCK
#endif

#ifdef unix                     /* A temporary sop to older compilers */
  #ifndef __unix                /* (good for long-term portability?)  */
    #define __unix    1
  #endif
#endif

#ifdef __unix
/* Generic unix -- hopefully a split into other variants will not be    */
/* needed.  However, beware the 'bsd' test above and safe_toupper etc.  */
/* which cope with backwards (pre-posix/X/open) unix compatility.       */
  #define COMPILING_ON_UNIX     1
#endif
#ifdef __helios
  /* start improving parameterisation.  Maybe we should also set          */
  /* COMPILING_ON_UNIX and use HELIOS as a special subcase?               */
  #define COMPILING_ON_HELIOS   1
#endif
#ifdef __acorn
  #define COMPILING_ON_ACORN_KIT  1
#endif
#ifdef __riscos
  #define COMPILING_ON_RISC_OS  1
#endif
#ifdef __arm
  #define COMPILING_ON_ARM      1 
#endif
#ifdef __ibm370
  #ifndef COMPILING_ON_UNIX
    #define __mvs 1           
  #endif
#endif
#ifdef __mvs
  #define COMPILING_ON_MVS      1
#endif
#if defined(_WIN32)
  #define COMPILING_ON_WIN32    1
  #define COMPILING_ON_WINDOWS  1
#endif
#if defined(_CONSOLE)
  #define COMPILING_ON_WINDOWS_CONSOLE 1
  #define COMPILING_ON_WINDOWS 1
#endif
#ifdef _MSDOS
  #define COMPILING_ON_MSDOS    1
#endif
#ifdef __WATCOMC__
  #define COMPILING_ON_MSDOS    1
#endif
#ifdef _MSC_VER
  #define COMPILING_ON_MSDOS    1
  #define COMPILING_ON_WINDOWS  1
    /* Waiting here for a complete solution for Windows targets */
    /* (perhaps IMPLEMENT_BOOL_AS_UCHAR_IN_C is the complete solution?) */
  /* with VC++ 6 sizeof([real C++] bool) == 1; sizeof(enum _bool) == 4 */
  #ifdef IMPLEMENT_BOOL_AS_BOOL
    #define IMPLEMENT_BOOL_AS_UCHAR_IN_C 1
  #else
    /* Warning: we're forcing 'typedef int bool' in C++ to be compatible with */
    /* 'typedef int bool' in C (and with old objects)? */
    #if defined(__cplusplus)
      #define IMPLEMENT_BOOL_AS_INT 1
    #endif
    #define IMPLEMENT_BOOL_AS_INT_IN_C 1 /* defaulted to this below in C anyway */
  #endif
#elif defined(macintosh)
  #define COMPILING_ON_MACINTOSH 1  /* for dependencies on how Macintosh handles filenames, etc. */
  #define COMPILING_ON_MPW 1      /* for dependencies on how MPW handles arguments, i/o, etc. */
#elif defined(__SUNOS__)
  #define COMPILING_ON_SUNOS 1
#elif defined(__svr4__) || defined(__SVR4)
  #define COMPILING_ON_SVR4 1
  #define COMPILING_ON_SOLARIS 1
  #if defined(__cplusplus) && defined(__SUNPRO_CC)
    #if __SUNPRO_CC <= 0x420 /* SparcWorks C++ 4.2 doesn't have 'bool' */
      #define NO_CPP_BOOL 1
      #ifdef IMPLEMENT_BOOL_AS_BOOL /* well, we can't, so... */
        #undef IMPLEMENT_BOOL_AS_BOOL
        #define IMPLEMENT_BOOL_AS_ENUM 1 /* at least this is a unique type */
        #define _WINDU_NATIVEBOOL /* make sure windu doesn't define its own */
      #else
        #define IMPLEMENT_BOOL_AS_INT 1
      #endif
    #endif
  #endif
#elif defined(__hppa)
  #define COMPILING_ON_HPUX 1
  #ifdef __GNUC__
    /* with g++ 2.95.2 sizeof(bool) == 4; sizeof(enum _bool) == 4 (unless -fshort-enums) */
    /* @@@ maybe should use IMPLEMENT_BOOL_AS_ENUM_IN_C */
    #define IMPLEMENT_BOOL_AS_INT_IN_C 1 /* defaulted to this below in C anyway */
  #else
    /* with aCC A.01.23 sizeof(bool) == 1; sizeof(enum _bool) == 4 */
    #define IMPLEMENT_BOOL_AS_UCHAR_IN_C 1*/ /* avoid darn bool problem */
  #endif
#endif

#ifdef __linux__
  #define COMPILING_ON_LINUX 1
  #if defined(__GNUC__) && defined(__i386__)
    /* with g++ 2.95.1 sizeof(bool) == 1; sizeof(enum _bool) == 4 (unless -fshort-enums) */
    #define IMPLEMENT_BOOL_AS_UCHAR_IN_C 1 /* avoid darn bool problem */
  #endif
#endif

/* By default, the following code defines the 'bool' type to be 'int'
 * in C and real 'bool' under C++.  It also avoids warnings such as
 * (in C) "C++ keyword 'bool' used as identifier".  It can be overridden
 * by defining IMPLEMENT_BOOL_AS_ENUM, IMPLEMENT_BOOL_AS_UCHAR or
 * IMPLEMENT_BOOL_AS_INT.
 */
#undef _bool
#undef _bool_defined

typedef int bool_int; /* an int used as a bool -- the same in C and C++.  may be
                       * useful for backward compatibility
                       */

#if defined(__cplusplus)
  /* SparcWorks 4.2 C++ doesn't have bool */
  #if !defined(NO_CPP_BOOL) && (!defined(__SUNPRO_CC) || __SUNPRO_CC != 0x420)
    typedef bool cpp_bool;  /* Have a real bool type in case being overridden */
  #else
    typedef int cpp_bool;   
  #endif
#endif

#if !defined(IMPLEMENT_BOOL_AS_ENUM_IN_C) && \
    !defined(IMPLEMENT_BOOL_AS_UCHAR_IN_C) && \
    !defined(IMPLEMENT_BOOL_AS_INT_IN_C)
  #define IMPLEMENT_BOOL_AS_INT_IN_C
#endif

#ifndef __cplusplus
  #if defined(IMPLEMENT_BOOL_AS_ENUM_IN_C)
    #define IMPLEMENT_BOOL_AS_ENUM 1
  #elif defined(IMPLEMENT_BOOL_AS_UCHAR_IN_C)
    #define IMPLEMENT_BOOL_AS_UCHAR 1
  #elif defined(IMPLEMENT_BOOL_AS_INT_IN_C)
    #define IMPLEMENT_BOOL_AS_INT 1
  #else
    #error do not know how to implement bool in C
  #endif
#endif

#if defined(IMPLEMENT_BOOL_AS_ENUM)
  #if defined(true) || defined(false)
    #error implement bool as enum but true or false already defined
  #endif
  enum _bool { _false, _true };
  #define _bool enum _bool
  #define _bool_defined 1
#elif defined(IMPLEMENT_BOOL_AS_UCHAR)
  /* we use 'unsigned char' instead of plain 'char' to avoid
   * inadvertent "'char' used as subscript" warnings from gcc
   */
  typedef unsigned char _bool;
  #define _false 0 /* '((bool)0)' would give correct type in C++ but makes '#if false' fail */
  #define _true  1 /* '((bool)1)' would give correct type in C++ but makes '#if true' fail */
  #define _bool_defined 1
#elif defined(IMPLEMENT_BOOL_AS_INT) || !defined(__cplusplus)
  typedef int _bool;
  #define _false 0
  #define _true 1
  #define _bool_defined 1
#endif

#ifdef _bool_defined
  #if (defined(__cplusplus) && !defined(NO_CPP_BOOL)) || defined(__CC_ARM)
    /* avoids "'bool' is reserved word in C++" warning in C and problems in C++
     * when the compiler has a native 'bool' but we don't use it (because it's
     * not the right size).
     */
    #define bool _bool
  #else
    /* a real typedef may be better for debugging */
    typedef _bool bool;
  #endif
/* if true and false are already defined assume they will work          */
  #if !defined(true) && !defined(false)
    #define true _true
    #define false _false
  #elif !(defined(true) && defined(false))
    #error only one of true and false defined
  #endif
#endif

#ifdef __cplusplus
  /* here we check (as best we can from C++) that bool we're using in C++ is the
   * same size as the bool we use in C
   */
  #if defined(IMPLEMENT_BOOL_AS_ENUM_IN_C)
    enum _bool_for_assertion { _false, _true };
    typedef int bool_size_assertion[1/(sizeof(bool) == sizeof(_bool_for_assertion))];
  #elif defined(IMPLEMENT_BOOL_AS_UCHAR_IN_C)
    typedef int bool_size_assertion[1/(sizeof(bool) == sizeof(unsigned char))];
  #elif defined(IMPLEMENT_BOOL_AS_INT_IN_C)
    typedef int bool_size_assertion[1/(sizeof(bool) == sizeof(int))];
  #else
    #error do not know how bool is implemented in C
  #endif
#endif

/*
 * The following typedefs may need alteration for obscure host machines.
 */
#if defined(__alpha) && defined(__osf__)  /* =========================== */
/* Under OSF the alpha has 64-bit pointers and 64-bit longs.            */
    typedef int int32;
    typedef unsigned int unsigned32;
/* IPtr and UPtr are 'ints of same size as pointer'.  Do not use in     */
/* new code.  Currently only used within 'ncc'.                         */
    typedef long int IPtr;
    typedef unsigned long int UPtr;

#else                           /* all hosts except alpha under OSF ============================= */

    typedef long int int32;
    typedef unsigned long int unsigned32;
/* IPtr and UPtr are 'ints of same size as pointer'.  Do not use in     */
/* new code.  Currently only used within 'ncc'.                         */
    #define IPtr int32
    #define UPtr unsigned32
#endif                          /* ============================================================= */

typedef short int int16;
typedef unsigned short int unsigned16;
typedef signed char int8;
typedef unsigned char unsigned8;
/*
 * HGB 22-July-97 - signedxx synomyms for intxx
 */
typedef int32 signed32;
typedef int16 signed16;
typedef int8 signed8;


#define YES   true
#define NO    false
#undef TRUE                     /* some OSF headers define as 1         */
#undef FALSE                    /* some OSF headers define as 0         */
#if defined(__cplusplus) && (defined(__GNUC__) || defined(__HP_aCC))
   /* needed for  g++ 2.95.2 and HP aCC A.01.18  */
  #define TRUE  1
  #define FALSE 0
#else
  #define TRUE  true
  #define FALSE false
#endif

/* 'uint' conflicts with some Unixen sys/types.h, so we now don't define it */
typedef unsigned8 uint8;
typedef unsigned16 uint16;
typedef unsigned32 uint32;

typedef unsigned Uint;
typedef unsigned8 Uint8;
typedef unsigned16 Uint16;
typedef unsigned32 Uint32;

#ifdef ALPHA_TASO_SHORT_ON_OSF  /* was __alpha && __osf.                */
/* The following sets ArgvType for 64-bit pointers so that              */
/* DEC Unix (OSF) cc can be used with the -xtaso_short compiler option  */
/* to force pointers to be 32-bit.  Not currently used since most/all   */
/* ARM tools accept 32 or 64 bit pointers transparently.  See IPtr.     */
#pragma pointer_size (save)
#pragma pointer_size (long)
    typedef char *ArgvType;
#pragma pointer_size (restore)
#else
    typedef char *ArgvType;
#endif

/*
 * Rotate macros
 */
#define ROL_32(val, n) \
((((unsigned32)(val) << (n)) | ((unsigned32)(val) >> (32-(n)))) & 0xFFFFFFFFL)
#define ROR_32(val, n) \
((((unsigned32)(val) >> (n)) | ((unsigned32)(val) << (32-(n)))) & 0xFFFFFFFFL)

/* For many hosts, this can be defined as just ((int32)val)>>(n) */
#define signed_rightshift_(val, n) \
((int32)(val)>=0 ? (val)>>(n) : ~((~(val))>>(n)))

/* The following two lines are a safety play against using ncc with a   */
/* vendor supplied library, many of which refuse to accept "wb".  POSIX */
/* and other unix standards require "wb" and "w" to have the same       */
/* effect and so the following is safe.                                 */
#ifdef COMPILING_ON_UNIX
#define FOPEN_WB     "w"
#define FOPEN_RB     "r"
#define FOPEN_RWB    "r+"
#if !defined(__STDC__) && !defined(__cplusplus)  /* caveat RISCiX... */
#define remove(file) unlink(file)  
#endif
#else
#define FOPEN_WB     "wb"
#define FOPEN_RB     "rb"
#define FOPEN_RWB    "rb+"
#endif

#ifdef COMPILING_ON_HPUX
/* HPUX defines FILENAME_MAX as 14 !!! */
#include <stdio.h>
#undef FILENAME_MAX
#define FILENAME_MAX 256
#endif

#ifndef FILENAME_MAX
#define FILENAME_MAX 256
#endif

#ifdef COMPILING_ON_SUNOS
    typedef int sig_atomic_t;
/* Use bcopy rather than memmove, as memmove is not available.     */
/* There does not seem to be a header for bcopy.                   */
    void bcopy(const char *src, char *dst, int length);
#define memmove(d,s,l) bcopy((char const *)s,(char *)d,l)

/* BSD/SUN don't have strtoul() */
#define strtoul(s, ptr, base) strtol(s, ptr, base)

/* strtod is present in the C-library but is not in stdlib.h       */
    extern double strtod(const char *str, char **ptr);

/* This isn't provided by SunOS */
    extern char *strerror(int /*errnum */ );

/*
 * not all C libraries define tolower() and toupper() over all character
 * values. BSD Unix, for example, defines tolower() only over UC chars.
 */
#define safe_tolower(ch)      (isupper(ch) ? tolower(ch) : ch)
#define safe_toupper(ch)      (islower(ch) ? toupper(ch) : ch)
#else                           /* ! COMPILING_ON_SUNOS */
#define safe_tolower(ch)      tolower(ch)  /* see comment below */
#define safe_toupper(ch)      toupper(ch)  /* see comment below */
#endif                          /* ! COMPILING_ON_SUNOS */

#ifdef __CC_NORCROFT
#ifndef COMPILING_ON_UNIX       /* suppress if maybe non-norcroft lib */
#define LIBRARY_IS_NORCROFT 1
#endif
#ifdef LIBRARY_IS_ALIEN         /* unify with the 3 previous lines?   */
#undef LIBRARY_IS_NORCROFT
#endif
#endif

#ifdef LIBRARY_IS_NORCROFT
/*
 * Calls to all non-ansi functions are removable by macros here.
 */
#ifdef __cplusplus
    extern "C"
    {
#endif
/* These routines have no floating point abilities.                     */
/* one of these is used by clx/disass.c it doesn't really seem worth the trouble */
        extern int _vfprintf(FILE * stream, const char *format, __va_list arg);
        extern int _vsprintf(char *s, const char *format, __va_list arg);
        extern int _fprintf(FILE * stream, const char *format,...);
        extern int _sprintf(char *s, const char *format,...);
#ifdef __cplusplus
    }
#endif
#else                           /* LIBRARY_IS_NORCROFT */
#define _vfprintf vfprintf
#define _vsprintf vsprintf
#define _fprintf fprintf
#define _sprintf sprintf
#endif                          /* LIBRARY_IS_NORCROFT */

#ifdef COMPILING_ON_MVS

#define HOST_USES_CCOM_INTERFACE 1
#define EXIT_warn                4
#define EXIT_error               8
#define EXIT_fatal              12
#define EXIT_syserr             16

#else                           /* ! COMPILING_ON_MVS */

#define EXIT_warn                0
#define EXIT_error               1
#define EXIT_fatal               1

#ifdef  COMPILING_ON_UNIX
#define EXIT_syserr            100
#else
#define EXIT_syserr            1
#endif

#endif                          /* ! COMPILING_ON_MVS */

/* For systems that do not define EXIT_SUCCESS and EXIT_FAILURE */
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS           0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE           EXIT_error
#endif

#ifdef COMPILING_ON_MPW
#include <CursorCtl.h>
#define UPDATE_PROGRESS()     SpinCursor(1)
#endif

#ifndef UPDATE_PROGRESS
#define UPDATE_PROGRESS() ((void)0)
#endif

#if defined(__STDC__) || defined(__cplusplus)
#define UNUSEDARG(X) ((void)(X)) /* Silence compiler warnings for unused arguments */
#else
#define UNUSEDARG(X) ((X) = (X))
#endif

#ifndef IGNORE 
#define IGNORE(x)  (UNUSEDARG(x))
#endif

/* Define endian-ness of host */

#if defined(__sparc) || defined(macintosh) || defined(__hppa)
#define HOST_ENDIAN_BIG
#elif defined(__acorn) || defined(__mvs) || defined(_WIN32) || \
  (defined(__alpha) && defined(__osf__)) || defined(__i386)
#define HOST_ENDIAN_LITTLE
#else
#define HOST_ENDIAN_UNKNOWN
#endif

/* Set INLINE to the correct inlining rules, if supported by the host */

#ifdef __cplusplus
  #define INLINE inline
#elif defined (__GNUC__)
  #define INLINE __inline__
#elif defined (COMPILING_ON_WINDOWS) && !defined(_WINDU_SOURCE)
  #define INLINE __inline
#elif defined (COMPILING_ON_SEMI)
  #define INLINE __inline
#else
  #define INLINE
#endif

#ifdef __cplusplus
  #if defined(__CC_ARM)
      #define HAS_NAMESPACES 0
      #define USE_STD_NAMESPACE 0
  #elif defined(__HP_aCC)
      /* HP aCC seems to support namespaces but it's version of the RW library doesn't
       * place the names there
       */
      #define HAS_NAMESPACES 1
      #define USE_STD_NAMESPACE 0
  #else
      /* Assume standard conformance including g++ and VC++
       * g++ seems to silently treat things like 'std::foo' as '::foo'
       * and sliently ignores 'using namespace std;' and 'using std::foo;'
       * so we could set USE_STD_NAMESPACE either way
       */       
      #define HAS_NAMESPACES 1
      #define USE_STD_NAMESPACE 1
  #endif

  #if USE_STD_NAMESPACE
    #define STD std
  #else
    #define STD /*nothing*/
  #endif
#endif


/*
 * 990506 KWelton
 *
 * We now have native int64 on all platforms - use it
 */
#if defined(__CC_ARM)
/*
 * long long is not available with -strict,
 * (and neither are long long constants).
 */
    typedef __int64 int64;
    typedef unsigned __int64 uint64;

#elif defined(COMPILING_ON_UNIX)
/*
 * Solaris  (EGCS), HP/UX, and ARM compilers
 */
/*__extension__*/ typedef long long int64;
/*__extension__*/ typedef unsigned long long uint64;
#elif defined(COMPILING_ON_WINDOWS)
/*
 * Windoze
 */
    typedef __int64 int64;
#ifndef __LCLINT__
    typedef unsigned __int64 uint64;
#else
    typedef __int64 uint64;
#endif

#else                           /* none of the above */
/*
 *  winDU build system - doesn't use "int64"
 */
#endif

#if defined(COMPILING_ON_UNIX) || defined(COMPILING_ON_SEMI)
  #define INT64PRId "lld"         /* like C99 PRId64 */
  #define INT64PRIu "llu"         /* like C99 PRIu64 */
#elif defined(COMPILING_ON_WIN32)
  #define INT64PRId "I64d"        /* like C99 PRId64 */
  #define INT64PRIu "I64u"        /* like C99 PRIu64 */
#else
  #error "platform not supported"
#endif

void int64tos(char *str, int64 number);
void uint64tos(char *str, uint64 number);

/* Allow functions to be exported from DLLs without .def files. */
#ifndef CLX_EXPORTED
#ifdef COMPILING_ON_WINDOWS
# define CLX_EXPORTED __declspec(dllexport)
#else
# define CLX_EXPORTED
#endif
#endif

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif                          /* ndef DO_NOT_INCLUDE_HOST_H */

#endif                          /* host_h */

/* end of host.h */
