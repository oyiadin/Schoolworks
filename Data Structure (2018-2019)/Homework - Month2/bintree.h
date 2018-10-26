#ifndef BINTREE_H
#define BINTREE_H

#include <string>
#include <vector>


template <typename T> class BinaryNode;
template <typename T> class BinaryTree;

template <typename T>
class BinaryNode {
  friend class BinaryTree<T>;

 public:
  BinaryNode() = default;
  explicit BinaryNode(const T &d) : data(d) {}
  BinaryNode(BinaryNode *p, const T &d) : parent(p), data(d) {}
  ~BinaryNode() = default;
  const T &get_data() const { return data; }
  BinaryNode *get_parent() const { return parent; }
  BinaryNode *get_lchild() const { return left; }
  BinaryNode *get_rchild() const { return right; }
  int get_degree() const {
    return (left ? 1 : 0) + (right ? 1 : 0);
  }

 public:  // protected:
  // public for reading convenience
  // as I dont want to put the functions, like `swap_left_right`, insides the class
  T data;
  BinaryNode *parent = nullptr, *left = nullptr, *right = nullptr;
};


template <typename T>
class BinaryTree {
 public:
  enum Order { PRE_ORDER, IN_ORDER, POST_ORDER };

 public:
  BinaryTree() = default;
  ~BinaryTree() = default;
  BinaryTree(Order order, std::vector<T> v, const T &empty_indicator) {
    create_tree(order, v, empty_indicator);
  }
  BinaryNode<T> *get_root() const { return root; }
  BinaryNode<T> *create_tree(
      Order order, std::vector<T> v, const T &empty_indicator);
  std::vector<T> traverse(Order order) const {
    std::vector<T> v;
    traverse(order, root, v);
    return v;
  }
  void clear() {
    clear(root);
    root = nullptr;
  }
  void clear(BinaryNode<T> *ptr);

 protected:
  BinaryNode<T> *root = nullptr;
  BinaryNode<T> *create_tree(
      Order order,
      typename std::vector<T>::iterator &begin,
      typename std::vector<T>::iterator &end,
      const T &empty_indicator);
  std::vector<T> &traverse(Order order, BinaryNode<T> *ptr, std::vector<T> &v) const;
};


template <typename T>
BinaryNode<T> *BinaryTree<T>::create_tree(
    Order order, std::vector<T> v, const T &empty_indicator) {

  if (root)
    throw "you cannot call create_tree() on a non-empty tree object";

  typename std::vector<T>::iterator begin = v.begin(), end = v.end();
  root = create_tree(order, begin, end, empty_indicator);
  return root;
}


template <typename T>
BinaryNode<T> *BinaryTree<T>::create_tree(
    Order order,
    typename std::vector<T>::iterator &begin,
    typename std::vector<T>::iterator &end,
    const T &empty_indicator) {

  if (begin == end)
    return nullptr;

  BinaryNode<T> *node = nullptr;
  switch (order) {
    case PRE_ORDER:
      if (*begin == empty_indicator)
        return nullptr;
      node = new BinaryNode<T>;
      node->data = *begin;
      node->left = create_tree(order, ++begin, end, empty_indicator);
      node->right = create_tree(order, ++begin, end, empty_indicator);
      node->left && (node->left->parent = node);
      node->right && (node->right->parent = node);
      break;

    case POST_ORDER:
      if (*(end-1) == empty_indicator)
        return nullptr;
      node = new BinaryNode<T>;
      node->data = *(end-1);
      node->right = create_tree(order, begin, --end, empty_indicator);
      node->left = create_tree(order, begin, --end, empty_indicator);
      node->left && (node->left->parent = node);
      node->right && (node->right->parent = node);
      break;

    default:
      throw "cannot create a binary tree according to the specific order";
  }

  return node;
}


template <typename T>
std::vector<T> &BinaryTree<T>::traverse(
    Order order, BinaryNode<T> *ptr, std::vector<T> &v) const {

  if (ptr == nullptr)
    return v;

  switch (order) {
    case PRE_ORDER:
      v.push_back(ptr->get_data());
      traverse(order, ptr->left, v);
      traverse(order, ptr->right, v);
      break;

    case IN_ORDER:
      traverse(order, ptr->left, v);
      v.push_back(ptr->get_data());
      traverse(order, ptr->right, v);
      break;

    case POST_ORDER:
      traverse(order, ptr->left, v);
      traverse(order, ptr->right, v);
      v.push_back(ptr->get_data());
      break;
  }

  return v;
}


template <typename T>
void BinaryTree<T>::clear(BinaryNode<T> *ptr) {
  if (!ptr) return;

  clear(ptr->left);
  clear(ptr->right);
  delete ptr;
}

#endif // BINTREE_H
