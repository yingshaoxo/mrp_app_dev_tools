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
..\..\..\clx\clx.b\intelrel\clx.lib 
LASTLIBS=user32.lib # extra host libraries, for end of linker command-line
EXTOBJS=
EXTOTHER=
DESTDIR=C:\release

OBJALL=watchdog.obj 
DLLUSEFLAGS=
DLLGENFLAGS=/DLL
DLLPIC=
EXTDLLOTHER=version.res 


Watchdog.dll: $(OBJALL) $(SRCDIR1)\sordi.def $(EXTOBJS) $(EXTOTHER) $(EXTDLLOTHER) $(EXTLIBS)
	$(DLLLINK) $(DLLGENFLAGS) $(LFLAGS) $(PRFL) /OUT:Watchdog.dll /DEF:$(SRCDIR1)\sordi.def /IMPLIB:Watchdog.lib $(OBJALL) $(EXTOBJS) $(EXTOTHER) $(EXTDLLOTHER) $(EXTLIBS) $(LASTLIBS)
	echo "Made Watchdog.dll"

release_dll_Watchdog:
	if not exist $(DESTDIR)\$(HOST)\nul mkdir $(DESTDIR)\$(HOST)
	if not exist $(DESTDIR)\$(HOST)\bin\nul mkdir $(DESTDIR)\$(HOST)\bin
	copy Watchdog.dll $(DESTDIR)\$(HOST)\bin
RSC=rc.exe
RSC_PROJ=/d RC_BUILDNUMBER_STRING="\"805\"" /d RC_BUILDSTRING_STRING="\"build 805\"" /d BUILD_NUMBER=805 /l 0x809 /foversion.res /d NDEBUG
version.res : ..\..\version.rc
	$(RSC) $(RSC_PROJ) ..\..\version.rc
tests: $(OBJALL)
	@echo No tests.


release: release_dll_Watchdog


# Generated dependencies

watchdog.obj: $(SRCDIR1)\watchdog.c
watchdog.obj: $(EXTDIR3)\host.h
watchdog.obj: $(EXTDIR3)\armtypes.h
watchdog.obj: $(EXTDIR1)\armul_types.h
watchdog.obj: $(EXTDIR2)\rdi_stat.h
watchdog.obj: $(EXTDIR2)\rdi_err.h
watchdog.obj: $(EXTDIR2)\rdi_info.h
watchdog.obj: $(EXTDIR2)\rdi100.h
watchdog.obj: $(EXTDIR3)\toolconf.h
watchdog.obj: $(EXTDIR2)\rdi150.h
watchdog.obj: $(EXTDIR2)\rdi151.h
watchdog.obj: $(EXTDIR2)\rdi.h
watchdog.obj: $(EXTDIR2)\rdi_hif.h
watchdog.obj: $(EXTDIR1)\armul_hostif.h
watchdog.obj: $(EXTDIR1)\simplelinks.h
watchdog.obj: $(EXTDIR2)\rdi_priv.h
watchdog.obj: $(EXTDIR1)\armsignal.h
watchdog.obj: $(EXTDIR2)\rdi_armul.h
watchdog.obj: $(EXTDIR1)\armul_askrdi.h
watchdog.obj: $(EXTDIR3)\errcodes.h
watchdog.obj: $(EXTDIR3)\cvector.h
watchdog.obj: $(EXTDIR1)\armul_method.h
watchdog.obj: $(EXTDIR1)\armul_bus.h
watchdog.obj: $(EXTDIR1)\armul_callbackid.h
watchdog.obj: $(EXTDIR1)\armul_access.h
watchdog.obj: $(EXTDIR1)\armul_copro.h
watchdog.obj: $(EXTDIR1)\minperip.h
watchdog.obj: $(EXTDIR2)\rdi_conf.h
watchdog.obj: $(EXTDIR1)\armul_cnf.h
watchdog.obj: $(EXTDIR2)\sordi.h
watchdog.obj: $(EXTDIR3)\findfile.h
watchdog.obj: $(EXTDIR3)\dir.h
watchdog.obj: $(EXTDIR2)\rdi_sdm.h
watchdog.obj: $(EXTDIR2)\rditools.h
watchdog.obj: $(EXTDIR1)\perip_sordi_if.h
watchdog.obj: $(EXTDIR1)\perip_rdi_agent.h
watchdog.obj: $(EXTDIR1)\perip_sordi.h
	$(CC) $(CFLALL) $(CFLAGS) $(CFLwatchdog) /Fowatchdog.obj $(SRCDIR1)\watchdog.c
