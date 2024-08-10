/* -*-C-*-
 *
 * ARM symbolic debugger toolbox interface
 *
 * Copyright 1992-1999 ARM Limited.
 * All Rights Reserved.
 */

/*
 * RCS $Revision: 1.186.2.6 $
 * Checkin $Date: 2001/08/20 14:30:08 $
 * Revising $Author: mmladeno $
 */

/* Minor points of uncertainty are indicated by a question mark in the
   LH margin.

 Iterators vs. Enumerators
 =========================

   Wherever an interface is required to iterate over things of some class,
   I prefer something of the form  EnumerateXXs(..., XXProc *p, void *arg)
   which results in a call of p(xx, arg) for each xx, rather than something
   of the form
     for (xxh = StartIterationOverXXs(); (xx = Next(xxh)) != 0; ) { ... }
     EndIterationOverXXs(xxh);
   Others may disagree.
   (Each XXProc returns an Error value: if this is not Err_OK, iteration
   stops immediately and the EnumerateXXs function returns that value).

 Callbacks
 =========

   Callbacks to be called on specific events are provided through pairs of
   functions:

       typedef void XXProc(state, void *arg, ...);
       Dbg_OnXX(state, XXProc *p, void *arg);
       Dbg_RemoveOnXX(state, XXProc *p, void *arg);

   registers a function 'p' to be called with argument 'arg' (and event-
   specific arguments) whenever XX happens. If 'p'/'arg' have already been
   supplied as a callback, the function is not registered again. The Remove
   function will return an error if 'p'/'arg' isn't found (e.g. if it has
   already been deregistered.

   ptrace has been retired as of insufficient utility.  If such fuctionality is
   required, it can be constructed using breakpoints.

   The file form of all name fields in debug areas is in-line, with a length
   byte and no terminator.  The debugger toolbox makes an in-store translation,
   where the strings are out of line (the namep variant in asdfmt.h) and have a
   terminating zero byte: the pointer is to the first character of the string
   with the length byte at ...->n.namep[-1].
 */

#ifndef armdbg_h
#define armdbg_h

/*#include <dbg_opti.h>*/

#define RDI_VERSION 151

#include <stddef.h>

#include "host.h"
#include "ieeeflt.h"
#include "msg.h"
#include "armtypes.h"
#include "toolconf.h"
#include "int64.h"

typedef unsigned char Dbg_Byte;

typedef int Dbg_Error;

typedef struct Dbg_MCState Dbg_MCState;
/* A representation of the state of the target.  The structure is not revealed.
   A pointer to one of these is returned by Dbg_Initialise(), is passed to all
   toolbox calls which refer to the state of the target, and is discarded
   by Dbg_Finalise().
   Nothing in the toolbox itself precludes there being multiple targets, each
   with its own state.
 */

#include "rdi_conf.h"
#include "rdi.h"
#include "rdi_rti.h"            /* Trace extensions */
#include "rdi_sdm.h"            /* self describing modules */

