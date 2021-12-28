#include <iostream>
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>
using namespace std;


//#include "../src/operation_based/CounterOB.hpp"
#include "../src/state_based/GSetSB.hpp"
#include "../src/state_based/TwoPSetSB.hpp"
#include "../src/state_based/ORSetSB.hpp"
#include "../src/state_based/VectorSB.hpp"

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

	SECTION("Test Conflict on Multiple Servers")
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
		std::set<uint32_t> test = {1,2,3,4,6,9};
		REQUIRE(handler1.queryPayload() == test);
		REQUIRE(handler2.queryPayload() == test);
		REQUIRE(handler3.queryPayload() == test);
	}
}

TEST_CASE("Test TwoPSetSB", "[classic]")
{
	SECTION("Test Insert Operation")
	{
		crdt::state::TwoPSetSB<uint32_t> handler(0); //Represents Server 1
		crdt::state::TwoPSetMetadata<uint32_t> replica1A(1,3);
		crdt::state::TwoPSetMetadata<uint32_t> replica1B(2,6);
		crdt::state::TwoPSetMetadata<uint32_t> replica1C(3,9);
		handler.addExternalReplica({replica1A,replica1B,replica1C});

		//for (auto const &e: handler.queryTwoPSet()) {
        //    std::cout << e << ' ';
        //}
        //cout << endl;

		std::set<uint32_t> test = {3,6,9};
		REQUIRE(handler.queryTwoPSet() == test);

		replica1B.insert(8);
		handler.addExternalReplica({replica1A,replica1B,replica1C});

		test = {3,6,8,9};
		REQUIRE(handler.queryTwoPSet() == test);

		replica1C.insert(2);
		replica1C.insert(10);
		replica1C.insert(10);
		replica1C.insert(36);
		handler.addExternalReplica({replica1A,replica1B,replica1C});
		test = {2,9,10,36};
		REQUIRE(handler.queryTwoPSetwithID(3) == test); 
		test = {2,3,6,8,9,10,36};
		REQUIRE(handler.queryTwoPSet() == test);
	}

	SECTION("Test Remove Operation")
	{
		crdt::state::TwoPSetSB<uint32_t> handler(0); //Represents Server 1
		crdt::state::TwoPSetMetadata<uint32_t> replica1A(1,{1,2,3});
		crdt::state::TwoPSetMetadata<uint32_t> replica1B(2,{2,6,4});
		crdt::state::TwoPSetMetadata<uint32_t> replica1C(3,{7,8,9,10});
		handler.addExternalReplica({replica1A,replica1B,replica1C});

		std::set<uint32_t> test = {1,2,3,4,6,7,8,9,10};
		REQUIRE(handler.queryTwoPSet() == test);

		replica1B.remove(4);
		handler.addExternalReplica({replica1A,replica1B,replica1C});

		test = {1,2,3,6,7,8,9,10};
		REQUIRE(handler.queryTwoPSet() == test);

		replica1B.insert(4); // should fail to insert
		replica1C.remove(7);
		replica1C.remove(7);
		replica1C.remove(8);
		replica1C.remove(9);
		handler.addExternalReplica({replica1A,replica1B,replica1C});
		test = {10};
		REQUIRE(handler.queryTwoPSetwithID(3) == test); 
		test = {1,2,3,6,10};
		REQUIRE(handler.queryTwoPSet() == test);
	}

	SECTION("Test Conflict on localServer")
	{
		crdt::state::TwoPSetSB<uint32_t> handler1(1); //Represents Server 1
		crdt::state::TwoPSetMetadata<uint32_t> replica1A(1,2);
		crdt::state::TwoPSetMetadata<uint32_t> replica1B(1,4);
		crdt::state::TwoPSetMetadata<uint32_t> replica1C(1,6);
		handler1.addExternalReplica({replica1A,replica1B,replica1C});
		std::set<uint32_t> val = {2,4,6};
		REQUIRE(handler1.queryTwoPSet() == val);

		crdt::state::TwoPSetSB<uint32_t> handler2(2); //Represents Server 1
		crdt::state::TwoPSetMetadata<uint32_t> replica2A(3,{1,2,3});
		crdt::state::TwoPSetMetadata<uint32_t> replica2B(3,{2,3,4});
		crdt::state::TwoPSetMetadata<uint32_t> replica2C(3,{4,5,6});
		crdt::state::TwoPSetMetadata<uint32_t> replica2D(4,{1,2,3,4,5,6});
		handler2.addExternalReplica({replica2A,replica2B,replica2C});
		val = {1,2,3,4,5,6};
		REQUIRE(handler2.queryTwoPSet() == val);
		replica2A.remove(3);
		replica2C.remove(4);
		handler2.addExternalReplica({replica2A,replica2B,replica2C,replica2D});
		val = {1,2,5,6};
		REQUIRE(handler2.queryTwoPSet() == val);
		REQUIRE(handler2.queryTwoPSetwithID(3) == val); 
		val = {1,2,3,4,5,6};
		REQUIRE(handler2.queryTwoPSetwithID(4) == val); 
	}

	SECTION("Test Conflict on Multiple Servers")
	{
		crdt::state::TwoPSetSB<uint32_t> handler1(1); //Represents Server 1
		crdt::state::TwoPSetSB<uint32_t> handler2(2); //Represents Server 1.2
		crdt::state::TwoPSetSB<uint32_t> handler3(3); //Represents Server 1.3
		crdt::state::TwoPSetMetadata<uint32_t> replicaA(4,{3,6,9});
		crdt::state::TwoPSetMetadata<uint32_t> replicaB(4,{2,4,6});
		crdt::state::TwoPSetMetadata<uint32_t> replicaC(4,{1,2,3});
		crdt::state::TwoPSetMetadata<uint32_t> replicaD(5,{1,2,3,6,9});
		crdt::state::TwoPSetMetadata<uint32_t> replicaE(6,{2,3,6,9,10});

		handler1.addExternalReplica({replicaA,replicaD});
		handler2.addExternalReplica({replicaB,replicaE});
		handler3.addExternalReplica({replicaC});
		REQUIRE(handler1.queryTwoPSetwithID(4) != handler2.queryTwoPSetwithID(4));
		REQUIRE(handler1.queryTwoPSetwithID(4) != handler3.queryTwoPSetwithID(4));
		REQUIRE(handler2.queryTwoPSetwithID(4) != handler3.queryTwoPSetwithID(4));
		handler1.updateLocalExternalPayload({handler1,handler2,handler3}); // merge
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryTwoPSetwithID(4) == handler2.queryTwoPSetwithID(4));
		REQUIRE(handler1.queryTwoPSetwithID(4) == handler3.queryTwoPSetwithID(4));
		REQUIRE(handler2.queryTwoPSetwithID(4) == handler3.queryTwoPSetwithID(4));
		std::set<uint32_t> test = {1,2,3,4,6,9,10};
		REQUIRE(handler1.queryTwoPSet() == test);
		REQUIRE(handler2.queryTwoPSet() == test);
		REQUIRE(handler3.queryTwoPSet() == test);
		test = {2,3,6,9,10};
		REQUIRE(handler1.queryTwoPSetwithID(6) == test);
		REQUIRE(handler2.queryTwoPSetwithID(6) == test);
		REQUIRE(handler3.queryTwoPSetwithID(6) == test);

		replicaB.remove(2);
		replicaE.remove(6);
		replicaE.remove(9);
		handler1.addExternalReplica({replicaA,replicaD});
		handler2.addExternalReplica({replicaB,replicaE});
		handler3.addExternalReplica({replicaC});
		handler1.updateLocalExternalPayload({handler1,handler2,handler3}); // merge
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryTwoPSetwithID(4) == handler2.queryTwoPSetwithID(4));
		REQUIRE(handler1.queryTwoPSetwithID(4) == handler3.queryTwoPSetwithID(4));
		REQUIRE(handler2.queryTwoPSetwithID(4) == handler3.queryTwoPSetwithID(4));
		test = {1,3,4,6,9};
		REQUIRE(handler1.queryTwoPSetwithID(4) == test);
		test = {2,3,10};
		REQUIRE(handler1.queryTwoPSetwithID(6) == test);
		REQUIRE(handler2.queryTwoPSetwithID(6) == test);
		REQUIRE(handler3.queryTwoPSetwithID(6) == test);
		test = {1,3,4,10};
		REQUIRE(handler1.queryTwoPSet() == test);
		REQUIRE(handler1.queryTwoPSet() == handler2.queryTwoPSet());
		REQUIRE(handler1.queryTwoPSet() == handler3.queryTwoPSet());
		REQUIRE(handler2.queryTwoPSet() == handler3.queryTwoPSet());

		replicaB.insert(2); // insert tombstone value 2 again will not change the set
		handler2.addExternalReplica({replicaB});
		handler1.updateLocalExternalPayload({handler1,handler2,handler3}); // merge
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryTwoPSetwithID(4) == handler2.queryTwoPSetwithID(4));
		REQUIRE(handler1.queryTwoPSetwithID(4) == handler3.queryTwoPSetwithID(4));
		REQUIRE(handler2.queryTwoPSetwithID(4) == handler3.queryTwoPSetwithID(4));
		test = {1,3,4,6,9};
		REQUIRE(handler1.queryTwoPSetwithID(4) == test);
		test = {2,3,10};
		REQUIRE(handler1.queryTwoPSetwithID(6) == test);
		REQUIRE(handler2.queryTwoPSetwithID(6) == test);
		REQUIRE(handler3.queryTwoPSetwithID(6) == test);
		test = {1,3,4,10};
		REQUIRE(handler1.queryTwoPSet() == test);
		REQUIRE(handler1.queryTwoPSet() == handler2.queryTwoPSet());
		REQUIRE(handler1.queryTwoPSet() == handler3.queryTwoPSet());
		REQUIRE(handler2.queryTwoPSet() == handler3.queryTwoPSet());
	}
}


