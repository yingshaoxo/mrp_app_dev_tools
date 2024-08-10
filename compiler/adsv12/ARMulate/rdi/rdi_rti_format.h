/* -*-C-*-
 *
 * $Revision: 1.4.4.2 $
 *   $Author: lmacgreg $
 *     $Date: 2001/06/14 16:13:57 $
 *
 * Copyright (c) ARM Limited 1998.  All rights reserved.
 *
 * rdi_rti_format.h - Trace extensions to RDI
 */


#ifndef __RDI_RTI_FORMAT_H__
#define __RDI_RTI_FORMAT_H__

#include "armtypes.h"
/* To allow this file to be used in Trace Components it should not pull in the rest of RDI */



#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

/*
 * RDI_TraceBufferFormat defines the formats to be used by the trace buffer (TPA). 
 * It consists of a sequence of bitmasks.
 */

typedef uint32 RDI_TraceBufferFormat; 

/* RDI_TraceBufferFormat bitmasks */

#define RDI_TraceBufferWidth4           0x00000001ul /* 4 bit data */
#define RDI_TraceBufferWidth8           0x00000002ul /* 8 bit data */
#define RDI_TraceBufferWidth16          0x00000004ul /* 16 bit data */

#define RDI_TraceMultiplexed            0x00000008ul /* muliplexing on ETM/buffer link */
#define RDI_TraceDemultiplexed          0x00000010ul /* demultiplexing on ETM/buffer link */
#define RDI_TraceClockBothEdges         0x00000020ul /* clock data on both edges on ETM/buffer link */

#define RDI_TraceETM7_9_protocols       0x00000040ul /* Use ETM7/ETM9 protocol */
#define RDI_TraceETM10_protocols        0x00000080ul /* Use ETM10 protocol */

#define RDI_TraceTimestamp              0x00000100ul /* Use timestamps */

/*
 * Raw Trace Format - used by the ARMulator
 */

/*
 * Typedef:  RDITrace_Raw
 * Purpose:  A trace packet of raw, uncompressed, data.  Each Trace packet
 *           consists of some flag words, an address, and a data word (which
 *           may or may not be used).  The structure is designed to be packed
 *           into a three-word packet. The exact packing is host dependent.
 */
typedef struct
{
    unsigned8 size; /*
                     * When a single transaction is split into
                     * multiple events the size field is set for the
                     * first event of the transaction as the total
                     * number of events in the transaction. For
                     * example, a burst transfer resulting from an LDM
                     * instruction may consist of one instruction
                     * event followed by up to sixteen data
                     * events. Subsequent events of the same
                     * transaction are flagged with a size of zero.  A
                     * Target that does not implement this facility
                     * can set size to one for all transactions.
                     */
    unsigned8 type; /*
                     * Encodes the format of the packet. (Six types
                     * are currently defined.)
                     */
    unsigned16 flags;
                    /* A bit-mask of values which determine the interpretation
                     * to be placed on the rest of the packet.  
                     */
    ARMword address;
    ARMword data;
} RDITrace_Raw;

/*
 * Enumeration: Allowed values of RDITrace_Raw.type
 */
enum
{
    RDITrace_Raw_PacketError            = 0,
    RDITrace_Raw_PacketInstr            = 1,
    RDITrace_Raw_PacketMem              = 2,
    RDITrace_Raw_PacketMem_Data         = 3,
    RDITrace_Raw_PacketMem_Addr         = 4,
    RDITrace_Raw_PacketMem_DataAddr     = 5
};

/*
 * Error packets are used to specify an error in the trace. Flags
 * encodes an error field; address and data may be used to convey more
 * information in an error-specific way.
 *
 * Where an error packet indicates a failure to decode a single instruction
 * it may be followed by the packets for that instruction (and its count
 * will include these packets). Some of the fields of these packets should#
 * be ignored.
 *
 * The following errors are defined:
 * Error Code   Description
 *
 *                  Errors codes indicating a break in the trace stream
 *                  ===================================================
 *
 * Stop  0      Trace stops at this point. (This isn't necessarily the last
 *              packet. It signals that the packets before and the packets
 *              after are not sequential.)
 *
 * SynchronisationLost  1 Where a decompressor (for example, one that
 *                        decompresses ARM Compressed format) is generating
 *                        the uncompressed format, this error may be used to
 *                        indicate that decompression has failed.
 *
 * UnknownCPDT  2 A particular case of SynchronisationLost where the
 *                synchronisation has been lost because of a CPDT instruction.
 *
 * Overflow 3 A particular case of SynchronisationLost where the
 *            synchronisation has been lost because of an internal FIFO
 *            overflow
 *
 * OutOfDebug 4 A particular case of a trace discontinuity, caused by the
 *              processor coming out of debug state
 *
 *              Errors codes indicating a problem on decoding a single instruction
 *              ==================================================================    
 *
 * BranchExpected  5 The trace stream indicated a branch, but the instruction
 *                   is not a branch - followed by all normal instruction and memory
 *                   packets
 *
 * UnexpectedIndirectBranch 6 The instruction is an indirect branch, but the trace
 *                            stream did not indicate a branch - followed by all normal
 *                            instruction an memory packets
 *
 * UnexpectedData 7 The trace stream contained data for an instruction that does not
 *                  access memory - followed by all normal instruction packets
 *
 * NotConditional 8 The trace stream indicated that an unconditional instruction was
 *                  not executed - followed by all normal instruction and memory packets
 *
 * InstructionNotRead 9 The decompressor could not read the memory containing the 
 *                      instruction - followed by an instruction packet only, ignore
 *                      instruction packet data
 * 
 * DataAddressUnknown 10 The decompressor could not determine the address of the data
 *                       associated with the instruction - followed by all normal instruction
 *                       and memory packets, ignore data addresses
 *
 */
