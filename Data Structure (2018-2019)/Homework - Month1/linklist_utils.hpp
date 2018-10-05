// build with -std=c++11

#ifndef LINKLIST_UTILS_HPP
#define LINKLIST_UTILS_HPP

#include <initializer_list>
#include <catch2/catch.hpp>

template <typename T>
Node<T> *build_linklist(std::initializer_list<T> il) {
    Node<T> *head, *at;
    at = head = new Node<T>;

    for (auto i : il) {
        auto *temp = new Node<T>;
        temp->value = i;
        at->next = temp;
        at = temp;
    }

    at->next = nullptr;
    return head;
}


template <typename T>
void destroy_linklist(Node<T> *va) {
    Node<T> *next;
    while (va) {
        next = va->next;
        delete va;
        va = next;
    }
}


template <typename T>
bool is_equals(Node<T> *va, std::initializer_list<T> il) {
    Node<T> *at = va->next;

    for (auto i : il) {
        if (!at) return false;

        if (at->value == i)
            at = at->next;
        else
            return false;
    }

    return at == nullptr;
}


TEST_CASE( "testing the util-functions", "[utils]" ) {
    Node<int> *list1 = build_linklist<int>({1, 2, 4});
    REQUIRE(!is_equals(list1, {}));
    REQUIRE(!is_equals(list1, {1, 2}));
    REQUIRE(!is_equals(list1, {1, 2, 4, 5}));
    REQUIRE(is_equals(list1, {1, 2, 4}));

    destroy_linklist(list1);

    Node<int> *list2 = build_linklist<int>({});
    REQUIRE(!is_equals(list2, {1}));
    REQUIRE(is_equals(list2, {}));
}

#endif