TEST_CASE("Test VectorSB", "[classic]")
{
	SECTION("Test Insert Operation")
	{
		crdt::state::VectorSB<uint32_t> handler(0); //Represents Server 1
		crdt::state::VectorMetadata<uint32_t> replica1A(1,3);
		crdt::state::VectorMetadata<uint32_t> replica1B(2,6);
		crdt::state::VectorMetadata<uint32_t> replica1C(3,9);
		handler.addExternalReplica({replica1A,replica1B,replica1C});

		std::vector<uint32_t> test = {3,6,9};
		REQUIRE(handler.queryPayload() == test);

		replica1B.push_back(16);
		handler.addExternalReplica({replica1A,replica1B,replica1C});

		test = {3,6,9,16};
		REQUIRE(handler.queryPayload() == test);

		replica1C.push_back(2);
		replica1C.push_back(10);
		replica1C.push_back(10);
		replica1C.push_back(36);
		handler.addExternalReplica({replica1A,replica1B,replica1C});
		test = {9,2,10,10,36};
		REQUIRE(handler.queryPayloadwithID(3) == test); 
		test = {3,6,9,2,10,10,16,36};
		REQUIRE(handler.queryPayload() == test);
	}
	
	SECTION("Test Conflict on localServer")
	{
		crdt::state::VectorSB<uint32_t> handler1(1); //Represents Server 1
		crdt::state::VectorMetadata<uint32_t> replica1A(1,2);
		crdt::state::VectorMetadata<uint32_t> replica1B(1,4);
		crdt::state::VectorMetadata<uint32_t> replica1C(1,6);
		handler1.addExternalReplica({replica1A,replica1B,replica1C});
		std::vector<uint32_t> val = {2,4,6};
		REQUIRE(handler1.queryPayload() == val);

		crdt::state::VectorSB<uint32_t> handler2(2); //Represents Server 1
		crdt::state::VectorMetadata<uint32_t> replica2A(3,{1,2,3});
		crdt::state::VectorMetadata<uint32_t> replica2B(3,{2,3,4,2});
		crdt::state::VectorMetadata<uint32_t> replica2C(3,{4,5,6});
		handler2.addExternalReplica({replica2A,replica2B,replica2C});
		val = {1,2,3,2,4,5,6};
		REQUIRE(handler2.queryPayload() == val);
	}
	
	SECTION("Test Conflict on Multiple Servers")
	{
		crdt::state::VectorSB<uint32_t> handler1(1); //Represents Server 1
		crdt::state::VectorSB<uint32_t> handler2(2); //Represents Server 1.2
		crdt::state::VectorSB<uint32_t> handler3(3); //Represents Server 1.3
		crdt::state::VectorMetadata<uint32_t> replicaA(4,{3,6,9});
		crdt::state::VectorMetadata<uint32_t> replicaB(4,{2,4,6});
		crdt::state::VectorMetadata<uint32_t> replicaC(4,{1,2,3,6,6});
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
		std::vector<uint32_t> test = {1,2,3,4,6,6,9};
		REQUIRE(handler1.queryPayload() == test);
		REQUIRE(handler2.queryPayload() == test);
		REQUIRE(handler3.queryPayload() == test);
	}
}

