/* -*-C-*-
 *
 * $Revision: 1.13.4.2 $
 *   $Author: lmacgreg $
 *     $Date: 2001/06/14 16:13:57 $
 *
 * Copyright (c) ARM Limited 1998.  All rights reserved.
 *
 * rdi_rti.h - Trace extensions to RDI
 */

#ifndef RDI_RTI_H
#define RDI_RTI_H

#include "host.h"
#include "rdi_info.h"
#include "rdi150.h"
#include "rdi_rti_format.h"


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

/*
 * Scope
 * -----
 *
 * The RDI Trace Interface (also occasionally known as RTI) defines
 * the interface between a trace-aware Debug Controller (such as a
 * enhanced version of MDW) and a Debug Target (such as an ARM9TDMI
 * with trace port, connected to a Hewlett-Packard nTRACE unit).
 */

/**********************************************************************/

/*
 * Generic typedefs
 */

/*
 * Typedef: RDI_TraceEvent
 * Purpose: An opaque structure containing a single trace event. The exact
 * structure of this type depends upon the format of the trace data.  */
typedef struct RDI_TraceEvent RDI_TraceEvent;

/*
 * Typedef: RDI_TraceIndex
 * Purpose: A number that uniquely identifies a single trace event
 *          within a trace buffer. If an event has an index of n,
 *          then the next event in that block will have an index of
 *          n + 1. An index may be negative -- some Targets may
 *          choose to place index zero at the start of the buffer
 *          (that is, all indexes are positive). Others may place
 *          the zero anywhere in the buffer, usually at the "trigger
 *          point" (so indexes range from negative through zero to
 *          positive. It is also possible that the buffer contains
 *          only negative values and zero.
 */
typedef int32 RDI_TraceIndex;


/**********************************************************************/

/*
 * RDI_InfoProc entry points
 */

/*
 * InfoProc: RDI_InfoProc(mh, RDIInfo_Trace, arg1, arg2)
 *
 * Purpose:  Inquire whether a processor on a Debug Target supports
 *           tracing. A Debug Controller should call this Info call
 *           before trying to use the other Trace calls.
 *
 * Params:   mh  handle identifies processor
 *
 * Return:   RDIError_NoError              Processor supports tracing.
 *           RDIError_UnimplementedMessage Processor does not support tracing.
 */
#define RDIInfo_Trace 0x600 /* Sanity Check - duplicated in rdi_info.h */


/*
 * Typedef:  RDI_TraceFormat
 *
 * Purpose:  An enumeration of trace formats. A Debug Controller that
 *           does not have any preference for a format the Debug Target
 *           should use may specify NoFormat.  The Uncompressed format
 *           and ARMCompressed format are defined in the RTI Specification
 *           Document TRACE-0000-PRIV-ESPC.
 *
 * THIS TYPE IS DEPRECATED, AND SHOULD NOT BE USED IN NEW CODE
 */
typedef enum
{
    RDITrace_NoFormat,
    RDITrace_Uncompressed,
    RDITrace_ARMCompressed,
    RDITrace_ARMUncompressedWithTime,
    RDITrace_ARMCompressedWithTime,
    RDITrace_XScaleCompressed
} RDI_TraceFormat;

/* 
 * InfoProc: RDI_InfoProc(mh, RDITrace_Format, arg1, arg2)
 *
 * Purpose:  RDITrace_Format allows a Debug Controller to specify what
 *           format of trace data it would prefer, and allows a Debug
 *           Target to specify what format it can provide.
 *
 *           ******************* OBSELETE *******************************
 *
 * Params:
 *  In: mh   Handle of module
 *  In: arg1 (RDI_TraceFormat *) The trace format the Debug Controller would
 *                               prefer. NoFormat if the Debug Controller has
 *                               no preference.
 *  Out:arg1                     The trace format the Debug Target will
 *                               provide. If the Debug Controller cannot use
 *                               this format, it should not attempt to use the
 *                               trace information.
 *  Out:arg2 (uint32 *)          subfmt - A supplementary word describing the
 *                               sub-format used. Its interpretation is
 *                               format-dependant.
 *
 * Return:   RDIError_NoError   
 *           RDIError_UnimplementedMessage Processor does not support tracing.
 *
 *  
 * THIS FUNCTION IS DEPRECATED, AND SHOULD NOT BE USED IN NEW CODE
 */
