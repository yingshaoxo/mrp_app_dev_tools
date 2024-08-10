@echo off
set reg=MWRegSvr /c
echo Registering core DLLs and IDE
%reg% .\plugins\support\MWComHelpers.dll
%reg% .\plugins\Support\MWRadModel.dll
%reg% .\plugins\Support\Catalog.dll
%reg% .\plugins\Support\JavaSourceGen.dll
%reg% .\plugins\Support\CPlusSourceGen.dll
%reg% .\plugins\com\LayoutEditor.dll
%reg% .\plugins\com\ObjectInspector.dll
%reg% .\plugins\com\MenuEditor.dll
%reg% .\plugins\com\CatalogPlugIn.dll
echo     IDE.exe
.\IDE.exe /RegServer

echo Registering Java RAD DLLs
%reg% .\plugins\Support\JavaRADServer.dll
%reg% .\plugins\Support\JavaWizards.dll

echo Registering PowerParts RAD DLLs
%reg% .\plugins\support\PowerPartsCOM.dll

echo Done.
pause