#ifdef __cplusplus
extern "C"
{
#endif

/* Most toolbox interfaces return an error status.  Only a few of the status
   values are expected to be interesting to clients and defined here; the
   rest are private (but a textual form can be produced by ErrorToChars()).
 */

#define Error_OK 0

/* Partitioning of the error code space: errors below Dbg_Error_Base are RDI
   errors (as defined in rdi.h). Codes above Dbg_Error_Limit are
   available to clients, who may impose some further structure.
 */
#define Dbg_Error_Base (RDIError_TargetErrorTop + 1)
#define Dbg_Error_Limit 0x2000

#define DbgError(n) ((Dbg_Error)(Dbg_Error_Base+(n)))

#define Dbg_Err_OK Error_OK
#define Dbg_Err_Interrupted             DbgError(1)
#define Dbg_Err_Overflow                DbgError(2)
#define Dbg_Err_FileNotFound            DbgError(3)
#define Dbg_Err_ActivationNotPresent    DbgError(4)
#define Dbg_Err_OutOfHeap               DbgError(5)
#define Dbg_Err_TypeNotSimple           DbgError(6)
#define Dbg_Err_BufferFull              DbgError(7)
#define Dbg_Err_AtStackBase             DbgError(8)
#define Dbg_Err_AtStackTop              DbgError(9)
#define Dbg_Err_DbgTableFormat          DbgError(10)
#define Dbg_Err_NotVariable             DbgError(11)
#define Dbg_Err_NoSuchBreakPoint        DbgError(12)
#define Dbg_Err_NoSuchWatchPoint        DbgError(13)
#define Dbg_Err_FileLineNotFound        DbgError(14)
#define Dbg_Err_DbgTableVersion         DbgError(15)
#define Dbg_Err_NoSuchPath              DbgError(16)
#define Dbg_Err_StateChanged            DbgError(17)
#define Dbg_Err_SoftInitialiseError     DbgError(18)
#define Dbg_Err_RegisterNotWritable     DbgError(19)
#define Dbg_Err_NotInHistory            DbgError(20)
#define Dbg_Err_ContextSyntax           DbgError(21)
#define Dbg_Err_ContextNoLine           DbgError(22)
#define Dbg_Err_ContextTwoLines         DbgError(23)
#define Dbg_Err_VarReadOnly             DbgError(24)
#define Dbg_Err_FileNewerThanImage      DbgError(25)
#define Dbg_Err_NotFound                DbgError(26)
#define Dbg_Err_DeviceNotFound          DbgError(27)
#define Dbg_Err_ExprNotFound            DbgError(28)
#define Dbg_Err_ExprUnexpected          DbgError(29)
#define Dbg_Err_DbgTable_Corrupt        DbgError(30)
#define Dbg_Err_BadStack                DbgError(31)
#define Dbg_Err_NotAvailable            DbgError(32)
#define Dbg_Err_Ambiguous               DbgError(61)
#define Dbg_Err_InadequateTarget        DbgError(109)
#define Dbg_Err_FileInfo_Corrupt        DbgError(130)
#define Dbg_Err_DwarfForwardRef         DbgError(133)
#define Dbg_Err_EmptyStatement          DbgError(134)
#define Dbg_Err_BPinLitPool             DbgError(135)
#define Dbg_Err_NoEntryPoint            DbgError(136)
#define Dbg_Err_BPBadAlignment          DbgError(137)
#define Dbg_Err_NotExecReadable         DbgError(138)
#define Dbg_Err_CantDebugNonStop        DbgError(139)
#define Dbg_Err_MixedImage              DbgError(146)


   /* functions which evaluate expressions may return this value, to indicate
      that execution became suspended within a function called in the debugee */

/* Functions returning characters take a BufDesc argument, with fields buffer
   and bufsize being input arguments describing the buffer to be filled, and
   filled being set on return to the number of bytes written to the buffer
   (omitting the terminating 0).
 */

typedef struct Dbg_BufDesc Dbg_BufDesc;

typedef void Dbg_BufferFullProc(Dbg_BufDesc *bd);

struct Dbg_BufDesc {
    char *buffer;
    size_t size,
           filled;
    Dbg_BufferFullProc *p;
    void *arg;
};

#define Dbg_InitBufDesc(bd, buf, bytes) \
    ((bd).buffer = (buf), (bd).size = (bytes), (bd).filled = 0,\
     (bd).p = NULL, (bd).arg = NULL)

#define Dbg_InitBufDesc_P(bd, buf, bytes, fn, a) \
    ((bd).buffer = (buf), (bd).size = (bytes), (bd).filled = 0,\
     (bd).p = (fn), (bd).arg = (a))

Dbg_Error Dbg_StringToBuf(Dbg_BufDesc *buf, char const *s);
Dbg_Error Dbg_BufPrintf(Dbg_BufDesc *buf, char const *form, ...);
#ifdef NLS
Dbg_Error Dbg_MsgToBuf(Dbg_BufDesc *buf, msg_t t);
Dbg_Error Dbg_BufMsgPrintf(Dbg_BufDesc *buf, msg_t form, ...);
#else
#define Dbg_MsgToBuf Dbg_StringToBuf
#define Dbg_BufMsgPrintf Dbg_BufPrintf
#endif
Dbg_Error Dbg_CharToBuf(Dbg_BufDesc *buf, int ch);

int Dbg_CIStrCmp(char const *s1, char const *s2);
/* Case-independent string comparison, interface as for strcmp */

int Dbg_CIStrnCmp(char const *s1, char const *s2, size_t n);
/* Case-independent string comparison, interface as for strncmp */

void Dbg_ErrorToChars(Dbg_MCState *state, Dbg_Error err, Dbg_BufDesc *buf);

typedef int Dbg_RDIResetCheckProc(int);
/* Type of a function to be called after each RDI operation performed by the
   toolbox, with the status from the operation as argument.  The value returned
   is treated as the status.  (The intent is to allow the toolbox's client to
   take special action to handle RDIDbg_Error_Reset).
 */

#if 0                           /* No longer used */
#include "rdi_cp.h"
typedef Dbg_Error Dbg_CoProFoundProc(
   Dbg_MCState *state, int cpno, RDI_CoProDesc const *cpd);
/* Type of a function to be called when the shape of a coprocessor is
 * discovered by enquiry of the target or agent (via RequestCoProDesc)
 */
#endif

Dbg_Error Dbg_RequestReset(Dbg_MCState *);

Dbg_Error Dbg_Initialise(
    toolconf config, RDI_HostosInterface const *i,
    Dbg_RDIResetCheckProc *checkreset,
    RDI_ProcVec const *rdi, Dbg_MCState **statep,
    RDI_OnErrorProc *onerrorproc);
/* values in config are updated if they call for default values */

/* Register a function to be called when Dbg_Finalise is called. This
 * function should probably not call back into the Debug Toolbox, as the
 * Toolbox will be mid-finalisation.
 */
typedef void Dbg_FinaliseProc(void *handle);
Dbg_Error Dbg_OnFinalise(
    Dbg_MCState *state, Dbg_FinaliseProc *p, void *handle);
Dbg_Error Dbg_RemoveOnFinalise(
    Dbg_MCState *state, Dbg_FinaliseProc *p, void *handle);

void Dbg_Finalise(
    Dbg_MCState *state, bool targetdead, RDI_OnErrorProc *onerrorproc);

/*--------------------------------------------------------------------------*/

/* Dbg_VerifyMemMap reads and parses a ".map" file, and returns an error if the
 * file doesn't exist / has a syntax error of some kind.
 */
Dbg_Error Dbg_VerifyMemMap(char const *fname);

typedef struct {
    char name[16];
    RDI_MemDescr md;
    RDI_MemAccessStats a;
} Dbg_MemStats;

/*--------------------------------------------------------------------------*/

/* Extra processor support */

#if 0                           /* deprecated */
Dbg_Error Dbg_AttachDevice(
    Dbg_MCState *parent, toolconf config, RDI_HostosInterface const *i,
    Dbg_RDIResetCheckProc *checkreset, RDI_ProcVec const *rdi, Dbg_MCState **statep);
/* attach another processor/RDI */
/* may return WrongByteSex if the device comes up in a different bytesex to the parent */
#endif

const char *Dbg_DeviceName(Dbg_MCState *);
/* return the name of a device */

typedef enum {
  Dbg_Device_Unknown,           /* DBT does not know about this device */
  Dbg_Device_ARM,               /* Device is an ARM processor */
  Dbg_Device_Piccolo,           /* Device is a Piccolo DSP processor */
  Dbg_Device_ETM,               /* Device is an Embedded Trace Module */
  Dbg_Device_XScaleTrace        /* Device is an on-chip XScale trace buffer */
} Dbg_DeviceType;               /* Enumeration of supported device types */

Dbg_DeviceType Dbg_GetDeviceType(Dbg_MCState *);
/* return the type of a device */

typedef Dbg_Error Dbg_DeviceProc(
    Dbg_MCState *state, Dbg_MCState *device, void *arg);

Dbg_Error Dbg_EnumerateDevices(
    Dbg_MCState *state, Dbg_DeviceProc *p, void *arg);
/* Call  p(state, its_state, arg)  for each device attached to the debugger,
   in no particular order 
 */


/*--------------------------------------------------------------------------*/

/* Enumeration used by $target_fpu variable.
 * (also used in Dbg_ImageDesc) */

typedef enum {
    Dbg_TargetFPU_Unknown,     /* NOT a valid value for $target_fpu */
    Dbg_TargetFPU_SoftVFP,     /* -fpu SoftVFP */
    Dbg_TargetFPU_SoftFPA,     /* -fpu SoftFPA */
    Dbg_TargetFPU_VFP,         /* -fpu VFP */
    Dbg_TargetFPU_FPA,         /* -fpu FPA */
    Dbg_TargetFPU_AMP          /* -fpu AMP */
} Dbg_TargetFPU_Enum;

#define Dbg_TargetFPU_EnumFirst Dbg_TargetFPU_SoftVFP
#define Dbg_TargetFPU_EnumLast Dbg_TargetFPU_AMP

/*--------------------------------------------------------------------------*/

/* Symbol table management.
   The structure of a Dbg_SymTable is not revealed.  It is created by
   Dbg_ReadSymbols() or by Dbg_LoadFile(), and associated with the argument
   Dbg_MCState.
   Many symbol tables may be concurrently active.
   A Dbg_SymTable is removed either explicitly (by call to Dbg_DeleteSymbols)
   or implicitly when a symbol table for an overlapping address range is read.

   There is a pre-defined symbol table containing entries for ARM registers,
   co-processor registers and the like.
 */

typedef struct Dbg_SymTable Dbg_SymTable;

typedef struct Dbg_ImageFragmentDesc {
    ARMaddress base, limit;
} Dbg_ImageFragmentDesc;

typedef struct Dbg_ImageExecRegion
{
  /* 
     The base and limit give the position of the exec region when loaded
  */ 
     
    ARMaddress base, limit;
  /* 
     The offset is the difference between the load location and 
     the execute location of the region.
  */
    long offset;
} Dbg_ImageExecRegion;

typedef enum {
    Dbg_Lang_None,
    Dbg_Lang_C,
    Dbg_Lang_Pascal,
    Dbg_Lang_Fortran,
    Dbg_Lang_Asm,
    Dbg_Lang_Cpp
} Dbg_Lang;

typedef struct Dbg_ImageDesc {
    Dbg_Lang lang;      /* language of entry point */
    unsigned langfound; /* languages found (bits according to Dbg_Lang) */
    int executable;
    Dbg_TargetFPU_Enum target_fpu_guess; /* may be Dbg_TargetFPU_Unknown */
    bool target_fpu_mismatch;
    ARMaddress robase, rolimit, rwbase, rwlimit;
    int nfrags;
    Dbg_ImageFragmentDesc *fragments;
    char *name;
    int nexec_regions;
    Dbg_ImageExecRegion *exec_regions; /* List of regions of the image that contain executable 
                                          instructions and nothing writable (required for memory 
                                          caching) */
} Dbg_ImageDesc;

Dbg_ImageDesc *Dbg_ImageAreas(Dbg_SymTable *st);

Dbg_SymTable *Dbg_LastImage(Dbg_MCState *state);

Dbg_SymTable *Dbg_NewSymTable(Dbg_MCState *state, const char *name);

Dbg_Error Dbg_ReadSymbols(Dbg_MCState *state, const char *filename, Dbg_SymTable **st);
/* Just read the symbols from the named image.  <st> is set to the allocated
   symbol table.
?  Maybe we could usefully allow other formats than AIF images to describe
   the symbols (eg) of shared libraries
 */

typedef struct Dbg_SymInfo {
    int isize;
    ARMaddress addr;
    char const *name;
    bool is_thumb_bl;
    bool is_data;
/* private to dbg: */
    bool is_pointer_to_function;
    bool is_veneer;
    bool is_sized;
    bool is_variable_function_call;
} Dbg_SymInfo;

Dbg_SymInfo *Dbg_AsmSym(Dbg_SymTable *st, ARMaddress addr);
int32 Dbg_AsmAddr(Dbg_SymTable *st, int32 line);
int32 Dbg_AsmLine(Dbg_SymTable *st, ARMaddress addr);
int32 Dbg_AsmLinesInRange(Dbg_SymTable *st, ARMaddress start, ARMaddress end);

Dbg_Error Dbg_LoadFile(Dbg_MCState *state, const char *filename, Dbg_SymTable **st);
/* load the image into target memory, and read its symbols.  <st> is set to
   the allocated symbol table.
   A null filename reloads the most recently loaded file (and rereads its
   symbols).
   Loading an image leaves breakpoints unchanged.  If a client wishes
   otherwise, it must remove the breakpoints explicitly.
*/
Dbg_Error Dbg_Reload(Dbg_MCState *state, Dbg_SymTable *st);
/* reload already loaded image into memory (i.e. only load into target memory, do not load
 * symbols).
 */

Dbg_Error Dbg_LoadFileToCache(Dbg_MCState *state, const char *filename, Dbg_SymTable **st);
/* loads the image into the memory cache without loading it to target memory, and read its symbols.  
   <st> is set to the allocated symbol table.
   A null filename reloads the most recently loaded file (and rereads its
   symbols).
   Loading an image leaves breakpoints unchanged.  If a client wishes
   otherwise, it must remove the breakpoints explicitly.
*/

Dbg_Error Dbg_CallGLoadFile(Dbg_MCState *state, const char *filename, Dbg_SymTable **st);

typedef void Dbg_ImageLoadProc(Dbg_MCState *, void *arg, Dbg_SymTable *);
Dbg_Error Dbg_OnImageLoad(Dbg_MCState *, Dbg_ImageLoadProc *, void *arg);
/* Register function to be called back whenever an image is loaded, or symbols
 * for an image read. (To allow multiple toolbox clients to coexist).
 * 'arg' is passed in to the callback.
 */
Dbg_Error Dbg_RemoveOnImageLoad(Dbg_MCState *, Dbg_ImageLoadProc *, void *);

Dbg_Error Dbg_LoadAgent(Dbg_MCState *state, const char *filename);
/* Load a debug agent, and start it.
   Symbols in the image for the agent are ignored.
*/

Dbg_Error Dbg_RelocateSymbols(Dbg_SymTable *st, ARMaddress reloc);
/* add <reloc> to the value of all symbols in <st> describing absolute memory
   locations.  The intent is to allow the symbols in a load-time relocating
   image (for example) to be useful.
 */

Dbg_Error Dbg_DeleteSymbols(Dbg_MCState *state, Dbg_SymTable **st);

typedef Dbg_Error Dbg_SymProc(
    Dbg_MCState *state,
    const char *symbol, ARMaddress value,
    void *arg);

Dbg_Error Dbg_EnumerateLowLevelSymbols(
    Dbg_MCState *state, const char *match, Dbg_SymProc *p,
    void *arg);
/* Call  p(name, value)  for each low level symbol in the tables of <state>
   whose name matches the regular expression <match> (a NULL <match> matches
   any name).  Symbols are enumerated in no particular order.
 */

/*--------------------------------------------------------------------------*/

/* Functions are provided here to allow quick mass access to register values
   for display.  There is no comparable need for quick mass update, so writing
   should be via Assign().
 */

/* bits in the modemask argument for ReadRegisters */

/*
#define Dbg_MM_User     1
#define Dbg_MM_FIQ      2
#define Dbg_MM_IRQ      4
#define Dbg_MM_SVC      8
#define Dbg_MM_Abort 0x10
#define Dbg_MM_Undef 0x20
#define Dbg_MM_System 0x40
 */

/*
 * Return the registers for a given mode.
 * if "curmode" != "mode" then return only the banked registers for that
 *   mode.
 * - A command-line debugger would pass "curmode" as the current mode.
 * - A windows debugger would pass "curmode" and "mode" the same.
 * The number of registers returned can be determined by calling
 *   Dbg_RegistersForMode()
 * regs is assumed to be "ARMword *" aligned
 */
Dbg_Error Dbg_ReadRegisters(Dbg_MCState *state, ARMword *regs,
                            unsigned int mode, unsigned int curmode);

Dbg_Error Dbg_WriteRegister(
    Dbg_MCState *state, unsigned int rno, ARMword const *val,
    unsigned int mode);

/*
 * Type of a function to format a register.
 */
typedef Dbg_Error Dbg_FormatFn(
    Dbg_MCState *state, int decode, char *b, ARMword *valp, void *formatarg);

/*
 * A generic "how to format a register" type.
 * You can't specify this for anything other than co-processor
 * registers (which the debugger may not know how to format already),
 *
 * The list constists of an array of Dbg_RegisterFormat elements, terminated
 * by 'Last'
 * Each element has 4 arguments to specify the part of the register to be formatted,
 * and a pointer (part of a union) which specifies data to be used for formatting.
 */

enum {
  Dbg_RegisterFormat_Last,
  /* 'Last' terminates the list, or a sub-list. the */

  Dbg_RegisterFormat_String,
  /* 'String' represents a literal string. the string is ignored, and
   * no data from the register is used. */

  Dbg_RegisterFormat_Word,
  /* 'Word' specifies that an entire word is to be formatted. 'wordno'
   * specifies which word - the bits are ignored. 's' specifies a
   * printf-format string which can be used. Valid printf-formats may
   * become restricted. */

  Dbg_RegisterFormat_Bits,
  /* 'Bits' specfies that a sub-portion of 'wordno' (defined by the
   * 'lowbit' and 'highbit', and rotated by 'rotate') are to be
   * formatted, using the printf format string 's'. */

  Dbg_RegisterFormat_Enum,
  /* As for 'Bits', execpt that the resulting value is used as an
   * index into the array 'tab' for the resulting string. */

  Dbg_RegisterFormat_FBits,
  /* 's' is a string of (lower-case) characters, one for each of the
   * specified bits (as defined for 'Bits'). Each character is the
   * 'name' of that bit. */

  Dbg_RegisterFormat_FBitsR,
  /* as 'FBits', but the bits are reversed */

  Dbg_RegisterFormat_Flag,
  /* The resulting value (typically only one bit) switches between two
   * sublists (each terminated by 'Last').  The first is the 'value is
   * non-zero' case, the second for 'zero'. 's' may be NULL, but
   * otherwise points to a 'name' for this flag */

  Dbg_RegisterFormat_FPRegVal,
  /* The specified word, and subsequent words, form a Dbg_FPRegVal, to
   * be formatted with FPRegToDouble */

  Dbg_RegisterFormat_DoubleFPRegVal,
  /* The specified word, and the subsequent word, form a double precision
   * value */

  Dbg_RegisterFormat_DoubleFPRegValRaw,
  /* The specified word, and the subsequent word, form a double precision
   * value to be formatted in raw hex (eg 0x3ff80000.00000000) */

  Dbg_RegisterFormat_SingleFPRegVal,
  /* The specified word forms a single precision value */

  /* These formats are NOT YET SUPPORTED, but included for discussion */
  Dbg_RegisterFormat_FBit,
  /* 's' points to a (lower-case) string which names the value
   * (typically only one bit) */

  Dbg_RegisterFormat_Group,
  /* This entry consumes no bits, but specifies a sub-grouping in the
   * register format. It is followed by a sublist. */

  Dbg_RegisterFormat_LASTUSED
};

typedef struct {
  union {
    char *s;                    /* string/printf/bits */
    char **tab;                 /* table of enumeration */
  } s;
  char type,                    /* one of the above enumeration */
       wordno,
       lowbit,                  /* range of bits, if necessary */
       highbit,
       rotate;                  /* number of bits to rotate (right) by */
} Dbg_RegisterFormat;

#define Dbg_Access_Readable  1
#define Dbg_Access_Writable  2

#define Dbg_DeviceRegister_Newline  0x10 /* output a newline when displaying */
#define Dbg_DeviceRegister_Table    0x20 /* put up to a tabstop */
#define Dbg_DeviceRegister_VFPTable 0x60 /* put up to different tabstops */
#define Dbg_DeviceRegister_SKIP    0x100 /* just move (for overlapping regs) */

typedef struct {
  const char *name;
  unsigned int flags;           /* READABLE/WRITABLE/NEWLINE */
  unsigned long reg;            /* reg number for writing back value */
  int size;                     /* size in bytes-all values are word-aligned */
  Dbg_FormatFn *format;         /* format function */
  void *formatarg;              /* argument to pass to the format fn */
} Dbg_DeviceRegisterSpec;

/*
 * see above for treatment of mode/curmode
 *
 * This, and similar functions, fill in p and n accordingly. Either value
 * may be NULL (for example, if all you want is the error code)
 */
Dbg_Error Dbg_RegistersOfMode(
    Dbg_MCState *state, Dbg_DeviceRegisterSpec const **p, unsigned int *n,
    const char *format, unsigned int mode, unsigned int curmode);

typedef struct {
  const char *name;
  unsigned int mode;            /* mode number, for passing to ReadRegisters */
} Dbg_DeviceModesSpec;

Dbg_Error Dbg_Modes(
    Dbg_MCState *state, Dbg_DeviceModesSpec const **, unsigned int *n);

unsigned int Dbg_CurrentMode(Dbg_MCState *state);

/* returns a signed int - result is either +ve (valid mode) [including 0]
 * or -ve, indicating an invalid mode name passed in.
 */
int Dbg_StringToMode(Dbg_MCState *, const char *name);

/* int Dbg_ModeToModeMask(Dbg_MCState *, unsigned int mode); */

/* Some implementations of the FP instruction set keep FP values loaded into
   registers in their unconverted format, converting only when necessary.
   Some RDI implementations deliver these values uninterpreted.
   (For the rest, register values will always have type F_Extended).
 */

/* returns a signed int - result is either +ve (valid register)
 * [including -], or -ve, indicating an invalid register name passed in.
 */
int Dbg_StringToRegister(Dbg_MCState *, char const *name, unsigned mode);

/* floating-point type is an anonymous structure */
typedef struct Dbg_FPRegVal { ARMword w[4]; } Dbg_FPRegVal;

/*
 * Read returns a block of words, either part of a Dbg_FPRegVal (above), or
 * a word (e.g. a PSR) - as determined by the "type" field returned by
 * FPRegisters.
 * Write: writes one register.
 */
Dbg_Error Dbg_ReadFPRegisters(Dbg_MCState *state, ARMword *regs);

Dbg_Error Dbg_WriteFPRegister(
    Dbg_MCState *state, unsigned long rno, ARMword *val);

Dbg_Error Dbg_FPRegisters(
    Dbg_MCState *state, Dbg_DeviceRegisterSpec const **p, unsigned int *n,
    const char *format);


Dbg_Error Dbg_FPRegToDouble(Dbg_MCState *, DbleBin *d, Dbg_FPRegVal const *f);
/* Converts from a FP register value (in any format) to a double with
   approximately the same value (or returns Dbg_Err_Overflow)
 */

void Dbg_DoubleToFPReg(Dbg_MCState *, Dbg_FPRegVal *f, DbleBin const *d);
/* Converts the double <d> to a Dbg_FPRegVal with type F_Extended */

/*--------------------------------------------------------------------------*/

#include "rdi_cp.h"

Dbg_Error Dbg_DescribeCoPro(
    Dbg_MCState *state, unsigned int cpnum, RDI_CoProDesc *p);

Dbg_Error Dbg_DescribeCoProRegFormat(
    Dbg_MCState *state, unsigned int cpnum, unsigned int rno,
    Dbg_RegisterFormat *);

Dbg_Error Dbg_DescribeCoPro_RDI(
    Dbg_MCState *state, int cpnum, RDI_CoProDesc *p);

Dbg_Error Dbg_ReadCPRegisters(
    Dbg_MCState *state, ARMword *regs, unsigned int cpnum);
Dbg_Error Dbg_WriteCPRegister(
    Dbg_MCState *state, unsigned int rno, ARMword *reg, unsigned int cpnum);

Dbg_Error Dbg_CPRegisters(
    Dbg_MCState *state, Dbg_DeviceRegisterSpec const **p, unsigned int *n,
    const char *format, unsigned int cpnum);

/*--------------------------------------------------------------------------*/

Dbg_Error Dbg_ReadWords(
    Dbg_MCState *state,
    ARMword *words, ARMaddress addr, unsigned count);
/* Reads a number of (32-bit) words from target store.  The values are in host
   byte order; if they are also to be interpreted as bytes Dbg_SwapByteOrder()
   must be called to convert to target byte order.
 */

Dbg_Error Dbg_WriteWords(
    Dbg_MCState *state,
    ARMaddress addr, const ARMword *words, unsigned count);
/* Writes a number of (32-bit) words to target store.  The values are in host
   byte order (if what is being written is actually a byte string it must be
   converted by Dbg_SwapByteOrder()).
 */

Dbg_Error Dbg_ReadHalf(Dbg_MCState *state, ARMhword *val, ARMaddress addr);
Dbg_Error Dbg_WriteHalf(Dbg_MCState *state, ARMaddress addr, ARMword val);

Dbg_Error Dbg_ReadBytes(Dbg_MCState *state, Dbg_Byte *val, ARMaddress addr,
                         unsigned count, RDI_AccessType type);
Dbg_Error Dbg_WriteBytes(Dbg_MCState *state, ARMaddress addr, const Dbg_Byte *val, 
                         unsigned count, RDI_AccessType type);

Dbg_Error Dbg_ReadBytesCheck(Dbg_MCState *state, Dbg_Byte *val, ARMaddress addr,
                         unsigned *pcount, RDI_AccessType type);
Dbg_Error Dbg_WriteBytesCheck(Dbg_MCState *state, ARMaddress addr, const Dbg_Byte *val, 
                         unsigned *pcount, RDI_AccessType type);

void Dbg_HostWords(Dbg_MCState *state, ARMword *words, unsigned wordcount);
/* (A noop unless host and target bytesexes differ) */

ARMword Dbg_HostWord(Dbg_MCState *state, ARMword v);

ARMhword Dbg_HostHalf(Dbg_MCState *state, ARMword v);

Dbg_Error Dbg_ReadHalves(Dbg_MCState *state, ARMhword *val, ARMaddress addr, unsigned count);

void Dbg_HostInt64(
    Dbg_MCState *state, ARMword const *word, Int64Bin *ll);
void Dbg_TargetInt64(
    Dbg_MCState *state, ARMword *word, Int64Bin const *ll);

void Dbg_HostDouble(
    Dbg_MCState *state, ARMword const *d, DbleBin *dest);
void Dbg_TargetDouble(
    Dbg_MCState *state, ARMword *dest, DbleBin const *d);

void Dbg_HostFloat(
    Dbg_MCState *state, ARMword const *d, DbleBin *dest);
void Dbg_TargetFloat(
    Dbg_MCState *state, ARMword *dest, DbleBin const *d);

/*--------------------------------------------------------------------------*/

/* Types describing various aspects of position within code.
   There are rather a lot of these, in the interests of describing precisely
   what fields must be present (rather than having a single type with many
   fields which may or may not be valid according to context).
 */

typedef struct Dbg_LLPos {
    Dbg_SymTable *st;
    char *llsym;
    ARMaddress offset;
} Dbg_LLPos;

typedef struct Dbg_File {
    Dbg_SymTable *st;
    char *file;
} Dbg_File;

typedef struct Dbg_Line {
    unsigned32 line;      /* linenumber in the file */
    unsigned16 statement, /* within the line (1-based) */
               charpos,   /* ditto */
               charpos2;
} Dbg_Line;
/* As an output value from toolbox functions, both statement and charpos are set
   if the version of the debugger tables for the section concerned permits.
   On input, <charpos> is used only if <statement> is 0 (in which case, if
   <charpos> is non-0, Dbg_Err_DbgTableVersion is returned if the version of
   the debugger tables concerned is too early.
 */

typedef struct Dbg_FilePos {
    Dbg_File f;
    Dbg_Line line;
} Dbg_FilePos;

typedef struct Dbg_ProcDesc {
    Dbg_File f;
    char *name;
} Dbg_ProcDesc;

typedef struct Dbg_ProcPos {
    Dbg_ProcDesc p;
    Dbg_Line line;
} Dbg_ProcPos;

/* Support for conversions between position representations */

Dbg_Error Dbg_ProcDescToLine(Dbg_MCState *state, Dbg_ProcDesc *proc, Dbg_Line *line);
/* If proc->f.file is null (and there is just one function proc->name), it is
   updated to point to the name of the file containing (the start of)
   proc->name.
 */

Dbg_Error Dbg_FilePosToProc(Dbg_MCState *state, const Dbg_FilePos *pos, char **procname);

/* Conversions from position representations to and from code addresses */

Dbg_Error Dbg_AddressToProcPos(
    Dbg_MCState *state, ARMaddress addr,
    Dbg_ProcPos *pos);
Dbg_Error Dbg_AddressToLLPos(
    Dbg_MCState *state, ARMaddress addr,
    Dbg_LLPos *pos);

Dbg_Error Dbg_ProcPosToAddress(
    Dbg_MCState *state, const Dbg_ProcPos *pos,
    ARMaddress *res);
Dbg_Error Dbg_LLPosToAddress(
    Dbg_MCState *state, const Dbg_LLPos *pos,
    ARMaddress *res);

typedef struct {
    ARMaddress start, end;
} Dbg_AddressRange;

typedef Dbg_Error Dbg_AddressRangeProc(void *arg, int32 first, int32 last, Dbg_AddressRange const *range);

Dbg_Error Dbg_MapAddressRangesForFileRange(
    Dbg_MCState *state,
    Dbg_SymTable *st, const char *f, int32 first, int32 last, Dbg_AddressRangeProc *p, void *arg);

typedef struct Dbg_Environment Dbg_Environment;
/* A Dbg_Environment describes the context required to make sense of a variable
   name and access its value.  Its format is not revealed.  Dbg_Environment
   values are allocated by Dbg_NewEnvironment() and discarded by
   Dbg_DeleteEnvironment().
 */

Dbg_Environment *Dbg_NewEnvironment(Dbg_MCState *state);
void Dbg_DeleteEnvironment(Dbg_MCState *state, Dbg_Environment *env);

Dbg_Error Dbg_StringToEnv(
    Dbg_MCState *state, char *str, Dbg_Environment *resenv,
    bool forcontext, Dbg_Environment const *curenv);

Dbg_Error Dbg_StringToEnvEx(
    Dbg_MCState *state, char *str, Dbg_Environment *resenv,
    bool forcontext, Dbg_Environment const *curenv, uint32 n);

Dbg_Error Dbg_ProcPosToEnvironment(
    Dbg_MCState *state, const Dbg_ProcPos *pos, int activation,
    const Dbg_Environment *current, Dbg_Environment *res);

/* Conversion from a position representation to an Dbg_Environment (as required
   to access variable values).  Only a Dbg_ProcPos argument here; other
   representations need to be converted first.

   Returns <res> describing the <activation>th instance of the function
   described by <pos>, up from the stack base if <activation> is negative,
   else down from <current>.
   If this function returns Dbg_Err_ActivationNotPresent, the result
   Dbg_Environment is still valid for accessing non-auto variables.
 */

typedef struct Dbg_DeclSpec Dbg_DeclSpec;

Dbg_Error Dbg_EnvToProcItem(
    Dbg_MCState *state, Dbg_Environment const *env, Dbg_DeclSpec *proc);

Dbg_Error Dbg_ContainingEnvironment(
    Dbg_MCState *state, const Dbg_Environment *context, Dbg_Environment *res);
/* Set <res> to describe the containing function, file if <context> is within
   a top-level function (or error if <context> already describes a file).
 */

/*--------------------------------------------------------------------------*/

/* ASD debug table pointers are not by themselves sufficient description,
   since there's an implied section context.  Hence the DeclSpec and TypeSpec
   structures.
 */


#ifndef Dbg_TypeSpec_Defined

struct Dbg_DeclSpec { void *a; };

#ifdef CALLABLE_COMPILER
typedef void *Dbg_TypeSpec;

/* The intention here is to give an alternative definition for Dbg_BasicType
   which follows.
 */

#define Dbg_T_Void      xDbg_T_Void
#define Dbg_T_Bool      xDbg_T_Bool
#define Dbg_T_SByte     xDbg_T_SByte
#define Dbg_T_SHalf     xDbg_T_Half
#define Dbg_T_SWord     xDbg_T_SWord
#define Dbg_T_SDWord    xDbg_T_SDWord
#define Dbg_T_UByte     xDbg_T_UByte
#define Dbg_T_UHalf     xDbg_T_UHalf
#define Dbg_T_UWord     xDbg_T_UWord
#define Dbg_T_UDWord    xDbg_T_UDWord
#define Dbg_T_Float     xDbg_T_Float
#define Dbg_T_Double    xDbg_T_Double
#define Dbg_T_LDouble   xDbg_T_LDouble
#define Dbg_T_Complex   xDbg_T_Complex
#define Dbg_T_DComplex  xDbg_T_DComplex
#define Dbg_T_String    xDbg_T_String
#define Dbg_T_Function  xDbg_T_Function

#define Dbg_BasicType   xDbg_BasicType
#define Dbg_PrimitiveTypeToTypeSpec xDbg_PrimitiveTypeToTypeSpec

#else
/* We want a Dbg_TypeSpec to be a an opaque type, but of known size (so the
   toolbox's clients can allocate the store to hold one); unfortunately, we
   can do this only by having one definition for the toolbox's clients and
   one (elsewhere) for the toolbox itself.
 */

typedef struct Dbg_TypeSpec Dbg_TypeSpec;
struct Dbg_TypeSpec { void *a; int32 b; };
#endif /* CALLABLE_COMPILER */

typedef enum {
    Dbg_T_Void,

    Dbg_T_Bool,

    Dbg_T_SByte,
    Dbg_T_SHalf,
    Dbg_T_SWord,
    Dbg_T_SDWord,

    Dbg_T_UByte,
    Dbg_T_UHalf,
    Dbg_T_UWord,
    Dbg_T_UDWord,

    Dbg_T_Float,
    Dbg_T_Double,
    Dbg_T_LDouble,

    Dbg_T_Complex,
    Dbg_T_DComplex,

    Dbg_T_String,

    Dbg_T_Function
} Dbg_BasicType;

#endif

void Dbg_PrimitiveTypeToTypeSpec(Dbg_TypeSpec *ts, Dbg_BasicType t);

bool Dbg_TypeIsIntegral(Dbg_TypeSpec const *ts);

bool Dbg_TypeIsPointer(Dbg_TypeSpec const *ts);

bool Dbg_TypeIsFunction(Dbg_TypeSpec const *ts);

bool Dbg_PruneType(Dbg_TypeSpec *tsres, Dbg_TypeSpec const *ts);
/* Return to tsres a version of ts which has been pruned by the removal of all
   toplevel typedefs. Result is YES if the result has changed.
 */

typedef Dbg_Error Dbg_FileProc(Dbg_MCState *state, const char *name, const Dbg_DeclSpec *procdef, void *arg);

Dbg_Error Dbg_EnumerateFiles(Dbg_MCState *state, Dbg_SymTable *st, Dbg_FileProc *p, void *arg);
/* The top level for a high level enumerate.  Lower levels are performed by
   EnumerateDeclarations (below).
 */
Dbg_Error Dbg_EnumerateSources(
        Dbg_MCState *state, Dbg_SymTable *st, Dbg_FileProc *p, void *arg);
/* Enumerates all source files. The Dbg_DeclSpec passed to p() may be invalid */

typedef enum {
    ds_Invalid,
    ds_Type,
    ds_Var,
    ds_Proc,
    ds_Enum,
    ds_Function,
    ds_Label
#ifdef CALLABLE_COMPILER
    ,ds_GlobalVar
#endif
        ,ds_Class
} Dbg_DeclSort;

Dbg_DeclSort Dbg_SortOfDeclSpec(Dbg_DeclSpec const *decl);

char *Dbg_NameOfDeclSpec(Dbg_DeclSpec const *decl);

Dbg_TypeSpec Dbg_TypeSpecOfDeclSpec(Dbg_DeclSpec const *decl);

/* for a ds_Proc sort only */
const char *Dbg_FileNameOfDeclSpec(Dbg_DeclSpec const *decl);

typedef enum {
    /* C/C++ storage classes */
    cs_None   = 0x0,
    cs_Extern = 0x1,
    cs_Static = 0x2,
    cs_Auto   = 0x3

} Dbg_StgClass;

Dbg_StgClass Dbg_StgClassOfDeclSpec(Dbg_DeclSpec const *decl);

bool Dbg_VarsAtSameAddress(Dbg_DeclSpec const *d1, Dbg_DeclSpec const *d2);

bool Dbg_VarsDecribedForDeclSpec(Dbg_DeclSpec const *decl);

int Dbg_ArgCountOfDeclSpec(Dbg_DeclSpec const *decl);


/***********************************************************
     Function name  : Dbg_DeclSpecToEnvironment
 ***********************************************************

     Author         : Lars Kurth
     Date           : 10/7/98
     Version        : 1.0
     Description    : Convert opaque declaration specification 
                      into environment:
                      In case the specification is an object 
                      which contains a function scope return 
                      its scope (e.g. compile unit, function 
                      or method). Otherwise return the containing
                      environment.
     Return type    : Dbg_Error 
     Return Desc    : Error_OK if conversion was possible. 
                      Error code otherwise. Invalid declaration 
                      specifications provoke an error.
     Parameter IN   : Dbg_MCState *state
     Parameter IN   : Dbg_DeclSpec const *decl
     Parameter OUT  : Dbg_Environment *env
     Parameter IN   : bool bTidy (TRUE, if source pos is also needed)

     Date     Initials     Version     MLS Thread/Reason
     ----     --------     -------     -----------------

 ***********************************************************/

Dbg_Error Dbg_DeclSpecToEnvironment(Dbg_MCState *state, Dbg_DeclSpec const *decl, Dbg_Environment *env, bool bTidy);


typedef struct Dbg_DComplex { DbleBin r, i; } Dbg_DComplex;

typedef union Dbg_ConstantVal {
    int32 l;
    uint32 u;
    Int64Bin ll;
    Uint64Bin ull;
    DbleBin d;
    Dbg_DComplex fc;
    ARMaddress a;
    char *s;
} Dbg_ConstantVal;

#define Dbg_constant_val_ll_(x) ((x)->ll)
#define Dbg_constant_val_ull_(x) ((x)->ull)
#define Dbg_constant_val_l_(x) ((x)->l)
#define Dbg_constant_val_u_(x) ((x)->u)
#define Dbg_constant_val_d_(x) ((x)->d)
#define Dbg_constant_val_fc_(x) ((x)->fc)
#define Dbg_constant_val_a_(x) ((x)->a)
#define Dbg_constant_val_s_(x) ((x)->s)

typedef struct Dbg_Constant {
    Dbg_TypeSpec type;
    Dbg_ConstantVal val;
} Dbg_Constant;

#define Dbg_constant_ll_(x) ((x)->val.ll)
#define Dbg_constant_ull_(x) ((x)->val.ull)
#define Dbg_constant_l_(x) ((x)->val.l)
#define Dbg_constant_u_(x) ((x)->val.u)
#define Dbg_constant_d_(x) ((x)->val.d)
#define Dbg_constant_fc_(x) ((x)->val.fc)
#define Dbg_constant_a_(x) ((x)->val.a)
#define Dbg_constant_s_(x) ((x)->val.s)

typedef enum Dbg_ValueSort {
    vs_register,
    vs_store,
    vs_constant,
    vs_local,
    vs_filtered,
    vs_moving,      /* Moving location */
    vs_multiple,    /* Value stored at multiple locations (e.g. PIECE) */
    vs_none,
    vs_error
} Dbg_ValueSort;

/* vs_local allows the use of symbol table entries to describe entities within
   the debugger's own address space, accessed in the same way as target
   variables.
   vs_filtered describes entities which may be read or written only via an
   access function (eg r15)
 */

/* #define fpr_base 16 */
/* There's only one register ValueSort (reflecting asd table StgClass);
   fp register n is encoded as register n+fpr_base.
 */

typedef struct Dbg_Value Dbg_Value;

typedef Dbg_Error Dbg_AccessFn(
    Dbg_MCState *state, int write, Dbg_Value *self, Dbg_Constant *c);
/* <write> == 0: read a vs_filtered value, updating the value self.
   <write> == 1: update a vs_filtered value, with the value described by c.
   <self> allows use of the same Dbg_AccessFn for several different entities
   (using different val.f.id fields).
 */

typedef struct { Dbg_AccessFn *f; int id; Dbg_MCState *state; } Dbg_AccessFnRec;

/* Type for abstract access to a pointer */
typedef struct { ARMaddress addr; Dbg_MCState *state; } Dbg_PtrRec;
typedef int Dbg_HandleRec;

struct Dbg_Value {
    Dbg_TypeSpec type;
    Dbg_ValueSort sort;
    Dbg_FormatFn *formatp;
    void *formatarg;
    int f77csize;
    union {
        struct { unsigned long reg; ARMaddress frame; Dbg_MCState *state; } r;
        Dbg_PtrRec ptr;
        Dbg_ConstantVal c;
        void *localp;
        Dbg_AccessFnRec f;
        /* describe moving entity */
        struct { void *handle; ARMaddress frame; Dbg_Environment *env; Dbg_MCState *state; } moving;
        /* describe complex entity */
        struct { void *handle; uint32 size; Dbg_MCState *state; } multiple;         
        Dbg_Error err;
    } val;
};

#define Dbg_value_ll_(x) ((x)->val.c.ll)
#define Dbg_value_ull_(x) ((x)->val.c.ull)
#define Dbg_value_l_(x) ((x)->val.c.l)
#define Dbg_value_u_(x) ((x)->val.c.u)
#define Dbg_value_d_(x) ((x)->val.c.d)
#define Dbg_value_fc_(x) ((x)->val.c.fc)
#define Dbg_value_a_(x) ((x)->val.c.a)
#define Dbg_value_s_(x) ((x)->val.c.s)

Dbg_Error Dbg_AddSymbol(Dbg_SymTable *st, char const *name, Dbg_Value const *val);

typedef struct Dbg_DeclSpecF {
  Dbg_DeclSpec decl;
  Dbg_FormatFn *formatp;
  void *formatarg;
} Dbg_DeclSpecF;

typedef Dbg_Error Dbg_DeclProc(Dbg_MCState *state, Dbg_Environment const *context,
                       Dbg_DeclSpecF const *var, Dbg_DeclSort sort, int masked,
                       void *arg);

Dbg_Error Dbg_EnumerateDeclarations(Dbg_MCState *state, Dbg_Environment const *context,
                            Dbg_DeclProc *p, void *arg);
/* call p once for every declaration local to the function described by
   <context> (or file if <context> describes a place outside a function).
   p's argument <masked> is true if the variable is not visible, thanks to
   a declaration in an inner scope.
 */
#define DECL_INSCOPE 0
#define DECL_ALL     1
#define DECL_BLOCK   2
Dbg_Error Dbg_EnumerateDeclarationsEx(Dbg_MCState *state, Dbg_Environment const *context,
                            int what, Dbg_DeclProc *p, void *arg);
/* Same as Dbg_EnumerateDeclarations if all==DECL_INSCOPE, if it equals DECL_ALL then enumerate
   over all declarations in all lexical blocks which are visible in the function context.
   If it is DECL_BLOCK, only enumerate over functions in the current block.
 */

Dbg_Error Dbg_ValueOfVar(Dbg_MCState *state, const Dbg_Environment *context,
                 const Dbg_DeclSpec *var, Dbg_Value *val);
/* Different from Dbg_EvalExpr() in that the thing being evaluated is described
   by a Dbg_DeclSpec (which must be for a variable), rather than a string
   needing to be decoded and associated with a symbol-table item.  Intended to
   be called from a Dbg_DeclProc called from Dbg_EnumerateDeclarations.
 */

Dbg_Error Dbg_EvalExpr(Dbg_MCState *state, Dbg_Environment const *context,
               char const *expr, int flags, Dbg_Value *val);

Dbg_Error Dbg_EvalExpr_ep(Dbg_MCState *state, Dbg_Environment const *context,
                  char const *expr, char **exprend, int flags, Dbg_Value *val);

/* Both Dbg_ValueOfVar and Dbg_EvalExpr mostly deliver a value still containing
   an indirection (since it may be wanted as the lhs of an assignment)
 */

void Dbg_RealLocation(Dbg_MCState *state, Dbg_Value *val);
/* If val describes a register, this may really be a register, or a place on
   the stack where the register's value is saved. In the latter case, val
   is altered to describe the save place. (In all others, it remains
   unchanged).
 */

Dbg_Error Dbg_DereferenceValue(Dbg_MCState *state, const Dbg_Value *value, Dbg_Constant *c);
/* This fails if <value> describes a structure or array, returning
   Dbg_Err_TypeNotSimple
 */

Dbg_Error Dbg_DerefMoving(Dbg_MCState *state, Dbg_Value const *value, Dbg_Value *deref);
/* In case value is of type vs_moving, then the location is determined using the 
   environment in value->val.moving.env. The result of this routine is a known
   location.
 */

typedef struct Dbg_Expr Dbg_Expr;
/* The result of parsing an expression in an environment: its structure is not
   revealed.  (Clients may wish to parse just once an expression which may be
   evaluated often).  In support of which, the following two functions partition
   the work of Dbg_EvalExpr().
 */

#define Dbg_exfl_heap 1    /* allocate Expr on the heap (FreeExpr must then be
                              called to discard it).  Otherwise, it goes in a
                              place overwritten by the next call to ParseExpr
                              or EvalExpr
                            */
