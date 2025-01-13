
#include <stdio.h>

#define i_type stack_i, int
#define i_capacity 100
#include "stc/stack.h"

#define i_type stack_c, char
#include "stc/stack.h"

int main(void) {
    stack_i stack = {0};
    stack_c chars = {0};

    for (c_range(i, 101))
        stack_i_push(&stack, (int)(i*i));

    printf("%d\n", *stack_i_top(&stack));

    for (c_range(i, 90))
        stack_i_pop(&stack);

    for (c_each(i, stack_i, stack))
        printf(" %d", *i.ref);
    puts("");
    printf("top: %d\n", *stack_i_top(&stack));

    stack_i_drop(&stack);
    stack_c_drop(&chars);
}
