#include <iostream>

#ifndef STACK_H
#define STACK_H

template <typename T>
class Stack {
private:
  struct Node {
    Node *next;
    T value;
  };
  Node *head = NULL;
  int maxsize = 0;

public:
  int length = 0;

public:
  Stack(int maxsz = 0) : maxsize(maxsz) {} 
  ~Stack();
  bool push(T e);
  const T pop();
  const T &peek() const;
  bool clear();
  inline bool is_empty() const;
};

template <typename T>
Stack<T>::~Stack() {
  clear();
}

template <typename T>
bool Stack<T>::push(T e) {
  if (maxsize && length >= maxsize)
    return false;

  Node *prev = head;
  head = new Node;
  if (head) {
    head->next = prev;
    head->value = e;
    ++length;
    return true;
  } else {
    return false;
  }
}

template <typename T>
const T Stack<T>::pop() {
  assert(head);

  Node *prev = head;
  head = head->next;

  const T ret = prev->value;
  delete prev;

  --length;
  return ret;
}

template <typename T>
const T &Stack<T>::peek() const {
  assert(head);
  return head->value;
}

template <typename T>
bool Stack<T>::clear() {
  Node *at = head;
  while (at) {
    Node *next = at->next;
    delete at;
    at = next;
  }

  head = NULL;
  length = 0;
  return true;
}

template <typename T>
bool Stack<T>::is_empty() const {
  return !length;
}

#endif