#define RDITrace_Format (RDIInfo_Trace + 1)


/*
 * Typedef:  RDI_TraceControl
 * Purpose:  An enumeration of values that may be passed to RDITrace_Control.
 */
typedef enum
{
    RDITrace_Start,
    RDITrace_Stop,
    RDITrace_Flush,
    RDITrace_ResetMarker
} RDI_TraceControl;

/*
 * InfoProc: RDI_InfoProc(mh, RDITrace_Control, arg1, arg2)
 * Purpose:  RDITrace_TraceControl allows a Debug Controller to turn on/off
 *           tracing.
 *
 * Params:
 *  In: arg1 (RDI_TraceControl *) Pointer to a word controlling tracing. 
 *
 * Return:   RDIError_NoError   
 *           RDIError_UnimplementedMessage Processor does not support tracing.
 */
#define RDITrace_Control (RDIInfo_Trace + 2)

/*
 * Typedef:  RDI_TraceBufferExtentAction
 * Purpose:  To define the action to be taken by RDITrace_BufferExtent
 */
typedef enum {
    RDITrace_GetLow,
    RDITrace_GetHigh,
    RDITrace_GetConfiguredCapacity,
    RDITrace_GetMinimumCapacity,
    RDITrace_GetMaximumCapacity,
    RDITrace_SetCapacity
} RDI_TraceBufferExtentAction;

/*
 * InfoProc: RDI_InfoProc(mh, RDITrace_BufferExtent, arg1, arg2)
 * Purpose:  To return the lowest and highest entries currently in the trace
 *           buffer. Optionally, to also set the size of that buffer.
 *
 * Params:
 *  In: arg1 (uint32) Must be zero.
 *      arg2 (uint32) The size of the buffer to set, or zero.
 *
 * Out: arg1 (RDI_TraceIndex) The lowest index in the buffer.
 *      arg2 (RDI_TraceIndex) The highest index in the buffer.
 *
 * Return:   RDIError_NoError   
 *           RDIError_UnimplementedMessage Processor does not support tracing.
 */
#define RDITrace_BufferExtent (RDIInfo_Trace + 3)


/*
 * InfoProc: RDI_InfoProc(mh, RDITrace_GetBlockSize, arg1, arg2)
 * Purpose:  To return the optimal size for loading data from the trace
 *           collection tool.
 *
 * Params:
 * Out: arg1 (uint32) The optimal size for fetching blocks, or zero.
 *
 * Return:   RDIError_NoError   
 *           RDIError_UnimplementedMessage Processor does not support tracing.
 */
#define RDITrace_GetBlockSize (RDIInfo_Trace + 4)


/*
 * Typedef:  RDI_TraceTriggerPosition
 * Purpose:  Enumeration of locations for trigger to be placed in the buffer.
 */

typedef enum {
    RDITrace_TriggerAtEnd   = -3,
    RDITrace_TriggerAbout   = -2,
    RDITrace_TriggerAtStart = -1
} RDI_TraceTriggerPosition;

/*
 * InfoProc: RDI_InfoProc(mh, RDITrace_SetTriggerPosition, arg1, arg2)
 * Purpose:  To return the optimal size for loading data from the trace
 *           collection tool.
 *
 * Params:
 *  In: arg1 (RDI_TraceTriggerPosition) The position to place the trigger at.
 *           May also be a +ve, target-dependent, value.
 *
 * Out: arg1 (RDI_TraceTriggerPosition) The position the trigger was actually
 *           set at.
 *
 * Return:   RDIError_NoError   
 *           RDIError_UnimplementedMessage Processor does not support tracing.
 */
#define RDITrace_SetTriggerPosition (RDIInfo_Trace + 5)


typedef enum {
    RDI_TraceReady_Readable = 0x1,
    RDI_TraceReady_Empty = 0x2, 
    RDI_TraceReady_Triggered = 0x4
} RDI_TraceReadyReason;

/*
 * Typedef:  RDI_TraceReadyArg
 * Purpose:  An opaque handle to be passed to a TraceReadyProc.
 */
typedef struct RDI_TraceReadyArgStr RDI_TraceReadyArg;

/*
 * Typedef:  RDI_TraceReadyProc
 * Purpose:  Function to call when the Trace Buffer becomes ready or not
 *           ready to be read.  Must be called whenever this state changes.
 *           A "reason" enumeration gives the reason why the call was made.
 */ 
