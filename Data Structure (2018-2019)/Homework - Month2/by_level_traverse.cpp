#include <iostream>
#include <vector>
#include <cassert>
#include "bintree.h"
#include "queue.h"
using namespace std;

using BiTree = BinaryTree<int>;
using BiNode = BinaryNode<int>;

void by_level_traverse(BiNode *root, vector<int> &v, Queue<BiNode *> &q) {
  if (root == nullptr)
    return;

  q.push(root);
  while (!q.is_empty()) {
    BiNode *temp = q.pop();
    v.push_back(temp->get_data());
    temp->get_lchild() && q.push(temp->get_lchild());
    temp->get_rchild() && q.push(temp->get_rchild());
  }
}

vector<int> by_level_traverse(BiTree &btree) {
  vector<int> v;
  Queue<BiNode *> q;
  by_level_traverse(btree.get_root(), v, q);
  return v;
}


int main() {
  // to generate an example binary tree for illustration
  // the very one generated here is the same as P125 Fig.-6.4(c)
  BiTree btree(
      BiTree::PRE_ORDER, {1, 2, 4, 0, 0, 5, 6, 0, 0, 7, 0, 0, 3, 0, 0}, 0);

  assert(by_level_traverse(btree) == vector<int>({1, 2, 3, 4, 5, 6, 7}));

}
