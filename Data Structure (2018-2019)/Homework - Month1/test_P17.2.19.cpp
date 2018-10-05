#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <initializer_list>
#include "P17.2.19.cpp"
#include "linklist_utils.hpp"


TEST_CASE( "testing the util-functions", "[utils]" ) {
    Node<int> *list1 = build_linklist<int>({1, 2, 4});
    REQUIRE(!is_equals(list1, {}));
    REQUIRE(!is_equals(list1, {1, 2}));
    REQUIRE(!is_equals(list1, {1, 2, 4, 5}));
    REQUIRE(is_equals(list1, {1, 2, 4}));

    Node<int> *list2 = build_linklist<int>({});
    REQUIRE(!is_equals(list2, {1}));
    REQUIRE(is_equals(list2, {}));

    destroy_linklist(list1);
    destroy_linklist(list2);
}


TEST_CASE( "testing function `remove_values_between`", "[function]" ) {
    Node<int> *list1 = build_linklist<int>({1, 2, 4, 5, 6});
    remove_values_between(list1, 0, 7);
    REQUIRE(is_equals(list1, {}));
    destroy_linklist(list1);

    Node<int> *list2 = build_linklist<int>({1, 2, 4, 5, 6});

    remove_values_between(list2, 0, 0);
    remove_values_between(list2, 1, 1);
    remove_values_between(list2, 0, 1);
    remove_values_between(list2, 6, 7);
    REQUIRE(is_equals(list2, {1, 2, 4, 5, 6}));

    remove_values_between(list2, 0, 3);
    REQUIRE(is_equals(list2, {4, 5, 6}));

    remove_values_between(list2, 4, 6);
    REQUIRE(is_equals(list2, {4, 6}));

    destroy_linklist(list1);
    destroy_linklist(list2);
}