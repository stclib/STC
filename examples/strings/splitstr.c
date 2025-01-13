#include <stdio.h>
#include "stc/cregex.h"
#include "stc/csview.h"

int main(void)
{
    puts("Split with c_token():");

    for (c_token(i, " ", "Hello World C99!"))
        printf("'" c_svfmt "'\n", c_svarg(i.token));

    puts("\nSplit with for (c_match(regex)):");

    cregex re = cregex_from("[^\\s]+");
    for (c_match(i, &re, "  Hello   \t World   \n   C99! "))
        printf("'" c_svfmt "'\n", c_svarg(i.match[0]));

    cregex_drop(&re);
}
