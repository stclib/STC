if [ "$OS" = "Windows_NT" ]; then EXE=.exe; fi
flex autocheck.ll
gcc -O2 lex.yy.c -o autocheck$EXE
rm lex.yy.c
strip autocheck$EXE
