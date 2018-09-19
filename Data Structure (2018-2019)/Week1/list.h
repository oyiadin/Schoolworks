#include "../predefine.h"

template <typename T>
class _List {

  typedef bool (*comp_func)(const T &a, const T &b);
  typedef bool (*visit_func)(T &);
  struct Node {
    Node *next;
    T value;
  };


private:
  Node *head = NULL;
  int size = 0;

public:
  _List() { init(); }
  void init() {}
  Status destroy();
  Status clear();
  inline bool is_empty();
  int length() { return size; }
  T &get_elem(int pos);
  int locate_elem(const T &e, comp_func compare = NULL);
  T &prior_elem(const T &cur_e);
  T &next_elem(const T &cur_e);
  Status insert(int pos, T e);
  T del(int pos);
  Status traverse(visit_func visit);
};

template <typename T>
Status _List<T>::destroy() { return clear(); }
// 对不起，我实现不了 destory (微笑)

template <typename T>
Status _List<T>::clear() {
  Node *at = head;
  for (int i = 0; i < size; ++i) {
    Node *next = head->next;
    delete at;
    at = next;
  }
  size = 0;
}

template <typename T>
bool _List<T>::is_empty() { return !size; }

template <typename T>
T &_List<T>::get_elem(int pos) {
  assert(pos > 0 && pos <= size);
  Node *at = head;
  for (int i = 1; i < pos; ++i) at = at->next;
  return at->value;
}

template <typename T>
int _List<T>::locate_elem(const T &e, comp_func compare) {
  Node *at = head;
  int pos = 1;
  while (at) {
    if (compare ? compare(e, at->value) : (e == at->value)) return pos;
    at = at->next;
    ++pos;
  }
  return -1;
}

template <typename T>
T &_List<T>::prior_elem(const T &cur_e) {
  Node *at = head;
  if (head) {
    while (at->next) {
      if (at->next->value == cur_e) return at->value;
      at = at->next;
    }
  }
}

template <typename T>
T &_List<T>::next_elem(const T &cur_e) {
  Node *at = head;
  while (at) {
    if (at->value == cur_e)
      return at->next->value;
  }
}

template <typename T>
Status _List<T>::insert(int pos, T e) {
  assert (pos > 0 && pos <= (size+1));
  if (pos == 1) {
    Node *temp = new Node;
    temp->next = head;
    temp->value = e;
    head = temp;

  } else {
    Node *at = head;
    for (int i = 2; i < pos; ++i) at = at->next;
    Node *temp = new Node;
    temp->next = at->next;
    temp->value = e;
    at->next = temp;
  }

  ++size;
  return OK;
}

template <typename T>
T _List<T>::del(int pos) {
  assert (pos > 0  && pos <= size);
  T to_be_deleted;
  if (pos == 1) {
    Node *temp = head;
    head = head->next;
    to_be_deleted = temp->value;
    delete temp;

  } else {
    Node *at = head;
    for (int i = 2; i < pos; ++i) at = at->next;
    Node *temp = at->next;
    at->next = temp->next;
    to_be_deleted = temp->value;
    delete temp;
  }

  --size;
  return to_be_deleted;
}

template <typename T>
Status _List<T>::traverse(visit_func visit) {
  Node *at = head;
  while (at) {
    if (!visit(at->value)) return ERROR;
    at = at->next;
  }
  return OK;
}

typedef _List<ElemType> List;
