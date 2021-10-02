out="plot_linux.csv"
echo "Compiler,Library,C,Method,Seconds,Ratio"> $out
sh run_gcc.sh >> $out
sh run_clang.sh >> $out
