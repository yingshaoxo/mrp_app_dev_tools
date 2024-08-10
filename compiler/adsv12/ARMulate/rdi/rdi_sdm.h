/* -*-C-*-
 *
 * $Revision: 1.8.8.5 $
 *   $Author: hbullman $
 *     $Date: 2001/08/07 16:27:57 $
 *
 * Copyright (c) ARM Limited 2000.  All Rights Reserved.
 *
 * rdi_sdm.h - RDI Self Describing Modules, and RDI Code Sequences (as
 *             used by self describing modules)
 */

#ifndef rdi_sdm_h
#define rdi_sdm_h

#include "host.h"
#include "rdi.h"

/*
 * RDI Self Describing Modules
 */

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

#define RDIAttrib_Readable      0x0001
#define RDIAttrib_Writeable     0x0002
#define RDIAttrib_GotEndianness 0x0004
#define RDIAttrib_LittleEndian  0x0008
#define RDIAttrib_Volatile      0x0010
#define RDIAttrib_DWARF         0x0020 /* Register has a DWARF mapping */

typedef struct RDIRegister
{
    uint32 reg_num;        /* Corresponds to a bit in the mask in
                            * RDI_CPU[Read/Write]Proc
                            * Only values 0-31 are significant */
    uint32 mode;           /* Corresponds to the mode parameter of
                            * RDI_CPU[Read/Write]Proc */
    uint32 width;          /* In bits.  Must be greater than zero.  The actual
                            * occupied width will be rounded up to the nearest
                            * 32-bits.  So, a 33 bit register occupies 64 bits
                            * of space */
    uint32 attrib;         /* Readable, writable, ... [RDIAttrib_*] */
    char const *name;      /* As seen by the user.  Must be non-NULL */
    char const *desc;      /* A longer description */
    char const *bankname;  /* May be NULL */
    char const *displaytype;
                           /* May be NULL, otherwise refers to either a
                            * standard type, or a  type exported by
                            * RDIInfo_DescribeTypeDefinitions */
} RDIRegister;

/*
 *        Function: RDIInfo_DescribeRegisterMap
 *         Purpose: Allow a module to describe its register map
 *
 *  Pre-conditions: 
 *
 *          Params:
 *                  Out: (unsigned *)arg1 = nRegs
 *                       (*nRegs) is the total number of registers in the
 *                        module, which is the number of elements in array.
 *
 *                  Out: (RDIRegister **)arg2 = array
 *                       Return an array of RDIRegister in this pointer, if it
 *                       is non-NULL.  The array memory is owned by the target
 *                       and is guaranteed to be valid until
 *                       RDI_CloseProc(module).
 *
 *         Returns: RDIError_NoError: OK
 *                  RDIError_UnimplementedMessage: Module does not support
 *                            self description of registers
 */
#define RDIInfo_DescribeRegisterMap 0x730


#define RDISpecialRegister_PC 1
#define RDISpecialRegister_LR 2

/*
 *        Function: RDIInfo_RequestVolatileMapping
 *         Purpose: 
 *
 *  Pre-conditions: 
 *
 *          Params:
 *                   In: 
 *               In/Out: 
 *                  Out: 
 *
 *         Returns: 
 *
 * Post-conditions: 
 *                                  
 */
#define RDIInfo_RequestVolatileMapping  0x731


struct RDIMemoryArea
{
    uint32 base;     /* The base address for this memory area, in the context
                      * of RDI_[Read/Write]Proc */
    uint32 length;   /* The length of this memory area, in the context of
                      * RDI_[Read/Write]Proc */
    uint32 priority; /* If memory areas overlap, higher priority areas override
                      * lower priority areas */
    RDI_AccessType access;
                     /* Describes the access type that should, by default, be
                      * used for this area */
    uint32 attrib;   /* Readable, writable, endianness ... [RDIAttrib_*] */
    char const *name;/* A name for this memory area.  May be NULL */
    char const *displaytype;
                     /* Default display type for memory within this area.
                      * May be NULL, otherwise refers to either a standard type
                      * or a type exported by RDIInfo_DescribeTypeDefinitions 
                      */
};


