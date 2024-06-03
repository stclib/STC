#include <stdio.h>
#define i_import // cstr + utf8 functions
#include "stc/cregex.h"
#define i_implement
#include "stc/csview.h"

int main(void)
{
    puts("Split with c_fortoken (csview):");

    c_fortoken_sv (i, " ", c_sv("Hello World C99!"))
        printf("'%.*s'\n", c_SVARG(i.token));

    puts("\nSplit with c_formatch (regex):");

    cregex re = cregex_from("[^\\s]+");
    c_formatch (i, &re, "  Hello   \t World   \n   C99! ")
        printf("'%.*s'\n", c_SVARG(i.match[0]));

    cregex_drop(&re);
}
