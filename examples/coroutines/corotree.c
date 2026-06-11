
#include <stdio.h>
#include "stc/coroutine.h"

typedef struct Node {
    int value;
    struct Node *left, *right;
} Node;

cco_task_struct (Traverse, int*) {
    Traverse_base base;
    struct Node* node;
};

int inorder(struct Traverse* o) {
    cco_async (o) {
        if (o->node->left)
            cco_await_task(c_new(struct Traverse, {{inorder}, o->node->left}));

        *cco_data(o) = o->node->value;
        cco_yield;

        if (o->node->right)
            cco_await_task(c_new(struct Traverse, {{inorder}, o->node->right}));
    }

    c_free_obj(o);
    return 0;
}

int main(void) {
    Node* tree =
        &(Node){1,
            &(Node){2,
                &(Node){3},
                &(Node){4}
            },
            &(Node){5}
        };

    struct Traverse* traverse = c_new(struct Traverse, {{inorder}, tree});
    int value;
    cco_run_task(fib, traverse, &value)
    {
        if (fib->status == cco_YIELD)
            printf("val: %d\n", value);
    }
}