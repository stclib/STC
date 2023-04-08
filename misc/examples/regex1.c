#define i_extern
#include <stc/cregex.h>

int main(int argc, char* argv[])
{
    if (argc <= 1) {
        printf("Usage: regex1 -i\n");
        return 0;
    }
    cstr input = {0};
    cregex float_expr = {0};

    int res = cregex_compile(&float_expr, "^[+-]?[0-9]+((\\.[0-9]*)?|\\.[0-9]+)$");
    // Until "q" is given, ask for another number
    if (res > 0) while (true)
    {
        printf("Enter a double precision number (q for quit): ");
        cstr_getline(&input, stdin);

        // Exit when the user inputs q
        if (cstr_equals(&input, "q"))
            break;

        if (cregex_is_match(&float_expr, cstr_str(&input)))
            printf("Input is a float\n");
        else
            printf("Invalid input : Not a float\n");
    }

    cstr_drop(&input);
    cregex_drop(&float_expr);
}
