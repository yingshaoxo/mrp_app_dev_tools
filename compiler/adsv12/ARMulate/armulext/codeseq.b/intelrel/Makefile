# This file was created by Megamake-3
# Copyright (C) ARM Ltd 2001. All rights reserved

SRCDIR1=..\..
EXTDIR1=..\..\..\rdi
EXTDIR2=..\..\..\armulif
EXTDIR3=..\..\..\clx
DERDIR=derived


ARM_RELEASE= /DARM_RELEASE="\"ADS1.2\""
SPECIFIC_RELEASE=
BUILD_INFO=/DBUILD_NUMBER=805

CFL= # local C compile flags
CDOFL=/O2 /MD /G5 /DNDEBUG # C debug and optimization flags
PRFL= # C and link profile flags
LFLAGS= # Linker flags
CINCLS=/I$(SRCDIR1) /I$(EXTDIR1) /I$(EXTDIR2) /I$(EXTDIR3) # C include flags
CCFLAGS=/D_CONSOLE /D_MBCS /DNLS /nologo /Zp4 /W4 /GX /WX -DRDI_VERSION=151 -DARM10MODEL $(DLLPIC)
CFLAGS=$(CINCLS) $(CCFLAGS) $(CDOFL) $(PRFL) $(CFL) $(ARM_RELEASE) $(SPECIFIC_RELEASE) $(BUILD_INFO) /I$(DERDIR)
CFLALL=/Za # pc special local C compile flags
CFLAGS2=$(CFLALL) $(CCFLAGS2)
CCFLAGS2=/D_CONSOLE /D_MBCS /DNLS /nologo /MD /O2 /DNDEBUG /Zp4 /W4  # C flags for helper tools
CC=cl /c
CL=cl
AR=lib
ARFLAGS=
LINK=link /INCREMENTAL:no
DLLLINK=link /INCREMENTAL:no
MKTAG=mktag
MKMSG=mkmsg
MKMSGRC=mkmsgrc
ERRFILE=
HOST=intelrel# also build_dir (no space before '#')
XHELPER= # command to run semi-hosted helper programs
RUNTEST= # command to run semi-hosted test programs
EXTLIBS=..\..\..\armulif\armulif.b\intelrel\armulif.lib \
..\..\..\clx\clx.b\intelrel\clx.lib 
LASTLIBS=user32.lib # extra host libraries, for end of linker command-line
EXTOBJS=
EXTOTHER=
DESTDIR=C:\release

OBJALL=codeseq.obj 
DLLUSEFLAGS=
DLLGENFLAGS=/DLL
DLLPIC=
EXTDLLOTHER=version.res 


Codeseq.dll: $(OBJALL) $(SRCDIR1)\sordi.def $(EXTOBJS) $(EXTOTHER) $(EXTDLLOTHER) $(EXTLIBS)
	$(DLLLINK) $(DLLGENFLAGS) $(LFLAGS) $(PRFL) /OUT:Codeseq.dll /DEF:$(SRCDIR1)\sordi.def /IMPLIB:Codeseq.lib $(OBJALL) $(EXTOBJS) $(EXTOTHER) $(EXTDLLOTHER) $(EXTLIBS) $(LASTLIBS)
	echo "Made Codeseq.dll"

release_dll_Codeseq:
	if not exist $(DESTDIR)\$(HOST)\nul mkdir $(DESTDIR)\$(HOST)
	if not exist $(DESTDIR)\$(HOST)\bin\nul mkdir $(DESTDIR)\$(HOST)\bin
	copy Codeseq.dll $(DESTDIR)\$(HOST)\bin
RSC=rc.exe
RSC_PROJ=/d RC_BUILDNUMBER_STRING="\"805\"" /d RC_BUILDSTRING_STRING="\"build 805\"" /d BUILD_NUMBER=805 /l 0x809 /foversion.res /d NDEBUG
version.res : ..\..\version.rc
	$(RSC) $(RSC_PROJ) ..\..\version.rc
tests: $(OBJALL)
	@echo No tests.


release: release_dll_Codeseq


# Generated dependencies

codeseq.obj: $(SRCDIR1)\codeseq.c
codeseq.obj: $(EXTDIR3)\host.h
codeseq.obj: $(EXTDIR3)\toolconf.h
codeseq.obj: $(EXTDIR1)\rdi_conf.h
codeseq.obj: $(EXTDIR2)\armul_cnf.h
codeseq.obj: $(EXTDIR3)\armtypes.h
codeseq.obj: $(EXTDIR2)\armul_types.h
codeseq.obj: $(EXTDIR1)\rdi_stat.h
codeseq.obj: $(EXTDIR1)\rdi_err.h
codeseq.obj: $(EXTDIR1)\rdi_info.h
codeseq.obj: $(EXTDIR1)\rdi100.h
codeseq.obj: $(EXTDIR1)\rdi150.h
codeseq.obj: $(EXTDIR1)\rdi151.h
codeseq.obj: $(EXTDIR1)\rdi.h
codeseq.obj: $(EXTDIR1)\rdi_hif.h
codeseq.obj: $(EXTDIR2)\armul_hostif.h
codeseq.obj: $(EXTDIR2)\simplelinks.h
codeseq.obj: $(EXTDIR1)\rdi_priv.h
codeseq.obj: $(EXTDIR2)\armsignal.h
codeseq.obj: $(EXTDIR1)\rdi_armul.h
codeseq.obj: $(EXTDIR2)\armul_askrdi.h
codeseq.obj: $(EXTDIR3)\errcodes.h
codeseq.obj: $(EXTDIR3)\cvector.h
codeseq.obj: $(EXTDIR2)\armul_method.h
codeseq.obj: $(EXTDIR2)\armul_bus.h
codeseq.obj: $(EXTDIR2)\armul_callbackid.h
codeseq.obj: $(EXTDIR2)\armul_access.h
codeseq.obj: $(EXTDIR2)\armul_copro.h
codeseq.obj: $(EXTDIR2)\minperip.h
codeseq.obj: $(EXTDIR1)\sordi.h
codeseq.obj: $(EXTDIR1)\rdi_sdm.h
codeseq.obj: $(EXTDIR1)\rditools.h
codeseq.obj: $(EXTDIR3)\dynlink.h
codeseq.obj: $(EXTDIR2)\armul_agent.h
codeseq.obj: $(EXTDIR3)\disass.h
codeseq.obj: $(EXTDIR2)\armul_defs.h
codeseq.obj: $(EXTDIR2)\armul_mem.h
codeseq.obj: $(EXTDIR3)\findfile.h
codeseq.obj: $(EXTDIR3)\dir.h
codeseq.obj: $(EXTDIR2)\perip_sordi_if.h
codeseq.obj: $(EXTDIR2)\perip_rdi_agent.h
codeseq.obj: $(EXTDIR2)\perip_sordi.h
	$(CC) $(CFLALL) $(CFLAGS) $(CFLcodeseq) /Focodeseq.obj $(SRCDIR1)\codeseq.c
