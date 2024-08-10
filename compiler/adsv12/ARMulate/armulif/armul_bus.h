/*
 * armul_bus.h
 *
 * RCS $Revision: 1.17.6.7 $
 * Checkin $Date: 2001/08/24 12:57:49 $
 * Copyright (C) 2000 - 2001 ARM Ltd.  All rights reserved.
 * Revising $Author: lmacgreg $
 */
#ifndef armul_bus__h
#define armul_bus__h

#ifndef armul_types__h
# include "armul_types.h" /* For ARMTime */
#endif

#include "cvector.h"      /* for CVector */
#include "simplelinks.h"  /* for ARMul_TimedCallbackList */

#include "armul_method.h" /* for InterfaceSource */


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

struct RDI_ModuleDesc;

/* We do not have BT_Harvard - there is no such thing as a harvard bus! */
enum BusType {
    BT_ByteLane = 1,
    BT_Endian = 2,
    BT_Coprocessor = 4,
    BT_Harvard = 8
};

/* Actions you can ask a Bus Registration Function to perform. */
enum BusRegAct {
    BusRegAct_Insert = 1, BusRegAct_Remove
};

struct ARMul_Bus;
typedef void ARMul_BusFinaliseFunc(struct ARMul_Bus *bus);

struct ARMul_AccessRequest;

/*
 * Type of a function to perform an access to a >Bus<
 * (Which may or may not access a peripheral.) 
 */
typedef int ARMul_BusAccessFunc(struct ARMul_Bus *bus, 
                                struct ARMul_AccessRequest *req);

/*
 * Type of function called to access a peripheral on a bus.
 * It's duty is to return one of
 * RDIError_NoError (with data read or written)
 * RDIError_DataAbort
 * RDIError_Busy (although it may advance time on the bus instead).
 */
typedef int ARMul_BusPeripAccessFunc(void *handle, 
                                 struct ARMul_AccessRequest *req);


typedef struct AddressRange
{
    uint32 mask, lo, hi;
} AddressRange;


/*
 * Type of data used to register a peripheral to a decoder on a bus.
 */
typedef struct ARMul_BusPeripAccessRegistration {
    ARMul_BusPeripAccessFunc *access_func;
    void *access_handle;
    uint32 capabilities; /* See PeripAccessCapability_* below */
    struct ARMul_Bus *bus;

    unsigned priority;

    unsigned access_frequency;

    unsigned addr_size; /* Number of elements in range[] */
    AddressRange range[1];
} ARMul_BusPeripAccessRegistration;

#define PeripAccessCapability_Byte (1 << 3)
#define PeripAccessCapability_HWord (1 << 4)
#define PeripAccessCapability_Word (1 << 5)
#define PeripAccessCapability_DWord (1 << 6)
#define PeripAccessCapability_Idles    0x10000UL
#define PeripAccessCapability_Endian   0x20000UL
#define PeripAccessCapability_Bytelane 0x40000UL

#define PeripAccessCapability_Minimum \
  (PeripAccessCapability_Endian + PeripAccessCapability_Word)
#define PeripAccessCapability_Typical \
  (PeripAccessCapability_Endian + 0x38)



/*
 * Type of function used to register a peripheral to a decoder on a bus.
 */

typedef int ARMul_BusRegisterPeripFunc(enum BusRegAct act,
                                       ARMul_BusPeripAccessRegistration *breg);


typedef struct ARMul_Bus {
    /* "Real" local time is bus_BusyUntil * bus_CycleLength + bus_Phase. */
    ARMTime bus_BusyUntil;

    /* - Configuration-options - */
    uint32 bus_CycleLength;
    uint32 bus_LogWidth;   /* log2 of number of bits on bus. */

    /* This callback allows a DMA-peripheral to query memory on the same
     * bus. It is provided by the owner of this structure during initialisation
     * (before the peripherals on that bus are initialised).
         */
    ARMul_BusAccessFunc *bus_decoderFunc;
    void *bus_DecoderHandle;
    /*
     * This function must allow "registration" of peripherals with the
     * decoder for that bus.
     */
    ARMul_BusRegisterPeripFunc *bus_registerPeripFunc;
    void *bus_RegisterPeripHandle;

    ARMul_BusFinaliseFunc *bus_finalise;

    enum BusType bus_Type;

        /* All the above are known about by  peripherals, though none
         * contain any buses. */

        /* Zero (and unsupported) in most models, this is here for future use. */
        uint32 bus_Phase;

        /* And for all the other functions - e.g. any Arbiters,
         * but especially timer-handlers. 
         * NB It shall become a requirement of all bus_ifsrc's that they allow
         * access to the bus-decoder, named "@ARMul_BusAccessFunc", as well as 
         * data-pointers to all the data on the bus. This allows the entire
         * structure to be abstracted.
         */
        InterfaceSource bus_ifsrc;
        /* we bolt some data for a timer-handler in here. */
        ARMul_TimedCallbackList bus_timed_callbacks;
        /* Speed optimisation - call ICycles if this is true. */
        unsigned bus_IsActive;

} ARMul_Bus;



/*
 * Purpose: Read extra signals from the current bus-master.
 * Parameters:
 *  In: req      - the transaction on the bus.
 *      SignalID - an identifier describing the signals requested.
 * Out: *data    - the requested signals.
 * Returns: Number of words written to *data, or zero.
 *
 * There are separate functions for these, because most peripheral-accesses
 *  do not care about anything more complex than whether the access
 *  is sequential or not, so we don't want to write them each time.
 */
