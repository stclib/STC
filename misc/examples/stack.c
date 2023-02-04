
#include <stdio.h>

#define i_tag i
#define i_capacity 100
#define i_val int
#include <stc/cstack.h>

#define i_tag c
#define i_val char
#include <stc/cstack.h>

int main() {
    c_AUTO (cstack_i, stack)
    c_AUTO (cstack_c, chars)
    {
        c_FORRANGE (i, 101)
            cstack_i_push(&stack, (int)(i*i));

        printf("%d\n", *cstack_i_top(&stack));

        c_FORRANGE (i, 90)
            cstack_i_pop(&stack);

        c_FOREACH (i, cstack_i, stack)
            printf(" %d", *i.ref);
        puts("");
        printf("top: %d\n", *cstack_i_top(&stack));
    }
}
