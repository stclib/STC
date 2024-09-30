#include <stdio.h>
#include "stc/cregex.h"
#include "stc/csview.h"

int main(void)
{
    puts("Split with c_fortoken():");

    c_fortoken (i, " ", "Hello World C99!")
        printf("'" c_svfmt "'\n", c_svarg(i.token));

    puts("\nSplit with c_formatch (regex):");

    cregex re = cregex_from("[^\\s]+");
    c_formatch (i, &re, "  Hello   \t World   \n   C99! ")
        printf("'" c_svfmt "'\n", c_svarg(i.match[0]));

    cregex_drop(&re);
}
