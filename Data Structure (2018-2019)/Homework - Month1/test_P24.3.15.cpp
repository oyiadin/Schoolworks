#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "P24.3.15.cpp"


TEST_CASE( "inits", "[two_way_stack]" ) {
    using TWS = TwoWayStack<int>;
    TWS tws0(0);
    REQUIRE(tws0.max_size == 0);
    REQUIRE(tws0.used_size == 0);
    REQUIRE(tws0.stack[0].base == nullptr);
    REQUIRE(tws0.stack[0].top == nullptr);
    REQUIRE(tws0.stack[1].base == nullptr);
    REQUIRE(tws0.stack[1].top == nullptr);

    TWS tws1(1);
    REQUIRE(tws1.max_size == 1);
    REQUIRE(tws1.used_size == 0);
    REQUIRE(tws1.stack[0].base == tws1.stack[0].top);
    REQUIRE(tws1.stack[1].base == tws1.stack[1].top);
    REQUIRE(tws1.stack[0].base == tws1.stack[1].base);

    TWS tws4(4);
    REQUIRE(tws4.max_size == 4);
    REQUIRE(tws4.used_size == 0);
    REQUIRE(tws4.stack[0].base == tws4.stack[0].top);
    REQUIRE(tws4.stack[1].base == tws4.stack[1].top);
    REQUIRE(tws4.stack[1].base - tws4.stack[0].base == 3);
}


TEST_CASE( "pushs & pops", "[two_way_stack]" ) {
    using TWS = TwoWayStack<int>;

    TWS tws0(0);
    REQUIRE(tws0.push(TWS::Stack_Left, 1));
    REQUIRE(tws0.push(TWS::Stack_Left, 2));
    REQUIRE(tws0.push(TWS::Stack_Right, 3));
    REQUIRE(tws0.push(TWS::Stack_Right, 4));
    REQUIRE(tws0.used_size == 4);
    REQUIRE(tws0.max_size >= 4);

    REQUIRE(tws0.pop(TWS::Stack_Left) == 2);
    REQUIRE(tws0.pop(TWS::Stack_Left) == 1);
    REQUIRE(tws0.pop(TWS::Stack_Right) == 4);
    REQUIRE(tws0.pop(TWS::Stack_Right) == 3);

    TWS tws1;
    REQUIRE(tws1.used_size == 0);
    REQUIRE(tws1.push(TWS::Stack_Left, 1));
    REQUIRE(tws1.used_size == 1);
    REQUIRE(tws1.pop(TWS::Stack_Left) == 1);
    REQUIRE(tws1.used_size == 0);

    REQUIRE(tws1.used_size == 0);
    REQUIRE(tws1.push(TWS::Stack_Right, 1));
    REQUIRE(tws1.used_size == 1);
    REQUIRE(tws1.pop(TWS::Stack_Right) == 1);
    REQUIRE(tws1.used_size == 0);

    REQUIRE(tws1.push(TWS::Stack_Left, 2));
    REQUIRE(tws1.push(TWS::Stack_Right, 3));
    REQUIRE(tws1.push(TWS::Stack_Left, 4));
    REQUIRE(tws1.push(TWS::Stack_Left, 5));
    REQUIRE(tws1.used_size == 4);
    REQUIRE(tws1.pop(TWS::Stack_Right) == 3);
    REQUIRE(tws1.used_size == 3);
    REQUIRE(tws1.push(TWS::Stack_Right, 6));
    REQUIRE(tws1.pop(TWS::Stack_Left) == 5);
    REQUIRE(tws1.pop(TWS::Stack_Left) == 4);
    REQUIRE(tws1.used_size == 2);
    REQUIRE(tws1.pop(TWS::Stack_Right) == 6);
    REQUIRE(tws1.pop(TWS::Stack_Left) == 2);
}
