if [ "$OS" = "Windows_NT" ]; then EXE=.exe; fi
flex checkauto.ll
gcc -O2 lex.yy.c -o checkauto$EXE
rm lex.yy.c
strip checkauto$EXE
