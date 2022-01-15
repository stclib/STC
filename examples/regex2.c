#include <stc/cregex.h>
#include <stc/csview.h>
#include <stc/cstr.h>

int main()
{
    const char* inputs[] = {"birthday: 1964-12-05", "https://en.cppreference.com/w/cpp/regex/regex_search", "!123abcabc!"};
    const char* patterns[] = {"([0-9]{4})-(1[0-2]|0[1-9])-(3[01]|[12][0-9]|0[1-9])",
                              "(https?:\\/\\/|ftp:\\/\\/|www\\.)([0-9A-Za-z-@:%_+~#=]+\\.)+([a-z]{2,3})(\\/[\\/0-9A-Za-z-\\.@:%_+~#=\\?&]*)?",
                              "!((abc|123)+)!",
    };
    c_forrange (i, c_arraylen(inputs))
    {
        c_auto (cregex, re)
        {
            re = cregex_new(patterns[i]);
            csview m;
            if (cregex_find_sv(&re, inputs[i], &m))
            {
                printf("input: %s\n", inputs[i]);
                c_forrange (j, cregex_capture_size(re))
                {
                    csview cap; 
                    bool res = cregex_capture_sv(&re, j, &cap);
                    if (res) printf("  submatch %zu: " c_PRIsv "\n", j, c_ARGsv(cap));
                }
                puts("");
            }
        }
    }
}
