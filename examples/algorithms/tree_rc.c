#include <stdio.h>
#include <stc/algorithm.h>
#include <stc/types.h>

declare_rc(TreeRc, union Tree);

c_union (Tree,
    (TreeEmpty, bool),
    (TreeLeaf, int),
    (TreeNode, struct { int value; TreeRc left, right; })
);
void Tree_drop(Tree*);

#define T TreeRc, Tree, (c_declared | c_keyclass)
#include <stc/rc.h>

void Tree_drop(Tree* self) {
    if (c_is(self, TreeNode, n)) {
        TreeRc_drop(&n->left);
        TreeRc_drop(&n->right);
    }
}

int tree_sum(TreeRc tree) {
    c_when (tree.get) {
        c_is(TreeEmpty) return 0;
        c_is(TreeLeaf, v) return *v;
        c_is(TreeNode, n) return n->value + tree_sum(n->left) + tree_sum(n->right);
    }
    return -1;
}

int main(void) {
    TreeRc tree =
    TreeRc_make(c_variant(TreeNode, {1,
        TreeRc_make(c_variant(TreeNode, {2,
            TreeRc_make(c_variant(TreeLeaf, 3)),
            TreeRc_make(c_variant(TreeLeaf, 4))
        })),
        TreeRc_make(c_variant(TreeLeaf, 5))
    }));

    printf("sum = %d\n", tree_sum(tree));
    TreeRc_drop(&tree);
}
