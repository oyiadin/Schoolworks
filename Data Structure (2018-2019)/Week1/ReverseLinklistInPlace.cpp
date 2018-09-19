// 对单链表进行就地逆置
#include <iostream>
using namespace std;

typedef int ElemType;

struct Node {
    struct Node *next;
    ElemType value;
};

Node *reverse_in_place(Node *head) {
    if (!head->next) return head;
    Node *prev = NULL, *at = head, *next = NULL;

    while (at) {
        next = at->next;
        at->next = prev;
        prev = at;
        at = next;
    }
    return prev;
}

int main() {
    Node n1, n2, n3, *head, *at;
    head = &n1;
    n1.next = &n2;
    n2.next = &n3;
    n3.next = NULL;
    n1.value = 11;
    n2.value = 22;
    n3.value = 33;

    at = head;
    while (at) (cout << at->value << ' '), at = at->next;

    head = reverse_in_place(head);
    cout << endl;

    at = head;
    while (at) (cout << at->value << ' '), at = at->next;

}