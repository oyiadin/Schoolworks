#include <iostream>
#include <exception>

#define check_boundary(x) \
  if (x >= size) \
    throw std::out_of_range("index out of range")

template <class Tp> class Set;
template <class Tp> std::istream &operator>>(std::istream &is, Set<Tp> &rs);
template <class Tp> std::ostream &operator<<(std::ostream &os, const Set<Tp> &rs);

template <class Tp>
class Set {
 public:
  typedef unsigned int size_t;
  struct Node {
    Tp element;
    Node *next, *prev;
  };

  Set() {}
  Set(const Set &old);
  Set &operator=(const Set &old);

  Set &insert(size_t at, const Tp &element);
  inline Set &insert(const Tp &element);  // insert(0, element)
  inline Set &append(const Tp &element);  // insert(size, element)
  Set &remove(size_t at);

  int index(const Tp &element) const;
  inline bool contains(const Tp &element) const;
  inline size_t length(void) const;

  Set &unio(const Set &set);  // 'union' isn't a legal identifier
  Set &intersection(const Set &set);
  Set operator+(const Set &rs) const;  // union
  Set operator*(const Set &rs) const;  // intersection
  Tp &operator[](size_t ptr1);

  friend std::istream &operator>> <Tp>(std::istream &is, Set<Tp> &rs);
  friend std::ostream &operator<< <Tp>(std::ostream &os, const Set<Tp> &rs);

 private:
  void make_it_distinct(size_t start_from = 0);
  size_t size = 0;
  Node *head = 0, *foot = 0;
};


template<class Tp>
Set<Tp>::Set(const Set &old) : size(old.size) {
  *this = old;
}

template <class Tp>
Set<Tp> &Set<Tp>::operator=(const Set &old) {
  // deep copy
  size = old.size;
  Node *ptr1 = 0, *ptr2 = old.head;
  while (ptr2) {
    Node *new_node = new Node;
    new_node->element = ptr2->element;
    new_node->prev = ptr1;
    if (ptr1) ptr1->next = new_node;
    else head = new_node;
    ptr1 = new_node;
    ptr2 = ptr2->next;
  }
  if (ptr1) ptr1->next = 0;
  foot = ptr1;

  return *this;
}


template <class Tp>
Set<Tp> &Set<Tp>::insert(size_t at, const Tp &element) {
  // insert an element at `at`
  if (at > size)  // make `insert(size, xxx)` legal
    throw std::out_of_range("index out of range");

  bool is_head = (at == 0);
  Node *ptr = head;
  while (at--) ptr = ptr->next;

  Node *new_node = new Node;
  new_node->element = element;
  if (ptr) {
    new_node->next = ptr;
    new_node->prev = ptr->prev;
    if (ptr->prev) {
      ptr->prev->next = new_node;
      ptr->prev = new_node;
    } else {
      head = new_node;
      ptr->prev = new_node;
    }
  } else {
    if (size == 0) {
      head = new_node;
      foot = new_node;
      new_node->next = new_node->prev = 0;
    } else {
      if (is_head) {
        new_node->next = head;
        new_node->prev = 0;
        if (head) {
          head->prev = new_node;
        }
        head = new_node;
      } else {  // is_foot
        new_node->prev = foot;
        new_node->next = 0;
        if (foot) {
          foot->next = new_node;
        }
        foot = new_node;
      }
    }
  }
  make_it_distinct(size++);
  return *this;
}

template<class Tp>
Set<Tp> &Set<Tp>::insert(const Tp &element) {
  insert(0, element);
}

template <class Tp>
Set<Tp> &Set<Tp>::append(const Tp &element) {
  insert(size, element);
}

template <class Tp>
Set<Tp> &Set<Tp>::remove(size_t at) {
  check_boundary(at);

  Node *ptr = head;
  while (at--) ptr = ptr->next;

  if (ptr->prev) ptr->prev->next = ptr->next;
  else head = ptr->next;
  if (ptr->next) ptr->next->prev = ptr->prev;
  else foot = ptr->prev;
  delete ptr;
  --size;

  return *this;
}


template <class Tp>
int Set<Tp>::index(const Tp &element) const {
  // :return: the position of element, return -1 if not found
  int i = 0;
  Node *ptr1 = head;
  while (ptr1) {
    if (ptr1->element == element)
      return i;
    ptr1 = ptr1->next;
    ++i;
  }
  return -1;
}

template<class Tp>
bool Set<Tp>::contains(const Tp &element) const {
  return (index(element) != -1);
}

