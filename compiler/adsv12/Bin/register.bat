@echo off
rem ***********************************************************
rem
rem register.bat
rem Copyright (C) ARM Limited 2000. All rights reserved.
rem
rem RCS $Revision: 1.9 $
rem Checkin $Date: 2000/09/22 14:54:50 $
rem
rem ***********************************************************

echo Installing OCX files for AXD

regcomif adw3backx.ocx
if not errorlevel 1 goto file2
if not errorlevel 2 goto usage
echo FAILED to install OCX adw3backx.ocx

:file2
regcomif adw3cli.ocx
if not errorlevel 1 goto file3
if not errorlevel 2 goto usage
echo FAILED to install OCX adw3cli.ocx

:file3
regcomif adw3comvwx.ocx
if not errorlevel 1 goto file4
if not errorlevel 2 goto usage
echo FAILED to install OCX adw3comvwx.ocx

:file4
regcomif adw3conx.ocx
if not errorlevel 1 goto file5
if not errorlevel 2 goto usage
echo FAILED to install OCX adw3conx.ocx

:file5
regcomif adw3hostx.ocx
if not errorlevel 1 goto file6
if not errorlevel 2 goto usage
echo FAILED to install OCX adw3hostx.ocx

:file6
regcomif adw3lowlevx.ocx
if not errorlevel 1 goto file7
if not errorlevel 2 goto usage
echo FAILED to install OCX adw3lowlevx.ocx

:file7
regcomif adw3memx.ocx
if not errorlevel 1 goto file8
if not errorlevel 2 goto usage
echo FAILED to install OCX adw3memx.ocx

:file8
regcomif adw3outx.ocx
if not errorlevel 1 goto file9
if not errorlevel 2 goto usage
echo FAILED to install OCX adw3outx.ocx

:file9
regcomif adw3regx.ocx
if not errorlevel 1 goto file10
if not errorlevel 2 goto usage
echo FAILED to install OCX adw3regx.ocx

:file10
regcomif adw3varx.ocx
if not errorlevel 1 goto file11
if not errorlevel 2 goto usage
echo FAILED to install OCX adw3varx.ocx

:file11
regcomif adw3watchx.ocx
if not errorlevel 1 goto file12
if not errorlevel 2 goto usage
echo FAILED to install OCX adw3watchx.ocx

:file12
regcomif defedm.ocx
if not errorlevel 1 goto file13
if not errorlevel 2 goto usage
echo FAILED to install OCX defedm.ocx

:file13
regcomif adw3internx.ocx
if not errorlevel 1 goto file14
if not errorlevel 2 goto usage
echo FAILED to install OCX adw3internx.ocx

:file14
regcomif adw3brkptx.ocx
if not errorlevel 1 goto file15
if not errorlevel 2 goto usage
echo FAILED to install OCX adw3brkptx.ocx

:file15
regcomif adw3watptx.ocx
if not errorlevel 1 goto file16
if not errorlevel 2 goto usage
echo FAILED to install OCX adw3watptx.ocx

:file16
regcomif dbex.ocx
if not errorlevel 1 goto finished
if not errorlevel 2 goto usage
echo FAILED to install OCX dbex.ocx

:finished
goto end


:usage
echo GIVING UP

:end

