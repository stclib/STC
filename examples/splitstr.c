#define i_val_str
#include <stc/cvec.h>
#include <stc/csview.h>

void print_split(csview str, csview sep)
{
    size_t pos = 0;
    while (pos != str.size) {
        csview tok = csview_token(str, sep, &pos);
        // print non-null-terminated csview
        printf("[" c_PRIsv "]\n", c_ARGsv(tok));
    }
}

cvec_str string_split(csview str, csview sep)
{
    cvec_str vec = cvec_str_init();
    size_t pos = 0;
    while (pos != str.size) {
        csview tok = csview_token(str, sep, &pos);
        cvec_str_push_back(&vec, cstr_from_sv(tok));
    }
    return vec;
}

int main()
{
    print_split(c_sv("//This is a//double-slash//separated//string"), c_sv("//"));
    puts("");
    print_split(c_sv("This has no matching separator"), c_sv("xx"));
    puts("");

    c_autovar (cvec_str v = string_split(c_sv("Split,this,,string,now,"), c_sv(",")), cvec_str_drop(&v))
        c_foreach (i, cvec_str, v)
            printf("[%s]\n", cstr_str(i.ref));
}
