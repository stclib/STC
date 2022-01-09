#include <stc/cregex.h>
#include <stc/csview.h>
#include <stc/cstr.h>

int main()
{
    const char* fnames[] = {"foofile.txt", "barfile.txt", "bazboy.dat", "zoidberg"};
    c_auto (cregex, re)
    {
        re = cregex_new("([a-z]+)\\.([a-z]+)");

        c_forrange (i, c_arraylen(fnames))
        {
            printf("%s\n", fnames[i]);
            if (cregex_is_match(&re, fnames[i]))
            {
                c_forrange (j, cregex_capture_size(re))
                {
                    csview cap; cregex_capture_v(&re, j, &cap);
                    printf("  submatch %d: " c_PRIsv "\n", j, c_ARGsv(cap));
                }
                puts("");
            }
        }
    }
}