template<class Tp>
size_t Set<Tp>::length(void) const {
  return size;
}


template <class Tp>
Set<Tp> &Set<Tp>::unio(const Set &set) {
  // extends `*this` with `set`
  // then all I have to do is just to call `make_it_distinct()`
  size_t size_backup = size;
  Node *ptr1 = foot, *ptr2 = set.head;
  while (ptr2) {
    Node *new_node = new Node;
    new_node->element = ptr2->element;
    if (!ptr1) {
      ptr1 = head = new_node;
      new_node->prev = 0;
    } else {
      ptr1->next = new_node;
      new_node->prev = ptr1;
    }
    ptr1 = new_node;
    ptr2 = ptr2->next;
    ++size;
  }
  foot = ptr1;
  foot->next = 0;
  make_it_distinct(size_backup);
  return *this;
}

template <class Tp>
Set<Tp> &Set<Tp>::intersection(const Set &set) {
  Node *ptr1 = head;
  size_t at = 0;
  while (ptr1) {
    Node *ptr2 = set.head;
    bool found = false;
    while (ptr2) {
      if (ptr1->element == ptr2->element) {
        found = true;
        break;
      } else {
        ptr2 = ptr2->next;
      }
    }
    ptr1 = ptr1->next;
    if (!found) {
      remove(at);
    } else {
      at++;
    }
  }

  return *this;
}

template <class Tp>
Set<Tp> Set<Tp>::operator+(const Set &rs) const {
  Set temp = *this;
  temp.unio(rs);
  return temp;
}

template <class Tp>
Set<Tp> Set<Tp>::operator*(const Set &rs) const {
  Set temp = *this;
  temp.intersection(rs);
  return temp;
}

template <class Tp>
Tp &Set<Tp>::operator[](size_t at) {
  check_boundary(at);

  Node *ptr1 = head;
  while (at--)
    ptr1 = ptr1->next;
  return ptr1->element;
}


template <class Tp>
std::istream &operator>>(std::istream &is, Set<Tp> &rs) {
  Tp temp;
  while (is >> temp)
    rs.append(temp);
  return is;
}

template <class Tp>
std::ostream &operator<<(std::ostream &os, const Set<Tp> &rs) {
  os << "{";
  typename Set<Tp>::Node *ptr = rs.head;
  while (ptr) {
    if (ptr != rs.head) os << ", ";
    os << ptr->element;
    ptr = ptr->next;
  }
  os << "}";
  return os;
}


template <class Tp>
void Set<Tp>::make_it_distinct(size_t start_from) {
  // :param start_from: where to start searching
  // delete the repeated ones

  // check_boundary(start_from);
  // because this function won't be exposed to the outside scope (private)
  // I assume that the argument will never be out of range
  Node *ptr1 = head;
  while (start_from--) ptr1 = ptr1->next;

  while (ptr1) {
    Node *ptr2 = head;
    while (ptr2) {
      if (ptr2->element == ptr1->element && ptr2 != ptr1) {  // if found
        // I assume that <Tp> supports the operator `==`
        if (ptr2->prev) ptr2->prev->next = ptr2->next;
        else head = ptr2->next;
        if (ptr2->next) ptr2->next->prev = ptr2->prev;
        else foot = ptr2->prev;

        delete ptr2;
        --size;
        break;
      }
      ptr2 = ptr2->next;
    }
    ptr1 = ptr1->next;
  }
}


int main() {
  using namespace std;
  typedef Set<int> IntSet;

  IntSet a;
  a.append(1).append(2);
  IntSet b = a;             // operator=(const Set &)
  b[0] = 233;               // operator[](size_t)
  cout << a << endl;
  cout << b << endl << endl;

  IntSet c;
  c.insert(11).insert(22);  // insert(const Tp &)
  c.insert(1, 233);         // insert(size_t, const Tp &)
  c.append(999);            // append(const Tp &)
  cout << c << endl;
  c.remove(0);
  cout << c << " : " << c.length() << endl << endl;

  cout << c.index(999) << " "       // index(const Tp &)
       << c.index(12345) << endl;
  cout << c.contains(999) << " "    // contains(const Tp &)
       << c.contains(12345) << endl << endl;

  IntSet a0, a1, a2;
  a1.append(1).append(2);
  a2.append(2).append(3);
  cout << a1+a2+a0 << endl;             // operator+(const Set<Tp> &)
  cout << a1*a2 << endl;                // operator*(const Set<Tp> &)
  cout << a1.unio(a2) << endl << endl;  // unio(const Set<Tp> &)
}