#define Dbg_exfl_needassign 2
#define Dbg_exfl_lowlevel   4

int Dbg_SetInputRadix(Dbg_MCState *state, int radix);
Dbg_Error Dbg_SetDefaultVarFormat(Dbg_MCState *state, int nID, char *format);
char *    Dbg_GetDefaultVarFormat(Dbg_MCState *state, int nID);

Dbg_Error Dbg_ParseExpr(
    Dbg_MCState *state, Dbg_Environment const *env, char *string,
    char **end, Dbg_Expr **res, int flags);
/* Just parse the argument string, returning a pointer to a parsed expression
   and a pointer to the first non-white space character in the input string
   which is not part of the parsed expression. (If macro expansion has taken
   place, the returned pointer will not be into the argument string at all,
   rather into the expanded version of it).
 */

Dbg_Error Dbg_ParseExprCheckEnd(
    Dbg_MCState *state, Dbg_Environment const *env, char *string,
    Dbg_Expr **res, int flags);
/* As Dbg_ParseExpr, but the parsed expression is required completely to fill
   the argument string (apart possibly for trailing whitespace), and an error
   is returned if it does not.
 */

Dbg_Error Dbg_ParsedExprToValue(
    Dbg_MCState *state, const Dbg_Environment *env, Dbg_Expr *expr, Dbg_Value *v);