#define RDITrace_Raw_ErrorPacket_Stop                       0
#define RDITrace_Raw_ErrorPacket_SynchronisationLost        1
#define RDITrace_Raw_ErrorPacket_UnknownCPDT                2
#define RDITrace_Raw_ErrorPacket_Overflow                   3
#define RDITrace_Raw_ErrorPacket_OutOfDebug                 4
#define RDITrace_Raw_ErrorPacket_BranchExpected             5
#define RDITrace_Raw_ErrorPacket_UnexpectedIndirectBranch   6
#define RDITrace_Raw_ErrorPacket_UnexpectedData             7
#define RDITrace_Raw_ErrorPacket_NotConditional             8
#define RDITrace_Raw_ErrorPacket_InstructionNotRead         9
#define RDITrace_Raw_ErrorPacket_DataAddressUnknown         10



/*
 * Memory packets
 *
 * Memory packets form a sequence of cycles. The address and data fields
 * represent the values on the address and data busses for that cycle.
 * Note: Only PacketMem_DataAddr has both the address and data field set. 
 * PacketMem_Data only has a data field; PacketMem_Addr only has an address
 * field; PacketMem has neither.
 *
 * The following flags are set for memory packets:
 */
#define RDITrace_Raw_MemoryWidth        0xful
#define RDITrace_Raw_MemoryByte         0x3ul
#define RDITrace_Raw_MemoryHalfword     0x4ul
#define RDITrace_Raw_MemoryWord         0x5ul

#define RDITrace_Raw_MemoryNrw          0x0010ul
#define RDITrace_Raw_MemorySeq          0x0020ul
#define RDITrace_Raw_MemoryNmreq        0x0040ul
#define RDITrace_Raw_MemoryNopc         0x0080ul
#define RDITrace_Raw_MemoryLock         0x0100ul
#define RDITrace_Raw_MemoryNtrans       0x0200ul
#define RDITrace_Raw_MemorySpec         0x0400ul
#define RDITrace_Raw_MemoryBurst        0x1000ul

/*
 * Instruction Packets
 *
 * Instruction packets come between memory packets, and represent an
 * instruction executed in the cycle represented by the preceding
 * memory packet. (Assuming that memory packets are being produced.)
 * The address field contains the PC value of the executed
 * instruction. The data field contains the instruction executed.  The
 * following flags are set for instruction packets:
 */

/*
 * Defines the instruction set the instruction was executed in. The
 * mask InstructionISA can be used to extract the instruction set
 * field. Its meaning is processor dependent. For the ARM, the values
 * ARM and Thumb are defined.
 */
#define RDITrace_Raw_InstructionISA     0xful
#define RDITrace_Raw_ISAARM             0x1ul
#define RDITrace_Raw_ISAThumb           0x2ul

/*
 * A field used to define when an instruction did not execute because
 * of some abort condition. A target may not necessarily set this
 * field. The field is set to one of the above values, depending on
 * which abort condition occurred. The InstructionTaken flag must
 * still be valid.
 */
#define RDITrace_Raw_InstructionAborted 0xf0ul
#define RDITrace_Raw_Undefined          0x10ul
#define RDITrace_Raw_SWI                0x20ul
#define RDITrace_Raw_PrefetchAbort      0x30ul
#define RDITrace_Raw_DataAbort          0x40ul
#define RDITrace_Raw_IRQ                0x60ul
#define RDITrace_Raw_FIQ                0x70ul
#define RDITrace_Raw_Reset              0xf0ul

/*
 * The instruction was executed. This will not necessarily be set for
 * instructions that are not conditional. All ARM instructions are
 * conditional. Only some Thumb instructions are conditional.
 * Where an instruction is not conditional, the value of this bit
 * should be considered as undefined.
 */
#define RDITrace_Raw_InstructionTaken   0x100ul

/*
 * A trigger point occurred during this instruction.
 */
#define RDITrace_Raw_Trigger            0x200ul

/**********************************************************************/

/*
 * Compressed Trace Format - used by ARMs TRACE port
 */

/*
 * SUBFORMAT
 *
 * The "subformat" word returned from RDITrace_Format is used by the ARM
 * Compressed format to specify:
 * - the width of the TRACEPKT port.
 * - the depth of the TRACEPKT fifo.
 * - whether data, addresses and data, or just addresses are being traced.
 */

/*
 * Macro:    RDITrace_ARMC_Data
 *
 * Purpose:  Trace port is configured to trace the data accessed on load/stores
 */
