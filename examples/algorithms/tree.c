#include <stdio.h>
#include <stdlib.h>
#include <stc/algorithm.h>
#include <stc/types.h>

c_union (Tree,
    (Tree_EMPTY, bool),
    (Tree_LEAF, int),
    (Tree_NODE, struct { int value; Tree *left, *right; }),
);

void Tree_drop(Tree* self) {
    if (c_is(self, Tree_NODE, n)) {
        Tree_drop(n->left);
        Tree_drop(n->right);
    }
    free(self);
}

int Tree_sum(Tree* tree) {
    c_when (tree) {
        c_is(Tree_EMPTY) return 0;
        c_is(Tree_LEAF, v) return *v;
        c_is(Tree_NODE, n) return n->value + Tree_sum(n->left) + Tree_sum(n->right);
    }
    return -1;
}
/*
int main(void) {
    Tree* tree =
    &c_variant(Tree_NODE, {1,
        &c_variant(Tree_NODE, {2,
            &c_variant(Tree_LEAF, 3),
            &c_variant(Tree_LEAF, 4)
        }),
        &c_variant(Tree_LEAF, 5)
    });

    printf("sum = %d\n", Tree_sum(tree));
}
*/

int main(void) {
    Tree* tree = 
    c_new(Tree, c_variant(Tree_NODE, {1,
        c_new(Tree, c_variant(Tree_NODE, {2,
            c_new(Tree, c_variant(Tree_LEAF, 3)),
            c_new(Tree, c_variant(Tree_LEAF, 4))
        })),
        c_new(Tree, c_variant(Tree_LEAF, 5))
    }));
    
    printf("sum = %d\n", Tree_sum(tree));
    Tree_drop(tree);
}
