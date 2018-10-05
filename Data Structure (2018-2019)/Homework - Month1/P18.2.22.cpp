template <typename T>
struct Node {
    Node *next;
    T value;
};

template <typename T>
void reverse_in_place(Node<T> * &list) {
    // special case: empty linklist
    if (!list->next) return;

    Node<T> *prev = nullptr, *at = list->next, *next = nullptr;

    // to reverse every `next` field
    while (at) {
        next = at->next;
        at->next = prev;
        prev = at;
        at = next;
    }
    list->next = prev;
}
