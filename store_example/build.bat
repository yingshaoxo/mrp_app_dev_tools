set curpath=%~dp0
cd ..
cd .\compiler
call path.bat
cd ..
cd %curpath%
cls

%MRPBUILDER%\mrpbuilder.NET.exe makefile.mpr
