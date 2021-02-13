cl.exe -nologo -EHsc -std:c++latest -I.. -O2 cdeq_benchmark.cpp >nul
cl.exe -nologo -EHsc -std:c++latest -I.. -O2 clist_benchmark.cpp >nul
cl.exe -nologo -EHsc -std:c++latest -I.. -O2 cmap_benchmark.cpp >nul
cl.exe -nologo -EHsc -std:c++latest -I.. -O2 csmap_benchmark.cpp >nul
cl.exe -nologo -EHsc -std:c++latest -I.. -O2 cvec_benchmark.cpp >nul
del *.obj >nul

cdeq_benchmark.exe VC-19.28
clist_benchmark.exe VC-19.28
cmap_benchmark.exe VC-19.28
csmap_benchmark.exe VC-19.28
cvec_benchmark.exe VC-19.28