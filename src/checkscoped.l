/* Check for illegal return/break/continue usage inside a STC-lib c_defer/c_with block (RAII).
 * Copyright Tyge Løvset, (c) 2025.
 */
%{
#include <stdbool.h>
enum Block { LOOP=1<<0, DEFER=1<<1 };
enum State { NORMAL, BRACES, BRACESDONE };
static int braces_lev = 0, block_lev = 0;
static enum State state = NORMAL;
static unsigned char block[64] = {0}, block_type = 0;
const char* fname;
int errors = 0, warnings = 0;
%}

ID [_a-zA-Z][_a-zA-Z0-9]*
STR \"([^"\\]|\\.)*\"

%option never-interactive noyymore noyywrap nounistd
%x cmt
%x prep

%%
\/\/.*          ; // line cmt
\/\*            BEGIN(cmt);
<cmt>\n         ++yylineno;
<cmt>\*\/       BEGIN(INITIAL);
<cmt>.          ;
^[ \t]*#.*\\\n  { ++yylineno; BEGIN(prep); }
<prep>.*\\\n    ++yylineno;
<prep>.*        BEGIN(INITIAL);
^[ \t]*#.*      ;
{STR}           ;
'\\?.'          ;
c_foreach_reverse |
c_foreach_kv    |
c_foreach       |
c_forrange      |
c_forrange32    |
c_forrange_t    |
c_fortoken_sv   |
c_fortoken      |
c_formatch      |
c_forfilter_reverse_from |
c_forfilter_reverse |
c_forfilter_from |
c_forfilter     |
for             |
while           |
switch          { block_type |= LOOP; state = BRACES; }
do              { block_type |= LOOP; state = BRACESDONE; }
c_with          |
c_defer         { block_type = DEFER; state = BRACES; }
\(              { if (state == BRACES) ++braces_lev; }
\)              { if (state == BRACES && --braces_lev == 0)
                    state = BRACESDONE;
                }
if              { if (state == BRACESDONE) {
                    if (block_type == DEFER) {
                      printf("%s:%d: warning: 'if' after c_defer/c_with not enclosed in curly braces.\n"
                             "    Make sure to enclose 'if - else' statement in { } after c_defer/c_with.\n",
                             fname, yylineno);
                      ++warnings;
                    }
                    state = BRACES;
                  }
                }
;[ \t]*else     ;
;               { if (state == BRACESDONE) {
                    block_type = block[block_lev];
                    state = NORMAL;
                  }
                }
\{              { if (state != BRACES) { block[++block_lev] = block_type; state = NORMAL; } }
\}              { if (state != BRACES) block_type = block[--block_lev]; }
return          { if (block_type == DEFER) {
                    printf("%s:%d: error: 'return' used inside a c_defer/c_with scope.\n"
                           "    Use 'continue' to exit the current c_defer/c_with before return.\n"
                           , fname, yylineno);
                    ++errors;
                  } else if (block_type & DEFER) {
                    printf("%s:%d: error: 'return' used in a loop inside a c_defer/c_with scope.\n"
                           "    Use 'break' to exit loop, then 'continue' to exit c_defer/c_with.\n"
                           , fname, yylineno);
                    ++errors;
                  }
                }
break           { if (block_type == DEFER) {
                    printf("%s:%d: error: 'break' used inside a c_defer/c_with scope.\n"
                           "    Use 'continue' to exit the current c_defer/c_with.\n"
                           , fname, yylineno);
                    ++errors;
                  }
                }
{ID}            ;
\n              ++yylineno;
.               ;

%%

#include <string.h>

int main(int argc, char **argv)
{
    if (argc == 1 || strcmp(argv[1], "--help") == 0) {
        printf("C-language source code checker for correct usage of STClib c_defer() / c_with().\n");
        printf("Usage: %s {C-file... | -}\n", argv[0]);
        return 0;
    }

    for (int i=1; i<argc; ++i) {
        if (strcmp(argv[i], "-") == 0) {
            fname = "<stdin>";
            yyin = stdin;
        } else {
            fname = argv[i];
            yyin = fopen(fname, "r");
        }
        yylineno = 1;
        braces_lev = 0, block_lev = 0;
        block_type = 0, state = NORMAL;

        yylex();

        fclose(yyin);
    }

    if (errors + warnings)
        printf("%d error%s, %d warning%s.\n", errors, errors == 1? "":"s",
                                              warnings, warnings == 1? "":"s");
    return errors;
}