Dbg_Error Dbg_ReadDecl(
    Dbg_MCState *state, Dbg_Environment const *env, char *string,
    Dbg_TypeSpec *p, char **varp, int flags);
/* Read a variable declaration, returing a description of the type of the
   variable to p, and a pointer to its name to varp.
 */

bool Dbg_IsCastToArrayType(Dbg_MCState *state, Dbg_Expr *expr);

void Dbg_FreeExpr(Dbg_Expr *expr);

Dbg_Error Dbg_CopyType(Dbg_TypeSpec *tdest, Dbg_TypeSpec const *tsource);
Dbg_Error Dbg_FreeCopiedType(Dbg_TypeSpec *ts);

Dbg_Error Dbg_TypeOfExpr(Dbg_MCState *state, Dbg_Expr *tree, Dbg_TypeSpec *restype);

Dbg_Error Dbg_ExprToVar(const Dbg_Expr *expr, Dbg_DeclSpec *var, Dbg_Environment *env);

Dbg_Error Dbg_Assign(Dbg_MCState *state, const Dbg_Value *lv, const Dbg_Value *rv);

typedef enum Dbg_TypeSort {
  ts_simple,
  ts_union,
  ts_struct,
  ts_array
} Dbg_TypeSort;

Dbg_TypeSort Dbg_TypeSortOfValue(Dbg_MCState *state, const Dbg_Value *val, int *fieldcount);