/*
 *        Function: RDIInfo_DescribeMemoryAreas
 *         Purpose: Allow a module to describe its memory map
 *
 *  Pre-conditions: 
 *
 *          Params:
 *               Out: (unsigned *)arg1 = nAreas
 *                    (*nAreas) is the total number of memory areas in the
 *                    module, which is the number of elements in array.
 *
 *               Out: (RDIMemoryArea **)arg2 = array
 *                    Return an array of RDIMemoryArea in this pointer, if it
 *                    is non-NULL.  The array memory is owned by the target
 *                    and is guaranteed to be valid until
 *                    RDI_CloseProc(module).
 *
 *         Returns: RDIError_NoError: OK
 *                  RDIError_UnimplementedMessage: Module does not support
 *                            self description of memory areas
 */
#define RDIInfo_DescribeMemoryAreas 0x732



/*
 *        Function: RDIInfo_RequestDataTypes
 *         Purpose: Return an array of strings, containing any data type
 *                  definitions required by the module
 *
 *  Pre-conditions: None
 *
 *          Params:
 *                  Out: (unsigned *)arg1 = nStrings
 *                       (*nStrings) is the number of items in the array.
 *                  Out: (char const ***)arg2 = arrayOfStrings
 *                       Target returns a pointer to an array of strings in
 *                        (*arrayOfStrings).  These strings 
 *                        contain any target defined type definitions,
 *                        in the format described above.  There may be one
 *                        or more than one type definition in each string
 *                        Memory space occupied by these is owned by the debug
 *                        target,and is guaranteed to last until RDI_CloseProc.
 *
 *         Returns: RDIError_NoError     OK
 *         RDIError_UnimplementedMessage Module does not have any user defined
 *                                        types
 *
 *
 * Post-conditions: None
 *                                  
 */
#define RDIInfo_RequestDataTypes 0x733

/*
 *        Function: RDIInfo_APCDescription
 *         Purpose: Provide processor name, revision and board to the
 *                  debug controller.
 *
 *  Pre-conditions: Module must be an APC module.
 *
 *          Params:
 *                  Out: arg1 is of type (RDIProcessorDescription **)
 *
 *                       The target sets arg1 to point to a target description
 *                       structure (which resides in memory owned by the debug
 *                       target). The structure is guaranteed to be unchanging
 *                       and valid until the module is closed.
 *
 *                       Any fields of arg1, except size, can be unknown,
 *                       in which case they must be NULL.
 *
 *                       arg2 is not used (and should be passed in as NULL)
 *
 *         Returns: RDIError_NoError OK
 *         RDIError_UnimplementedMessage This is not an APC module
 *
 * Post-conditions: none
 *                                  
 */
typedef struct RDIProcessorDescription
{
    unsigned size; /* = 24 */
    char const *processor_name;     /* Parent processor, e.g., "ARM9E-S" */
    char const *processor_revision; /* Revision of above, e.g., "0" */
    char const *board_name;         /* Name of the development board the
                                     * processor is on */
    char const *core_name;          /* If the processor is an ASIC, this
                                     * identifies the underlying core, else
                                     * NULL */
    char const *core_revision;      /* If the processor is an ASIC, this
                                     * identifies the revision of the
                                     * underlying core, else NULL */
} RDIProcessorDescription;

#define RDIInfo_APCDescription   0x734

/*
 *        Function: RDIInfo_RequestDWARFMapping
 *         Purpose: Provide dwarf mapping
 *
 *          Params:
 *                  In: (*(unsigned *)arg1) =  Index into register array
 *                                             returned from
 *                                             RDIInfo_DescribeRegisterMap
 *
 *                  Out: (*(unsigned *)arg2) = DWARF register index
 *
 *         Returns:
 *          RDIError_NoError              OK
 *          RDIError_UnimplementedMessage This register is not a DWARF register
 *
 */
#define RDIInfo_RequestDWARFMapping 0x735


/*****************************************************************************/

/*
 * RDI Code Sequences
 */

typedef enum RDICodeSequence_ISet
{
    RDICodeSequenceISet_ARMv4T,
    RDICodeSequenceISet_Thumbv4T,
    RDICodeSequenceISet_ARMv5T,
    RDICodeSequenceISet_Thumbv5T,
    RDICodeSequenceISet_JTAG

} RDICodeSequence_ISet;

