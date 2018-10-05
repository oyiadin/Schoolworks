#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "P17.2.11.cpp"


TEST_CASE( "integer inputs of binary search", "[binary_search]" ) {
    int temp, va[] = {1, 3, 4, 5, 7};
    const int size = sizeof(va) / sizeof(int);

    // empty array
    REQUIRE(binary_search(1, va, 0) == 0);
    // one-element array
    temp = binary_search(1, va, 1);
    REQUIRE((temp == 0 || temp == 1));
    REQUIRE(binary_search(-1, va, 1) == 0);  
    REQUIRE(binary_search(2, va, 1) == 1);
    // two-elements array
    REQUIRE(binary_search(2, va, 2) == 1);

    temp = binary_search(1, va, size);
    REQUIRE((temp == 0 || temp == 1));

    temp = binary_search(3, va, size);
    REQUIRE((temp == 1 || temp == 2));

    temp = binary_search(4, va, size);
    REQUIRE((temp == 2 || temp == 3));

    REQUIRE(binary_search(2, va, size) == 1);
    REQUIRE(binary_search(6, va, size) == 4);

    REQUIRE(binary_search(-1, va, size) == 0);
    REQUIRE(binary_search(8, va, size) == 5);
}


TEST_CASE( "double inputs of binary search", "[binary_search]" ) {
    int temp;
    double va[] = {-1.0, 3.4, 4.2, 5.8, 7.1};
    const int size = sizeof(va) / sizeof(double);

    // empty array
    REQUIRE(binary_search<double>(1.1, va, 0) == 0);
    // one-element array
    temp = binary_search<double>(-1.0, va, 1);
    REQUIRE((temp == 0 || temp == 1));
    REQUIRE(binary_search<double>(-2.2, va, 1) == 0);
    REQUIRE(binary_search<double>(2.5, va, 1) == 1);
    // two-elements array
    REQUIRE(binary_search<double>(2.1, va, 2) == 1);

    temp = binary_search<double>(-1, va, size);
    REQUIRE((temp == 0 || temp == 1));

    temp = binary_search<double>(3.4, va, size);
    REQUIRE((temp == 1 || temp == 2));

    temp = binary_search<double>(4.2, va, size);
    REQUIRE((temp == 2 || temp == 3));

    REQUIRE(binary_search<double>(2.1, va, size) == 1);
    REQUIRE(binary_search<double>(6.6, va, size) == 4);

    REQUIRE(binary_search<double>(-2.2, va, size) == 0);
    REQUIRE(binary_search<double>(8.9, va, size) == 5);
}


template <typename T>
bool is_increasing(const T *va, const int length) {
    if (length == 0)
        return true;

    T last = va[0];
    for (int i = 1; i < length; ++i) {
        if (va[i] < last)
            return false;
        last = va[i];
    }

    return true;
}


TEST_CASE( "do insertion", "[insertion]" ) {
    int va[10] = {1, 3, 4, 5, 7};
    int size = 5;

    insert_into_nonstrictly_increasing_ordered_list(-1, va, size);
    REQUIRE(size == 6);
    REQUIRE(is_increasing(va, size));

    insert_into_nonstrictly_increasing_ordered_list(2, va, size);
    REQUIRE(size == 7);
    REQUIRE(is_increasing(va, size));

    insert_into_nonstrictly_increasing_ordered_list(5, va, size);
    REQUIRE(size == 8);
    REQUIRE(is_increasing(va, size));

    insert_into_nonstrictly_increasing_ordered_list(10, va, size);
    REQUIRE(size == 9);
    REQUIRE(is_increasing(va, size));
}
