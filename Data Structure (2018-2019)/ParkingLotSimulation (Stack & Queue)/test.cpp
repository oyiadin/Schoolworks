#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "queue.h"
#include "stack.h"

TEST_CASE( "basic pushs & pops of stack", "[stack]" ) {
	Stack<int> s;
	REQUIRE(s.push(666));
	REQUIRE(s.pop() == 666);
	REQUIRE(s.push(555));
	REQUIRE(s.push(444));
	REQUIRE(s.pop() == 444);
	REQUIRE(s.push(333));
	REQUIRE(s.pop() == 333);
	REQUIRE(s.push(222));
	REQUIRE(s.pop() == 222);
	REQUIRE(s.pop() == 555);
	REQUIRE(s.is_empty());
}

TEST_CASE( "covering all functions & properties of stack", "[stack]" ) {
	Stack<int> q(3);

	REQUIRE(q.length == 0);
	REQUIRE(q.is_empty());

	q.push(666);
	REQUIRE(q.length == 1);
	REQUIRE(!q.is_empty());
	REQUIRE(q.peek() == 666);

	q.push(555);
	REQUIRE(q.length == 2);
	REQUIRE(q.peek() == 555);
	REQUIRE(q.pop() == 555);
	REQUIRE(q.length == 1);

	q.push(444);
	q.push(333);
	REQUIRE(q.length == 3);
	REQUIRE(q.peek() == 333);
	REQUIRE(!q.is_empty());

	REQUIRE(!q.push(222));  // maxsize
	REQUIRE(q.length == 3);

	REQUIRE(q.pop() == 333);
	REQUIRE(q.pop() == 444);
	REQUIRE(!q.is_empty());
	REQUIRE(q.pop() == 666);
	REQUIRE(q.is_empty());

	q.push(222);
	REQUIRE(q.clear());
	REQUIRE(q.is_empty());
}

TEST_CASE( "basic pushs & pops of queue", "[queue]" ) {
	Queue<int> q;
	REQUIRE(q.push(666));
	REQUIRE(q.pop() == 666);
	REQUIRE(q.push(555));
	REQUIRE(q.push(444));
	REQUIRE(q.pop() == 555);
	REQUIRE(q.push(333));
	REQUIRE(q.pop() == 444);
	REQUIRE(q.push(222));
	REQUIRE(q.pop() == 333);
	REQUIRE(q.pop() == 222);
	REQUIRE(q.is_empty());
}

TEST_CASE( "covering all functions & properties of queue", "[queue]" ) {
	Queue<int> q(3);

	REQUIRE(q.length == 0);
	REQUIRE(q.is_empty());

	q.push(666);
	REQUIRE(q.length == 1);
	REQUIRE(!q.is_empty());
	REQUIRE(q.peek() == 666);

	q.push(555);
	REQUIRE(q.length == 2);
	REQUIRE(q.peek() == 666);
	REQUIRE(q.pop() == 666);
	REQUIRE(q.length == 1);

	q.push(444);
	q.push(333);
	REQUIRE(q.length == 3);
	REQUIRE(q.peek() == 555);
	REQUIRE(!q.is_empty());

	REQUIRE(!q.push(222));  // maxsize
	REQUIRE(q.length == 3);

	REQUIRE(q.pop() == 555);
	REQUIRE(q.pop() == 444);
	REQUIRE(!q.is_empty());
	REQUIRE(q.pop() == 333);
	REQUIRE(q.is_empty());

	q.push(222);
	REQUIRE(q.clear());
	REQUIRE(q.is_empty());
}
