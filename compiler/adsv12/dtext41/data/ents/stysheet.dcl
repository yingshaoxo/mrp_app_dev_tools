	<!SGML  "ISO 8879:1986"
-- 		DocBook DTD           Release 1.2.2

   17 May 1993  2:00 PST

   Copyright 1992, 1993  HaL Computer Systems, Inc., and
     O'Reilly & Associates, Inc.

   Permission to use, copy, modify and distribute the DocBook DTD and
     its accompanying documentation for any purpose and without fee is
     hereby granted, provided that this copyright notice appears in all
     copies.  If you modify the DocBook DTD, rename your modified
     version.  HaL Computer Systems and O'Reilly & Associates make no
     representation about the suitability of the DTD for any purpose.
     It is provided "as is" without expressed or implied warranty.

   The DocBook DTD is maintained by O'Reilly & Associates.  Please
     direct all questions, bug reports, or suggestions for changes to
     docbook@ora.com or by mail to: Terry Allen, O'Reilly & Associates,
     Inc., 103A Morris Street, Sebastopol, California, 95472.

   Please note that an SGML declaration is provided for this DTD.
     
   Public Identifier:
	"-//HaL and O'Reilly//DTD DocBook 1.2.2//EN"

   Edited by Christopher R. Maden <crm@ebt.com> to allow ISO Latin 1
      encoding.

--

CHARSET
         BASESET  "ISO 646:1983//CHARSET
                   International Reference Version (IRV)//ESC 2/5 4/0"
         DESCSET  0   9   UNUSED
                  9   2   9
                  11  2   UNUSED
                  13  1   13
                  14  18  UNUSED
                  32  95  32
                  127 1   UNUSED

   BASESET "ISO Registration Number 100//CHARSET ECMA-94
            Right Part of Latin Alphabet Nr. 1//ESC 2/13 4/1"

   DESCSET 128    32    UNUSED
           160    95    32
           255     1    UNUSED

CAPACITY	SGMLREF
		TOTALCAP	300000
		GRPCAP		250000
		IDCAP		200000
  
SCOPE    DOCUMENT
SYNTAX   
         SHUNCHAR CONTROLS 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18
                           19 20 21 22 23 24 25 26 27 28 29 30 31 127 255
         BASESET  "ISO 646:1983//CHARSET
                   International Reference Version (IRV)//ESC 2/5 4/0"
         DESCSET  0 128 0
         FUNCTION RE          13
                  RS          10
                  SPACE       32
                  TAB SEPCHAR  9
         NAMING   LCNMSTRT ""
                  UCNMSTRT ""
                  LCNMCHAR ".-"
                  UCNMCHAR ".-"
                  NAMECASE GENERAL YES
                           ENTITY  NO
         DELIM    GENERAL  SGMLREF
                  SHORTREF SGMLREF
         NAMES    SGMLREF
         QUANTITY SGMLREF
                  NAMELEN  44
                  TAGLVL   100
                  LITLEN   4800
                  GRPGTCNT 253
                  GRPCNT   200                   

FEATURES
  MINIMIZE
    DATATAG  NO
    OMITTAG  YES
    RANK     NO
-- While SHORTTAG is set to YES to accomodate FIXED attribute 
	values, the use of short tagging in instances is deprecated
					--
    SHORTTAG YES 
  LINK
    SIMPLE   NO
    IMPLICIT NO
    EXPLICIT NO
  OTHER
    CONCUR   NO
    SUBDOC   NO
    FORMAL   NO
  APPINFO    NONE
>
