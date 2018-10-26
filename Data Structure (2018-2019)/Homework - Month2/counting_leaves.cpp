#include <iostream>
#include <cassert>
#include "bintree.h"
using namespace std;

using BiTree = BinaryTree<int>;
using BiNode = BinaryNode<int>;

int counting_leaves(BiNode *root) {
  if (root->get_degree() == 0)
    return 1;
  else
    return   counting_leaves(root->get_lchild())
           + counting_leaves(root->get_rchild());
}

int counting_leaves(BiTree &btree) {
  return counting_leaves(btree.get_root());
}


int main() {
  // to generate an example binary tree for illustration
  // the very one generated here is the same as P125 Fig.-6.4(c)
  BiTree btree(
      BiTree::PRE_ORDER, {1, 2, 4, 0, 0, 5, 6, 0, 0, 7, 0, 0, 3, 0, 0}, 0);

  assert(counting_leaves(btree) == 4);

}
