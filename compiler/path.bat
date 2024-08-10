set ads=%~dp0adsv12

set ARMCONF=%ads%\Bin
set ARMDLL=%ads%\Bin
set ARMHOME=%ads%\adsv12
set ARMINC=%ads%\include
set ARMLIB=%ads%\lib

set MRPBUILDER=%~dp0skysdk20\compiler
set path=%ARMDLL%;%path%
set armlmd_license_file=%~dp0adsv12\license.dat;%armlmd_license_file%
echo armcc:%armcc%