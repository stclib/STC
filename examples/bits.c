#include <stdio.h>
#include <stc/cbitvec.h>

int main() {
    CBitVec vec = cbitvec_make(23, true);
    cbitvec_unset(&vec, 9);
    cbitvec_resize(&vec, 43, false);
    printf("%4zu: ", vec.size);
    for (int i=0; i<vec.size; ++i)
        printf("%d", cbitvec_value(&vec, i));puts("");

    cbitvec_set(&vec, 28);
    cbitvec_resize(&vec, 77, true);
    cbitvec_resize(&vec, 93, false);
    cbitvec_resize(&vec, 102, true);
    cbitvec_setValue(&vec, 99, false);
    printf("%4zu: ", vec.size);
        for (int i=0; i<vec.size; ++i)
            printf("%d", cbitvec_value(&vec, i));puts("");

    cbitvec_setAll(&vec, false);
    printf("%4zu: ", vec.size);
    for (int i=0; i<vec.size; ++i)
        printf("%d", cbitvec_value(&vec, i));puts("");
    cbitvec_destroy(&vec);
}