typedef void RDI_TraceReadyProc(
        RDI_ModuleHandle *module, 
        RDI_TraceReadyArg *arg,
        RDI_TraceReadyReason reason);

/*
 * InfoProc: RDI_InfoProc(mh, RDITrace_SetReadyProc, arg1, arg2)
 * Purpose:  RDITrace_SetReadyProc allows a function to be registered, to be
 *           called when a Trace Buffer is ready to be read.
 *
 * Params:
 *  In: arg1 (RDI_TraceReadyProc *) Pointer to a function to call when the
 *                                  Trace Buffer is ready to be read. If NULL,
 *                                  the callback is removed.
 *  In: arg2 (RDI_TraceReadyArg *)  Argument to pass to the TraceReadyProc.
 *
 * Return:   RDIError_NoError   
 *           RDIError_UnimplementedMessage Processor does not support tracing.
 */
#define RDITrace_SetReadyProc (RDIInfo_Trace + 6)


/*
 * InfoProc: RDI_InfoProc(mh, RDITrace_Read, arg1, arg2)
 * Purpose:  RDITrace_Read reads data from a trace block into a buffer.
 *
 * Params:
 * In/Out:
 *     arg1  (RDI_TraceEvent *)  A buffer to place the trace data into.
 * In: arg2  A structure of the type:
 *           struct
 *           {
 *              uint32 size;
 *              RDI_TraceIndex start;
 *           }
 *           size is the size of the buffer, in terms of the size of a
 *           single RDI_TraceEvent, that is, the number of events that
 *           can fit in the buffer. start is the index of the first
 *           event to copy to buffer.
 *
 * Out:arg2  The same structure, but with size modified to be the number of
 *           events written to the buffer; and start modified to be the index
 *           of the first event actually copied.
 *           The first event copied must be the one specified, or any earlier
 *           event. The events copied must include the one specified.
 *
 * Return:   RDIError_NoError   
 *           RDIError_UnimplementedMessage Processor does not support tracing.
 *           RDIError_NoSuchHandle         No event with that index has been
 *                                         traced.
 */
#define RDITrace_Read (RDIInfo_Trace + 7)

/* 
 * InfoProc: RDI_InfoProc(mh, RDITrace_SetInputFormat, arg1, arg2)
 *
 * Purpose:  RDITrace_SetInputFormat allows an RDI Controller to tell a Trace
 *           Buffer DebugTarget what format the Trace Run Debug Target is
 *           providing.
 *
 *
 * Params:
 *  In: mh   Handle of module
 *
 *  In: arg1 (RDI_TraceFormat *) The trace format the Debug Controller would
 *                               prefer. NoFormat if the Debug Controller has
 *                               no preference.
 *  In: arg2 (uint32 *) Word describing the subformat.         
 *
 *
 * Return:   RDIError_NoError   
 *           RDIError_UnimplementedMessage Processor does not support tracing.
 *
 * THIS FUNCTION IS DEPRECATED, AND SHOULD NOT BE USED IN NEW CODE
 */
#define RDITrace_SetInputFormat (RDIInfo_Trace + 8)


/* 
 * InfoProc: RDI_InfoProc(mh, RDITrace_GetFormat, arg1, arg2)
 *
 * Purpose:  RDITrace_GetFormat allows a Debug Controller to specify what
 *           format of trace data it would prefer, and allows a Debug
 *           Target to specify what format it can provide.
 *
 *
 * Params:
 *  In: mh   Handle of module
 *  In: arg1 (RDI_TraceFormat *) The trace format the Debug Controller would
 *                               prefer. NoFormat if the Debug Controller has
 *                               no preference.
 *  Out:arg1                     The trace format the Debug Target will
 *                               provide. If the Debug Controller cannot use
 *                               this format, it should not attempt to use the
 *                               trace information.
 *  Out:arg2 (uint32 *)          subfmt - A supplementary word describing the
 *                               sub-format used. Its interpretation is
 *                               format-dependant.
 *
 * Return:   RDIError_NoError   
 *           RDIError_UnimplementedMessage Processor does not support tracing.
 *
 * THIS FUNCTION IS DEPRECATED, AND SHOULD NOT BE USED IN NEW CODE
 */
#define RDITrace_GetFormat (RDIInfo_Trace + 9)

