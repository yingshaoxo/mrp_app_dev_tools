/* pagetab.c - Module to provide page tables for MMU and PU based processors
 * Copyright (C) Advanced RISC Machines Limited, 1998-1999.
 * Copyright (C) ARM Limited, 1999-2001. All rights reserved.
 *
 * RCS $Revision: 1.10.2.11 $
 * Checkin $Date: 2001/10/19 08:55:32 $
 * Revising $Author: ljameson $
 */

#define MODEL_NAME Pagetab

#include <string.h>
#include "minperip.h"
#include "armul_cnf.h"

#ifndef NDEBUG
# if 1
# else
#  define VERBOSE_RESET
#  define VERBOSE
# endif
#endif


enum access {
  NO_ACCESS,
  NO_USR_W,
  SVC_RW,
  ALL_ACCESS
  };

enum entrytype {
  INVALID,
  PAGE,
  SECTION
  };

#define U_BIT 16
#define C_BIT 8
#define B_BIT 4

#define L1Entry(type,addr,dom,ucb,acc) \
  ((type==SECTION) ? (((addr)&0xfff00000)|((acc)<<10)|  \
                      ((dom)<<5)|(ucb)|(type)) :        \
   (type==PAGE) ? (((addr)&0xfffffc00)|((dom)<<5)|      \
                   ((ucb)&U_BIT)|(type)) :              \
   0)

static struct {
  tag_t option;
  ARMword bit;
  int def;
} ctl_flags[] = {
  ARMulCnf_MMU,         ARM_MMU_M, TRUE,
  ARMulCnf_AlignFaults, ARM_MMU_A, FALSE,
  ARMulCnf_Cache,       ARM_MMU_C, TRUE,
  ARMulCnf_WriteBuffer, ARM_MMU_W, TRUE,
  ARMulCnf_Prog32,      ARM_MMU_P, TRUE,
  ARMulCnf_Data32,      ARM_MMU_D, TRUE,
  ARMulCnf_LateAbort,   ARM_MMU_L, TRUE,
/* - most users of page-tables will allow the debugger to choose ByteSex. */
/*   ARMulCnf_BigEnd,      ARM_MMU_B, FALSE, */
  ARMulCnf_SystemProt,  ARM_MMU_S, FALSE,
  ARMulCnf_ROMProt,     ARM_MMU_R, FALSE,
  ARMulCnf_BranchPredict, ARM_MMU_Z, TRUE,
  ARMulCnf_ICache,      ARM_MMU_I, TRUE,
  ARMulCnf_HighExceptionVectors, ARM_MMU_V, FALSE
  };


BEGIN_STATE_DECL(Pagetab)
    toolconf clone;
    GenericAccessCallback *ih_gac; 
    bool hasMMU, hasPU, hasTCRAM;

    bool Set_Endian,   
         Debugger_Bigend; 

END_STATE_DECL(Pagetab)


static void disable_MMU(PagetabState *tab)
{
    ARMword cp15ctrl[8]; /* Reg1 can have 2 words in XScale */



    /* Read what's there already. */
    ARMulif_CPRead(&tab->coredesc,15,1,&cp15ctrl[0]);
    
    cp15ctrl[0] &= ~ARM_MMU_M;
    
    ARMulif_CPWrite(&tab->coredesc,15,1,&cp15ctrl[0]);

#ifdef VERBOSE_RESET
    printf("Pagetab.c:disable_MMU write %08lx to CP15 reg1.\n",
           (unsigned long)cp15ctrl[0]);
#endif
}


