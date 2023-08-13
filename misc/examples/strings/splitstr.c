#include <stdio.h>
#define i_import // cstr + utf8 functions
#include <stc/cregex.h>
#define i_implement
#include <stc/csubstr.h>

int main(void)
{
    puts("Split with c_fortoken (csubstr):");

    c_fortoken (i, "Hello World C99!", " ")
        printf("'%.*s'\n", c_SS(i.token));

    puts("\nSplit with c_formatch (regex):");

    cregex re = cregex_from("[^ ]+");
    c_formatch (i, &re, "  Hello   World      C99! ")
        printf("'%.*s'\n", c_SS(i.match[0]));

    cregex_drop(&re);
}
