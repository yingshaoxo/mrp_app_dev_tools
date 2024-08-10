# This file was created by Megamake-3
# Copyright (C) ARM Ltd 2001. All rights reserved

SRCDIR1=..\..
EXTDIR1=..\..\..\armulif
EXTDIR2=..\..\..\rdi
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
CCFLAGS=/D_CONSOLE /D_MBCS /DNLS /nologo /Zp4 /W3 /GX /WX -DRDI_VERSION=151 -DARM10MODEL $(DLLPIC)
CFLAGS=$(CINCLS) $(CCFLAGS) $(CDOFL) $(PRFL) $(CFL) $(ARM_RELEASE) $(SPECIFIC_RELEASE) $(BUILD_INFO) /I$(DERDIR)
CFLALL=/Za # pc special local C compile flags
CFLAGS2=$(CFLALL) $(CCFLAGS2)
CCFLAGS2=/D_CONSOLE /D_MBCS /DNLS /nologo /MD /O2 /DNDEBUG /Zp4 # C flags for helper tools
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
..\..\..\rdi\rditools.b\intelrel\rditools.lib \
..\..\..\clx\clx.b\intelrel\clx.lib 
LASTLIBS=user32.lib # extra host libraries, for end of linker command-line
EXTOBJS=
EXTOTHER=
DESTDIR=C:\release

OBJALL=flatmem.obj 
DLLUSEFLAGS=
DLLGENFLAGS=/DLL
DLLPIC=
EXTDLLOTHER=version.res 


Flatmem.dll: $(OBJALL) $(SRCDIR1)\sordi.def $(EXTOBJS) $(EXTOTHER) $(EXTDLLOTHER) $(EXTLIBS)
	$(DLLLINK) $(DLLGENFLAGS) $(LFLAGS) $(PRFL) /OUT:Flatmem.dll /DEF:$(SRCDIR1)\sordi.def /IMPLIB:Flatmem.lib $(OBJALL) $(EXTOBJS) $(EXTOTHER) $(EXTDLLOTHER) $(EXTLIBS) $(LASTLIBS)
	echo "Made Flatmem.dll"

release_dll_Flatmem:
	if not exist $(DESTDIR)\$(HOST)\nul mkdir $(DESTDIR)\$(HOST)
	if not exist $(DESTDIR)\$(HOST)\bin\nul mkdir $(DESTDIR)\$(HOST)\bin
	copy Flatmem.dll $(DESTDIR)\$(HOST)\bin
RSC=rc.exe
RSC_PROJ=/d RC_BUILDNUMBER_STRING="\"805\"" /d RC_BUILDSTRING_STRING="\"build 805\"" /d BUILD_NUMBER=805 /l 0x809 /foversion.res /d NDEBUG
version.res : ..\..\version.rc
	$(RSC) $(RSC_PROJ) ..\..\version.rc
tests: $(OBJALL)
	@echo No tests.


release: release_dll_Flatmem


# Generated dependencies

flatmem.obj: $(SRCDIR1)\flatmem.c
flatmem.obj: $(EXTDIR3)\host.h
flatmem.obj: $(EXTDIR3)\armtypes.h
flatmem.obj: $(EXTDIR2)\rdi_stat.h
flatmem.obj: $(EXTDIR2)\rdi_err.h
flatmem.obj: $(EXTDIR2)\rdi_info.h
flatmem.obj: $(EXTDIR2)\rdi100.h
flatmem.obj: $(EXTDIR3)\toolconf.h
flatmem.obj: $(EXTDIR2)\rdi150.h
flatmem.obj: $(EXTDIR2)\rdi151.h
flatmem.obj: $(EXTDIR2)\rdi.h
flatmem.obj: $(EXTDIR2)\rdi_priv.h
flatmem.obj: $(EXTDIR2)\rdi_prop.h
flatmem.obj: $(EXTDIR1)\armul_types.h
flatmem.obj: $(EXTDIR1)\simplelinks.h
flatmem.obj: $(EXTDIR1)\armsignal.h
flatmem.obj: $(EXTDIR1)\armul_callbackid.h
flatmem.obj: $(EXTDIR1)\perip2rdi.h
flatmem.obj: $(EXTDIR2)\rdi_hif.h
flatmem.obj: $(EXTDIR1)\armul_hostif.h
flatmem.obj: $(EXTDIR2)\rdi_armul.h
flatmem.obj: $(EXTDIR1)\armul_askrdi.h
flatmem.obj: $(EXTDIR3)\errcodes.h
flatmem.obj: $(EXTDIR3)\cvector.h
flatmem.obj: $(EXTDIR1)\armul_method.h
flatmem.obj: $(EXTDIR1)\armul_bus.h
flatmem.obj: $(EXTDIR1)\armul_access.h
flatmem.obj: $(EXTDIR1)\armul_copro.h
flatmem.obj: $(EXTDIR1)\minperip.h
flatmem.obj: $(EXTDIR1)\addcounter.h
flatmem.obj: $(EXTDIR1)\armul_lib.h
flatmem.obj: $(EXTDIR2)\sordi.h
flatmem.obj: $(EXTDIR2)\rdi_sdm.h
flatmem.obj: $(EXTDIR2)\rditools.h
flatmem.obj: $(EXTDIR3)\dynlink.h
flatmem.obj: $(EXTDIR1)\armul_agent.h
flatmem.obj: $(EXTDIR1)\armul_defs.h
flatmem.obj: $(EXTDIR1)\armul_mem.h
flatmem.obj: $(EXTDIR2)\rdi_conf.h
flatmem.obj: $(EXTDIR1)\armul_cnf.h
flatmem.obj: $(EXTDIR3)\clxlist.h
flatmem.obj: $(EXTDIR1)\armul_cyc.h
flatmem.obj: $(EXTDIR3)\findfile.h
flatmem.obj: $(EXTDIR3)\dir.h
flatmem.obj: $(EXTDIR1)\perip_sordi_if.h
flatmem.obj: $(EXTDIR1)\perip_rdi_agent.h
flatmem.obj: $(EXTDIR1)\perip_sordi.h
	$(CC) $(CFLALL) $(CFLAGS) $(CFLflatmem) /Foflatmem.obj $(SRCDIR1)\flatmem.c
