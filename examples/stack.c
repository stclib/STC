
#include <stc/cstack.h>
#include <stdio.h>

declare_cvec(i, int);
declare_cstack(i, cvec_i);

int main() {
    cstack_i stack = cstack_i_init();

    for (int i=0; i<100; ++i)
        cstack_i_push(&stack, i*i);
    
    for (int i=0; i<90; ++i)
        cstack_i_pop(&stack);
    
    printf("top: %d\n", *cstack_i_top(&stack));
}