Dbg_Error Dbg_TypeToChars(const Dbg_TypeSpec *var, Dbg_BufDesc *buf);

Dbg_Error Dbg_TypeSize(Dbg_MCState *state, const Dbg_TypeSpec *type, unsigned32 *res);

typedef int Dbg_ValToChars_cb(Dbg_MCState *state, Dbg_Value *subval, const char *fieldname,
                          Dbg_BufDesc *buf, void *arg);

Dbg_Error Dbg_CheckFormat(char const *form);
/* Check whether <form> is an acceptable format for use with Dbg_ValToChars().
   Returs Error_OK if so.
 */

Dbg_Error Dbg_ValToChars(Dbg_MCState *state, Dbg_Value *val, int base,
                 Dbg_ValToChars_cb *cb, void *arg,
                 const char *form, Dbg_BufDesc *buf);
/*
   <base> is used for (any size) integer values.
   If <val> is of an array or structure type, <cb> is called for each element,
   with <arg> as its last parameter, and <subbuf> describing the space remaining
   in <buf>.  If <cb> returns 0, conversion ceases.
 */

Dbg_Error Dbg_NthElement(
    Dbg_MCState *state,
    const Dbg_Value *val, unsigned32 n, char **fieldname, Dbg_Value *subval);

Dbg_Error Dbg_NthElementEx(
    Dbg_MCState *state, const Dbg_Value *val, unsigned32 n,
            char **fieldname, Dbg_Value *subval, unsigned32 *pIsStatic);

typedef Dbg_Error Dbg_HistoryProc(void *, int, Dbg_Value *);

Dbg_Error Dbg_RegisterHistoryProc(
    Dbg_MCState *state, Dbg_HistoryProc *p, void *arg);

typedef enum {
  ls_cpu,
  ls_store,
  ls_copro,
  ls_local,
  ls_filtered
} Dbg_LocSort;

typedef struct {
  Dbg_LocSort sort;
  union {
    struct { Dbg_MCState *state; ARMaddress addr, size; } store;
    struct { Dbg_MCState *state; unsigned int mode; unsigned int r; } cpu;
    struct { Dbg_MCState *state; unsigned int no; unsigned int r; } cp;
    void *localp;
    Dbg_AccessFnRec f;
  } loc;
} Dbg_Loc;

