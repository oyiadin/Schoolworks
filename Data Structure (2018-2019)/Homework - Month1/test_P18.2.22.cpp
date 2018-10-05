#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "P18.2.22.cpp"
#include "linklist_utils.hpp"


TEST_CASE( "testing the function reverse_in_place", "[function]" ) {
    Node<int> *list1 = build_linklist<int>({});
    reverse_in_place(list1);
    REQUIRE(is_equals(list1, {}));

    Node<int> *list2 = build_linklist<int>({1});
    reverse_in_place(list2);
    REQUIRE(is_equals(list2, {1}));

    Node<int> *list3 = build_linklist<int>({1, 2});
    reverse_in_place(list3);
    REQUIRE(is_equals(list3, {2, 1}));

    Node<int> *list4 = build_linklist<int>({1, 2, 3});
    reverse_in_place(list4);
    REQUIRE(is_equals(list4, {3, 2, 1}));

    Node<int> *list5 = build_linklist<int>({1, 2, 3, 4, 5, 6});
    reverse_in_place(list5);
    REQUIRE(is_equals(list5, {6, 5, 4, 3, 2, 1}));
    reverse_in_place(list5);
    REQUIRE(is_equals(list5, {1, 2, 3, 4, 5, 6}));

    destroy_linklist(list1);
    destroy_linklist(list2);
    destroy_linklist(list3);
    destroy_linklist(list4);
    destroy_linklist(list5);
}
