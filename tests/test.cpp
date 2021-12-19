#include <iostream>
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>

#include "../src/operation_based/CounterOB.hpp"

TEST_CASE("test equality", "[classic]")
{
	crdt::operation::CounterOB<uint32_t> obj1(5);
	crdt::operation::CounterOB<uint32_t> obj2(5);
	crdt::operation::CounterOB<uint32_t> obj3;
	obj3 = obj1 + obj2;

	SECTION("test multiple")
	{
		REQUIRE(obj1.query() == 5);
		REQUIRE(obj2.query() == 5);
		REQUIRE(obj3.query() == 10);
	}
}
