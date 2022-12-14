#include <stdio.h>
#define i_extern // cstr + utf8 functions
#include <stc/cregex.h>
#include <stc/csview.h>

int main()
{
    puts("Split with c_fortoken (csview):");

    c_fortoken (i, "Hello World C99!", " ")
        printf("'%.*s'\n", c_ARGsv(i.token));


    puts("\nSplit with c_formatch (regex):");

    c_with (cregex re = cregex_from("[^ ]+", CREG_DEFAULT), cregex_drop(&re))
        c_formatch (i, &re, "  Hello   World      C99! ")
            printf("'%.*s'\n", c_ARGsv(i.match[0]));
}
