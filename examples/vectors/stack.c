
#include <stdio.h>

#define T IVec, int, (c_use_cmp), 100
#include <stc/stack.h>

#define T CVec, char,, 100
#include <stc/stack.h>

int main(void) {
    IVec stack = {0};
    CVec chars = {0};

    IVec_sort(&stack);

    for (c_range(i, 100))
        IVec_push(&stack, (int)(i*i));

    printf("top: %d\n", *IVec_top(&stack));

    for (c_range(i, 90))
        IVec_pop(&stack);

    for (c_each_ref(i, IVec, stack))
        printf(" %d", *i);
    puts("");
    printf("top: %d\n", *IVec_top(&stack));

    IVec_drop(&stack);
    CVec_drop(&chars);
}
