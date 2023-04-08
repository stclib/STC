set out=plot_win.csv
echo Compiler,Library,C,Method,Seconds,Ratio> %out%
echo gcc
sh run_gcc.sh >> %out%
echo clang
sh run_clang.sh >> %out%
REM call run_vc.bat >> %out%
