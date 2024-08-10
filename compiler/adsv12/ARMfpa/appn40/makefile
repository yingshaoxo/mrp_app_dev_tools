# Makefile for the Application Note 40 configuration of the FPASC.
#
# Copyright (C) Advanced RISC Machines Limited, 1997. All rights reserved.
#
# RCS $Revision: 1.4 $
# Checkin $Date: 1999/10/08 09:16:58 $
# Revising $Author: statham $

COREDIR   = ../coresrc/#
VENDIR    = ../vensrc/appn40/#

COREFILES = $(COREDIR)arith.s \
            $(COREDIR)armdefs.s \
            $(COREDIR)defaults.s \
            $(COREDIR)fpadefs.s \
            $(COREDIR)fpaundef.s \
            $(COREDIR)fpeundef.s \
            $(COREDIR)interf.s \
            $(COREDIR)ldst.s \
            $(COREDIR)macros.s \
            $(COREDIR)main.s \
            $(COREDIR)nans.s \
            $(COREDIR)optcheck.s \
            $(COREDIR)regnames.s \
            $(COREDIR)rounding.s \
            $(COREDIR)toplevel.s \
            $(COREDIR)transhw.s \
            $(COREDIR)wsdefs.s

VENFILES  = $(VENDIR)ctxtws.s \
            $(VENDIR)end.s \
            $(VENDIR)globalws.s \
            $(VENDIR)header.s \
            $(VENDIR)options.s \
            $(VENDIR)start.s

fpasc.o: $(VENFILES) $(COREFILES)
	armasm -cpu 3 -fpu fpa -i $(COREDIR) -i $(VENDIR) \
		-o $@ $(VENDIR)header.s