TEST_CASE("Test ORSetSB", "[classic]")
{
	SECTION("Test Insert Operation")
	{
		crdt::state::ORSetSB<uint32_t> handler(0); //Represents Server 1
		crdt::state::ORSetMetadata<uint32_t> replica1A(1,3);
		crdt::state::ORSetMetadata<uint32_t> replica1B(2,6);
		crdt::state::ORSetMetadata<uint32_t> replica1C(3,9);
		handler.addExternalReplica({replica1A,replica1B,replica1C});

		std::set<uint32_t> test = {3,6,9};
		REQUIRE(handler.queryORSet() == test);

		replica1B.insert(8);
		handler.addExternalReplica({replica1A,replica1B,replica1C});

		test = {3,6,8,9};
		REQUIRE(handler.queryORSet() == test);

		replica1C.insert(2);
		replica1C.insert(10);
		replica1C.insert(10);
		replica1C.insert(36);
		handler.addExternalReplica({replica1A,replica1B,replica1C});
		test = {2,9,10,36};
		REQUIRE(handler.queryORSetwithID(3) == test); 
		test = {2,3,6,8,9,10,36};
		REQUIRE(handler.queryORSet() == test);
	}

	SECTION("Test Remove Operation")
	{
		crdt::state::ORSetSB<uint32_t> handler(0); //Represents Server 1
		crdt::state::ORSetMetadata<uint32_t> replica1A(1,{1,2,3});
		crdt::state::ORSetMetadata<uint32_t> replica1B(2,{2,6,4});
		crdt::state::ORSetMetadata<uint32_t> replica1C(3,{7,8,9,10});
		handler.addExternalReplica({replica1A,replica1B,replica1C});

		std::set<uint32_t> test = {1,2,3,4,6,7,8,9,10};
		REQUIRE(handler.queryORSet() == test);

		replica1B.remove(2);
		std::vector<uint32_t> removed_itemsB = {2}; 
		handler.addExternalReplica({replica1A,replica1B,replica1C},{{replica1B.queryId(),removed_itemsB}});

		test = {4,6};
		REQUIRE(handler.queryORSetwithID(2) == test);
		test = {1,2,3,4,6,7,8,9,10};
		REQUIRE(handler.queryORSet() == test);

		replica1B.remove(4);
		removed_itemsB = {4}; 
		handler.addExternalReplica({replica1A,replica1B,replica1C},{{replica1B.queryId(),removed_itemsB}});

		test = {6};
		REQUIRE(handler.queryORSetwithID(2) == test);
		test = {1,2,3,6,7,8,9,10};
		REQUIRE(handler.queryORSet() == test);

		replica1C.remove(7);
		replica1C.remove(8);
		replica1C.remove(9);
		std::vector<uint32_t> removed_itemsC = {7,8,9}; 
		handler.addExternalReplica({replica1A,replica1B,replica1C}, {{replica1C.queryId(),removed_itemsC}});
		test = {10};
		REQUIRE(handler.queryORSetwithID(3) == test); 
		test = {1,2,3,6,10};
		REQUIRE(handler.queryORSet() == test);
	}

	SECTION("Test Conflict on localServer")
	{
		crdt::state::ORSetSB<uint32_t> handler1(1); //Represents Server 1
		crdt::state::ORSetMetadata<uint32_t> replica1A(1,2);
		crdt::state::ORSetMetadata<uint32_t> replica1B(1,4);
		crdt::state::ORSetMetadata<uint32_t> replica1C(1,6);
		handler1.addExternalReplica({replica1A,replica1B,replica1C});
		std::set<uint32_t> val = {2,4,6};
		REQUIRE(handler1.queryORSetwithID(1) == val);
		REQUIRE(handler1.queryORSet() == val);

		crdt::state::ORSetSB<uint32_t> handler2(2); //Represents Server 1
		crdt::state::ORSetMetadata<uint32_t> replica2A(3,{1,2});
		crdt::state::ORSetMetadata<uint32_t> replica2B(3,{3,4});
		crdt::state::ORSetMetadata<uint32_t> replica2C(3,{5,6});
		crdt::state::ORSetMetadata<uint32_t> replica2D(4,{1,2,3,4,5,6});
		handler2.addExternalReplica({replica2A,replica2B,replica2C});
		val = {1,2,3,4,5,6};
		REQUIRE(handler2.queryORSetwithID(3) == val);
		REQUIRE(handler2.queryORSet() == val);
		replica2A.remove(2);
		replica2B.remove(4);
		std::vector<uint32_t> removed_items2 = {3,4}; 
		 
		handler2.addExternalReplica({replica2A,replica2B,replica2C,replica2D}, {{replica2A.queryId(),removed_items2}});
		val = {1,2,5,6};
		//REQUIRE(handler2.queryORSet() == val);
		REQUIRE(handler2.queryORSetwithID(3) == val); 
		val = {1,2,3,4,5,6};
		REQUIRE(handler2.queryORSetwithID(4) == val); 
	}

	SECTION("Test Conflict on Multiple Servers")
	{
		crdt::state::ORSetSB<uint32_t> handler1(1); //Represents Server 1
		crdt::state::ORSetSB<uint32_t> handler2(2); //Represents Server 1.2
		crdt::state::ORSetSB<uint32_t> handler3(3); //Represents Server 1.3
		crdt::state::ORSetMetadata<uint32_t> replicaA(4,{3,6,9});
		crdt::state::ORSetMetadata<uint32_t> replicaB(4,{2,4,6});
		crdt::state::ORSetMetadata<uint32_t> replicaC(4,{1,2,3});
		crdt::state::ORSetMetadata<uint32_t> replicaD(5,{1,2,3,6,9});
		crdt::state::ORSetMetadata<uint32_t> replicaE(6,{2,3,6,9,10});

		handler1.addExternalReplica({replicaA,replicaD});
		handler2.addExternalReplica({replicaB,replicaE});
		handler3.addExternalReplica({replicaC});
		REQUIRE(handler1.queryORSetwithID(4) != handler2.queryORSetwithID(4));
		REQUIRE(handler1.queryORSetwithID(4) != handler3.queryORSetwithID(4));
		REQUIRE(handler2.queryORSetwithID(4) != handler3.queryORSetwithID(4));
		handler1.updateLocalExternalPayload({handler1,handler2,handler3}); // merge
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryORSetwithID(4) == handler2.queryORSetwithID(4));
		REQUIRE(handler1.queryORSetwithID(4) == handler3.queryORSetwithID(4));
		REQUIRE(handler2.queryORSetwithID(4) == handler3.queryORSetwithID(4));
		std::set<uint32_t> test = {1,2,3,4,6,9};
		REQUIRE(handler1.queryORSetwithID(4) == handler2.queryORSetwithID(4));
		test = {1,2,3,4,6,9,10};
		REQUIRE(handler1.queryORSet() == test);
		REQUIRE(handler2.queryORSet() == test);
		REQUIRE(handler3.queryORSet() == test);
		test = {2,3,6,9,10};
		REQUIRE(handler1.queryORSetwithID(6) == test);
		REQUIRE(handler2.queryORSetwithID(6) == test);
		REQUIRE(handler3.queryORSetwithID(6) == test);

		replicaB.remove(2);
		replicaE.remove(6);
		replicaE.remove(9);
		std::vector<uint32_t> removed_itemsB = {2}; 
		std::vector<uint32_t> removed_itemsE = {6,9}; 		

		handler1.addExternalReplica({replicaA,replicaD});
		handler2.addExternalReplica({replicaB,replicaE},{{replicaB.queryId(),removed_itemsB}, {replicaE.queryId(),removed_itemsE}});
		handler3.addExternalReplica({replicaC});
		handler1.updateLocalExternalPayload({handler1,handler2,handler3},{{replicaB.queryId(),removed_itemsB}, {replicaE.queryId(),removed_itemsE}}); // merge
		handler2.updateLocalExternalPayload({handler1,handler2,handler3},{{replicaB.queryId(),removed_itemsB}, {replicaE.queryId(),removed_itemsE}});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3},{{replicaB.queryId(),removed_itemsB}, {replicaE.queryId(),removed_itemsE}});
		REQUIRE(handler1.queryORSetwithID(4) == handler2.queryORSetwithID(4));
		REQUIRE(handler1.queryORSetwithID(4) == handler3.queryORSetwithID(4));
		REQUIRE(handler2.queryORSetwithID(4) == handler3.queryORSetwithID(4));
		test = {1,3,4,6,9}; 
		REQUIRE(handler1.queryORSetwithID(4) == test);
		test = {2,3,10};
		REQUIRE(handler1.queryORSetwithID(6) == test);
		REQUIRE(handler2.queryORSetwithID(6) == test);
		REQUIRE(handler3.queryORSetwithID(6) == test);

		replicaE.insert(6);
		handler3.addExternalReplica({replicaE});
		handler1.updateLocalExternalPayload({handler1,handler2,handler3}); // merge
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});

		test = {2,3,6,10};
		REQUIRE(handler1.queryORSetwithID(6) == test);
		REQUIRE(handler2.queryORSetwithID(6) == test);
		REQUIRE(handler3.queryORSetwithID(6) == test);
	}
}
