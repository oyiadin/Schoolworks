// 输入：递增单链表，带表头
// 删除 (>mink) && (<maxk) 的项
#include <iostream>
using namespace std;

#ifdef DEBUG
#define watch(x) (cout << #x "=" << (x) << endl)
#endif

typedef int ElemType;

struct Node {
    Node *next;
    ElemType value;
};

typedef Node *LinkList;

void remove_between(LinkList list, double mink, double maxk) {
    Node *at = list, *next = at->next;
    while (next) {
        if (next->value > mink) {
            Node *nextnext = at->next;
            while (next && next->value < maxk) {
                nextnext = next->next;
                free(next);
                next = nextnext;
            }
            at->next = nextnext;
            return;
        }
        at = at->next;
        next = at->next;
    }
}

int main() {
    Node *node0, *node1, *node2, *node3, *node4, *at;
    node0 = (Node *) malloc(sizeof(Node));
    node1 = (Node *) malloc(sizeof(Node));
    node2 = (Node *) malloc(sizeof(Node));
    node3 = (Node *) malloc(sizeof(Node));
    node4 = (Node *) malloc(sizeof(Node));

    LinkList list = node0;
    node0->next = node1;
    node1->next = node2;
    node2->next = node3;
    node3->next = node4;
    node4->next = NULL;
    node1->value = 11;
    node2->value = 22;
    node3->value = 33;
    node4->value = 44;

    at = node0;
    while (at->next) (cout << at->next->value << ' '), at=at->next;

    remove_between(list, 15, 35);
    cout << endl;

    at = node0;
    while (at->next) (cout << at->next->value << ' '), at=at->next;
}