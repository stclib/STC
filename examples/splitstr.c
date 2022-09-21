#include <stdio.h>
#define i_implement // cstr functions
#include <stc/cstr.h>
#include <stc/csview.h>
#include <stc/cregex.h>

int main()
{
    puts("Split with c_fortoken (csview):");

    c_fortoken (i, "Hello World C99!", " ")
        printf("'%.*s'\n", c_ARGsv(i.token));


    puts("\nSplit with c_formatch (regex):");

    c_with (cregex re = cregex_from("[^ ]+", 0), cregex_drop(&re))
        c_formatch (i, &re, "  Hello   World      C99! ")
            printf("'%.*s'\n", c_ARGsv(i.match[0]));
}

#include "../src/cregex.c"
#include "../src/utf8code.c"