static void pagetab_MMU(PagetabState *tab)
{
  toolconf config = tab->clone;
  const char *option;
  ARMword value;
  ARMword page;
  ARMword entry;
  unsigned int i;


  disable_MMU(tab);

  value=ToolConf_DLookupUInt(config, ARMulCnf_DAC, 0x3);
  ARMulif_CPWrite(&tab->coredesc,15,3,&value);
  
  value=ToolConf_DLookupUInt(config, ARMulCnf_PageTableBase, 0x40000000);
  value&=0xffffc000;            /* align */
  ARMulif_CPWrite(&tab->coredesc,15,2,&value);
  
  /* start with all pages flat, uncacheable, read/write, unbufferable */
  entry=L1Entry(SECTION,0,0,U_BIT|C_BIT|B_BIT,ALL_ACCESS);
  for (i=0, page=value; i<4096; i++, page+=4) {
    if (ARMulif_WriteWord(&tab->coredesc, page, entry | (i<<20)))
    {
        Hostif_ConsolePrint(tab->hostif,"\nPagetab failed to write memory "
                            "at %08lx\n",
                            (unsigned long)page);
        return;
    };
  }

  for (i=0; i<100; i++) {
    char buffer[32];
    ARMword low,physical,mask;
    ARMword page;
    toolconf region;
    int j,n;

    sprintf(buffer,"REGION[%d]",i);
    region=ToolConf_FlatChild(config,(tag_t)buffer);
    if (region==NULL) break;    /* stop */

    option=ToolConf_Lookup(region,ARMulCnf_VirtualBase);
    if (option) {
      low=option ? strtoul(option,NULL,0) : 0;
      physical=ToolConf_DLookupUInt(region, ARMulCnf_PhysicalBase, low);
    } else {
      low=physical=ToolConf_DLookupUInt(region, ARMulCnf_PhysicalBase,0);
    }

    option = ToolConf_Lookup(region, ARMulCnf_Size);
    if (option != NULL) {
      unsigned long size = ToolConf_Power(option, TRUE);
      if (size == 0) {          /* assume 4GB */
        n = 4096;
      } else {
        n = size >> 20;
      }
    } else {
      /* backwards compatability */
      n = ToolConf_DLookupUInt(region, ARMulCnf_Pages, 4096);
    }
    mask=0;
    option=ToolConf_Lookup(region,ARMulCnf_Cacheable);
    mask=ToolConf_AddFlag(option,mask,C_BIT,TRUE);
    option=ToolConf_Lookup(region,ARMulCnf_Bufferable);
    mask=ToolConf_AddFlag(option,mask,B_BIT,TRUE);
    option=ToolConf_Lookup(region,ARMulCnf_Updateable);
    mask=ToolConf_AddFlag(option,mask,U_BIT,TRUE);

    mask |= (ToolConf_DLookupUInt(region, ARMulCnf_Domain, 0) & 0xf) << 5;
    mask |= (ToolConf_DLookupUInt(region, ARMulCnf_AccessPermissions,
                                  ALL_ACCESS) & 3) << 10;

    /* set TRANSLATE=NO to generate translation faults */
    if (ToolConf_DLookupBool(region, ARMulCnf_Translate, TRUE))
      mask |= SECTION;
    
    low&=0xfff00000;
    mask=(physical & 0xfff00000) | (mask & 0xdfe);
    
    j=low>>20;          /* index of first section */
    n+=j; if (n>4096) n=4096;
    for (page=(value+j*4); j<n; j++, mask+=0x100000, page+=4) {
      ARMulif_WriteWord(&tab->coredesc,page,mask);
    }
  }
  /* now enable the caches, etc. */
  { 
    ARMword cp15ctrl[8]; /* Reg1 can have 2 words in XScale */
    /* Read what's there already. */
    ARMulif_CPRead(&tab->coredesc,15,1,&cp15ctrl[0]);
#ifdef VERBOSE_RESET
    printf("Pagetab.c:pagetab_MMU reads %08lx from CP15 reg1.\n",
           (unsigned long)cp15ctrl[0]);
#endif
    for (i=0;i<sizeof(ctl_flags)/sizeof(ctl_flags[0]);i++) 
    {
        bool b = 
            ToolConf_DLookupBool(config,ctl_flags[i].option,ctl_flags[i].def);
#ifdef VERBOSE_RESET
    printf("Pagetab.c:pagetab_MMU, %08lx in CP15 reg1, "
           "bool=%u, bit = %08lx.\n",
           (unsigned long)cp15ctrl[0], (unsigned)b, 
           (unsigned long)ctl_flags[i].bit
        );

#endif
      if (b)
        cp15ctrl[0]|=ctl_flags[i].bit;
      else
        cp15ctrl[0]&= ~(ctl_flags[i].bit);
    }
    if (!(ToolConf_DLookupBool(config, ARMulCnf_FastBus, FALSE)))
        cp15ctrl[0] |= 0x40000000;
    else
         cp15ctrl[0] &= ~0x40000000;

    if(tab->hasTCRAM)
    {
        if ((ToolConf_DLookupBool(config, ARMulCnf_TCIRAM, FALSE)))
        {
            ARMWord value[4];
            ARMulif_CPRead(&tab->coredesc,15,9,&value[0]);
            value[3] |= 1;
            ARMulif_CPWrite(&tab->coredesc,15,9,&value[0]);
        }
        if ((ToolConf_DLookupBool(config, ARMulCnf_TCDRAM, FALSE))) {
            ARMWord value[4];
            ARMulif_CPRead(&tab->coredesc,15,9,&value[0]);
            value[2] |= 1;
            ARMulif_CPWrite(&tab->coredesc,15,9,&value[0]);
        }
    }
#ifdef VERBOSE_RESET
    printf("Pagetab.c:pagetab_MMU before setting endian, %08lx for CP15 reg1.\n",
           (unsigned long)cp15ctrl[0]);
#endif
    if (tab->Set_Endian)
    {
#ifdef VERBOSE_RESET
        printf("Setting endian to %u\n",tab->Debugger_Bigend);
#endif
        if (tab->Debugger_Bigend)
            cp15ctrl[0] |= ARM_MMU_B;
        else
            cp15ctrl[0] &= ~(ARM_MMU_B);
    }
    
 
#ifdef VERBOSE_RESET
    printf("Pagetab.c:pagetab_MMU writes %08lx to CP15 reg1.\n",
           (unsigned long)cp15ctrl[0]);
#endif
    ARMulif_CPWrite(&tab->coredesc,15,1,&cp15ctrl[0]);
  }
}