/* 
 * InfoProc: RDI_InfoProc(mh, RDIInfo_TraceRun, arg1, arg2)
 *
 * Purpose:  RDIInfo_TraceRun allows a Debug Controller to detect whether this
 *           RDI target supports the trace run control functions.
 *
 *
 * Params:
 *  In: mh   Handle of module
 *
 * Return:   RDIError_NoError   
 *           RDIError_UnimplementedMessage Processor does not support trace run control.
 */
#define RDIInfo_TraceRun (RDIInfo_Trace + 10)

/* 
 * InfoProc: RDI_InfoProc(mh, RDIInfo_TraceBuffer, arg1, arg2)
 *
 * Purpose:  RDIInfo_TraceRun allows a Debug Controller to detect whether this
 *           RDI target supports the trace buffer control functions.
 *
 *
 * Params:
 *  In: mh   Handle of module
 *
 * Return:   RDIError_NoError   
 *           RDIError_UnimplementedMessage Processor does not support trace buffer control.
 */
#define RDIInfo_TraceBuffer (RDIInfo_Trace + 11)

/* 
 * InfoProc: RDI_InfoProc(mh, RDITrace_ReadMarkerPos, arg1, arg2)
 *
 * Purpose:  RDITrace_ReadMarkerPos allows a Debug Controller to read the position of the
 *           trace marker
 *
 *
 * Params:
 *  In: mh   Handle of module
 *  Out:arg1                     The position of the trace marker, within the trace buffer.
 *                               The trace buffer has a readable size as given by 
 *                               RDITrace_BufferExtent.
 *
 *                               If the marker position is outside the extent of the buffer, 
 *                               then it is said to be invalid.  This can occur when a wrapping 
 *                               trace buffer has overwritten the event that the marker 
 *                               originally pointed at.  This also occurs if the marker has been 
 *                               reset (using RDITrace_Control), but no further trace events have 
 *                               yet been generated.
 *
 * Return:   RDIError_NoError   
 *           RDIError_UnimplementedMessage Processor does not support tracing.
 */
#define RDITrace_ReadMarkerPos (RDIInfo_Trace + 12)
/* 
 * InfoProc: RDI_InfoProc(mh, RDITrace_GetBufferFormats, arg1, arg2)
 *
 * Purpose:  Using RDITrace_GetBufferFormats the application can discover what formats, and
 *           combinations of formats, the Trace Buffer supports. On each call the application
 *           asks the target "If I select these features of the format, what additional format
 *           bits do I have to set, and to what values?"
 *
 *
 * Params:
 *  In: mh   Handle of module
 *  In:arg1  (RDI_TraceBufferFormat *)  A mask of the bits of the format value already chosen
 *                                      by the application.
 *  Out:arg1 (RDI_TraceBufferFormat *)  A mask of the bits that must be set in any format 
 *                                      compatible with the format asked about. This will be
 *                                      those bits that were in the input mask, plus any other
 *                                      bits that are forced to a particular value by having
 *                                      set those bits.
 *  In: arg2 (RDI_TraceBufferFormat *)  The values of the bits already chosen by the debug
 *                                      controller. Bits not in the input mask should be 0 but
 *                                      must be ignored by the target.
 *  Out:arg2 (RDI_TraceBufferFormat *)  The values of bits forced by the target. Bits not in the
 *                                      output mask should be 0 but must be ignored by the
 *                                      application.
 * Return:   RDIError_NoError   
 *           RDIError_UnimplementedMessage  Processor does not support tracing.
 *           RDIError_BadFormat             The Trace Buffer cannot support any format that includes the 
 *                                          bits defined in the input arguments.
 */
#define RDITrace_GetBufferFormats (RDIInfo_Trace + 13)
/* 
 * InfoProc: RDI_InfoProc(mh, RDITrace_SetBufferFormat, arg1, arg2)
 *
 * Purpose:  Sets the formats to be be used by the trace buffer on its two interfaces.
 *
 * Params:
 *  In: mh   Handle of module
 *  In:arg1  (RDI_TraceBufferFormat *)  The format to be used.
 *
 * Return:   RDIError_NoError   
 *           RDIError_UnimplementedMessage  Processor does not support tracing.
 *           RDIError_BadFormat             The Trace Buffer cannot support this format
 */
#define RDITrace_SetBufferFormat (RDIInfo_Trace + 14)
/**********************************************************************/
/**********************************************************************/


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif


#endif /* !def RDI_RTI_H */

/* EOF rdi_rti.h */

