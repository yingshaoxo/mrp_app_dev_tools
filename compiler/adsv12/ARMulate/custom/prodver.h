/*
 * Copyright (C) ARM Limited 1998-2001. All rights reserved.
 *
 * RCS $Revision: 1.45.4.13 $
 * Checkin $Date: 2001/09/04 16:21:05 $
 * Revising $Author: dcleland $
 *
 *
 * David Earlam (5th March, 1998)
 *
 *  Attempt to create a header file which can be read by Installshield and Resource compiler
 *  and the C/C++ compilers, so that
 * (1)  Windows tools and Uninstaller all use the same name for the product,
 * (2)  APM and ADW (MDW, ADW3 , ADW for C++ ?) create profile settings with a per version key
 *   eg
 *   HKEY_CURRENT_USER
 *              /Software
 *                      /ARM Limited
 *                              / Toolkit
 *                                      /v1.1
 *                                              /Program text may appear here
 *                                              /Program text may appear here
 *                                      /v1.2
 *                                              /Program text may appear here
 *                                              /Program text may appear here
 *                                              
 * 
 *  The uninstaller dll will remove the registry key 'v2.50' and all keys below it.
 *  It does not care what the programs use for their own key (normally AFX_IDS_APP_TITLE resource string),
 *  so long as they get the string for the registry key from resource strings in custom.dll,
 *  
 *    INSTALL_COMPANY/INSTALL_PACKAGE_NAME/INSTALL_PACKAGE_VERSION
 *
 *
 *  This file will be read by custom.rc and setup.rul and the C/C++ compilers.
 *
 *  Unfortunately, neither Installshield's compiler preprocessor, nor Microsoft's Resource compiler
 *  can concatenate strings, so 'v2.50' occurs at multiple places in this text,
 *  and hopefully NOWHERE ELSE.  (RC does seems to be able to have two string literals in a
 *  row -- effectively concatenating them, so I don't understand that part of the comment.)
 *  
 *  Install Shield cannot handle macros continued onto multiple lines even if it doesn't use
 *  them (sigh).
 *
 *  So to change the version globally replace 'v2.50' in this file and rebuild the install script
 *  and custom.dll.
 *  Ensure that the deinstallkey is correct for the version too! 
 *
 *  I am continuing the trend established with the SDT211 demo, that it uses the same defaults
 *  as the real SDT211.
 *  Installing the demo (say to c:\arm211) installing the real thing (say to c:\arm211real),
 *  uninstalling the demo, would remove the copy in c:\arm211 and the defaults for both the demo
 *  and the real thing.
 * 
 *  If we wish demo and real to coexist with different defaults then one of INSTALL_PACKAGE_NAME
 *  or INSTALL_PACKAGE_VERSION will need to be changed based on the makefile defined SDTDEMO flag
 *  and the same flag would have to be defined in the makefile for custom.dll and the build system
 *  needs to ensure both are in sync.
 *
 */

#ifndef PRODVER_H
#define PRODVER_H


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif


/* Firstly strings that will become resource strings in custom.dll and entries in the registry */

/* Changes with each release*/
/* NOTE: For ADS 1.0.1, the registry keys (identified by INSTALL_PACKAGE_VERSION) do NOT change
   from ADS 1.0. But all the other strings DO change to ADS 1.0.1. This is deliberate, because it
   means that users can upgrade their installation while keeping their registry settings.
   Since this is a bugfix release, that is what we want them to do.
 */
#define INSTALL_PACKAGE_VERSION      "v1.2" /* Registry Key */
#define INSTALL_PACKAGE_EVAL_VERSION "v1.2 EVALUATION VERSION" /* Registry Key */
#define PRODUCT_VERSION_1  1
#define PRODUCT_VERSION_2  2
#define PRODUCT_VERSION_3  0
#define PRODUCT_VERSION_4  0
#define PRODUCT_VERSION_STR_1 "1"
#define PRODUCT_VERSION_STR_2 "2"
#define PRODUCT_VERSION_STR_3 "0"
#define PRODUCT_VERSION_STR_4 "0"
#define PRODUCT_VERSION_NULLTERM "1.2\0"
#define PRODUCT_NAME_AND_VERSION_NULLTERM "ADS v1.2\0"
#define PRODUCT_VERSION_STR_BASE "1.2.0.0"