#define RDITrace_ARMC_Data              0x100000ul


/* 
 * Macro:   RDITrace_ARMC_Address
 *
 * Purpose: Trace port is configured to trace the address of an access
 *          on load/stores. (For address and data, both
 *          RDITrace_ARMC_Data and RDITrace_ARMC_Address are set.)
 *          Note: If neither bits are set, then loads and stores are
 *          not being traced, in which case the system will never
 *          produce ID or BD packets.
 */
#define RDITrace_ARMC_Address   0x200000ul

/*
 * Macro:   RDITrace_ARMC_Port...
 * Purpose: A field in sub-format that specifies the width of the TRACEPKT port
 */
#define RDITrace_ARMC_PortWidth  0xf0000ul
#define RDITrace_ARMC_Port4bit   0x20000ul
#define RDITrace_ARMC_Port8bit   0x30000ul
#define RDITrace_ARMC_Port16bit  0x40000ul

/*
 * Macro:   RDITrace_ARMC_FIFODepth
 *
 * Purpose: A mask which, when ANDed with sub-format, gives the depth
 *          of the TRACEPKT FIFO. May be zero, in which case the Trace
 *          Driver does not know the depth of the on-chip FIFO.
 */
#define RDITrace_ARMC_FIFODepth    0xfffful

/*
 * TBD: Address Packet Framing may be an option.
 */


/*
 * PACKET FORMAT
 *
 * Each Trace Event consists of a pair of values-stat and packet-related to
 * PIPESTAT, TRACESYNC and TRACEPKT in the Trace Port. However, the encodings
 * differ.
 */


/*
 * Typedef:  RDITrace_ARMC_Stat
 *
 * Purpose:  An enumeration of the possible values that can form an
 *           entry in the stat part of a Trace Event. For a detailed
 *           definition of each type, refer to [TRACE].
 */
typedef enum
{
    RDITrace_ARMC_IE,           /* Instruction, executed */
    RDITrace_ARMC_ID,           /* Instruction, executed, with data */
    RDITrace_ARMC_IN,           /* Instruction, not executed */
    RDITrace_ARMC_WT,           /* Wait */
    RDITrace_ARMC_BE,           /* Branch, executed */
    RDITrace_ARMC_BD,           /* Branch, executed, with data */
    RDITrace_ARMC_TG,           /* Trigger */
    RDITrace_ARMC_TD            /* Trace disabled */
} RDITrace_ARMC_Stat;

/*
 * Typedef:  RDITrace_ARMC
 *
 * Purpose:  A structure for an ARM Compressed Format trace event. Stat
 *           is actually of type RDITrace_ARMC_Stat, but is packed into
 *           an unsigned8. Packet is a 16-bit value, read from the
 *           TRACEPKT part of the Trace Port. In reality it may be 4-,
 *           8-, or 16-bits, depending on which format the Target
 *           declares to the Debug Controller.  Note: This format is
 *           designed to pack into a single word, in a host-dependent
 *           way.
 */
typedef struct
{
    unsigned8 stat;
    unsigned8 sync;
    unsigned16 packet;
} RDITrace_ARMC;

/*
 * Timestamping varieties of the compressed and un-compressed formats
 */
typedef struct
{
   uint32 lo;
   uint32 hi;
} RDITrace_ARMTime;

typedef struct
{
   RDITrace_Raw event;
   RDITrace_ARMTime time;
} RDITrace_Raw_WithTime;

typedef struct
{
   RDITrace_ARMC event;
   RDITrace_ARMTime time;
} RDITrace_ARMC_WithTime;

typedef struct
{
    RDITrace_Raw event;
    uint32 proc_id;
} RDITrace_Raw_WithProcId;

typedef struct
{
    RDITrace_Raw event;
    uint32 proc_id;
    RDITrace_ARMTime time;
} RDITrace_Raw_WithProcIdAndTime;

/* A bit corresponding to the status of Cycle Accurate tracing. */
/* May be used to correctly determine trace indexes in AXD */
#define RDITrace_Raw_CycleAccurate      0x8000
#define RDITrace_Raw_CycleMask          ~RDITrace_Raw_CycleAccurate

/**********************************************************************/

/*
 * XScale Compressed Trace Format - used by XScale's on-chip trace buffer
 */

#define RDITrace_XScale_Exception(vector)   (0x00 + (((vector) << 2) & 0x70))
#define RDITrace_XScale_ExceptionOf(byte, hivecs) ((((byte) & 0x70) >> 2) | (hivecs))
#define RDITrace_XScale_NonExceptionMask     0x80
#define RDITrace_XScale_DirectBranch         0x80
#define RDITrace_XScale_DirectBranchChecked  0xc0
#define RDITrace_XScale_CheckedMask          0x40
#define RDITrace_XScale_IndirectBranch       0x90
#define RDITrace_XScale_IndirectBranchChecked 0xa0
#define RDITrace_XScale_IndirectMask         0x10
#define RDITrace_XScale_RollOver             0xff
#define RDITrace_XScale_CountMask            0x0f


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /*__RDI_RTI_FORMAT_H__*/

