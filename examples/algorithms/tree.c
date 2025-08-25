#include <stdio.h>
#include <stdlib.h>
#include <stc/algorithm.h>
#include <stc/types.h>

c_union (Tree,
    (TreeEmpty, bool),
    (TreeLeaf, int),
    (TreeNode, struct { int value; Tree *left, *right; })
);

void Tree_drop(Tree* self) {
    if (c_is(self, TreeNode, n)) {
        Tree_drop(n->left);
        free(n->left);
        Tree_drop(n->right);
        free(n->right);
    }
    free(self);
}

int tree_sum(Tree* tree) {
    c_when (tree) {
        c_is(TreeEmpty) return 0;
        c_is(TreeLeaf, v) return *v;
        c_is(TreeNode, n) return n->value + tree_sum(n->left) + tree_sum(n->right);
    }
    return -1;
}

int main(void) {
    Tree* tree =
    &c_variant(TreeNode, {1,
        &c_variant(TreeNode, {2,
            &c_variant(TreeLeaf, 3),
            &c_variant(TreeLeaf, 4)
        }),
        &c_variant(TreeLeaf, 5)
    });

    printf("sum = %d\n", tree_sum(tree));
}
