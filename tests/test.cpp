#include <iostream>
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>

#include "../src/operation_based/CounterOB.hpp"

TEST_CASE("Test CounterOB", "[classic]")
{
	SECTION("Test Overloaded Operators")
	{
		crdt::operation::CounterOB<uint32_t> obj1(1);
		crdt::operation::CounterOB<uint32_t> obj2(2);
		crdt::operation::CounterOB<uint32_t> obj3;
		crdt::operation::CounterOB<uint32_t> obj4;
		obj3 = obj1 + obj2;
		obj4 = obj2 - obj1;

		REQUIRE(obj1.query() == 1);
		REQUIRE(obj2.query() == 2);
		REQUIRE(obj3.query() == 3);
		REQUIRE(obj4.query() == 1);
	}
}
