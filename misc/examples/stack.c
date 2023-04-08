
#include <stdio.h>

#define i_tag i
#define i_capacity 100
#define i_val int
#include <stc/cstack.h>

#define i_tag c
#define i_val char
#include <stc/cstack.h>

int main() {
    cstack_i stack = {0};
    cstack_c chars = {0};

    c_forrange (i, 101)
        cstack_i_push(&stack, (int)(i*i));

    printf("%d\n", *cstack_i_top(&stack));

    c_forrange (i, 90)
        cstack_i_pop(&stack);

    c_foreach (i, cstack_i, stack)
        printf(" %d", *i.ref);
    puts("");
    printf("top: %d\n", *cstack_i_top(&stack));

    cstack_i_drop(&stack);
    cstack_c_drop(&chars);
}