#define COPYRIGHT_THIS_YEAR "Copyright (C) 2001\0"

#define INSTALL_PACKAGE_COPYRIGHT "Copyright (C) ARM Limited 2001. All Rights Reserved\0"
#define INSTALL_PACKAGE_COPYRIGHT_SPLIT "Copyright (C) ARM Limited 2001.\nAll Rights Reserved\0"

#define INSTALL_PACKAGE_NAME "ARM Developer Suite" 

#define INSTALL_PACKAGE_ACRONYM "ADS"

#define INSTALL_COMPANY "ARM Limited"
#define INSTALL_COMPANY_NULLTERM "ARM Limited\0"

/* Used in the window tool about dialogs - keep it fairly short, shame you can't form it from the previous defines*/
#define INSTALL_PACKAGE_NAME_VERSION "ARM Developer Suite v1.2"

/* Used by the installer when upgrading an existing installation to a new version.
   NB! If we produce a second version and we are also going to permit upgrades to it,
   then we must have a list here of possible previous version names, AND we must
   add some new code to the installer to cope with such a list. At present there is
   just a single previous version from which upgrades are supported.
   */
#define INSTALL_PACKAGE_PREVIOUS_VERSION "v1.0"
#define INSTALL_PACKAGE_PREVIOUS_NAME_VERSION "ARM Developer Suite v1.0"

/* I've added these in case they are useful, but the installer doesn't use them, yet */
#define INSTALL_PACKAGE_PREVIOUS_VERSION_2 "v1.1"
#define INSTALL_PACKAGE_PREVIOUS_NAME_VERSION_2 "ARM Developer Suite v1.1"

/* Secondly strings from the install script, so we only have to edit this file for a new version */

#define STR_LONG_PRODUCT_NAME_STANDARD  "ARM Developer Suite v1.2"
#define STR_LONG_PRODUCT_NAME_DEMO      "Evaluation version of the ARM Developer Suite v1.2"

#define STR_SHORT_PRODUCT_NAME_STANDARD "ADS v1.2.0"
#define STR_SHORT_PRODUCT_NAME_DEMO     "ADS v1.2.0 Eval"

#define STR_REG_COMP                    INSTALL_COMPANY
#define STR_REG_PRODUCT                 INSTALL_PACKAGE_NAME
#define STR_REG_VERSION                 INSTALL_PACKAGE_VERSION

/* The default installation path  */
#define STR_DEFAULT_PATH                "\\ADS"
#define STR_DEFAULT_PATH_UNIX           "/opt/ADS"

/* Product information for windows installer */
#define INSTALL_PACKAGE_REL             "FINAL"
#define INSTALL_PACKAGE_PATH            "ADSv1_2"

/* The above names are used by ADS, but other products, with different version numbers,
 * use this file also */
#define RTM_PRODUCT_VERSION_1  1
#define RTM_PRODUCT_VERSION_2  0
#define RTM_PRODUCT_VERSION_3  0
#define RTM_PRODUCT_VERSION_4  0
#define RTM_PRODUCT_VERSION_STR_1  "1"
#define RTM_PRODUCT_VERSION_STR_2  "0"
#define RTM_PRODUCT_VERSION_STR_3  "0"
#define RTM_PRODUCT_VERSION_STR_4  "0"
/* Please expand these as they casued compialtion errors in InstallShield */
#define RTM_INSTALL_PACKAGE_VERSION  "v1.0"
#define RTM_INSTALL_PACKAGE_EVAL_VERSION "v1.0 EVAULATION VERSION"
#define RTM_PRODUCT_NAME "RealMonitor"
#define RTM_PRODUCT_VERSION_NULLTERM "1.0\0"
#define RTM_PRODUCT_NAME_AND_VERSION_NULLTERM "RealMonitor v1.0\0"
/* Please do not split this line or it will cause compilation errors in InstallShield */
#define RTM_PRODUCT_VERSION_STR_BASE "1.0.0.0"
#define RTM_INSTALL_PACKAGE_NAME "ARM RealMonitor"
#define RTM_INSTALL_PACKAGE_ACRONYM "RM"
#define RTM_INSTALL_PACKAGE_NAME_VERSION "ARM RealMonitor v1.0"

