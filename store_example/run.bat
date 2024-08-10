call build.bat
cls
del ..\compiler\vmrp\mythroad\dsm_gm.mrp
copy .\mrpshop.mrp ..\compiler\vmrp\mythroad\dsm_gm.mrp
cd ..
cd .\compiler
cd .\vmrp
call main.exe
