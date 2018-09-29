#ifndef QUEUE_H
#define QUEUE_H

template <typename T>
class Queue {
private:
  struct Node {
    Node *next;
    T value;
  };
  Node *head = NULL, *foot = NULL;
  int maxsize = 0;

public:
  int length = 0;

public:
  Queue(int maxsz = 0) : maxsize(maxsz) {}
  ~Queue() { clear(); }
  bool push(T e);
  const T pop();
  const T &peek();
  bool clear();
  inline bool is_empty() const;

};

template <typename T>
bool Queue<T>::push(T e) {
  if (maxsize && length >= maxsize)
    return false;

  Node *node = new Node;

  if (node) {
    node->next = NULL;
    node->value = e;

    if (!foot) {
      head = foot = node;
    } else {
      foot->next = node;
      foot = node;
    }

    ++length;
    return true;

  } else {
    return false;
  }
}

template <typename T>
const T Queue<T>::pop() {
  assert(head);

  Node *next = head->next;
  const T ret = head->value;
  delete head;
  head = next;

  if (!head) foot = NULL;

  --length;
  return ret;
}

template <typename T>
const T &Queue<T>::peek() {
  assert(head);

  return head->value;
}

template <typename T>
bool Queue<T>::clear() {
  Node *at = head;
  while (at) {
    Node *next = at->next;
    delete at;
    at = next;
  }

  head = foot = NULL;
  length = 0;
  return true;
}

template <typename T>
bool Queue<T>::is_empty() const {
  return !length;
}

#endif
