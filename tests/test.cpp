#include <iostream>
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>

#include "../src/operation_based/CounterOB.hpp"
#include "../src/state_based/GCounterSB.hpp"

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

		REQUIRE(obj1.query_payload() == 1);
		REQUIRE(obj2.query_payload() == 2);
		REQUIRE(obj3.query_payload() == 3);
		REQUIRE(obj4.query_payload() == 1);

		obj4 = obj3;
		REQUIRE(obj4.query_payload() == 3);

		crdt::operation::CounterOB<uint32_t> one(1);
		crdt::operation::CounterOB<uint32_t> two(2);
		crdt::operation::CounterOB<uint32_t> three(3);
		crdt::operation::CounterOB<uint32_t> four(4);
		crdt::operation::CounterOB<uint32_t> five(5);

		REQUIRE(one < two);
		REQUIRE(five > three);

		four++;
		REQUIRE((four == five) == true);
		REQUIRE((one != two) == true);

		three--;

		REQUIRE(one.query_num_increments() == 1);
		REQUIRE(two.query_num_increments() == 2);
		REQUIRE(three.query_num_increments() == 3);
		REQUIRE(three.query_num_decrements() == 1);
		REQUIRE(three.query_payload() == 2);
		REQUIRE(four.query_num_increments() == 5);
	}
}



TEST_CASE("Test GCounterSB", "[classic]")
{
	SECTION("Test Insert Operation")
	{
		crdt::state::GCounterSB<uint32_t,uint32_t> obj1(1);
		crdt::state::GCounterSB<uint32_t,uint32_t> obj2(2);
		crdt::state::GCounterSB<uint32_t,uint32_t> obj3(3);
		crdt::state::GCounterSB<uint32_t,uint32_t> obj4(4);
		obj1.setNumIncrements(6);
		obj1.setNumIncrements(7);
		obj1.setNumIncrements(8);
		obj2.setNumIncrements(6);
		obj2.setNumIncrements(3);
		obj2.setNumIncrements(5);
		obj3.setNumIncrements(100);
		REQUIRE(obj1.getTotalNumIncrements() == 21);
		REQUIRE(obj2.getTotalNumIncrements() == 14);
	}
	SECTION("Test Merge Operation")
	{
		crdt::state::GCounterSB<uint32_t,uint32_t> obj1(1);
		crdt::state::GCounterSB<uint32_t,uint32_t> obj2(2);
		crdt::state::GCounterSB<uint32_t,uint32_t> obj3(3);
		crdt::state::GCounterSB<uint32_t,uint32_t> obj4(4);
		obj1.setNumIncrements(6);
		obj1.setNumIncrements(7);
		obj1.setNumIncrements(8);
		obj2.setNumIncrements(6);
		obj2.setNumIncrements(3);
		obj2.setNumIncrements(5);
		obj3.setNumIncrements(100);
		obj1.merge({obj2,obj3,obj4});
		obj2.merge({obj1,obj3,obj4});
		obj3.merge({obj1,obj2,obj4});
		obj4.merge({obj1,obj2,obj3});
		REQUIRE(obj1.getTotalNumIncrements() == obj2.getTotalNumIncrements());
		REQUIRE(obj3.getTotalNumIncrements() == obj4.getTotalNumIncrements());
		REQUIRE(obj1.getTotalNumIncrements() == obj4.getTotalNumIncrements());  
	}
}