static void pagetab_ProtectionUnit(PagetabState *tab)
{
  toolconf config = tab->clone;
  const char *option;


    /* Variables to hold the values eventually sent to copro 15. 
       They are all different sizes, but for simplicity's sake they
       are stored as longs.
     */
    unsigned long Config_reg=0;
    unsigned long Cacheable_reg=0;
    unsigned long WriteBuffer_reg=0;
    unsigned long Protection_reg=0;
    unsigned long Region_regs[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    ARMword start;
    ARMword size;
    toolconf region;
    char buffer[32];
    int i, j, x;

    disable_MMU(tab);


    for (i = 1; i < 4; i++) {   /* a bitmask, 'DI' */
        /* For each type of region. Highest priority comes last and ovewrites */
        for (j = 0; j < 8; j++) {
            switch (i) {
            case 1:
                sprintf(buffer,"IREGION[%d]",j);
                break;
            case 2:
                sprintf(buffer,"DREGION[%d]",j);
                break;
            case 3:
                sprintf(buffer,"REGION[%d]",j);
                break;
            }
            region = ToolConf_Child(config,(tag_t)buffer);
            if (region != NULL) {
                start = ToolConf_DLookupUInt(region, ARMulCnf_PhysicalBase, 0);

                option = ToolConf_Lookup(region, ARMulCnf_Size);
                if (option != NULL) {
                  size = ToolConf_Power(option, TRUE);
                } else {
                  int n = ToolConf_DLookupUInt(region, ARMulCnf_Pages, 4096);
                  size = n << 20;
                }

                /* to get the 5 bit size value from the actual size,
                 * we have to do a linear search, to find the most 
                 * significant bit set.(Any other bit is ignored)
                 */
                for (x = 31; x >= 12; x--) {
                    /* Any value for size less than 0x1000 is considered 0 i.e. 4G */
                    if (size & (1 << x)) break;
                }
                size = (x < 12) ? 0x1f : x-1;

                /* for the moment I am assuming if we get this far we want the region enabled */
                option = ToolConf_Lookup(region, ARMulCnf_Translate);
                if (i & 2) {    /* D side */
                  Region_regs[j] = ToolConf_AddFlag(option, (size << 1) | (start << 12), 1, TRUE);
                }
                if (i & 1) {    /* I side */
                  Region_regs[j+8] = ToolConf_AddFlag(option, (size << 1) | (start << 12), 1, TRUE);
                }

                option = ToolConf_Lookup(region, ARMulCnf_Cacheable);
                if (i & 2) {    /* D side */
                  Cacheable_reg |= ToolConf_AddFlag(option, Cacheable_reg, 1<<j, TRUE);
                }
                if (i & 1) {    /* I side */
                  Cacheable_reg |= ToolConf_AddFlag(option, Cacheable_reg, 1<<(j+8), TRUE);/* I side */
                }
                if (i & 2) {    /* D side */
                  option = ToolConf_Lookup(region, ARMulCnf_Bufferable);
                  WriteBuffer_reg = ToolConf_AddFlag(option, WriteBuffer_reg, 1<<j, TRUE);
                }

                if (i & 1) {    /* I side */
                  Protection_reg |= (ToolConf_DLookupUInt(region, ARMulCnf_AccessPermissions, ALL_ACCESS) & 3) << (j*2);
                }
                if (i & 2) {    /* D side */
                  Protection_reg |= (ToolConf_DLookupUInt(region, ARMulCnf_AccessPermissions, ALL_ACCESS) & 3) << ((j*2) + 16);
                }
            }
        }
    }
    /* Now write all the values to the copro */
    ARMulif_CPWrite(&tab->coredesc, 15, 2, &Cacheable_reg);
    ARMulif_CPWrite(&tab->coredesc, 15, 3, &WriteBuffer_reg);
    ARMulif_CPWrite(&tab->coredesc, 15, 5, &Protection_reg);
    ARMulif_CPWrite(&tab->coredesc, 15, 6, Region_regs);

    /* Final thing to do is set the Configuration register */
    ARMulif_CPRead(&tab->coredesc, 15, 1, &Config_reg);
    option = ToolConf_Lookup(config,ARMulCnf_Cache);
    Config_reg = ToolConf_AddFlag(option, Config_reg, ARM_MMU_C, TRUE);
    option = ToolConf_Lookup(config,ARMulCnf_WriteBuffer);
    Config_reg = ToolConf_AddFlag(option, Config_reg, ARM_MMU_W, TRUE);
    option = ToolConf_Lookup(config,ARMulCnf_ICache);
    Config_reg = ToolConf_AddFlag(option, Config_reg, ARM_MMU_I, TRUE);
    if (tab->Set_Endian)
    {
        if (tab->Debugger_Bigend)
            Config_reg |= ARM_MMU_B;
        else
            Config_reg &= ~(ARM_MMU_B);
    }
    option = ToolConf_Lookup(config,ARMulCnf_MMU);
    Config_reg = ToolConf_AddFlag(option, Config_reg, ARM_MMU_M, TRUE);
    option = ToolConf_Lookup(config,ARMulCnf_HighExceptionVectors);
    Config_reg = ToolConf_AddFlag(option, Config_reg, ARM_MMU_V, TRUE);

    if (!(ToolConf_DLookupBool(config, ARMulCnf_FastBus, FALSE)))
        Config_reg |= 0x40000000;
    else
        Config_reg &= ~0x40000000;

    if(tab->hasTCRAM)
    {
        if ((ToolConf_DLookupBool(config, ARMulCnf_TCIRAM, FALSE)))
        {
            Config_reg |= (1<<18);
        }
        if ((ToolConf_DLookupBool(config, ARMulCnf_TCDRAM, FALSE))) {
            Config_reg |= (1<<16);
        }
        
    }
#ifdef VERBOSE
  printf("Pagetab.c:PU: writing %08lx to CP15 Reg1\n", 
         (unsigned long)Config_reg);
#endif
    ARMulif_CPWrite(&tab->coredesc, 15, 1, &Config_reg);

  return;
}

static void pagetab_TCRAM(PagetabState *tab)
{
  toolconf config = tab->clone;
  const char *option;


    /* Variables to hold the values eventually sent to copro 15. 
       They are all different sizes, but for simplicity's sake they
       are stored as longs.
     */
    unsigned long Config_reg=0;
 
     /* Final thing to do is set the Configuration register */
    ARMulif_CPRead(&tab->coredesc, 15, 1, &Config_reg);
    option = ToolConf_Lookup(config,ARMulCnf_WriteBuffer);
    Config_reg = ToolConf_AddFlag(option, Config_reg, ARM_MMU_W, TRUE);
    option = ToolConf_Lookup(config,ARMulCnf_BigEnd);
    Config_reg = ToolConf_AddFlag(option, Config_reg, ARM_MMU_B, TRUE);
    option = ToolConf_Lookup(config,ARMulCnf_HighExceptionVectors);
    Config_reg = ToolConf_AddFlag(option, Config_reg, ARM_MMU_V, TRUE);
    /* These are for the 966E-S (not the same as 946) */
    if ((ToolConf_DLookupBool(config, ARMulCnf_TCIRAM, FALSE)))
        Config_reg |= 0x1000;
    if ((ToolConf_DLookupBool(config, ARMulCnf_TCDRAM, FALSE)))
        Config_reg |= 0x4;
#ifdef VERBOSE
  printf("Pagetab.c:TCRAM: writing %08lx to CP15 Reg1\n", 
         (unsigned long)Config_reg);
#endif
    ARMulif_CPWrite(&tab->coredesc, 15, 1, &Config_reg);

  return;
}



static void pagetab_reset(PagetabState *tab)
{
    if (tab->hasPU)
    {
        /* protection unit */
        pagetab_ProtectionUnit(tab);
    }
    else if (tab->hasMMU)
    {
        /* MMU */
        pagetab_MMU(tab);
    } 
    else if (tab->hasTCRAM)
    {
        pagetab_TCRAM(tab);
    }
    else
    {
        uint32 value = 
            ARMulif_GetConfig(&tab->coredesc,ARM_MMU_P | ARM_MMU_D);
        uint32 old_value = value;
        if(ToolConf_DLookupBool(tab->clone,ARMulCnf_Prog32,TRUE))
            value |=ARM_MMU_P;
        else
            value &= ~(uint32)ARM_MMU_P;

        if(ToolConf_DLookupBool(tab->clone,ARMulCnf_Data32,TRUE))
            value |=ARM_MMU_D;
        else
            value &= ~(uint32)ARM_MMU_D;
#ifdef VERBOSE_RESET
        printf("Pagetab.c:NoSysCopro: writing %08lx to Config\n", 
               (unsigned long)value);
#endif
        ARMulif_SetConfig(&tab->coredesc, old_value ^ value, value);
    }
}



static unsigned pagetab_ConfigEvents(void *handle, void *data)
{
    PagetabState *tab = (PagetabState *)handle;
    ARMul_Event *evt = data;
    if (evt->event == ConfigEvent_Reset)
    {
        pagetab_reset(tab);
    }
    return FALSE;
}


BEGIN_EXIT(Pagetab)
{
  PagetabState *tab = state;
  ToolConf_Reset(tab->clone);
}
END_EXIT(Pagetab)

BEGIN_INIT(Pagetab)
{
  PagetabState *tab = state;
  Hostif_PrettyPrint(hostif, config, ", Pagetables" );
  if (tab != NULL) tab->clone = ToolConf_Clone(config);
  if (tab != NULL && tab->clone != NULL) {
      ARMulif_InstallEventHandler(&tab->coredesc,
                                ConfigEventSel,
                                pagetab_ConfigEvents, tab);
    /* Continue... */
  }
  else
  {
      return RDIError_OutOfStore;
  }
  tab->hasMMU = ToolConf_DLookupBool(config,(tag_t)"HASMMU",FALSE);
  tab->hasPU = ToolConf_DLookupBool(config,(tag_t)"HASPU",FALSE);
  tab->hasTCRAM = ToolConf_DLookupBool(config,(tag_t)"HASTCRAM",FALSE);

  /* Decide whether the debugger has asked us to set the bytesex,
   * and which endianness if so. */
  { 
      /* First, see if I've been configured */
      char const *option = ToolConf_Lookup(config, ARMulCnf_BigEnd);
      if (option && strchr("bBlLYyNn", option[0]))
      {
          state->Debugger_Bigend = (strchr("bBYy", option[0]) != NULL);
          state->Set_Endian = TRUE;
#ifdef VERBOSE_RESET
          printf("BIGEND=%u\n",state->Debugger_Bigend);
#endif
      }
      /* Else see if I want to follow the debugger (default is yes) or
      * hardware (not default). */
      else if (ToolConf_DLookupBool(config,Dbg_Cnf_TargetHWEndian,FALSE))
      {
          state->Set_Endian = false;
      }
      else if ((type & RDIOpen_ByteSexMask) != RDIOpen_DontCareEndian)
      {
          state->Debugger_Bigend = 
              ((type & RDIOpen_ByteSexMask) == RDIOpen_BigEndian);
          state->Set_Endian = TRUE;
#ifdef VERBOSE_RESET
          printf("RDIOpen, BIGEND=%u\n",state->Debugger_Bigend);
#endif
      }
      else if ( ((option=ToolConf_Lookup(config, Dbg_Cnf_ByteSex))!=NULL) &&
                strchr("bBlL", option[0]))
      {
          state->Set_Endian = TRUE;
          state->Debugger_Bigend = (strchr("bB", option[0]) != NULL);
#ifdef VERBOSE_RESET
          printf("ByteSex=%u\n",state->Debugger_Bigend);
#endif
      }
      else
      {
          state->Set_Endian = false;
      }
  }





}
END_INIT(PageTab)



/*--- <SORDI STUFF> ---*/

#define SORDI_DLL_NAME_STRING "Pagetab"
#define SORDI_DLL_DESCRIPTION_STRING "Page-table initialiser"
#define SORDI_RDI_PROCVEC Pagetab_AgentRDI
#include "perip_sordi.h"

#include "perip_rdi_agent.h"
    IMPLEMENT_AGENT_PROCS_NOEXE_NOMODULE(Pagetab)
    IMPLEMENT_AGENT_PROCVEC_NOEXE(Pagetab)

/*--- </> ---*/







/* EOF pagetab.c */