typedef Dbg_Error Dbg_ObjectWriteProc(
    Dbg_MCState *state, void *arg, Dbg_Loc const *loc);
Dbg_Error Dbg_OnObjectWrite(
    Dbg_MCState *state, Dbg_ObjectWriteProc *p, void *arg);
/* Register function to be called back whenever the toolbox has written to any
 * object accessible by the debuggee (or to local variables belonging to a
 * toolbox client). The write has already been done.
 * 'arg' is passed to the callback.
 * (To allow multiple toolbox clients to coexist).
 */
Dbg_Error Dbg_RemoveOnObjectWrite(
    Dbg_MCState *, Dbg_ObjectWriteProc *, void *);

Dbg_Error Dbg_ObjectWritten(Dbg_MCState *state, Dbg_Loc const *loc);

Dbg_Error Dbg_UnaliasObject(Dbg_MCState *state, Dbg_Loc *to, Dbg_Loc const *from);
/* Given a loc of type ls_cpu (or ls_copro - to be implemented later) fill
 * in the missing data for the "to" loc. In the case of ls_cpu, this will
 * be the register number - the passed in mode will be correct.
 * For example, with ARM, if passed (ls_cpu/svc/??, ls_cpu/usr/r1), fills
 * in "r1" in new.
 * Returns an error if no alias exists; the locs are of different types
 */

/*--------------------------------------------------------------------------*/

/* To deal with $statistics and $statistics_inc (now that the size of these
 * counters isn't fixed), the toolbox exports a pair of functions for a
 * frontend, to help it implement these variables.
 */
/*
 * read current statistics, and subtract from the "last" value
 * read. Optionally set "last" value to the current value.
 * "base" and "stats" are two buffers to use. If passed as NULL, the
 * toolbox uses it's own buffers.
 */
ARMword *Dbg_StatisticsInc(
   Dbg_MCState *state, bool update_base, ARMword *base, ARMword *stats);

/* Return the number of a buffer (in bytes) required for a statistics
 * buffer.
 */
int Dbg_StatisticsSize(Dbg_MCState *state);


/*--------------------------------------------------------------------------*/

/* Control of target program execution.
   Currently, only synchronous operation is provided.
   Execution could possibly be asynchronous where the target is a seperate
   processor, but is necessarily synchronous if the target is Armulator.
   Unfortunately, this may require modification to the RDI implementation
   if multitasking is required but the the host system provides it only
   cooperatively, or if there is no system-provided way to generate SIGINT.
 */

Dbg_Error Dbg_SetCommandline(Dbg_MCState *state, const char *args);
/* Set the argument string to the concatenation of the name of the most
   recently loaded image and args.
 */

typedef enum Dbg_ProgramState {
    ps_notstarted,
    /* Normal ways of stopping */
    ps_atbreak, ps_atwatch, ps_stepdone,
    ps_interrupted,
    ps_stopped,
    ps_waiting,                 /* another device has stopped, causing this
                                 * device to wait. */
    /* abnormal (but unsurprising) ways of stopping */
    ps_atextwatch,              /* h/w watch set externally by user */
    ps_lostwatch,
    ps_branchthrough0, ps_undef, ps_caughtswi, ps_prefetch,
    ps_abort, ps_addrexcept, ps_caughtirq, ps_caughtfiq,
    ps_error,
    /* only as a return value from Call() */
    ps_callfailed, ps_callreturned,
    /* internal inconsistencies */
    ps_broken,                  /* target has "broken" */
    ps_unknownbreak,
    ps_unknown,
    ps_unused                   /* module/handle is not used for execution */
} Dbg_ProgramState;

int Dbg_IsCallLink(Dbg_MCState *state, ARMaddress pc);

typedef struct {
    Dbg_FPRegVal fpres;
    ARMword intres;
} Dbg_CallResults;

Dbg_CallResults *Dbg_GetCallResults(Dbg_MCState *state);

#define Dbg_S_STATEMENTS      0
#define Dbg_S_INSTRUCTIONS    1
#define Dbg_S_STEPINTOPROCS   2
#define Dbg_S_STEPOVERINLINES 4

Dbg_Error Dbg_Step(Dbg_MCState *state, int32 stepcount, int stepby, Dbg_ProgramState *status);
/*  <stepby> is a combination of the Dbg_S_... values above */

Dbg_Error Dbg_StepOut(Dbg_MCState *state, Dbg_ProgramState *status);

bool Dbg_CanGo(Dbg_MCState *state);

bool Dbg_IsExecuting(Dbg_MCState *state);

Dbg_Error Dbg_Go(Dbg_MCState *state, Dbg_ProgramState *status);

Dbg_Error Dbg_Stop(Dbg_MCState *state);
/* Asynchronous Stop request, for call from SIGINT handler.  On return to the
   caller, the call of Dbg_Go, Dbg_Step or Dbg_Call which started execution
   should return ps_interrupted.
 */
Dbg_Error Dbg_PostExecutionStep(Dbg_MCState *pState, Dbg_ProgramState *pStatus, Dbg_Error eErr, RDI_ModuleHandle hModule, RDI_PointHandle hPoint);
/* Has to be called if we have asynchronous execution on the RDI agent and we have received
   the event signalling the end of execution. This event returns eErr, hModule and hPoint.
   The routine then returns pStatus as if it was executed synchronously.
 */
typedef void Dbg_ExecuteProc(
     Dbg_MCState *state, void *arg, Dbg_ProgramState status);
Dbg_Error Dbg_OnExecute(Dbg_MCState *, Dbg_ExecuteProc *, void *arg);
/* Register function to be called back whenever execution stops. 'arg' is
 * passed as the second argument to the proc. This is called WHENEVER
 * execution stops, even for processors other than that passed into
 * OnExecute(). The 'state' passed to the callback is that of the stopped
 * processor.
 * (To allow multiple toolbox clients to coexist).
 */
Dbg_Error Dbg_RemoveOnExecute(Dbg_MCState *, Dbg_ExecuteProc *, void *);
/* Remove the callback setup with those arguments.  Returns NotFound
 * error if the callback has not been registered/has already been deregistered.
 */

Dbg_Error Dbg_SetReturn(Dbg_MCState *state,
                const Dbg_Environment *context, const Dbg_Value *value);
/* Prepare continuation by returning <value> from the function activation
   described by <context>.  (Dbg_Go() or Dbg_Step() actually perform the
   continuation).
 */

Dbg_Error Dbg_SetExecution(Dbg_MCState *state, Dbg_Environment *context);
/* Set the pc in a high-level fashion */

Dbg_Error Dbg_ProgramStateToChars(Dbg_MCState *state, Dbg_ProgramState event, Dbg_BufDesc *buf);
/* This is guaranteed to give a completely accurate description of <event> if
   this was the value returned by the most recent call of Dbg_Go, Dbg_Step,
   or Dbg_Call.
 */

/*--------------------------------------------------------------------------*/

ARMaddress Dbg_CurrentPC(Dbg_MCState *state);

/* @@@ probably should have an enumeration of instruction types - arm, thumb, picco */
unsigned int Dbg_CurrentInstructionSize(Dbg_MCState *state);

Dbg_Error Dbg_CurrentEnvironment(Dbg_MCState *state, Dbg_Environment *context);

Dbg_Error Dbg_PrevFrame(Dbg_MCState *state, Dbg_Environment *context);
/* towards the base of the stack */

Dbg_Error Dbg_NextFrame(Dbg_MCState *state, Dbg_Environment *context);
/* away from the base of the stack */

typedef struct Dbg_AnyPos {
    enum { pos_source, pos_ll, pos_none } postype;
    ARMaddress pc;
    union {
        Dbg_ProcPos source;
        Dbg_LLPos ll;
        ARMaddress none;
    } pos;
} Dbg_AnyPos;

Dbg_Error Dbg_EnvironmentToPos(Dbg_MCState *state, const Dbg_Environment *context, Dbg_AnyPos *pos);
/* <pos> is set to a Dbg_ProcPos if these is one corresponding to <context>
   else a Dbg_LLPos if there is one.
 */

Dbg_Error Dbg_SwitchEnvironmentToProcPos(Dbg_MCState *state, Dbg_ProcPos *pPos, Dbg_AnyPos *pAnyPos);

/*--------------------------------------------------------------------------*/

/* Source file management.
   Pretty vestigial.  Handles source path (per loaded image),
   and translates from line-number (as given in debugger tables) to character
   position (as required to access files)
 */

Dbg_Error Dbg_ClearPaths(Dbg_MCState *state, Dbg_SymTable *st);
Dbg_Error Dbg_AddPath(Dbg_MCState *state, Dbg_SymTable *st, const char *path);
Dbg_Error Dbg_DeletePath(Dbg_MCState *state, Dbg_SymTable *st, const char *path);

typedef enum {
  Dbg_PathsCleared,
  Dbg_PathAdded,
  Dbg_PathDeleted
} Dbg_PathAlteration;

typedef void Dbg_PathAlteredProc(
    Dbg_MCState *state, void *arg, Dbg_SymTable *st, char const *path,
    Dbg_PathAlteration sort);

Dbg_Error Dbg_OnPathAlteration(
    Dbg_MCState *state, Dbg_PathAlteredProc *p, void *arg);
/* Register function to be called back whenever one of the source path
 * modification functions above is called. (To allow multiple toolbox
 * clients to coexist).
 * 'arg' is passed to the callback.
 */
Dbg_Error Dbg_RemoveOnPathAlteration(
    Dbg_MCState *, Dbg_PathAlteredProc *, void *);

typedef struct Dbg_FileRec Dbg_FileRec;
typedef struct {
  unsigned32 linecount;
  Dbg_FileRec *handle;
  char *fullname;
} Dbg_FileDetails;

Dbg_Error Dbg_GetFileDetails(
    Dbg_MCState *state, const Dbg_File *fname, Dbg_FileDetails *res);
Dbg_Error Dbg_FinishedWithFile(Dbg_MCState *state, Dbg_FileRec *handle);

Dbg_Error Dbg_GetFileDetails_fr(
    Dbg_MCState *state, Dbg_FileRec *handle, Dbg_FileDetails *res);
/* Refresh details about the file associated with <handle> (in particular,
 * its linecount).
 */

Dbg_Error Dbg_FileLineLength(
    Dbg_MCState *state, Dbg_FileRec *handle, int32 lineno, int32 *len);
/* Return to <len> the length of line <lineno> of the file associated with
 * <handle> (without necessarily reading from the file).
 */

Dbg_Error Dbg_GetFileLine_fr(
    Dbg_MCState *state, Dbg_FileRec *handle, int32 lineno, Dbg_BufDesc *buf);
/* Return to <buf> the contents of line <lineno> of the file associated with
 * <handle> (including its terminating newline).
 */