/* Strings used by ADW/ADU and AXD */

#define AXD_PRODUCT_LINE_1    "AXD Debugger"
#define AXD_PRODUCT_LINE_2    "for ARM Developer Suite 1.2"
#define AXD_SPLASH_COPYRIGHT  "Copyright (c) ARM Limited 1999-2001.\nAll Rights Reserved"
#define AXD_ABOUT_COPYRIGHT   "Copyright (c) ARM Limited 1999-2001. All Rights Reserved"

#ifdef __unix
#define ADx_PRODUCT_LINE_1    "ARM Debugger for Unix"
#else
#define ADx_PRODUCT_LINE_1    "ARM Debugger for Windows"
#endif
#define ADx_PRODUCT_LINE_2    AXD_PRODUCT_LINE_2
#define ADx_SPLASH_COPYRIGHT  AXD_SPLASH_COPYRIGHT
#define ADx_ABOUT_COPYRIGHT   AXD_ABOUT_COPYRIGHT

/* Version info for TDT */
#define TDT_PRODUCT_VERSION_1 1
#define TDT_PRODUCT_VERSION_2 2
#define TDT_PRODUCT_VERSION_3 0
#define TDT_PRODUCT_VERSION_STR_1 "1"
#define TDT_PRODUCT_VERSION_STR_2 "2"
#define TDT_PRODUCT_VERSION_STR_3 "0"
#define TDT_PRODUCT_LINE      "Trace Debug Tools\0"
#define TDT_PRODUCT_VERSION   "Version 1.2.0\0"
#define TDT_PACKAGE_NAME      "ARM Trace Debug Tools\0"
/* Don't know if these are used anymore leaving them in just in case */
#define TDT_COPYRIGHT         "Copyright (c) 2001\0"
#define TDT_INSTALL_COPYRIGHT "Copyright (c) ARM Limited 1999-2001. All Rights Reserved\0"

/*
 * This string defines the product name as registered with C-Dilla's protection
 * software (only used for the Demo or Eval product at present). With each new
 * version that we release, a new string of this kind is needed, otherwise users
 * will not be able to try the new version on any machine on which they had
 * previously tried an earlier version.
 */
#define CDILLA_ARM_PRODUCT_NAME   "ARM Developer Suite Version 1.2"


/* These string define where the installer will put the license file relative
 * to the main installation directory.
 */
#define STR_LICENSE_DIR     "licenses"
#define STR_LICENSE_FILE    "license.dat"

/*
   Feature names and license version numbers of the various license
   managed components in ADS

   See  david.cleland@arm.com   before adding any new names to this list

   If you REALLY have to add a new feature name name set its value to 
   be 'armdummy'.  

   The real values  will be sorted out before the final builds.

*/

#define ARMASM_FEATURE_NAME  "armasm"
#define ARMASM_FEATURE_VERSION  "1.2"

#define ARMCC_FEATURE_NAME  "compiler"
#define ARMCC_FEATURE_VERSION  "1.2"

#define ARMULATE_FEATURE_NAME  "armulate"
#define ARMULATE_FEATURE_VERSION  "1.2"

#define AXD_FEATURE_NAME  "axd"
#define AXD_FEATURE_VERSION  "1.2"

