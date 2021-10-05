
#include <stdio.h>
#include <stc/cstr.h>

#define i_tag i
#define i_val int
#include <stc/cstack.h>

#define i_tag c
#define i_val char
#include <stc/cstack.h>

int main() {
    cstack_i stack = cstack_i_init();
    cstack_c chars = cstack_c_init();
    c_autodefer (cstack_i_del(&stack), cstack_c_del(&chars))
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