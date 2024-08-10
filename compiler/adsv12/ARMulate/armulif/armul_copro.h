/*
 * armul_copro.h   - coprocessor interface for ARMUL/ARMulator.
 *
 * RCS $Revision: 1.7.6.7 $
 * Checkin $Date: 2001/10/08 15:17:40 $
 * Revising $Author: dsinclai $
 * Copyright (C) 2000 - 2001 ARM Limited. All rights reserved.
 *
 */

#ifndef amulif_armul_copro__h
#define amulif_armul_copro__h

#ifndef rdi_info_h
# include "rdi_info.h" /* for RDIInfo_RequestCoProDesc */
#endif
#ifndef simplelinks__h
# include "simplelinks.h" /* for NCAccessFunc */
#endif

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

/*
 * Coprocessor Interface
 * NB This enum includes both status-returns and request-types :-(
 */

/*
 * Todo: Decide how to fit ARM9's
 * "FIRST GO GO GO LAST" sequence in.
 * (cf FIRST BUSY BUSY DONE" for ARM7).
 */
enum ARMul_coproIF {
   ARMul_CP_FIRST,
   ARMul_CP_TRANSFER,
   ARMul_CP_DATA,
   ARMul_CP_INTERRUPT,

   ARMul_CP_BUSY=4,
   ARMul_CP_GO,
   ARMul_CP_DONE,
   ARMul_CP_CANT,
   ARMul_CP_INC,
   ARMul_CP_LAST,

   ARMul_CP_READ=10,
   ARMul_CP_WRITE,

   ARMul_CP_QUERY_REGSIZE=12,
   ARMul_CP_QUERY_NUMREGS,
   ARMul_CP_READ_DESC =  RDIInfo_RequestCoProDesc,
   ARMul_CP_WRITE_DESC = RDIInfo_DescribeCoPro

};

typedef int ARMul_CoproIF;

/* Tags for coprocessor-methods */
#define ARMUL_CP_CDP 0x0e000000
#define ARMUL_CP_MCR 0x0e000010
#define ARMUL_CP_MRC 0x0e100010
#define ARMUL_CP_STC 0x0c200000
#define ARMUL_CP_LDC 0x0c300000
#define ARMUL_CP_MCRR 0x0c400000
#define ARMUL_CP_MRRC 0x0c500000

/* Tags for PIPELINED coprocessor */
#define ARMUL_CP_CDP_NEXT 0x0e000001
#define ARMUL_CP_MCR_NEXT 0x0e000011
#define ARMUL_CP_MRC_NEXT 0x0e100011
#define ARMUL_CP_STC_NEXT 0x0c200001
#define ARMUL_CP_LDC_NEXT 0x0c300001
#define ARMUL_CP_MCRR_NEXT 0x0c400001
#define ARMUL_CP_MRRC_NEXT 0x0c500001

/* RDI functions (for non-SDM-aware debuggers, and pagetable.) */
#define ARMUL_CP_DESC 0x00000003
#define ARMUL_CP_READREG 0x00000002
#define ARMUL_CP_WRITEREG 0x00000001


typedef NCAccessFunc (*CDPHandler);
typedef NCAccessFunc (*MRCHandler);
typedef NCAccessFunc (*MCRHandler);

typedef NCAccessFunc (*MCRRHandler);
typedef NCAccessFunc (*MRRCHandler);

typedef NCAccessFunc (*LDCHandler);
typedef NCAccessFunc (*STCHandler);
typedef NCAccessFunc  (*CPReadRegHandler);
typedef NCAccessFunc  (*CPWriteRegHandler);
typedef NCAccessFunc  ARMulCopro_DescFn;

struct ARMul_CoprocessorV5 {
   CDPHandler CDP;
   MCRHandler MCR;
   MRCHandler MRC;
   MCRRHandler MCRR;
   MRRCHandler MRRC;
   LDCHandler LDC;
   STCHandler STC;
   ARMulCopro_DescFn *Desc;
   CPReadRegHandler ReadReg;
   CPWriteRegHandler WriteReg; 
   };
typedef struct ARMul_CoprocessorV5 ARMul_CoprocessorV5;



struct RDI_CoProDesc;
int ARMulif_MakeCoProDescs(unsigned const *map, struct RDI_CoProDesc *descs);

int ARMulif_CheckCoProDescs(unsigned const *map, struct RDI_CoProDesc *cpd);


/* This macro implements the "desc" function for a struct ARMul_CoprocessorV5.
 * Parameters:
 *  funcname  -  Name of function which the macro will implement.
 *  stateType - Type of the state structure used.
 *               If you used BEGIN_INIT(mymodel),
 *               then this should be mymodelState.
 *  regbytes  -  Name of an array of unsigned integers. 
 *               The first entry is the number of registers to describe 
 *               to armsd (which uses the read and write function pointers)
 *               and the rest are the sizes of each debugger-visible register,
 *               in bytes.
 */
#define IMPLEMENT_COPRO_DESC_FN_FROM_ARRAY(funcname,stateType,regbytes)\
static int funcname(void *handle, ARMul_CoproIF type, \
                    ARMword instr, ARMword *data)\
{\
    stateType *state = (stateType *)handle;\
    /*  unsigned numregs=sizeof(sizes)/sizeof(sizes[0]); */ \
    (void)state; \
    switch (type) {\
    case ARMul_CP_READ_DESC:\
        return ARMulif_MakeCoProDescs(regbytes, \
                                  (struct RDI_CoProDesc *)data); \
    case ARMul_CP_WRITE_DESC:\
        return ARMulif_CheckCoProDescs(regbytes,\
                                       (struct RDI_CoProDesc *)data); \
    case ARMul_CP_QUERY_REGSIZE: return (regbytes[(instr & 0x1f) + 1]+3) >> 2;\
    case ARMul_CP_QUERY_NUMREGS: return regbytes[0]; \
    case ARMul_CP_READ: \
    default:\
        return RDIError_UnimplementedType; \
    } \
}

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /*ndef amulif_armul_copro__h*/
/* EOF armul_copro.h */