Dbg_Error Dbg_StartFileAccess(Dbg_MCState *state, Dbg_FileRec *handle);
Dbg_Error Dbg_EndFileAccess(Dbg_MCState *state, Dbg_FileRec *handle);
/* These two calls bracket a sequence of calls to GetFileLine. Between the
 * calls, the toolbox is permitted to retain state allowing more rapid
 * access to text on the file associated with <handle>.
 */

Dbg_Error Dbg_ControlSourceFileAccess(
    Dbg_MCState *state, uint32 cachesize, bool closefiles);
/* Control details of how the toolbox manages source files.
 *   If <cachesize> is non-zero, the text from the most recently accessed
 *     source files (of total size not to exceed <cachesize>) is saved in
 *     store on first access to the file; subsequent access to the text of
 *     the file uses this copy.
 *   If <closefiles> is true, no stream is left attached to uncached source
 *     files after Dbg_EndFileAccess has been closed. Otherwise, the toolbox
 *     may retain such streams, in order to improve access.
 */

/*--------------------------------------------------------------------------*/

/* disassembly */

/*
 ? More exact control is wanted here, but that requires a more complicated
 ? disass callback interface.
 */

typedef const char *Dbg_SWI_Decode(Dbg_MCState *state, ARMword swino);

Dbg_Error Dbg_InstructionAt(Dbg_MCState *state, ARMaddress addr,
                    int isize, ARMhword *inst, Dbg_SymTable *st,
                    Dbg_SWI_Decode *swi_name, Dbg_BufDesc *buf, int *length);
/* <isize> describes the form of disassembly wanted: 2 for 16-bit, 4 for
 *   32-bit, 0 for 16- or 32-bit depending whether addr addresses 16- or
 *   32-bit code.
 * <inst> is a pointer to a pair of halfwords *in target byte order*
 * Possibly only the first halfword will be consumed: the number of bytes used
 *   is returned via <length>.
 * <st> is used to locate the address, and decide whether it is a code or
 *   data address. (Data is disassembled as "DCD ...".). If <st>==NULL, this
 *   decision always comes out as "code". If <st>==NULL, isize must not be 0.
 */

/*--------------------------------------------------------------------------*/

int Dbg_RDIOpen(Dbg_MCState *state, unsigned type);
int Dbg_RDIInfo(Dbg_MCState *state, unsigned type, ARMword *arg1, ARMword *arg2);
ARMword Dbg_RDIGetCapability(Dbg_MCState *state);  /* BGC - Function to return RDI capabiliity */ 

/*--------------------------------------------------------------------------*/

typedef enum {
    Dbg_Point_Toolbox,
    Dbg_Point_RDI_Unknown,
    Dbg_Point_RDI_SW,
    Dbg_Point_RDI_HW
} Dbg_PointType;

/* breakpoint management
   Associated with a breakpoint there may be any of
     a count
     an expression
     a function
   the breakpoint is activated if
        the expression evaluates to a non-zero value (or fails to evaluate).
     && decrementing the count reaches zero (the count is then reset to its
        initial value).
     && the function, called with the breakpoint address as argument, returns
        a non-zero value.
?  (The order here may be open to debate.  Note that the first two are in
    the opposite order in armsd, but I think this order more rational)
 */

typedef enum Dbg_BreakPosType {
    bt_procpos,
    bt_procexit,
    bt_address
} Dbg_BreakPosType;

typedef union {
      Dbg_ProcPos procpos;
      Dbg_ProcDesc procexit;
      ARMaddress address;
} Dbg_BreakPosPos;

typedef struct Dbg_BreakPos {
    Dbg_BreakPosType sort;
    Dbg_BreakPosPos loc;
} Dbg_BreakPos;

typedef int Dbg_BPProc(Dbg_MCState *state, void *BPArg, Dbg_BreakPos *where);

typedef struct Dbg_BreakStatus {
    int index;
    int initcount, countnow;
    Dbg_BreakPos where;
    char *expr;
    Dbg_BPProc *p; void *p_arg;
    int incomplete;
    Dbg_PointType type;
    ARMword hwresource;
    unsigned requested_size;   /* 0 (unspecified), 16 or 32 */
} Dbg_BreakStatus;

Dbg_Error Dbg_StringToBreakPos(
    Dbg_MCState *state, Dbg_Environment *env, char const *str, size_t len,
    Dbg_BreakPos *bpos, char *b);

Dbg_Error Dbg_SetBreakPoint(Dbg_MCState *state, Dbg_BreakPos *where,
                    int count,
                    const char *expr,
                    Dbg_BPProc *p, void *arg);
Dbg_Error Dbg_SetBreakPoint16(Dbg_MCState *state, Dbg_BreakPos *where,
                    int count,
                    const char *expr,
                    Dbg_BPProc *p, void *arg);
Dbg_Error Dbg_SetBreakPointNaturalSize(Dbg_MCState *state, Dbg_BreakPos *where,
                    int count,
                    const char *expr,
                    Dbg_BPProc *p, void *arg);
/* Setting a breakpoint at the same address as a previous breakpoint
   completely removes the previous one.
 */

Dbg_Error Dbg_DeleteBreakPoint(Dbg_MCState *state, Dbg_BreakPos *where);

Dbg_Error Dbg_SuspendBreakPoint(Dbg_MCState *state, Dbg_BreakPos *where);
/* Temporarily remove the break point (until Reinstated) but leave intact
   its associated expr, the value its count has reached, etc.
?  The debugger toolbox itself wants this, but I'm not sure what use a client
   could have for it.  Ditto Reinstate...
 */

Dbg_Error Dbg_ReinstateBreakPoint(Dbg_MCState *state, Dbg_BreakPos *where);
/* Undo the effect of Dbg_SuspendBreakPoint
 */

Dbg_Error Dbg_DeleteAllBreakPoints(Dbg_MCState *state);

Dbg_Error Dbg_SuspendAllBreakPoints(Dbg_MCState *state);

Dbg_Error Dbg_ReinstateAllBreakPoints(Dbg_MCState *state);

typedef Dbg_Error Dbg_BPEnumProc(Dbg_MCState *state, Dbg_BreakStatus *status, void *arg);

Dbg_Error Dbg_EnumerateBreakPoints(Dbg_MCState *state, Dbg_BPEnumProc *p, void *arg);

Dbg_Error Dbg_BreakPointStatus(Dbg_MCState *state,
                       const Dbg_BreakPos *where, Dbg_BreakStatus *status);

/*typedef void Dbg_BreakAlteredProc(
    Dbg_MCState *state, void *arg, ARMaddress addr, bool set);*/
typedef void Dbg_BreakAlteredProc(
    Dbg_MCState *state, void *arg, ARMaddress addr, bool set, Dbg_BreakPosType where);

Dbg_Error Dbg_OnBreakAltered(
    Dbg_MCState *state, Dbg_BreakAlteredProc *p, void *arg);
/* Register function to be called back whenever a breakpoint is set or
 * cleared. (To allow multiple toolbox clients to coexist).
 * 'arg' is passed
 */
Dbg_Error Dbg_RemoveOnBreakAltered(
    Dbg_MCState *state, Dbg_BreakAlteredProc *p, void *arg);

bool Dbg_StoppedAtBreakPoint(Dbg_MCState *state, const Dbg_BreakPos *where);
/* Called after execution which resulted in ps_atbreak, to find out whether
   the specified breakpoint was hit (could be >1, eg. exit break and another
   high-level breakpoint at the same position).
   Returns NO if specified breakpoint not found, or execution didn't stop
   with ps_atbreak status.
 */

/*--------------------------------------------------------------------------*/

typedef struct 
{
  Dbg_Value val;
  char *name;
} Dbg_WatchPos;

typedef int Dbg_WPProc(Dbg_MCState *state, void *WPArg, Dbg_WatchPos *where);

typedef struct Dbg_WPStatus {
    int index;
    int initcount, countnow;
    Dbg_WatchPos what;
    Dbg_WatchPos target;
    bool bTargetSet;
    char *expr;
    Dbg_WPProc *p; void *p_arg;
    Dbg_PointType type;
    ARMword hwresource;
    int skip;
} Dbg_WPStatus;

Dbg_Error Dbg_SetWatchPoint(
    Dbg_MCState *state, Dbg_Environment *context, char const *watchee,
    char const *target,
    int count,
    char const *expr,
    Dbg_WPProc *p, void *arg);

/* Cause a watchpoint event if the value of <watchee> changes to the value of
   <target> (or changes at all if <target> is NULL).  <watchee> should
   evaluate either to an L-value (when the size of the object being watched is
   determined by its type) or to an integer constant (when the word with this
   address is watched).
 */

Dbg_Error Dbg_DeleteWatchPoint(Dbg_MCState *state, Dbg_Environment *context, char const *watchee);


Dbg_Error Dbg_SetWatchPoint_V(
    Dbg_MCState *state,
    char const *name, Dbg_Value const *val, char const *tname, Dbg_Value const *tval,
    int count,
    char const *expr,
    Dbg_WPProc *p, void *arg);

Dbg_Error Dbg_DeleteWatchPoint_V(Dbg_MCState *state, Dbg_Value const *val);


Dbg_Error Dbg_DeleteAllWatchPoints(Dbg_MCState *state);

typedef Dbg_Error Dbg_WPEnumProc(Dbg_MCState *state, Dbg_WPStatus const *watchee, void *arg);

Dbg_Error Dbg_EnumerateWatchPoints(Dbg_MCState *state, Dbg_WPEnumProc *p, void *arg);

Dbg_Error Dbg_WatchPointStatus(Dbg_MCState *state,
                       Dbg_WatchPos const *where, Dbg_WPStatus *status);

typedef void Dbg_WPRemovedProc(void *arg, Dbg_WPStatus const *wp);
Dbg_Error Dbg_RegisterWPRemovalProc(
    Dbg_MCState *state, Dbg_WPRemovedProc *p, void *arg);
/* When a watchpoint goes out of scope it is removed by the toolbox, and the
   function registered here gets called back to adjust its view
 */

typedef void Dbg_WatchAlteredProc(
    Dbg_MCState *state, void *arg, Dbg_Value const *where, bool set);
Dbg_Error Dbg_OnWatchAltered(
    Dbg_MCState *state, Dbg_WatchAlteredProc *p, void *arg);
/* Register function to be called back whenever a watchpoint is set or
 * cleared. (To allow multiple toolbox clients to coexist).
 */
Dbg_Error Dbg_RemoveOnWatchAltered(
    Dbg_MCState *state, Dbg_WatchAlteredProc *p, void *arg);

/*------------------- Handle-based Watch Point Interface -------------------*/

/* The handle-based Watch Point interface was introduced to permit multiple
   Watch Points to be set on the same Variable/Location. This requires that 
   the location of a WP is not used to identify it.

   This interface co-exists with the original interface which is required by
   existing DBT clients that do not support multiple WP per location.
   
   New DBT clients should use the Handle-based interface.
*/

/* The type of a Watch Point handle */
typedef int Dbg_WPHandle;

