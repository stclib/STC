
@echo off
if "%VSINSTALLDIR%"=="" call "C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cl.exe -nologo -EHsc -std:c++latest -I../include -O2 cdeq_benchmark.cpp >nul
cl.exe -nologo -EHsc -std:c++latest -I../include -O2 clist_benchmark.cpp >nul
cl.exe -nologo -EHsc -std:c++latest -I../include -O2 cmap_benchmark.cpp >nul
cl.exe -nologo -EHsc -std:c++latest -I../include -O2 csmap_benchmark.cpp >nul
cl.exe -nologo -EHsc -std:c++latest -I../include -O2 cvec_benchmark.cpp >nul
if exist *obj del *.obj

set c=VC-19.36
cdeq_benchmark.exe %c%
clist_benchmark.exe %c%
cmap_benchmark.exe %c%
csmap_benchmark.exe %c%
cvec_benchmark.exe %c%
