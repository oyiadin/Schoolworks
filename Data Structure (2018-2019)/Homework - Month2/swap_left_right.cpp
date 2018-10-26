#include <iostream>
#include <cassert>
#include "bintree.h"
using namespace std;

using BiTree = BinaryTree<int>;
using BiNode = BinaryNode<int>;

bool swap_left_right(BiNode *root) {
  if (!root)
    return true;

  BiNode *temp = root->left;
  root->left = root->right;
  root->right = temp;

  return swap_left_right(root->left) && swap_left_right(root->right);
}

bool swap_left_right(BiTree &btree) {
  return swap_left_right(btree.get_root());
}


int main() {
  // to generate an example binary tree for illustration
  // the very one generated here is the same as P125 Fig.-6.4(c)
  BiTree btree(
      BiTree::PRE_ORDER, {1, 2, 4, 0, 0, 5, 6, 0, 0, 7, 0, 0, 3, 0, 0}, 0);
  swap_left_right(btree);

  assert(btree.traverse(BiTree::PRE_ORDER) == vector<int>({1, 3, 2, 5, 7, 6, 4}));
}