/* Set a Watch Point and return a unique Handle to it on success */
Dbg_Error Dbgcpp_SetWatchPoint_VH(Dbg_MCState     *state,
                                  char const      *name, 
                                  Dbg_Value const *val, 
                                  char const      *tname, 
                                  Dbg_Value const *tval,
                                  int              count,
                                  char const      *expr,
                                  Dbg_WPProc      *p, 
                                  void            *arg,
                                  Dbg_WPHandle    *hWP);

/* Set a Watch Point and retunr a unique handle to it on success */
Dbg_Error Dbgcpp_SetWatchPoint_H(Dbg_MCState     *state,
                                 Dbg_Environment *env, 
                                 char const      *watchee, 
                                 char const      *target,
                                 int              count,
                                 char const      *expr,
                                 Dbg_WPProc      *p, 
                                 void            *arg,
                                 Dbg_WPHandle    *hWP);

/* Delete a Watch Point using it's handle */
Dbg_Error Dbgcpp_DeleteWatchPoint_VH(Dbg_MCState        *state, 
                                     Dbg_WPHandle const hWP);

/* Get the status of a Watch Point given it's handle */
Dbg_Error Dbgcpp_GetWatchPointStatus_H(Dbg_MCState  *state, 
                                       Dbg_WPHandle  const hWP,
                                       unsigned32   *bIsHWActionPt, 
                                       int          *nHWResource, 
                                       int          *nCountNow);

/* Determine if a watch point is valid */
Dbg_Error Dbgcpp_IsWatchPointValid_H(Dbg_MCState  *state,
                                     Dbg_WPHandle  const hWP,
                                     unsigned32   *pbValid);

/*--------------------------------------------------------------------------*/

Dbg_Error Dbg_ProfileLoad(Dbg_MCState *state);

Dbg_Error Dbg_ProfileStart(Dbg_MCState *state, ARMword interval);
Dbg_Error Dbg_ProfileStop(Dbg_MCState *state);

Dbg_Error Dbg_ProfileClear(Dbg_MCState *state);

Dbg_Error Dbg_WriteProfile(Dbg_MCState *state, char const *filename,
                           char const *toolid, char const *arg);

/*--------------------------------------------------------------------------*/

Dbg_Error Dbg_ConnectChannel_ToHost(Dbg_MCState *state, RDICCProc_ToHost *p, void *arg);
Dbg_Error Dbg_ConnectChannel_FromHost(Dbg_MCState *state, RDICCProc_FromHost *p, void *arg);

/*--------------------------------------------------------------------------*/

/* Configuration data management */

Dbg_Error Dbg_LoadConfigData(Dbg_MCState *state, char const *filename);

Dbg_Error Dbg_SelectConfig(
    Dbg_MCState *state,
    RDI_ConfigAspect aspect, char const *name, RDI_ConfigMatchType matchtype,
    unsigned versionreq, unsigned *versionp);

Dbg_Error Dbg_ParseConfigVersion(
    char const *s, RDI_ConfigMatchType *matchp, unsigned *versionp);

typedef Dbg_Error Dbg_ConfigEnumProc(Dbg_MCState *state, RDI_ConfigDesc const *desc, void *arg);

Dbg_Error Dbg_EnumerateConfigs(Dbg_MCState *state, Dbg_ConfigEnumProc *p, void *arg);

/*--------------------------------------------------------------------------*/

/* Angel OS support */

Dbg_Error Dbg_CreateTask(Dbg_MCState **statep, Dbg_MCState *parent, bool inherit);
/* This is called when a task is to be debugged which has not been debugged
   before - ie. there is no existing Dbg_MCState for this task. It
   initialises a new Dbg_MCState and returns a pointer to it.
   <parent> is any valid previously-created MCCState. If <inherit> is TRUE,
   the new MCState inherits certain features from it (eg. symbols).
   Otherwise, only features which are the same across all tasks are inherited,
   (eg. global breakpoints).
 */

Dbg_Error Dbg_DeleteTask(Dbg_MCState *state);
/* This is called when a task dies, and frees up everything which relates to that
   task which is controlled by armdbg.
 */

Dbg_Error Dbg_DetachTask(Dbg_MCState *state);

Dbg_Error Dbg_AttachTask(Dbg_MCState *state);
/* These are called to request a switch of the current task.  First
   Dbg_DetachTask should be called with the state of the old task.
   Dbg_DetachTask will ensure that any cached state held by armdbg for
   the old task is immediately written out to the target.

   After Dbg_DetachTask is called and before Dbg_AttachTask is called
   the OS channel manager should tell the target that any future
   requests from the debugger will be fore the new task.

   If the new task does not have an armdbg state structure
   already, then Dbg_CreateTask should be called to create one (see
   above).  Then Dbg_AttachTask is called to tell armdbg to treat the
   new armdbg state as the current task.
 */

typedef Dbg_Error Dbg_TaskSwitchProc(void *arg, Dbg_MCState *newstate);

Dbg_Error Dbg_OnTaskSwitch(
    Dbg_MCState *state, Dbg_TaskSwitchProc *fn, void *arg);
/* The front end may register a callback which gets called by armdbg whenever
   Dbg_AttachTask is called.  This callback tells the front end the new current
   Dbg_MCState it should use to call armdbg.
   [Note that this is only useful if there is one front end shared between all
    tasks rather than one front end per task]
   The value of <arg> passed to Dbg_OnTaskSwitch is passed to <fn>
   when it is called.
 */
Dbg_Error Dbg_RemoveOnTaskSwitch(
    Dbg_MCState *, Dbg_TaskSwitchProc *, void *);

typedef Dbg_Error Dbg_RestartProc(
    void *arg, Dbg_MCState *curstate, Dbg_MCState **newstate);

Dbg_Error Dbg_OnRestart(Dbg_MCState *state, Dbg_RestartProc *fn, void *arg);
/* This is used by the OS channels layer to register a callback which
   will be made by the debugger toolbox early in the process of resuming
   execution.

   This callback must determine which task will be resumed when the target
   restarts execution.  If this is not already the current task then it must
   call Dbg_DetachTask and Dbg_AttachTask as decribed above to switch to the
   task about to be resumed and return the state for the new task in
   <newstate>.

   This will ensure that armdbg updates the correct task on execution as well
   as ensuring that stepping over a breakpointed instruction on restarting
   happens correctly.

   The value of <arg> passed to Dbg_OnRestart is passed to <fn>
   when it is called.
 */

Dbg_Error Dbg_RemoveOnRestart(Dbg_MCState *, Dbg_RestartProc *, void *);
/* Removes upcall as above */

/* This function is used to return the path of the image currently loaded.*/
char *Dbg_GetImagePath(Dbg_MCState *state, Dbg_SymTable *st);

int Dbg_LLNSyms(Dbg_SymTable *st);
Dbg_Error Dbg_GetLLSymbolInfo(Dbg_SymTable *st, unsigned int nSymNo, unsigned int *value, char **name);

Dbg_Error Dbg_GetFileTimeStamp(Dbg_MCState *state, Dbg_SymTable *st, char const *fname);


/*--------------------------------------------------------------------------*/

/* Minimalist Trace support */

Dbg_Error Dbg_TraceWrite(
    Dbg_MCState *state, char const *fname,
    RDI_TraceIndex low, RDI_TraceIndex high);
Dbg_Error Dbg_TraceStart(Dbg_MCState *state);
Dbg_Error Dbg_TraceStop(Dbg_MCState *state);
Dbg_Error Dbg_TraceFlush(Dbg_MCState *state);
Dbg_Error Dbg_TraceExtent(
    Dbg_MCState *state, 
    uint32 size, 
    RDI_TraceBufferExtentAction action, 
    void *value);
Dbg_Error Dbg_TraceTrigger(Dbg_MCState *state, RDI_TraceTriggerPosition *i);

/*--------------------------------------------------------------------------*/

/* Self-Describing Modules */

/*
 * Reading and writing SDM registers is controlled by the debugger,
 * the toolbox is just a route to the RDI calls.  This allows for
 * a much more natural and symmetric implementation.
 *
 * In the future the DBT may need to maintain cached data for expression
 * evaluation and backtrace if SDM is extended to executable processors.
 */


/*
 *  Enumerate the children of the current modules
 */
typedef Dbg_Error Dbg_SDMEnumerateChildModulesProc(Dbg_MCState *child, void *arg);

Dbg_Error Dbg_SDMEnumerateChildModules(Dbg_MCState *parent, Dbg_SDMEnumerateChildModulesProc *cb, void *arg);


Dbg_Error Dbg_SDMInit(Dbg_MCState *state);

Dbg_Error Dbg_SDMReadRegisterDescriptions(Dbg_MCState *state, unsigned int *sdm_regcount, struct RDIRegister const **sdm_regdescs);

Dbg_Error Dbg_SDMReadMemoryDescriptions(Dbg_MCState *state, unsigned int *sdm_memareacount, struct RDIMemoryArea const **sdm_memareas);

Dbg_Error Dbg_SDMReadTypeDescriptions(Dbg_MCState *state, unsigned int *sdm_typecount, char const ***sdm_typestrings);

/* this call is not part of the public interface and is currently not used */
/* Dbg_Error Dbg_SDMIdentifySpecialRegister(Dbg_MCState *state, unsigned int reg_id, unsigned int *regnum, unsigned int *regmode); */

Dbg_Error Dbg_SDMRequestVolatileMapping(Dbg_MCState *state, unsigned int regno, unsigned int mode, unsigned int *mappedregno, unsigned int *mappedmode);

Dbg_Error Dbg_SDMReadRegister(Dbg_MCState *state, unsigned int mode, unsigned int regno, ARMword *pvalue);

Dbg_Error Dbg_SDMReadRegisters(Dbg_MCState *state, unsigned int mode, unsigned int regmask, ARMword *pvalues);

Dbg_Error Dbg_SDMWriteRegister(Dbg_MCState *state, unsigned int mode, unsigned int regno, ARMword *pvalue);

Dbg_Error Dbg_SDMWriteRegisters(Dbg_MCState *state, unsigned int mode, unsigned int regmask, ARMword *values);

/*--------------------------------------------------------------------------*/

/* armdbg testing via magic armsd */

Dbg_Error Dbg_RunTests(Dbg_MCState *state);


/*
 * Extra internal toolconf words used to extend the armdbg interface. 
 * These are all of the form CONTROLLER_... and as such should be 
 * ignored by targets.
 */
#define Dbg_Cnf_Controller_TryNonStop       (tag_t)"CONTROLLER_TRYNONSTOP"
#define Dbg_Cnf_Controller_ForceNonStop     (tag_t)"CONTROLLER_FORCENONSTOP"
#define Dbg_Cnf_Controller_StartedNonStop   (tag_t)"CONTROLLER_STARTEDNONSTOP"
#define Dbg_Cnf_Controller_RDIVersion       (tag_t)"CONTROLLER_RDIVERSION"

#include "dbg_copy.h"



#ifdef __cplusplus
}
#endif


#endif /* armdbg_h */

/* End of armdbg.h */
