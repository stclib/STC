
#include <stdio.h>
#include <stc/cstr.h>
#include <stc/cstack.h>

using_cvec(i, int);
using_cvec(c, char);
using_cstack(i, cvec_i);
using_cstack(c, cvec_c);

int main() {
    cstack_i stack = cstack_i_init();
    cstack_c chars = cstack_c_init();
    c_fordefer (cstack_i_del(&stack), cstack_c_del(&chars))
    {
        c_forrange (i, int, 101)
            cstack_i_push(&stack, i*i);

        printf("%d\n", *cstack_i_top(&stack));

        c_forrange (i, int, 90)
            cstack_i_pop(&stack);

        c_foreach (i, cstack_i, stack)
            printf(" %d", *i.ref);
        puts("");
        printf("top: %d\n", *cstack_i_top(&stack));
    }
}