typedef enum RDICodeSequence_Type
{
    RDICodeSequenceType_Arbitrary,
    RDICodeSequenceType_CoProReg,
    RDICodeSequenceType_CoProMem,
    RDICodeSequenceType_Memory

} RDICodeSequence_Type;

typedef enum RDICodeSequence_Mode
{
    RDICodeSequenceMode_User = 0x10,
    RDICodeSequenceMode_FIQ = 0x11,
    RDICodeSequenceMode_IRQ = 0x12,
    RDICodeSequenceMode_SVC = 0x13,
    RDICodeSequenceMode_Abort = 0x17,
    RDICodeSequenceMode_Undef = 0x1b,
    RDICodeSequenceMode_System = 0x1f,
    RDICodeSequenceMode_Any = -1,
    RDICodeSequenceMode_AnyPrivileged = -2

} RDICodeSequence_Mode;

typedef struct RDICodeSequence_Data
{
    RDICodeSequence_ISet iset;
    RDICodeSequence_Type type;
    RDICodeSequence_Mode mode;

    unsigned numDataIn, numDataOut, numInstructions;

    uint32 *dataIn;
    uint32 *dataOut;
    uint32 *instructions;

    uint32  flags;      /* see #defines below */
    uint32  stacksize;  /* zero if no stack required */
    uint32  data;       /* for expansion */

} RDICodeSequence_Data;

#define RDICodeSequence_Flag_DataRepresentsRegs 0x0001
#define RDICodeSequence_Flag_UseJTAGIfPossible  0x0002

#define RDICodeSequence_FlagCare_IRQ            0x0010
#define RDICodeSequence_Flag_IRQEnabled         0x0020
#define RDICodeSequence_FlagCare_FIQ            0x0040
#define RDICodeSequence_Flag_FIQEnabled         0x0080
#define RDICodeSequence_FlagCare_SystemSpeed    0x0100
#define RDICodeSequence_Flag_RunAtSystemSpeed   0x0200

/*
 * if (flags & RDICodeSequence_EnumCare_OperationType)
 *    enum RDICodeSequence_Enum_Operation operation = 
 *        (flags & RDICodeSequence_EnumMask_OperationType) >>
 *        RDICodeSequence_EnumShift_OperationType
 */
#define RDICodeSequence_EnumShift_OperationType 11
#define RDICodeSequence_EnumMask_OperationType  0x7800
enum RDICodeSequence_Enum_Operation
{
    RDICodeSequence_Enum_NotCacheOperation = 0,
    RDICodeSequence_Enum_JointCacheOperation,
    RDICodeSequence_Enum_ICacheOperation,
    RDICodeSequence_Enum_DCacheOperation,
    RDICodeSequence_Enum_JointRAMOperation,
    RDICodeSequence_Enum_IRAMOperation,
    RDICodeSequence_Enum_DRAMOperation
};

/*
 *        Function: RDIInfo_CodeSequenceInquiry
 *         Purpose: Does the target support code sequences over RDI?
 *
 *  Pre-conditions: none
 *
 *          Params: none
 *
 *         Returns: RDIError_NoError - code sequences supported
 *                  RDIError_UnimplementedMessage - code sequences unsupported
 *
 * Post-conditions: none
 *                                  
 */
#define RDIInfo_CodeSequenceInquiry 0x740

/*
 *        Function: RDIInfo_CodeSequence
 *         Purpose: Instruct the target to execute a sequence of code
 *
 *  Pre-conditions: none
 *
 *          Params:
 *               In: arg1 = (RDICodeSequence_Data const *) Contents are not
 *                                                         to be modified by
 *                                                         target
 *
 *         Returns: RDIError_NoError - code executed as requested
 *                  RDIError_BadCPUStateSetting - unsupported "iset"
 *                  RDIError_UnimplementedType - unsupported "type"
 *                  RDIError_InsufficientPrivilege - unsupported "mode"
 *                  RDIError_OutOfStore - too much stack space requested
 *                  RDIError_IllegalInstruction - target refuses to run opcodes
 *                  RDIError_UnimplementedMessage - code sequences unsupported
 *
 * Post-conditions: If no error, sequence was run
 *                                  
 */
#define RDIInfo_CodeSequence 0x741


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /* !def rdi_sdm_h */

/* EOF rdi_sdm.h */
