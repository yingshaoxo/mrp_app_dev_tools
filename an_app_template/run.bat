del .\*.mrp
call build.bat
cls
del ..\compiler\vmrp\mythroad\dsm_gm.mrp
copy .\mrp_browser_by_yingshaoxo.mrp ..\compiler\vmrp\mythroad\dsm_gm.mrp
cd ..
cd .\compiler
cd .\vmrp
call main.exe