typedef int ARMul_ExtraSignalsRequestFunc(struct ARMul_AccessRequest *req,
                                          uint32 SignalID,
                                          uint32 *data);


typedef void ARMul_AbortHandlerProc(struct ARMul_AccessRequest*req);

/*
 * Purpose: Hold all the parameters except the instance-handle for an
 * access-request.
 * The access-request may pass through several busses. Peripherals and
 * bridges find out about their busses at config-time.
 * !WIP! Move common fields to low offsets.
 *
 * Usage notes:
 * . For speed, almost all of these should be pre-prepared, so the req_owner
 *   usually just fills in the req_address (and perhaps the data).
 */

typedef struct ARMul_AccessRequest {

    /* This is intended to allow an ACI-like interface to query the mode or
     * other registers of the core-model. It can also be used for interrupts,
     * etc., with care. It must not be used for data-aborts. */
    struct RDI_ModuleDesc *req_owner;

    /* "NEXT" pointers
    * ================*/
    /* This is provided for coverification models. */
    struct ARMul_AccessRequest *req_predicted;

    /* This is provided in case we want to pass two requests to a
     * Harvard-bus-handler. */
    struct ARMul_AccessRequest *req_other_in_harvard;

    /* Provided in case we want to pass a list of requests to an arbiter. */
    struct ARMul_AccessRequest *req_next_to_arbiter;

    /* NB if a core wants to have a list of all the requests it has,
     * e.g. in order to update their types when it changes mode, then it
     * should maintain that list outside this structure. */


    /* The callback for data-aborts: Whoever first returns RDIError_DataAbort
     * must call this (except if RPC is involved).*/
    ARMul_AbortHandlerProc *req_abort_proc;
    void *req_abort_handle;/* Usually the bus-master */

    /* For a Harvard bus, the decoder should place the results from the
     * peripherals here. Otherwise, this is private to the bus-master  - 
     * it can be used as an "abort" flag if the abort-handler sets it. */
    uint32 req_status;

    /* For peripheral models on a byte-lane-accessible bus,
     *  the byte-strobes live here.
     * This pointer must be non-null iff the access-type includes
     *  RDIAccess_ByteLaneWrite.
     * Note that bit 0 always means the lowest byte in the first word of data
     * (which may be at address 0 or 3 depending on endianness).
     * To allow busses up to 1024 bits wide, we need 128 byte-strobes.
     */
    uint32 *req_byte_strobes; /* !WIP! */

    /* Defines whether this is a read or a write etc. - see [1] */
    /* (For compatibility with VSIA-ISSAPI, we can pass a pointer to
     * req_access_type..) */
    uint32 req_access_type;

    /* This contains the bus-master number in its least significant bits. 
     * It is owned by the Arbiter (usually part of the core/mmu model). */
    uint32 req_access_type_2;


    /* - DATA - */
    unsigned req_data_size; /* in bytes */
    uint32 *req_data;

    /* The transfer-address. Note that if we have an address > 32 bits, 
     * we must reserve more memory at the end of this structure.*/
    unsigned req_addr_size;/* How many uint32's in address. */
    uint32 req_address[1];

} ARMul_AccessRequest;

/* Functions in armul_bus.c 
 * ------------------------*/
void ARMul_Bus_Initialise(ARMul_Bus *bus, uint32 cycleLength,
                    ARMul_BusAccessFunc *bus_decoderFunc,
                    void *bus_DecoderHandle,
                    ARMul_BusRegisterPeripFunc *bus_registerPeripFunc,
                    void *bus_RegisterPeripHandle,
                    ARMul_BusFinaliseFunc *bus_finalise);


/* This is the "default" bus_registerPeripFunc in the ARMulif library */

ARMul_BusRegisterPeripFunc ARMul_BusRegisterPerip;

/* This is the "default" bus_decoderFunc */
ARMul_BusAccessFunc ARMul_BusAccess;

void ARMul_BusLimits(struct ARMul_Bus *bus, ARMword *lo, ARMword *hi, 
                     unsigned n);




struct RDI_ModuleDesc;struct RDI_HostosInterface;
#include "toolconf.h"

/*--------------------------------------------------------------*/

/* 
 * We expose this to abuse it, for speed, in ARMiss.
 */
#define PERIPNUM_CLASH 255
#define PERIPNUM_EMPTY 0
#define PERIPNUM_MEMORY 254

typedef struct ARMul_DecoderData {
    ARMul_Bus *dd_bus; /* parent (owning) bus. */
    /* Contains a pointer to every (non-retracted) 
     * ARMul_BusPeripAccessRegistration registered on the bus. */
    CVector dd_regs;
    /* Contains, for each 64k page of memory, one of...
     * 0              - no peripheral
     * PERIPNUM_CLASH - more than one peripheral.
     * n+1            - where n is the index into dd_regs.
     */
    unsigned char PeripNums[1<<16];
} ARMul_DecoderData;


/* These allow a built-in memory to query the limits inside
 * which the decoder should be called. */
void ARMul_DecoderLimits(ARMul_DecoderData *dd,ARMword *lo,ARMword *hi,
                         unsigned n);
int ARMulif_ReadBusRange(struct RDI_ModuleDesc *mdesc, 
                         struct RDI_HostosInterface const *hostif,
                         toolconf config,
                         struct ARMul_BusPeripAccessRegistration *breg,
                         uint32 default_base, uint32 default_size,
                         char const *default_bus_name);


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /*ndef armul_bus__h */