#define ADU_FEATURE_NAME  "adwu"
#define ADU_FEATURE_VERSION  "1.2"

#define FROMELF_FEATURE_NAME  "fromelf"
#define FROMELF_FEATURE_VERSION  "1.2"

#define ARMLINK_FEATURE_NAME  "armlink"
#define ARMLINK_FEATURE_VERSION  "1.2"

#define IDE_FEATURE_NAME  "codewarrior"
#define IDE_FEATURE_VERSION  "1.2"

#define ARMSD_FEATURE_NAME  "armsd"
#define ARMSD_FEATURE_VERSION  "1.2"

/* Trace */
#define TRACE_FEATURE_NAME  "trace"
#define TRACE_FEATURE_VERSION  "1.1"

 /* Non ADS tools */
#define RTM_FEATURE_NAME "armdummy"
#define RTM_FEATURE_VERSION "1.0"

#ifdef RC_INVOKED /* Install Shield cannot handle macros continued onto multiple lines (sigh) */

/*
 Macro to generate the Windows version resource in RC

/////////////////////////////////////////////////////////////////////////////
//
// Version
//

*/

#ifndef _MAC
  #ifdef RC_BUILDNUMBER_STRING
  # define BUILD_NUMBER_STR RC_BUILDNUMBER_STRING
  #else
  # define BUILD_NUMBER_STR "unreleased"
  #endif

  #ifndef BUILD_NUMBER
  # define BUILD_NUMBER 0
  #endif

  #ifdef _DEBUG
    #define VERSION_FILEFLAGS 0x1L
  #else
    #define VERSION_FILEFLAGS 0x0L
  #endif

  #define VERSION_INFO_RESOURCE(FileDescription, InternalName, OriginalFilename) \
      VS_VERSION_INFO VERSIONINFO \
       /* This file version is the one that shows up under Windows 2000 */ \
       FILEVERSION        PRODUCT_VERSION_1, \
                          PRODUCT_VERSION_2, \
                          PRODUCT_VERSION_3, \
                          BUILD_NUMBER \
       PRODUCTVERSION     PRODUCT_VERSION_1, \
                          PRODUCT_VERSION_2, \
                          PRODUCT_VERSION_3, \
                          0 \
       FILEFLAGSMASK 0x3fL \
       FILEFLAGS VERSION_FILEFLAGS \
       FILEOS 0x4L \
       FILETYPE 0x2L \
       FILESUBTYPE 0x0L \
      BEGIN \
          BLOCK "StringFileInfo" \
          BEGIN \
              BLOCK "040904B0" \
              BEGIN \
                  /* This file version is the one that shows up under Windows NT 4.0 */ \
                  VALUE "FileVersion", PRODUCT_VERSION_STR_1 "." \
                                       PRODUCT_VERSION_STR_2 "." \
                                       PRODUCT_VERSION_STR_3 "." \
                                       BUILD_NUMBER_STR "\0"\
                  VALUE "CompanyName", INSTALL_COMPANY_NULLTERM \
                  VALUE "LegalCopyright", COPYRIGHT_THIS_YEAR \
                  VALUE "ProductName", INSTALL_PACKAGE_NAME \
                  VALUE "ProductVersion", PRODUCT_VERSION_NULLTERM \
                  VALUE "Copyright", INSTALL_PACKAGE_COPYRIGHT \
                  VALUE "FileDescription", FileDescription "\0" \
                  VALUE "InternalName", InternalName "\0" \
                  VALUE "OriginalFilename", OriginalFilename "\0" \
              END \
          END \
          BLOCK "VarFileInfo" \
          BEGIN \
              VALUE "Translation", 0x409, 1200 \
          END \
      END
#else
  #define VERSION_INFO_RESOURCE(FileDescription, InternalName, OriginalFilename) /*nothing*/
#endif    /* !_MAC */

#endif /* def RC_INVOKED */



#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
          }
#endif


#endif /* PRODVER_H */
