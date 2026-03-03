#include <stdio.h>
#include <stc/algorithm.h>
#include <stc/types.h>

declare_rc(RcTree, union Tree);

c_union (Tree,
    (Tree_EMPTY, bool),
    (Tree_LEAF, int),
    (Tree_NODE, struct { int value; RcTree left, right; })
);
void Tree_drop(Tree*);


#define T RcTree, Tree, (c_class_key | c_declared)
#include <stc/rc.h>

void Tree_drop(Tree* self) {
    if (c_is(self, Tree_NODE, n)) {
        RcTree_drop(&n->left);
        RcTree_drop(&n->right);
    }
}

int RcTree_sum(RcTree tree) {
    c_when (tree.get) {
        c_is(Tree_EMPTY) return 0;
        c_is(Tree_LEAF, v) return *v;
        c_is(Tree_NODE, n) return n->value + RcTree_sum(n->left) + RcTree_sum(n->right);
    }
    return -1;
}

int main(void) {
    RcTree tree =
    RcTree_make(c_variant(Tree_NODE, {1,
        RcTree_make(c_variant(Tree_NODE, {2,
            RcTree_make(c_variant(Tree_LEAF, 3)),
            RcTree_make(c_variant(Tree_LEAF, 4))
        })),
        RcTree_make(c_variant(Tree_LEAF, 5))
    }));

    printf("sum = %d\n", RcTree_sum(tree));
    RcTree_drop(&tree);
}
