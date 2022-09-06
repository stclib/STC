#include <stdio.h>
#define i_implement // cstr functions
#include <stc/cstr.h>
#include <stc/csview.h>
#include <stc/cregex.h>

int main()
{
    puts("Split with c_foreach_token (csview):");

    c_foreach_token (i, "Hello World C99!", " ")
        printf("'%.*s'\n", c_ARGsv(i.token));


    puts("\nSplit with c_foreach_match (regex):");

    c_with (cregex re = cregex_from("[^ ]+", 0), cregex_drop(&re))
        c_foreach_match (i, &re, "  Hello   World      C99! ")
            printf("'%.*s'\n", c_ARGsv(i.match[0]));
}

#include "../src/cregex.c"
#include "../src/utf8code.c"
