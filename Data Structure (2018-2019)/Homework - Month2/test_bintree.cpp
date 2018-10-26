#include <vector>
#include <iostream>
#include <functional>
#include "bintree.h"


TEST_CASE ( ) {
  using BT = BinaryTree<char>;
  BT bt;

  REQUIRE(bt.get_root() == nullptr);
  REQUIRE(bt.create_tree(BT::PRE_ORDER, {'.'}, '.') == nullptr);
  REQUIRE(bt.create_tree(BT::PRE_ORDER, {'.', '.'}, '.') == nullptr);
  REQUIRE(bt.create_tree(BT::POST_ORDER, {'.'}, '.') == nullptr);
  REQUIRE(bt.create_tree(BT::POST_ORDER, {'.', '.'}, '.') == nullptr);
  REQUIRE(bt.get_root() == nullptr);
}


TEST_CASE ( ) {
  using BT = BinaryTree<char>;
  BT bt;

  REQUIRE(bt.create_tree(BT::PRE_ORDER, {'A', 'B', '.', 'C', '.', '.', 'D', '.', '.'}, '.'));
  BinaryNode<char> *root = bt.get_root();
  REQUIRE(root->get_data() == 'A');
  REQUIRE(root->get_lchild()->get_data() == 'B');
  REQUIRE(root->get_lchild()->get_lchild() == nullptr);
  REQUIRE(root->get_lchild()->get_rchild()->get_data() == 'C');
  REQUIRE(root->get_lchild()->get_rchild()->get_lchild() == nullptr);
  REQUIRE(root->get_lchild()->get_rchild()->get_rchild() == nullptr);
  REQUIRE(root->get_rchild()->get_data() == 'D');
  REQUIRE(root->get_rchild()->get_lchild() == nullptr);
  REQUIRE(root->get_rchild()->get_rchild() == nullptr);

  bt.clear();
  REQUIRE(bt.get_root() == nullptr);
}


TEST_CASE ( ) {
  using BT = BinaryTree<char>;
  BT bt;

  REQUIRE(bt.create_tree(BT::POST_ORDER, {'.', '.', '.', 'C', 'B', '.', '.', 'D', 'A'}, '.'));
  BinaryNode<char> *root = bt.get_root();
  REQUIRE(root->get_data() == 'A');
  REQUIRE(root->get_lchild()->get_data() == 'B');
  REQUIRE(root->get_lchild()->get_lchild() == nullptr);
  REQUIRE(root->get_lchild()->get_rchild()->get_data() == 'C');
  REQUIRE(root->get_lchild()->get_rchild()->get_lchild() == nullptr);
  REQUIRE(root->get_lchild()->get_rchild()->get_rchild() == nullptr);
  REQUIRE(root->get_rchild()->get_data() == 'D');
  REQUIRE(root->get_rchild()->get_lchild() == nullptr);
  REQUIRE(root->get_rchild()->get_rchild() == nullptr);

  bt.clear();
  REQUIRE(bt.get_root() == nullptr);
}


TEST_CASE ( ) {
  using BT = BinaryTree<char>;
  BT bt;

  bt.create_tree(
      BT::PRE_ORDER,
      {'A', 'B', '.', 'D', 'F', '.', '.', 'G', '.', '.', 'C', '.', 'E', '.', 'H', '.', '.'},
      '.');

  REQUIRE(bt.traverse(BT::PRE_ORDER) == std::vector<char>{'A', 'B', 'D', 'F', 'G', 'C', 'E', 'H'});
  REQUIRE(bt.traverse(BT::IN_ORDER) == std::vector<char>{'B', 'F', 'D', 'G', 'A', 'C', 'E', 'H'});
  REQUIRE(bt.traverse(BT::POST_ORDER) == std::vector<char>{'F', 'G', 'D', 'B', 'H', 'E', 'C', 'A'});
}