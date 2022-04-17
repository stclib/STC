#include <stc/cstr.h>
#include <stc/cregex.h>

int main(int argc, char* argv[])
{
    if (argc <= 1) {
        printf("Usage: regex1 -i\n");
        return 0;
    }
    c_auto (cstr, input)
    c_auto (cregex, float_expr)
    {
        int res = cregex_compile(&float_expr, "^[+-]?[0-9]+((\\.[0-9]*)?|\\.[0-9]+)$", 0);
        // Until "q" is given, ask for another number
        if (res > 0) while (true)
        {
            printf("Enter a double precision number (q for quit): ");
            cstr_getline(&input, stdin);

            // Exit when the user inputs q
            if (cstr_equals(input, "q"))
                break;
            
            if (cregex_find(&float_expr, cstr_str(&input), 0, NULL, 0) > 0)
                printf("Input is a float\n");
            else
                printf("Invalid input : Not a float\n");
        }
    }
}

#include "../src/cregex.c"
