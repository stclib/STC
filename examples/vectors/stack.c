
#include <stdio.h>

#define T IVec, int, 100, (c_use_cmp)
#include <stc/stack.h>

#define T CVec, char, 100, (0)
#include <stc/stack.h>

int main(void) {
    IVec stack = {0};
    CVec chars = {0};

    IVec_sort(&stack);

    for (c_range(i, 101))
        IVec_push(&stack, (int)(i*i));

    printf("%d\n", *IVec_top(&stack));

    for (c_range(i, 90))
        IVec_pop(&stack);

    for (c_each(i, IVec, stack))
        printf(" %d", *i.ref);
    puts("");
    printf("top: %d\n", *IVec_top(&stack));

    IVec_drop(&stack);
    CVec_drop(&chars);
}
