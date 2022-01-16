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
        float_expr = cregex_new("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)");
        // Until "q" is given, ask for another number
        while (true)
        {
            printf("Enter a double precision number (q for quit): ");
            cstr_getline(&input, stdin);

            // Exit when the user inputs q
            if (cstr_equals(input, "q"))
                break;
            
            if (cregex_matches(&float_expr, input.str))
                printf("Input is a float\n");
            else
                printf("Invalid input : Not a float\n");
        }
    }
}
