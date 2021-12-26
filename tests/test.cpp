#include <iostream>
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>
using namespace std;


//#include "../src/operation_based/CounterOB.hpp"
#include "../src/state_based/GSetSB.hpp"
/*
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
*/
TEST_CASE("Test GSetSB", "[classic]")
{
	SECTION("Test Insert Operation")
	{
		crdt::state::GSetSB<uint32_t> handler(0); //Represents Server 1
		crdt::state::GSetMetadata<uint32_t> replica1A(1,3);
		crdt::state::GSetMetadata<uint32_t> replica1B(2,6);
		crdt::state::GSetMetadata<uint32_t> replica1C(3,9);
		handler.addExternalReplica({replica1A,replica1B,replica1C});

		//for (auto const &e: handler.queryPayload()) {
        //    std::cout << e << ' ';
        //}
        //cout << endl;

		std::set<uint32_t> test = {3,6,9};
		REQUIRE(handler.compare_sets(handler.queryPayload(), test));

		replica1B.insert(8);
		handler.addExternalReplica({replica1A,replica1B,replica1C});

		test = {3,6,8,9};
		REQUIRE(handler.compare_sets(handler.queryPayload(), test));

		replica1C.insert(2);
		replica1C.insert(10);
		replica1C.insert(10);
		replica1C.insert(36);
		handler.addExternalReplica({replica1A,replica1B,replica1C});
		test = {2,9,10,36};
		REQUIRE(handler.queryPayloadwithID(3) == test); 
		test = {2,3,6,8,9,10,36};
		REQUIRE(handler.compare_sets(handler.queryPayload(), test));
	}

	SECTION("Test Conflict on localServer")
	{
		crdt::state::GSetSB<uint32_t> handler1(1); //Represents Server 1
		crdt::state::GSetMetadata<uint32_t> replica1A(1,2);
		crdt::state::GSetMetadata<uint32_t> replica1B(1,4);
		crdt::state::GSetMetadata<uint32_t> replica1C(1,6);
		handler1.addExternalReplica({replica1A,replica1B,replica1C});
		std::set<uint32_t> val = {2,4,6};
		REQUIRE(handler1.queryPayload() == val);

		crdt::state::GSetSB<uint32_t> handler2(2); //Represents Server 1
		crdt::state::GSetMetadata<uint32_t> replica2A(3,{1,2,3});
		crdt::state::GSetMetadata<uint32_t> replica2B(3,{2,3,4});
		crdt::state::GSetMetadata<uint32_t> replica2C(3,{4,5,6});
		handler2.addExternalReplica({replica2A,replica2B,replica2C});
		val = {1,2,3,4,5,6};
		REQUIRE(handler2.queryPayload() == val);
	}

	SECTION("Test Conflict Non-String on Multiple Servers")
	{
		crdt::state::GSetSB<uint32_t> handler1(1); //Represents Server 1
		crdt::state::GSetSB<uint32_t> handler2(2); //Represents Server 1.2
		crdt::state::GSetSB<uint32_t> handler3(3); //Represents Server 1.3
		crdt::state::GSetMetadata<uint32_t> replicaA(4,{3,6,9});
		crdt::state::GSetMetadata<uint32_t> replicaB(4,{2,4,6});
		crdt::state::GSetMetadata<uint32_t> replicaC(4,{1,2,3});
		handler1.addExternalReplica({replicaA});
		handler2.addExternalReplica({replicaB});
		handler3.addExternalReplica({replicaC});
		REQUIRE(handler1.queryPayloadwithID(4) != handler2.queryPayloadwithID(4));
		REQUIRE(handler1.queryPayloadwithID(4) != handler3.queryPayloadwithID(4));
		REQUIRE(handler2.queryPayloadwithID(4) != handler3.queryPayloadwithID(4));
		handler1.updateLocalExternalPayload({handler1,handler2,handler3}); // merge
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryPayloadwithID(4) == handler2.queryPayloadwithID(4));
		REQUIRE(handler1.queryPayloadwithID(4) == handler3.queryPayloadwithID(4));
		REQUIRE(handler2.queryPayloadwithID(4) == handler3.queryPayloadwithID(4));
	}
}
