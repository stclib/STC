#include <stdio.h>
#include <stc/csview.h>

void print_split(csview input, csview sep)
{
    size_t pos = 0;
    while (pos <= input.size) {
        csview tok = csview_token(input, sep, &pos);
        // print non-null-terminated csview
        printf("[%.*s]\n", c_ARGsv(tok));
    }
}

#include <stc/cstr.h>
#define i_val_str
#include <stc/cstack.h>

cstack_str string_split(csview input, csview sep)
{
    cstack_str out = cstack_str_init();
    size_t pos = 0;
    while (pos <= input.size) {
        csview tok = csview_token(input, sep, &pos);
        cstack_str_push(&out, cstr_from_sv(tok));
    }
    return out;
}

int main()
{
    print_split(c_sv("//This is a//double-slash//separated//string"), c_sv("//"));
    puts("");
    print_split(c_sv("This has no matching separator"), c_sv("xx"));
    puts("");

    c_with (cstack_str s = string_split(c_sv("Split,this,,string,now,"), c_sv(",")), cstack_str_drop(&s))
        c_foreach (i, cstack_str, s)
            printf("[%s]\n", cstr_str(i.ref));
}
