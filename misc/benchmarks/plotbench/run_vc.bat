
@echo off
if "%VSINSTALLDIR%"=="" call "C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cl.exe -nologo -EHsc -std:c++latest -I../include -O2 deque_benchmark.cpp >nul
cl.exe -nologo -EHsc -std:c++latest -I../include -O2 list_benchmark.cpp >nul
cl.exe -nologo -EHsc -std:c++latest -I../include -O2 hmap_benchmark.cpp >nul
cl.exe -nologo -EHsc -std:c++latest -I../include -O2 smap_benchmark.cpp >nul
cl.exe -nologo -EHsc -std:c++latest -I../include -O2 vec_benchmark.cpp >nul
if exist *obj del *.obj

set c=VC-19.36
deque_benchmark.exe %c%
list_benchmark.exe %c%
hmap_benchmark.exe %c%
smap_benchmark.exe %c%
vec_benchmark.exe %c%
