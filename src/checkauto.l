/* Check for illegal return/break/continue usage inside a STC-lib c_auto* block (RAII).
 * Copyright Tyge Løvset, (c) 2022.
 */
%{
#include <stdbool.h>
enum { LOOP=1<<0, AUTO=1<<1 };
enum { NORMAL, BRACES, BRACESDONE };
static int braces_lev = 0, block_lev = 0;
static int state = NORMAL;
static unsigned int block[64] = {0}, block_type = 0;
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
c_foreach       |
c_forpair       |
c_forrange      |
for             |
while           |
switch          { block_type |= LOOP; state = BRACES; }
do              { block_type |= LOOP; state = BRACESDONE; }
c_autovar       |
c_autoscope     |
c_autodefer     |
c_auto          { block_type = AUTO; state = BRACES; }
\(              { if (state == BRACES) ++braces_lev; }
\)              { if (state == BRACES && --braces_lev == 0) {
                    state = BRACESDONE;
                  }
                }
if              { if (state == BRACESDONE) {
                    if (block_type == AUTO) {
                      printf("%s:%d: warning: 'if' after c_auto* not enclosed in curly braces.\n"
                             "    Make sure to enclose 'if - else' statement in { } after c_auto*.\n",
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
return          { if (block_type == AUTO) {
                    printf("%s:%d: error: 'return' used inside a c_auto* scope.\n"
                           "    Use 'c_breakauto' to exit the current c_auto* scope.\n",
                           fname, yylineno);
                    ++errors;
                  } else if (block_type & AUTO) {
                    printf("%s:%d: error: 'return' used in a loop inside a c_auto* scope.\n"
                           "    Use 'break' to exit loops, then 'c_breakauto' to exit c_auto*.\n",
                           fname, yylineno);
                    ++errors;
                  }
                }
break           { if (block_type == AUTO) {
                    printf("%s:%d: error: 'break' used inside a c_auto* scope.\n"
                           "    Use 'c_breakauto' to exit the current c_auto* scope.\n",
                           fname, yylineno);
                    ++errors;
                  }
                }
continue        { if (block_type == AUTO) {
                    printf("%s:%d: warning: 'continue' used inside a c_auto* scope.\n"
                           "    It will only break out of the current c_auto* scope.\n"
                           "    Use 'c_breakauto' instead to make it explicit.\n",
                           fname, yylineno);
                    ++warnings;
                  }
                }
c_breakauto     { if (block_type != AUTO) {
                    printf("%s:%d: warning: 'c_breakauto' used outside a c_auto* scope.\n"
                           "    Did you mean 'continue' instead?",
                           fname, yylineno);
                    ++warnings;
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
        printf("usage: %s [--help] {C-file... | -}\n", argv[0]);
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
