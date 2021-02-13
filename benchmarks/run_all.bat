set out=plot_perf.csv
echo Compiler,Library,C,Method,Seconds,Ratio> %out%
sh run_gcc.sh >> %out%
sh run_clang.sh >> %out%
rem make sure cl.exe environment is available.
call run_vc.bat >> %out%
