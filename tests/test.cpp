#include <iostream>
#include <vector>
#include <set>
#include <chrono>
#include <iostream>
#include <fstream>
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>

#include "../src/state_based/GSetSB.hpp"
#include "../src/state_based/TwoPSetSB.hpp"
#include "../src/state_based/ORSetSB.hpp"
#include "../src/state_based/VectorSB.hpp"
#include "../src/operation_based/CounterOB.hpp"
#include "../src/state_based/GCounterSB.hpp"
#include "../src/state_based/PNCounterSB.hpp"
#include "../src/state_based/GMapSB.hpp"
#include "../src/state_based/PriorityQueueSB.hpp"
#include "../src/state_based/MultiSetSB.hpp"
#include "../src/state_based/LWWMultiSetSB.hpp"
#include "../src/state_based/TwoPTwoPGraphSB.hpp"
#include "../src/operation_based/StringOB.hpp"


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

	SECTION("Test serialize function")
	{
		crdt::state::GSetMetadata<uint32_t> replica1A(1,3);
		crdt::state::GSetMetadata<uint32_t> replica1B(2,6);
		crdt::state::GSetMetadata<uint32_t> replica1C(3,9);

		REQUIRE(replica1A.serialize() == "{\"id\":1,\"payload\":[3]}");
		REQUIRE(replica1B.serialize() == "{\"id\":2,\"payload\":[6]}");
		REQUIRE(replica1C.serialize() == "{\"id\":3,\"payload\":[9]}");
	}

	SECTION("Test deserialize function")
	{
		crdt::state::GSetMetadata<uint32_t> replica1A(1,3);
		crdt::state::GSetMetadata<uint32_t> replica1B(2,6);
		crdt::state::GSetMetadata<uint32_t> replica1C(3,9);

		REQUIRE(replica1A.serialize() == "{\"id\":1,\"payload\":[3]}");
		REQUIRE(replica1B.serialize() == "{\"id\":2,\"payload\":[6]}");
		REQUIRE(replica1C.serialize() == "{\"id\":3,\"payload\":[9]}");

		crdt::state::GSetMetadata<uint32_t> replica2A;
		crdt::state::GSetMetadata<uint32_t> replica2B;
		crdt::state::GSetMetadata<uint32_t> replica2C;

		replica2A.deserialize(replica1A.serialize());
		replica2B.deserialize(replica1B.serialize());
		replica2C.deserialize(replica1C.serialize());

		REQUIRE(replica2A.serialize() == "{\"id\":1,\"payload\":[3]}");
		REQUIRE(replica2B.serialize() == "{\"id\":2,\"payload\":[6]}");
		REQUIRE(replica2C.serialize() == "{\"id\":3,\"payload\":[9]}");
	}
#ifdef LOCAL_TESTING
	SECTION("Test serialize function saving to a file")
	{
		crdt::state::GSetMetadata<uint32_t> replica1A(1,3);
		crdt::state::GSetMetadata<uint32_t> replica1B(2,6);
		crdt::state::GSetMetadata<uint32_t> replica1C(3,9);

		replica1A.serializeFile("../../tests/temp_data/gset1A.json");
		replica1B.serializeFile("../../tests/temp_data/gset1B.json");
		replica1C.serializeFile("../../tests/temp_data/gset1C.json");

        std::string replica1AString;
        std::ifstream replica1Ai("../../tests/temp_data/gset1A.json");
        replica1Ai >> replica1AString;

        std::string replica1BString;
        std::ifstream replica1Bi("../../tests/temp_data/gset1B.json");
        replica1Bi >> replica1BString;

        std::string replica1CString;
        std::ifstream replica1Ci("../../tests/temp_data/gset1C.json");
        replica1Ci >> replica1CString;

        REQUIRE(replica1A.serialize() == replica1AString);
		REQUIRE(replica1B.serialize() == replica1BString);
		REQUIRE(replica1C.serialize() == replica1CString);
	}
#endif
#ifdef LOCAL_TESTING
	SECTION("Test deserialize function reading from a file")
	{
		crdt::state::GSetMetadata<uint32_t> replica1A(1,3);
		crdt::state::GSetMetadata<uint32_t> replica1B(2,6);
		crdt::state::GSetMetadata<uint32_t> replica1C(3,9);

		replica1A.serializeFile("../../tests/temp_data/gset1A.json");
		replica1B.serializeFile("../../tests/temp_data/gset1B.json");
		replica1C.serializeFile("../../tests/temp_data/gset1C.json");

		crdt::state::GSetMetadata<uint32_t> replica2A;
		crdt::state::GSetMetadata<uint32_t> replica2B;
		crdt::state::GSetMetadata<uint32_t> replica2C;

		replica2A.deserializeFile("../../tests/temp_data/gset1A.json");
		replica2B.deserializeFile("../../tests/temp_data/gset1B.json");
		replica2C.deserializeFile("../../tests/temp_data/gset1C.json");

		REQUIRE(replica1A.serialize() == replica2A.serialize());
		REQUIRE(replica1B.serialize() == replica2B.serialize());
		REQUIRE(replica1C.serialize() == replica2C.serialize());
	}
#endif
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

	SECTION("Test serialize function")
	{
		crdt::state::TwoPSetMetadata<uint32_t> replicaA(4,{3,6,9});
		crdt::state::TwoPSetMetadata<uint32_t> replicaB(4,{2,4,6});
		crdt::state::TwoPSetMetadata<uint32_t> replicaC(4,{1,2,3});

		REQUIRE(replicaA.serialize() == "{\"id\":4,\"payload\":[3,6,9],\"tombstome\":[]}");
		REQUIRE(replicaB.serialize() == "{\"id\":4,\"payload\":[2,4,6],\"tombstome\":[]}");
		REQUIRE(replicaC.serialize() == "{\"id\":4,\"payload\":[1,2,3],\"tombstome\":[]}");
	}

	SECTION("Test deserialize function")
	{
		crdt::state::TwoPSetMetadata<uint32_t> replica1A(4,{3,6,9});
		crdt::state::TwoPSetMetadata<uint32_t> replica1B(4,{2,4,6});
		crdt::state::TwoPSetMetadata<uint32_t> replica1C(4,{1,2,3});

		REQUIRE(replica1A.serialize() == "{\"id\":4,\"payload\":[3,6,9],\"tombstome\":[]}");
		REQUIRE(replica1B.serialize() == "{\"id\":4,\"payload\":[2,4,6],\"tombstome\":[]}");
		REQUIRE(replica1C.serialize() == "{\"id\":4,\"payload\":[1,2,3],\"tombstome\":[]}");

		crdt::state::TwoPSetMetadata<uint32_t> replica2A;
		crdt::state::TwoPSetMetadata<uint32_t> replica2B;
		crdt::state::TwoPSetMetadata<uint32_t> replica2C;

		replica2A.deserialize(replica1A.serialize());
		replica2B.deserialize(replica1B.serialize());
		replica2C.deserialize(replica1C.serialize());

		REQUIRE(replica2A.serialize() == "{\"id\":4,\"payload\":[3,6,9],\"tombstome\":[]}");
		REQUIRE(replica2B.serialize() == "{\"id\":4,\"payload\":[2,4,6],\"tombstome\":[]}");
		REQUIRE(replica2C.serialize() == "{\"id\":4,\"payload\":[1,2,3],\"tombstome\":[]}");
	}
#ifdef LOCAL_TESTING
	SECTION("Test serialize function saving to a file")
	{
		crdt::state::TwoPSetMetadata<uint32_t> replica1A(4,{3,6,9});
		crdt::state::TwoPSetMetadata<uint32_t> replica1B(4,{2,4,6});
		crdt::state::TwoPSetMetadata<uint32_t> replica1C(4,{1,2,3});

		replica1A.serializeFile("../../tests/temp_data/twopset1A.json");
		replica1B.serializeFile("../../tests/temp_data/twopset1B.json");
		replica1C.serializeFile("../../tests/temp_data/twopset1C.json");

        std::string replica1AString;
        std::ifstream replica1Ai("../../tests/temp_data/twopset1A.json");
        replica1Ai >> replica1AString;

        std::string replica1BString;
        std::ifstream replica1Bi("../../tests/temp_data/twopset1B.json");
        replica1Bi >> replica1BString;

        std::string replica1CString;
        std::ifstream replica1Ci("../../tests/temp_data/twopset1C.json");
        replica1Ci >> replica1CString;

        REQUIRE(!(replica1AString == "{\"id\":4,\"payload\":[3,6,9],\"tombstome\":[]}"));
		REQUIRE(!(replica1BString == "{\"id\":4,\"payload\":[2,4,6],\"tombstome\":[]}"));
		REQUIRE(!(replica1CString == "{\"id\":4,\"payload\":[1,2,3],\"tombstome\":[]}"));
	}
#endif
#ifdef LOCAL_TESTING
	SECTION("Test deserialize function reading from a file")
	{
		crdt::state::TwoPSetMetadata<uint32_t> replica1A(4,{3,6,9});
		crdt::state::TwoPSetMetadata<uint32_t> replica1B(4,{2,4,6});
		crdt::state::TwoPSetMetadata<uint32_t> replica1C(4,{1,2,3});

		replica1A.serializeFile("../../tests/temp_data/twopset1A.json");
		replica1B.serializeFile("../../tests/temp_data/twopset1B.json");
		replica1C.serializeFile("../../tests/temp_data/twopset1C.json");

		crdt::state::TwoPSetMetadata<uint32_t> replica2A;
		crdt::state::TwoPSetMetadata<uint32_t> replica2B;
		crdt::state::TwoPSetMetadata<uint32_t> replica2C;

		replica2A.deserializeFile("../../tests/temp_data/twopset1A.json");
		replica2B.deserializeFile("../../tests/temp_data/twopset1B.json");
		replica2C.deserializeFile("../../tests/temp_data/twopset1C.json");

		REQUIRE(replica1A.serialize() == replica2A.serialize());
		REQUIRE(replica1B.serialize() == replica2B.serialize());
		REQUIRE(replica1C.serialize() == replica2C.serialize());
	}
#endif
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
	
	SECTION("Test serialize function")
	{
		crdt::state::VectorMetadata<uint32_t> replica1A(4,{3,6,9});
		crdt::state::VectorMetadata<uint32_t> replica1B(4,{2,4,6});
		crdt::state::VectorMetadata<uint32_t> replica1C(4,{1,2,3,6,6});

		REQUIRE(replica1A.serialize() == "{\"id\":4,\"payload\":[3,6,9]}");
		REQUIRE(replica1B.serialize() == "{\"id\":4,\"payload\":[2,4,6]}");
		REQUIRE(replica1C.serialize() == "{\"id\":4,\"payload\":[1,2,3,6,6]}");
	}

	SECTION("Test deserialize function")
	{
		crdt::state::VectorMetadata<uint32_t> replica1A(4,{3,6,9});
		crdt::state::VectorMetadata<uint32_t> replica1B(4,{2,4,6});
		crdt::state::VectorMetadata<uint32_t> replica1C(4,{1,2,3,6,6});

		REQUIRE(replica1A.serialize() == "{\"id\":4,\"payload\":[3,6,9]}");
		REQUIRE(replica1B.serialize() == "{\"id\":4,\"payload\":[2,4,6]}");
		REQUIRE(replica1C.serialize() == "{\"id\":4,\"payload\":[1,2,3,6,6]}");

		crdt::state::VectorMetadata<uint32_t> replica2A;
		crdt::state::VectorMetadata<uint32_t> replica2B;
		crdt::state::VectorMetadata<uint32_t> replica2C;

		replica2A.deserialize(replica1A.serialize());
		replica2B.deserialize(replica1B.serialize());
		replica2C.deserialize(replica1C.serialize());

		REQUIRE(replica2A.serialize() == "{\"id\":4,\"payload\":[3,6,9]}");
		REQUIRE(replica2B.serialize() == "{\"id\":4,\"payload\":[2,4,6]}");
		REQUIRE(replica2C.serialize() == "{\"id\":4,\"payload\":[1,2,3,6,6]}");
	}
#ifdef LOCAL_TESTING
	SECTION("Test serialize function saving to a file")
	{
		crdt::state::VectorMetadata<uint32_t> replica1A(4,{3,6,9});
		crdt::state::VectorMetadata<uint32_t> replica1B(4,{2,4,6});
		crdt::state::VectorMetadata<uint32_t> replica1C(4,{1,2,3,6,6});

		replica1A.serializeFile("../../tests/temp_data/vector1A.json");
		replica1B.serializeFile("../../tests/temp_data/vector1B.json");
		replica1C.serializeFile("../../tests/temp_data/vector1C.json");

        std::string replica1AString;
        std::ifstream replica1Ai("../../tests/temp_data/vector1A.json");
        replica1Ai >> replica1AString;

        std::string replica1BString;
        std::ifstream replica1Bi("../../tests/temp_data/vector1B.json");
        replica1Bi >> replica1BString;

        std::string replica1CString;
        std::ifstream replica1Ci("../../tests/temp_data/vector1C.json");
        replica1Ci >> replica1CString;

        REQUIRE(replica1AString == replica1A.serialize());
		REQUIRE(replica1BString == replica1B.serialize());
		REQUIRE(replica1CString == replica1C.serialize());
	}
#endif
#ifdef LOCAL_TESTING
	SECTION("Test deserialize function reading from a file")
	{
		crdt::state::VectorMetadata<uint32_t> replica1A(4,{3,6,9});
		crdt::state::VectorMetadata<uint32_t> replica1B(4,{2,4,6});
		crdt::state::VectorMetadata<uint32_t> replica1C(4,{1,2,3,6,6});

		replica1A.serializeFile("../../tests/temp_data/vector1A.json");
		replica1B.serializeFile("../../tests/temp_data/vector1B.json");
		replica1C.serializeFile("../../tests/temp_data/vector1C.json");

		crdt::state::VectorMetadata<uint32_t> replica2A;
		crdt::state::VectorMetadata<uint32_t> replica2B;
		crdt::state::VectorMetadata<uint32_t> replica2C;

		replica2A.deserializeFile("../../tests/temp_data/vector1A.json");
		replica2B.deserializeFile("../../tests/temp_data/vector1B.json");
		replica2C.deserializeFile("../../tests/temp_data/vector1C.json");

		REQUIRE(replica1A.serialize() == replica2A.serialize());
		REQUIRE(replica1B.serialize() == replica2B.serialize());
		REQUIRE(replica1C.serialize() == replica2C.serialize());
	}
#endif
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

	SECTION("Test serialize function")
	{
		crdt::state::ORSetMetadata<uint32_t> replica1A(4,{1,2,3});
		crdt::state::ORSetMetadata<uint32_t> replica1B(5,{1,2,3,6,9});
		crdt::state::ORSetMetadata<uint32_t> replica1C(6,{2,3,6,9,10});

		REQUIRE(replica1A.serialize() == "{\"id\":4,\"payload\":[1,2,3]}");
		REQUIRE(replica1B.serialize() == "{\"id\":5,\"payload\":[1,2,3,6,9]}");
		REQUIRE(replica1C.serialize() == "{\"id\":6,\"payload\":[2,3,6,9,10]}");
	}

	SECTION("Test deserialize function")
	{
		crdt::state::ORSetMetadata<uint32_t> replica1A(4,{1,2,3});
		crdt::state::ORSetMetadata<uint32_t> replica1B(5,{1,2,3,6,9});
		crdt::state::ORSetMetadata<uint32_t> replica1C(6,{2,3,6,9,10});

		REQUIRE(replica1A.serialize() == "{\"id\":4,\"payload\":[1,2,3]}");
		REQUIRE(replica1B.serialize() == "{\"id\":5,\"payload\":[1,2,3,6,9]}");
		REQUIRE(replica1C.serialize() == "{\"id\":6,\"payload\":[2,3,6,9,10]}");

		crdt::state::ORSetMetadata<uint32_t> replica2A;
		crdt::state::ORSetMetadata<uint32_t> replica2B;
		crdt::state::ORSetMetadata<uint32_t> replica2C;

		replica2A.deserialize(replica1A.serialize());
		replica2B.deserialize(replica1B.serialize());
		replica2C.deserialize(replica1C.serialize());

		REQUIRE(replica2A.serialize() == "{\"id\":4,\"payload\":[1,2,3]}");
		REQUIRE(replica2B.serialize() == "{\"id\":5,\"payload\":[1,2,3,6,9]}");
		REQUIRE(replica2C.serialize() == "{\"id\":6,\"payload\":[2,3,6,9,10]}");

	}
#ifdef LOCAL_TESTING
	SECTION("Test serialize function saving to a file")
	{
		crdt::state::ORSetMetadata<uint32_t> replica1A(4,{1,2,3});
		crdt::state::ORSetMetadata<uint32_t> replica1B(5,{1,2,3,6,9});
		crdt::state::ORSetMetadata<uint32_t> replica1C(6,{2,3,6,9,10});

		replica1A.serializeFile("../../tests/temp_data/orset1A.json");
		replica1B.serializeFile("../../tests/temp_data/orset1B.json");
		replica1C.serializeFile("../../tests/temp_data/orset1C.json");

        std::string replica1AString;
        std::ifstream replica1Ai("../../tests/temp_data/orset1A.json");
        replica1Ai >> replica1AString;

        std::string replica1BString;
        std::ifstream replica1Bi("../../tests/temp_data/orset1B.json");
        replica1Bi >> replica1BString;

        std::string replica1CString;
        std::ifstream replica1Ci("../../tests/temp_data/orset1C.json");
        replica1Ci >> replica1CString;

        REQUIRE(replica1AString == replica1A.serialize());
		REQUIRE(replica1BString == replica1B.serialize());
		REQUIRE(replica1CString == replica1C.serialize());
	}
#endif
#ifdef LOCAL_TESTING
	SECTION("Test deserialize function reading from a file")
	{
		crdt::state::ORSetMetadata<uint32_t> replica1A(4,{1,2,3});
		crdt::state::ORSetMetadata<uint32_t> replica1B(5,{1,2,3,6,9});
		crdt::state::ORSetMetadata<uint32_t> replica1C(6,{2,3,6,9,10});

		replica1A.serializeFile("../../tests/temp_data/orset1A.json");
		replica1B.serializeFile("../../tests/temp_data/orset1B.json");
		replica1C.serializeFile("../../tests/temp_data/orset1C.json");

		crdt::state::ORSetMetadata<uint32_t> replica2A;
		crdt::state::ORSetMetadata<uint32_t> replica2B;
		crdt::state::ORSetMetadata<uint32_t> replica2C;

		replica2A.deserializeFile("../../tests/temp_data/orset1A.json");
		replica2B.deserializeFile("../../tests/temp_data/orset1B.json");
		replica2C.deserializeFile("../../tests/temp_data/orset1C.json");

		REQUIRE(replica1A.serialize() == replica2A.serialize());
		REQUIRE(replica1B.serialize() == replica2B.serialize());
		REQUIRE(replica1C.serialize() == replica2C.serialize());
	}
#endif
}

TEST_CASE("Test GCounterSB", "[classic]")
{
	SECTION("Test Insert Operation")
	{
		crdt::state::GCounterSB<uint32_t> handler(1); //Represents Server 1
		/* Belongs to Server 1 */
		crdt::state::GCounterMetadata<uint32_t> replica1A(1,6);
		crdt::state::GCounterMetadata<uint32_t> replica1B(2,7);
		crdt::state::GCounterMetadata<uint32_t> replica1C(3,8);
		handler.addExternalReplica({replica1A,replica1B,replica1C});
		crdt::state::GCounterSB<uint32_t> handler2(2); //Represents Server 2
		/* Belongs to Server 2 */
		crdt::state::GCounterMetadata<uint32_t> replica2A(1,6);
		crdt::state::GCounterMetadata<uint32_t> replica2B(2,3);
		crdt::state::GCounterMetadata<uint32_t> replica2C(3,5);
		handler2.addExternalReplica({replica2A,replica2B,replica2C});
		REQUIRE(handler.queryPayload() == 21);
		REQUIRE(handler2.queryPayload() == 14);
	}
	SECTION("Test Conflict on LocalServer")
	{
		crdt::state::GCounterSB<uint32_t> handler(1); //Represents Server 1
		/* Create several replicas all with key = 1 */
		crdt::state::GCounterMetadata<uint32_t> replica1A(1,6);
		crdt::state::GCounterMetadata<uint32_t> replica1B(1,15);
		crdt::state::GCounterMetadata<uint32_t> replica1C(1,8);
		crdt::state::GCounterMetadata<uint32_t> replica1D(1,2);
		handler.addExternalReplica({replica1A,replica1B,replica1C,replica1D});
		REQUIRE(handler.queryPayload()== 15); 
		replica1A.updatePayload(25);
		handler.addExternalReplica({replica1A,replica1B,replica1C,replica1D});
		REQUIRE(handler.queryPayload()== 31);
		replica1C.updatePayload(100);
		handler.addExternalReplica({replica1A,replica1B,replica1C,replica1D});
		REQUIRE(handler.queryPayload()== 108);
	}

	SECTION("Test Merge Operation")
	{
		crdt::state::GCounterSB<uint32_t> handler(1); //Represents Server 1
		/* Belongs to Server 1 */
		crdt::state::GCounterMetadata<uint32_t> replica1A(2,6);
		replica1A.updatePayload(7);
		replica1A.updatePayload(8);
		
		handler.addExternalReplica({replica1A});
		REQUIRE(handler.queryPayload() == 21);

		crdt::state::GCounterSB<uint32_t> handler2(3); //Represents Server 2
		/* Belongs to Server 2 */
		crdt::state::GCounterMetadata<uint32_t> replica2A(4,6);
		replica2A.updatePayload(3);
		replica2A.updatePayload(5);

		handler2.addExternalReplica({replica2A});
		REQUIRE(handler2.queryPayload() == 14);

		crdt::state::GCounterSB<uint32_t> handler3(5); //Represents Server 3
		crdt::state::GCounterSB<uint32_t> handler4(6); //Represents Server 4
		crdt::state::GCounterMetadata<uint32_t> replica4A(7,10);
		crdt::state::GCounterMetadata<uint32_t> replica4B(8,10);
		replica4B.updatePayload(300);
		crdt::state::GCounterMetadata<uint32_t> replica4C(9,15);
		handler4.addExternalReplica({replica4A,replica4B,replica4C});
		REQUIRE(handler4.queryPayload() == 335);
		handler.updateLocalExternalPayload({handler,handler2,handler3,handler4});
		handler2.updateLocalExternalPayload({handler,handler2,handler3,handler4});
		handler3.updateLocalExternalPayload({handler,handler2,handler3,handler4});
		handler4.updateLocalExternalPayload({handler,handler2,handler3,handler4});
		REQUIRE(handler.queryPayload() == 370);
		REQUIRE(handler.queryPayload() == handler2.queryPayload());
		REQUIRE(handler3.queryPayload() == handler4.queryPayload());
		REQUIRE(handler.queryPayload() == handler4.queryPayload());

		// replica on server A decides to update
		replica1A.updatePayload(7); // value == 28
		handler.addExternalReplica({replica1A});
		//30 s have passed and now we poll from all servers
		handler.updateLocalExternalPayload({handler,handler2,handler3,handler4});
		handler2.updateLocalExternalPayload({handler,handler2,handler3,handler4});
		handler3.updateLocalExternalPayload({handler,handler2,handler3,handler4});
		handler4.updateLocalExternalPayload({handler,handler2,handler3,handler4});
		REQUIRE(handler.queryPayload() == handler2.queryPayload());
		REQUIRE(handler3.queryPayload() == handler4.queryPayload());
		REQUIRE(handler.queryPayload() == handler4.queryPayload());
		replica2A.updatePayload(3);
		
		handler3.addExternalReplica({replica2A}); //Server 3 will now cause conflict with server 2
		//30 s have passed and now we poll from all servers
		handler.updateLocalExternalPayload({handler,handler2,handler3,handler4});
		handler2.updateLocalExternalPayload({handler,handler2,handler3,handler4});
		handler3.updateLocalExternalPayload({handler,handler2,handler3,handler4});
		handler4.updateLocalExternalPayload({handler,handler2,handler3,handler4});
		REQUIRE(handler2.queryPayload() == 380);
		REQUIRE(handler.queryPayloadwithID(2) == handler2.queryPayloadwithID(2));
		REQUIRE(handler.queryPayload() == handler2.queryPayload());
		REQUIRE(handler3.queryPayload() == handler4.queryPayload());
		REQUIRE(handler.queryPayload() == handler4.queryPayload());
	}

	SECTION("Test serialize function")
	{
		crdt::state::GCounterMetadata<uint32_t> replica1A(1,6);
		crdt::state::GCounterMetadata<uint32_t> replica1B(1,15);
		crdt::state::GCounterMetadata<uint32_t> replica1C(1,8);

		REQUIRE(replica1A.serialize() == "{\"id\":1,\"payload\":6}");
		REQUIRE(replica1B.serialize() == "{\"id\":1,\"payload\":15}");
		REQUIRE(replica1C.serialize() == "{\"id\":1,\"payload\":8}");
	}

	SECTION("Test deserialize function")
	{
		crdt::state::GCounterMetadata<uint32_t> replica1A(1,6);
		crdt::state::GCounterMetadata<uint32_t> replica1B(1,15);
		crdt::state::GCounterMetadata<uint32_t> replica1C(1,8);

		REQUIRE(replica1A.serialize() == "{\"id\":1,\"payload\":6}");
		REQUIRE(replica1B.serialize() == "{\"id\":1,\"payload\":15}");
		REQUIRE(replica1C.serialize() == "{\"id\":1,\"payload\":8}");

		crdt::state::GCounterMetadata<uint32_t> replica2A;
		crdt::state::GCounterMetadata<uint32_t> replica2B;
		crdt::state::GCounterMetadata<uint32_t> replica2C;

		replica2A.deserialize(replica1A.serialize());
		replica2B.deserialize(replica1B.serialize());
		replica2C.deserialize(replica1C.serialize());

		REQUIRE(replica2A.serialize() == "{\"id\":1,\"payload\":6}");
		REQUIRE(replica2B.serialize() == "{\"id\":1,\"payload\":15}");
		REQUIRE(replica2C.serialize() == "{\"id\":1,\"payload\":8}");
	}
#ifdef LOCAL_TESTING
	SECTION("Test serialize function saving to a file")
	{
		crdt::state::GCounterMetadata<uint32_t> replica1A(1,6);
		crdt::state::GCounterMetadata<uint32_t> replica1B(1,15);
		crdt::state::GCounterMetadata<uint32_t> replica1C(1,8);

		replica1A.serializeFile("../../tests/temp_data/gcounterA.json");
		replica1B.serializeFile("../../tests/temp_data/gcounterB.json");
		replica1C.serializeFile("../../tests/temp_data/gcounterC.json");

        std::string replica1AString;
        std::ifstream replica1Ai("../../tests/temp_data/gcounterA.json");
        replica1Ai >> replica1AString;

        std::string replica1BString;
        std::ifstream replica1Bi("../../tests/temp_data/gcounterB.json");
        replica1Bi >> replica1BString;

        std::string replica1CString;
        std::ifstream replica1Ci("../../tests/temp_data/gcounterC.json");
        replica1Ci >> replica1CString;

        REQUIRE(replica1AString == replica1A.serialize());
		REQUIRE(replica1BString == replica1B.serialize());
		REQUIRE(replica1CString == replica1C.serialize());
	}
#endif
#ifdef LOCAL_TESTING
	SECTION("Test deserialize function reading from a file")
	{
		crdt::state::GCounterMetadata<uint32_t> replica1A(1,6);
		crdt::state::GCounterMetadata<uint32_t> replica1B(1,15);
		crdt::state::GCounterMetadata<uint32_t> replica1C(1,8);

		replica1A.serializeFile("../../tests/temp_data/gcounter1A.json");
		replica1B.serializeFile("../../tests/temp_data/gcounter1B.json");
		replica1C.serializeFile("../../tests/temp_data/gcounter1C.json");

		crdt::state::GCounterMetadata<uint32_t> replica2A;
		crdt::state::GCounterMetadata<uint32_t> replica2B;
		crdt::state::GCounterMetadata<uint32_t> replica2C;

		replica2A.deserializeFile("../../tests/temp_data/gcounter1A.json");
		replica2B.deserializeFile("../../tests/temp_data/gcounter1B.json");
		replica2C.deserializeFile("../../tests/temp_data/gcounter1C.json");

		REQUIRE(replica1A.serialize() == replica2A.serialize());
		REQUIRE(replica1B.serialize() == replica2B.serialize());
		REQUIRE(replica1C.serialize() == replica2C.serialize());
	}
#endif
}

TEST_CASE("Test PNCounterSB", "[classic]")
{
	SECTION("Test Insert Operation")
	{
			crdt::state::PNCounterSB<uint32_t> handler(1); //Represents Server 1
			/* Belongs to Server 1 */
			crdt::state::PNCounterMetadata<uint32_t> replica1A(2,6);
			crdt::state::PNCounterMetadata<uint32_t> replica1B(3,7);
			crdt::state::PNCounterMetadata<uint32_t> replica1C(4,8);
			handler.addExternalReplica({replica1A,replica1B,replica1C});

			crdt::state::PNCounterSB<uint32_t> handler2(5); //Represents Server 2
			/* Belongs to Server 2 */
			crdt::state::PNCounterMetadata<uint32_t> replica2A(6,6);
			crdt::state::PNCounterMetadata<uint32_t> replica2B(7,3);
			crdt::state::PNCounterMetadata<uint32_t> replica2C(8,5);
			handler2.addExternalReplica({replica2A,replica2B,replica2C});

			REQUIRE(handler.queryPayload() == 21);
			REQUIRE(handler2.queryPayload() == 14);
	}
	SECTION("Test Add/Subtract Operation")
	{
		crdt::state::PNCounterSB<uint32_t> handler(1); //Represents Server 1
		crdt::state::PNCounterMetadata<uint32_t> replica1A(2,6);
		handler.addExternalReplica({replica1A});
		replica1A.increasePayload(7);
		replica1A.increasePayload(16);
		handler.addExternalReplica({replica1A});
		REQUIRE(handler.queryPayload() == 29);
		replica1A.increasePayload(2);
		replica1A.decreasePayload(8);
		handler.addExternalReplica({replica1A});
		REQUIRE(handler.queryPayload() == 23);
	}
	SECTION("Test Handling Same Keys on 1 Server")
	{
		crdt::state::PNCounterSB<uint32_t> handler(1); //Represents Server 1
		/* Create several replicas all with key = 1 */
		crdt::state::PNCounterMetadata<uint32_t> replica1A(1,6);
		crdt::state::PNCounterMetadata<uint32_t> replica1B(1,15);
		crdt::state::PNCounterMetadata<uint32_t> replica1C(1,16);
		crdt::state::PNCounterMetadata<uint32_t> replica1D(1,2);
		handler.addExternalReplica({replica1A,replica1B,replica1C,replica1D});
		replica1A.increasePayload(25);
		handler.addExternalReplica({replica1A,replica1B,replica1C,replica1D});
		REQUIRE(handler.queryPayload()== 31);
		replica1A.decreasePayload(30);
		handler.addExternalReplica({replica1A,replica1B,replica1C,replica1D});
		REQUIRE(handler.queryPayload()== 1);
	}
	SECTION("Test Conflict on localServer")
	{
		crdt::state::PNCounterSB<uint32_t> handler(1); //Represents Server 1
		crdt::state::PNCounterMetadata<uint32_t> replica1A(2,0);
		crdt::state::PNCounterMetadata<uint32_t> replica1B(3,0);
		handler.addExternalReplica({replica1A,replica1B});
		REQUIRE(handler.queryPayload() == 0);
		replica1A.increasePayload(6);
		replica1A.increasePayload(7);
		replica1A.increasePayload(8);
		replica1B.increasePayload(6);
		replica1B.increasePayload(3);
		replica1B.increasePayload(5);
		handler.addExternalReplica({replica1A,replica1B}); // Fetch the local replicas that got updated
		REQUIRE(handler.queryPayload() == 35);
		replica1A.decreasePayload(6);
		handler.addExternalReplica({replica1A}); 
		REQUIRE(handler.queryPayload() == 29);
		replica1B.decreasePayload(5);
		handler.addExternalReplica({replica1B}); 
		REQUIRE(handler.queryPayload() == 24);
		crdt::state::PNCounterMetadata<uint32_t> replica1C(2,200); //Conflict with replica1A
		replica1C.decreasePayload(4);
		replica1C.decreasePayload(20);
		handler.addExternalReplica({replica1A,replica1B,replica1C}); 
		REQUIRE(handler.queryPayload() == 185);	
	}
	SECTION("Test Conflict on Multiple Server")
	{
		crdt::state::PNCounterSB<uint32_t> handler1(1); //Represents Server 1
		crdt::state::PNCounterSB<uint32_t> handler2(2); //Represents Server 2
		crdt::state::PNCounterSB<uint32_t> handler3(3); //Represents Server 3
		crdt::state::PNCounterMetadata<uint32_t> replica1A(1,10);
		crdt::state::PNCounterMetadata<uint32_t> replica1B(2,10);
		crdt::state::PNCounterMetadata<uint32_t> replica2A(3,10);
		crdt::state::PNCounterMetadata<uint32_t> replica2B(4,10);
		crdt::state::PNCounterMetadata<uint32_t> replica3A(5,10);
		crdt::state::PNCounterMetadata<uint32_t> replica3B(6,10);
		handler1.addExternalReplica({replica1A,replica1B});
		handler2.addExternalReplica({replica2A,replica2B});
		handler3.addExternalReplica({replica3A,replica3B});
		replica1A.decreasePayload(5);
		replica1B.decreasePayload(5);
		handler1.addExternalReplica({replica1A,replica1B});
		replica2A.decreasePayload(5);
		replica2B.decreasePayload(5);
		handler2.addExternalReplica({replica2A,replica2B});
		replica3A.decreasePayload(5);
		replica3B.decreasePayload(5);
		handler3.addExternalReplica({replica3A,replica3B});
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryPayload() == 30);
		REQUIRE(handler1.queryPayload() == handler2.queryPayload());
		REQUIRE(handler2.queryPayload() == handler3.queryPayload());
		replica1A.increasePayload(20);
		handler1.addExternalReplica({replica1A});
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler2,handler1,handler3});
		handler3.updateLocalExternalPayload({handler3,handler1,handler2});
		REQUIRE(handler1.queryPayload() == 50);
		REQUIRE(handler1.queryPayload() == handler2.queryPayload());
		REQUIRE(handler2.queryPayload() == handler3.queryPayload());
		replica1A.increasePayload(50);
		replica1B.increasePayload(50);
		replica3A.increasePayload(50);
		handler1.addExternalReplica({replica3A}); // Server 1 Conflict With Server 3
		handler2.addExternalReplica({replica1B}); // Server 2 Conflict with Server 1
		handler3.addExternalReplica({replica1A}); // Server 3 Conflict with Server 1
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryPayloadwithID(1) == handler2.queryPayloadwithID(1));
		REQUIRE(handler2.queryPayloadwithID(1) == handler3.queryPayloadwithID(1));
		REQUIRE(handler1.queryPayload() == 200);
		REQUIRE(handler1.queryPayload() == handler2.queryPayload());
		REQUIRE(handler2.queryPayload() == handler3.queryPayload());
	}

	SECTION("Test serialize function")
	{
		crdt::state::PNCounterMetadata<uint32_t> replica1A(1,10);
		crdt::state::PNCounterMetadata<uint32_t> replica1B(2,10);
		crdt::state::PNCounterMetadata<uint32_t> replica1C(3,10);

		REQUIRE(replica1A.serialize() == "{\"id\":1,\"negativePayload\":0,\"positivePayload\":10,\"totalPayload\":10}");
		REQUIRE(replica1B.serialize() == "{\"id\":2,\"negativePayload\":0,\"positivePayload\":10,\"totalPayload\":10}");
		REQUIRE(replica1C.serialize() == "{\"id\":3,\"negativePayload\":0,\"positivePayload\":10,\"totalPayload\":10}");
	}

	SECTION("Test deserialize function")
	{
		crdt::state::PNCounterMetadata<uint32_t> replica1A(1,10);
		crdt::state::PNCounterMetadata<uint32_t> replica1B(2,10);
		crdt::state::PNCounterMetadata<uint32_t> replica1C(3,10);

		REQUIRE(replica1A.serialize() == "{\"id\":1,\"negativePayload\":0,\"positivePayload\":10,\"totalPayload\":10}");
		REQUIRE(replica1B.serialize() == "{\"id\":2,\"negativePayload\":0,\"positivePayload\":10,\"totalPayload\":10}");
		REQUIRE(replica1C.serialize() == "{\"id\":3,\"negativePayload\":0,\"positivePayload\":10,\"totalPayload\":10}");

		crdt::state::PNCounterMetadata<uint32_t> replica2A;
		crdt::state::PNCounterMetadata<uint32_t> replica2B;
		crdt::state::PNCounterMetadata<uint32_t> replica2C;

		replica2A.deserialize(replica1A.serialize());
		replica2B.deserialize(replica1B.serialize());
		replica2C.deserialize(replica1C.serialize());

		REQUIRE(replica2A.serialize() == "{\"id\":1,\"negativePayload\":0,\"positivePayload\":10,\"totalPayload\":10}");
		REQUIRE(replica2B.serialize() == "{\"id\":2,\"negativePayload\":0,\"positivePayload\":10,\"totalPayload\":10}");
		REQUIRE(replica2C.serialize() == "{\"id\":3,\"negativePayload\":0,\"positivePayload\":10,\"totalPayload\":10}");
	}
#ifdef LOCAL_TESTING
	SECTION("Test serialize function saving to a file")
	{
		crdt::state::PNCounterMetadata<uint32_t> replica1A(1,10);
		crdt::state::PNCounterMetadata<uint32_t> replica1B(2,10);
		crdt::state::PNCounterMetadata<uint32_t> replica1C(3,10);

		replica1A.serializeFile("../../tests/temp_data/pncounter1A.json");
		replica1B.serializeFile("../../tests/temp_data/pncounter1B.json");
		replica1C.serializeFile("../../tests/temp_data/pncounter1C.json");

        std::string replica1AString;
        std::ifstream replica1Ai("../../tests/temp_data/pncounter1A.json");
        replica1Ai >> replica1AString;

        std::string replica1BString;
        std::ifstream replica1Bi("../../tests/temp_data/pncounter1B.json");
        replica1Bi >> replica1BString;

        std::string replica1CString;
        std::ifstream replica1Ci("../../tests/temp_data/pncounter1C.json");
        replica1Ci >> replica1CString;

        REQUIRE(replica1AString == replica1A.serialize());
		REQUIRE(replica1BString == replica1B.serialize());
		REQUIRE(replica1CString == replica1C.serialize());
	}
#endif
#ifdef LOCAL_TESTING
	SECTION("Test deserialize function reading from a file")
	{
		crdt::state::PNCounterMetadata<uint32_t> replica1A(1,10);
		crdt::state::PNCounterMetadata<uint32_t> replica1B(2,10);
		crdt::state::PNCounterMetadata<uint32_t> replica1C(3,10);

		replica1A.serializeFile("../../tests/temp_data/pncounter1A.json");
		replica1B.serializeFile("../../tests/temp_data/pncounter1B.json");
		replica1C.serializeFile("../../tests/temp_data/pncounter1C.json");

		crdt::state::PNCounterMetadata<uint32_t> replica2A;
		crdt::state::PNCounterMetadata<uint32_t> replica2B;
		crdt::state::PNCounterMetadata<uint32_t> replica2C;

		replica2A.deserializeFile("../../tests/temp_data/pncounter1A.json");
		replica2B.deserializeFile("../../tests/temp_data/pncounter1B.json");
		replica2C.deserializeFile("../../tests/temp_data/pncounter1C.json");

		REQUIRE(replica1A.serialize() == replica2A.serialize());
		REQUIRE(replica1B.serialize() == replica2B.serialize());
		REQUIRE(replica1C.serialize() == replica2C.serialize());
	}
#endif
}

TEST_CASE("Test GMapSB", "[classic]")
{
	SECTION("Test Insert Operation")
	{
			crdt::state::GMapSB<uint32_t, uint32_t> handler(1); //Represents Server 1
			crdt::state::GMapSB<char, uint32_t> handler2(1); //Represents Server 1.2
			crdt::state::GMapSB<uint32_t, std::string> handler3(1); //Represents Server 1.3
			/* Create 5 different hashmaps*/
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1A(1,0,1);
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1B(2,1,5);
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1C(3,2,7);
			crdt::state::GMapMetadata<char, uint32_t> replica1D(4,'a',99);
			crdt::state::GMapMetadata<uint32_t, std::string> replica1E(5,0,"Hello World");

			handler.addExternalReplica({replica1A,replica1B,replica1C});
			REQUIRE(handler.queryPayloadwithID(2,1) == 5);
			replica1B.insert(1,25);
			handler.addExternalReplica({replica1A,replica1B,replica1C});
			REQUIRE(handler.queryPayloadwithID(2,1) == 25);
			replica1C.insert(4,20);
			replica1C.insert(0,52);
			replica1C.insert(2,-500);
			handler.addExternalReplica({replica1A,replica1B,replica1C});
			REQUIRE(handler.queryPayloadwithID(3,4) == 20);
			REQUIRE(handler.queryPayloadwithID(3,0) == 52);
			REQUIRE(handler.queryPayloadwithID(3,2) == -500);
			handler2.addExternalReplica({replica1D});
			handler3.addExternalReplica({replica1E});
			REQUIRE(handler2.queryPayloadwithID(4,'a') == 99);
			REQUIRE(handler3.queryPayloadwithID(5,0) == "Hello World");
	}
	SECTION("Test Conflict on localServer String")
	{
			crdt::state::GMapSB<uint32_t, uint32_t> handler(1); //Represents Server 1
			crdt::state::GMapSB<uint32_t, std::string> handler2(1); //Represents Server 1.2
			crdt::state::GMapSBString<uint32_t, std::string> handler3(1); //Represents Server 1.2
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1A(0,10,0);
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1B(0,10,2);
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1C(0,10,3);
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1D(0,10,3);
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1E(0,10,4);
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1F(0,10,2);
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1G(0,10,1);
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1H(0,10,0);
			crdt::state::GMapMetadata<uint32_t, std::string> replica1I(0,10,"Hello World");
			crdt::state::GMapMetadata<uint32_t, std::string> replica1J(0,10,"Hello World how are ya");
			crdt::state::GMapMetadata<uint32_t, std::string> replica1K(0,10,"Hello World how are");
			handler.addExternalReplica({replica1A,replica1B,replica1C,replica1D,replica1E,replica1F,replica1G,replica1H});
			handler2.addExternalReplica({replica1I,replica1J,replica1K});
			REQUIRE(handler.queryPayloadwithID(0,10) == 4);
			REQUIRE(handler2.queryPayloadwithID(0,10) == "Hello World how are ya");
			crdt::state::GMapMetadata<uint32_t, std::string> replica1L(0,10,"Z");
			handler2.addExternalReplica({replica1I,replica1J,replica1K,replica1L});
			handler3.addExternalReplica({replica1I,replica1J,replica1K,replica1L});
			REQUIRE(handler2.queryPayloadwithID(0,10) == "Z");
			REQUIRE(handler3.queryPayloadwithID(0,10) == "Hello World Z are how ya");
			replica1I.insert(11,"dfojsfsdojfiod");
			replica1J.insert(11,"fgklfgkfd;");
			replica1K.insert(11,"zzzzzzzzzzzz");
			replica1L.insert(11,"#$#%#$%$^$^^");
			handler2.addExternalReplica({replica1I,replica1J,replica1K,replica1L});
			handler3.addExternalReplica({replica1I,replica1J,replica1K,replica1L});
			REQUIRE(handler2.queryPayloadwithID(0,11) == "zzzzzzzzzzzz");
			REQUIRE(handler3.queryPayloadwithID(0,11) == "#$#%#$%$^$^^ dfojsfsdojfiod fgklfgkfd; zzzzzzzzzzzz");
	}
	SECTION("Test Conflict Non-String on Multiple Servers")
	{
			crdt::state::GMapSB<uint32_t, uint32_t> handler(1); //Represents Server 1
			crdt::state::GMapSB<uint32_t, uint32_t> handler2(2); //Represents Server 2
			crdt::state::GMapSB<uint32_t, uint32_t> handler3(3); //Represents Server 3
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1A(0,10,1);
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1B(0,10,2);
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1C(0,10,3);
			replica1A.insert(25,100);
			handler.addExternalReplica({replica1A});
			REQUIRE(handler.queryPayloadwithID(0,25) == 100);
			handler2.addExternalReplica({replica1B});
			handler3.addExternalReplica({replica1C});
			
			REQUIRE(handler2.queryPayloadwithID(0,10) != handler.queryPayloadwithID(0,10));
			REQUIRE(handler3.queryPayloadwithID(0,10) != handler2.queryPayloadwithID(0,10));
			handler.updateLocalExternalPayload({handler,handler2,handler3});
			handler2.updateLocalExternalPayload({handler,handler2,handler3});
			handler3.updateLocalExternalPayload({handler,handler2,handler3});
			REQUIRE(handler2.queryPayloadwithID(0,10) == handler.queryPayloadwithID(0,10));
			REQUIRE(handler3.queryPayloadwithID(0,10) == handler2.queryPayloadwithID(0,10));
			
			REQUIRE(handler.queryPayloadwithID(0,25) == 100);
			REQUIRE(handler2.queryPayloadwithID(0,25) == 100);
			REQUIRE(handler3.queryPayloadwithID(0,25) == 100);
			replica1B.insert(3,200);
			replica1B.insert(75,350);
			handler2.addExternalReplica({replica1B});
			handler.updateLocalExternalPayload({handler,handler2,handler3});
			handler2.updateLocalExternalPayload({handler,handler2,handler3});
			handler3.updateLocalExternalPayload({handler,handler2,handler3});
			REQUIRE(handler.queryPayloadwithID(0,3) == 200);
			REQUIRE(handler3.queryPayloadwithID(0,75) == 350);
			REQUIRE(handler.queryPayloadwithID(0,3) == handler3.queryPayloadwithID(0,3));
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1D(1,0,5000);
			handler3.addExternalReplica({replica1D});
			handler.updateLocalExternalPayload({handler,handler2,handler3});
			handler2.updateLocalExternalPayload({handler,handler2,handler3});
			handler3.updateLocalExternalPayload({handler,handler2,handler3});
			REQUIRE(handler3.queryPayloadwithID(1,0) == 5000);
			REQUIRE(handler2.queryPayloadwithID(1,0) == handler.queryPayloadwithID(1,0));
			REQUIRE(handler3.queryPayloadwithID(1,0) == handler2.queryPayloadwithID(1,0));
			REQUIRE(handler.queryPayload(0) == 5000);
			REQUIRE(handler.queryPayload(0) == handler2.queryPayload(0));
			REQUIRE(handler2.queryPayload(0) == handler3.queryPayload(0));
			std::vector<uint32_t> test1 = {0,3,10,25,75};
			REQUIRE(handler.queryAllKeys() == test1);
			REQUIRE(handler2.queryAllKeys() == test1);
			REQUIRE(handler3.queryAllKeys() == test1);
			std::vector<uint32_t> test2 = {5000,200,3,100,350};
			REQUIRE(handler.queryAllValues() == test2);
			REQUIRE(handler2.queryAllValues() == test2);
			REQUIRE(handler3.queryAllValues() == test2);
	}
	
	SECTION("Test Conflict String on Multiple Servers")
	{
		crdt::state::GMapSBString<uint32_t, std::string> handler1(1);
		crdt::state::GMapSBString<uint32_t, std::string> handler2(2); 
		crdt::state::GMapSBString<uint32_t, std::string> handler3(3);
		crdt::state::GMapMetadata<uint32_t, std::string> replica1A(0,10,"Hello");
		crdt::state::GMapMetadata<uint32_t, std::string> replica1B(0,10,"HelloMelo");
		crdt::state::GMapMetadata<uint32_t, std::string> replica1C(0,10,"Hello Hello");
		handler1.addExternalReplica({replica1A});
		handler2.addExternalReplica({replica1B});
		handler3.addExternalReplica({replica1C});
		REQUIRE(handler2.queryPayloadwithID(0,10) != handler1.queryPayloadwithID(0,10));
		REQUIRE(handler3.queryPayloadwithID(0,10) != handler2.queryPayloadwithID(0,10));
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler3.queryPayloadwithID(0,10) == "Hello Hello HelloMelo");
		REQUIRE(handler2.queryPayloadwithID(0,10) == handler1.queryPayloadwithID(0,10));
		REQUIRE(handler3.queryPayloadwithID(0,10) == handler2.queryPayloadwithID(0,10));
		replica1A.insert(1,"Hello, my name is Bob!");
		handler1.addExternalReplica({replica1A});
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryPayloadwithID(0,1) == handler2.queryPayloadwithID(0,1));
		REQUIRE(handler2.queryPayloadwithID(0,1) == handler3.queryPayloadwithID(0,1));
		crdt::state::GMapMetadata<uint32_t, std::string> replica1D(1,10,"ABC");
		handler3.addExternalReplica({replica1D});
		replica1D.insert(10,"DEF");
		handler2.addExternalReplica({replica1D});
		replica1D.insert(10,"U");
		handler1.addExternalReplica({replica1D});
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryPayloadwithID(1,10) == "ABC DEF U");
		REQUIRE(handler1.queryPayloadwithID(1,10) == handler2.queryPayloadwithID(1,10));
		REQUIRE(handler2.queryPayloadwithID(1,10) == handler3.queryPayloadwithID(1,10));
		crdt::state::GMapMetadata<uint32_t, std::string> replica1E(2,10,"ASDHUIFDHIUSDHFUI");
		handler2.addExternalReplica({replica1E});
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryPayloadwithID(2,10) == handler2.queryPayloadwithID(2,10));
		REQUIRE(handler2.queryPayloadwithID(2,10) == handler3.queryPayloadwithID(2,10));
		REQUIRE(handler1.queryPayload(1) == handler2.queryPayload(1));
		REQUIRE(handler2.queryPayload(1) == handler3.queryPayload(1));
		std::vector<uint32_t> test1 = {1,10};
		REQUIRE(handler1.queryAllKeys() == test1);
		REQUIRE(handler2.queryAllKeys() == test1);
		REQUIRE(handler3.queryAllKeys() == test1);
		std::vector<std::string> test2 = { "Bob! Hello, is my name", "ABC ASDHUIFDHIUSDHFUI DEF Hello Hello HelloMelo U"};
		REQUIRE(handler1.queryAllValues() == test2);
		REQUIRE(handler2.queryAllValues() == test2);
		REQUIRE(handler3.queryAllValues() == test2);
	}

	SECTION("Test serialize function")
	{
		crdt::state::GMapMetadata<uint32_t, uint32_t> replica1A(0,10,1);
		crdt::state::GMapMetadata<uint32_t, uint32_t> replica1B(0,10,2);
		crdt::state::GMapMetadata<uint32_t, uint32_t> replica1C(0,10,3);

		REQUIRE(replica1A.serialize() == "{\"id\":0,\"payload\":\"{\\\"10\\\":\\\"1\\\"}\"}");
		REQUIRE(replica1B.serialize() == "{\"id\":0,\"payload\":\"{\\\"10\\\":\\\"2\\\"}\"}");
		REQUIRE(replica1C.serialize() == "{\"id\":0,\"payload\":\"{\\\"10\\\":\\\"3\\\"}\"}");
	}

	SECTION("Test deserialize function")
	{
		crdt::state::GMapMetadata<uint32_t, uint32_t> replica1A(0,10,1);
		crdt::state::GMapMetadata<uint32_t, uint32_t> replica1B(0,10,2);
		crdt::state::GMapMetadata<uint32_t, uint32_t> replica1C(0,10,3);

		REQUIRE(replica1A.serialize() == "{\"id\":0,\"payload\":\"{\\\"10\\\":\\\"1\\\"}\"}");
		REQUIRE(replica1B.serialize() == "{\"id\":0,\"payload\":\"{\\\"10\\\":\\\"2\\\"}\"}");
		REQUIRE(replica1C.serialize() == "{\"id\":0,\"payload\":\"{\\\"10\\\":\\\"3\\\"}\"}");

		crdt::state::GMapMetadata<uint32_t> replica2A;
		crdt::state::GMapMetadata<uint32_t> replica2B;
		crdt::state::GMapMetadata<uint32_t> replica2C;

		replica2A.deserialize(replica1A.serialize());
		replica2B.deserialize(replica1B.serialize());
		replica2C.deserialize(replica1C.serialize());

		REQUIRE(replica2A.serialize() == "{\"id\":0,\"payload\":\"{\\\"10\\\":\\\"1\\\"}\"}");
		REQUIRE(replica2B.serialize() == "{\"id\":0,\"payload\":\"{\\\"10\\\":\\\"2\\\"}\"}");
		REQUIRE(replica2C.serialize() == "{\"id\":0,\"payload\":\"{\\\"10\\\":\\\"3\\\"}\"}");
	}
#ifdef LOCAL_TESTING
	SECTION("Test serialize function saving to a file")
	{
		crdt::state::GMapMetadata<uint32_t, uint32_t> replica1A(0,10,1);
		crdt::state::GMapMetadata<uint32_t, uint32_t> replica1B(0,10,2);
		crdt::state::GMapMetadata<uint32_t, uint32_t> replica1C(0,10,3);

		replica1A.serializeFile("../../tests/temp_data/gmap1A.json");
		replica1B.serializeFile("../../tests/temp_data/gmap1B.json");
		replica1C.serializeFile("../../tests/temp_data/gmap1C.json");

        std::string replica1AString;
        std::ifstream replica1Ai("../../tests/temp_data/gmap1A.json");
        replica1Ai >> replica1AString;

        std::string replica1BString;
        std::ifstream replica1Bi("../../tests/temp_data/gmap1B.json");
        replica1Bi >> replica1BString;

        std::string replica1CString;
        std::ifstream replica1Ci("../../tests/temp_data/gmap1C.json");
        replica1Ci >> replica1CString;

        REQUIRE(replica1AString == replica1A.serialize());
		REQUIRE(replica1BString == replica1B.serialize());
		REQUIRE(replica1CString == replica1C.serialize());
	}
#endif
#ifdef LOCAL_TESTING
	SECTION("Test deserialize function reading from a file")
	{
		crdt::state::GMapMetadata<uint32_t, uint32_t> replica1A(0,10,1);
		crdt::state::GMapMetadata<uint32_t, uint32_t> replica1B(0,10,2);
		crdt::state::GMapMetadata<uint32_t, uint32_t> replica1C(0,10,3);

		replica1A.serializeFile("../../tests/temp_data/gmap1A.json");
		replica1B.serializeFile("../../tests/temp_data/gmap1B.json");
		replica1C.serializeFile("../../tests/temp_data/gmap1C.json");

		crdt::state::GMapMetadata<uint32_t> replica2A;
		crdt::state::GMapMetadata<uint32_t> replica2B;
		crdt::state::GMapMetadata<uint32_t> replica2C;

		replica2A.deserializeFile("../../tests/temp_data/gmap1A.json");
		replica2B.deserializeFile("../../tests/temp_data/gmap1B.json");
		replica2C.deserializeFile("../../tests/temp_data/gmap1C.json");

		REQUIRE(replica1A.serialize() == replica2A.serialize());
		REQUIRE(replica1B.serialize() == replica2B.serialize());
		REQUIRE(replica1C.serialize() == replica2C.serialize());
	}
#endif
}

TEST_CASE("Test PriorityQueueSB", "[classic]")
{
	SECTION("Test Insert Operation")
	{
		crdt::state::PriorityQueueSB<uint32_t> handler(1); //Represents Server 1
		crdt::state::PriorityQueueMetadata<uint32_t> replica1A(0,5);
		replica1A.push(7);
		replica1A.push(2);
		handler.addExternalReplica({replica1A});
		std::vector<uint32_t> test1 = {7,5,2};
		std::vector<uint32_t> test2;
		auto totalPQ = handler.queryPayload();
		while (!totalPQ.empty()) {
			test2.push_back(totalPQ.top());
			totalPQ.pop();
		}
		REQUIRE(test1 == test2);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1B(1,100);
		replica1B.push(200);
		replica1B.push(50);
		replica1B.push(300);
		replica1B.push(20);
		replica1B.push(1);
		handler.addExternalReplica({replica1B});
		std::vector<uint32_t> test3 = {300,200,100,50,20,7,5,2,1};
		std::vector<uint32_t> test4;
		auto query2 = handler.queryPayload();
		while (!query2.empty())
		{
			test4.push_back(query2.top());
			query2.pop();
		}
		REQUIRE(test3 == test4);
	}
	SECTION("Test Conflict on localServer")
	{
		crdt::state::PriorityQueueSB<uint32_t> handler1(1);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1A(0,5);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1B(0);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1C(0);
		replica1A.push(20);
		replica1A.push(25);
		replica1B.push(30);
		replica1B.push(35);
		replica1C.push(40);
		replica1C.push(45);
		replica1C.push(30);
		replica1C.push(30);
		handler1.addExternalReplica({replica1A,replica1B,replica1C});
		std::vector<uint32_t> test1 = {45,40,35,30, 30, 25,20,5};
		REQUIRE(handler1.queryPayloadVector() == test1);
	}

	SECTION("Test Union Merge")
	{
		crdt::state::PriorityQueueSB<uint32_t> handler1(1);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1A(0);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1B(0);
		replica1A.push({5,5,5});
		replica1B.push({5,5,5,5,5});
		handler1.addExternalReplica({replica1A,replica1B});
		std::vector<uint32_t> test1 = {5,5,5,5,5};
		REQUIRE(handler1.queryPayloadVector() == test1);
		crdt::state::PriorityQueueSB<uint32_t> handler2(2);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1C(1);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1D(1);
		replica1C.push({5,5,5,5,5});
		replica1D.push({5,5,5});
		handler2.addExternalReplica({replica1C,replica1D});
		REQUIRE(handler2.queryPayloadVector() == test1);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1E(2);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1F(2);
		replica1E.push({5,2,3});
		replica1F.push({7,1,2});
		std::vector<uint32_t> test2 = {7,5,3,2,1};
		crdt::state::PriorityQueueSB<uint32_t> handler3(3);
		handler3.addExternalReplica({replica1E,replica1F});
		REQUIRE(handler3.queryPayloadVector() == test2);
	}

	SECTION("Test Conflict on multiple Servers")
	{
		crdt::state::PriorityQueueSB<uint32_t> handler1(2);
		crdt::state::PriorityQueueSB<uint32_t> handler2(2);
		crdt::state::PriorityQueueSB<uint32_t> handler3(3);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1A(0);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1B(1);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1C(2);
		replica1A.push({1,2,3});
		replica1B.push({4,5,6});
		replica1C.push({7,8,9});
		handler1.addExternalReplica({replica1A,replica1B,replica1C});
		std::vector<uint32_t> test1 = {9,8,7,6,5,4,3,2,1};
		REQUIRE(handler1.queryPayloadVector() == test1);
		crdt::state::PriorityQueueMetadata<uint32_t> replica2A(0);
		crdt::state::PriorityQueueMetadata<uint32_t> replica2B(1);
		crdt::state::PriorityQueueMetadata<uint32_t> replica2C(2);
		replica2A.push({10,11,12});
		replica2B.push({13,14,15});
		replica2C.push({16,17,18});
		handler2.addExternalReplica({replica2A,replica2B,replica2C});
		std::vector<uint32_t> test2 = {18,17,16,15,14,13,12,11,10};
		REQUIRE(handler2.queryPayloadVector() == test2);
		crdt::state::PriorityQueueMetadata<uint32_t> replica3A(0);
		crdt::state::PriorityQueueMetadata<uint32_t> replica3B(1);
		crdt::state::PriorityQueueMetadata<uint32_t> replica3C(2);
		replica3A.push({19,20,21});
		replica3B.push({22,23,24});
		replica3C.push({25,26,27});
		handler3.addExternalReplica({replica3A,replica3B,replica3C});
		std::vector<uint32_t> test3 = {27,26,25,24,23,22,21,20,19};
		REQUIRE(handler3.queryPayloadVector() == test3);
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryPayloadVector() == handler2.queryPayloadVector());
		REQUIRE(handler2.queryPayloadVector() == handler3.queryPayloadVector());
		crdt::state::PriorityQueueMetadata<uint32_t> replica3D(3);
		replica3D.push({100,200,300});
		handler3.addExternalReplica({replica3D});
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryPayloadVector() == handler2.queryPayloadVector());
		REQUIRE(handler2.queryPayloadVector() == handler3.queryPayloadVector());
		replica3D.push({400,500});
		handler1.addExternalReplica({replica3D});
		replica3D.push({1000,2000,564});
		handler2.addExternalReplica({replica3D});
		REQUIRE(handler1.queryPayloadVector() != handler2.queryPayloadVector());
		REQUIRE(handler2.queryPayloadVector() != handler3.queryPayloadVector());
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.convertPQtoVector(handler1.queryPayloadwithID(0)) == handler2.convertPQtoVector(handler2.queryPayloadwithID(0)));
		REQUIRE(handler2.convertPQtoVector(handler2.queryPayloadwithID(0)) == handler3.convertPQtoVector(handler3.queryPayloadwithID(0)));
		REQUIRE(handler1.queryPayloadVector() == handler2.queryPayloadVector());
		REQUIRE(handler2.queryPayloadVector() == handler3.queryPayloadVector());
	}

	SECTION("Test serialize function")
	{
		crdt::state::PriorityQueueMetadata<uint32_t> replica1A(0);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1B(1);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1C(2);
		replica1A.push({1,2,3});
		replica1B.push({4,5,6});
		replica1C.push({7,8,9});

		REQUIRE(replica1A.serialize() == "{\"id\":0,\"payload\":[3,2,1]}");
		REQUIRE(replica1B.serialize() == "{\"id\":1,\"payload\":[6,5,4]}");
		REQUIRE(replica1C.serialize() == "{\"id\":2,\"payload\":[9,8,7]}");
	}

	SECTION("Test deserialize function")
	{
		crdt::state::PriorityQueueMetadata<uint32_t> replica1A(0);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1B(1);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1C(2);
		replica1A.push({1,2,3});
		replica1B.push({4,5,6});
		replica1C.push({7,8,9});

		REQUIRE(replica1A.serialize() == "{\"id\":0,\"payload\":[3,2,1]}");
		REQUIRE(replica1B.serialize() == "{\"id\":1,\"payload\":[6,5,4]}");
		REQUIRE(replica1C.serialize() == "{\"id\":2,\"payload\":[9,8,7]}");

		crdt::state::PriorityQueueMetadata<uint32_t> replica2A;
		crdt::state::PriorityQueueMetadata<uint32_t> replica2B;
		crdt::state::PriorityQueueMetadata<uint32_t> replica2C;

		replica2A.deserialize(replica1A.serialize());
		replica2B.deserialize(replica1B.serialize());
		replica2C.deserialize(replica1C.serialize());

		REQUIRE(replica2A.serialize() == "{\"id\":0,\"payload\":[3,2,1]}");
		REQUIRE(replica2B.serialize() == "{\"id\":1,\"payload\":[6,5,4]}");
		REQUIRE(replica2C.serialize() == "{\"id\":2,\"payload\":[9,8,7]}");
	}
#ifdef LOCAL_TESTING
	SECTION("Test serialize function saving to a file")
	{
		crdt::state::PriorityQueueMetadata<uint32_t> replica1A(0);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1B(1);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1C(2);
		replica1A.push({1,2,3});
		replica1B.push({4,5,6});
		replica1C.push({7,8,9});

		replica1A.serializeFile("../../tests/temp_data/priorityqueue1A.json");
		replica1B.serializeFile("../../tests/temp_data/priorityqueue1B.json");
		replica1C.serializeFile("../../tests/temp_data/priorityqueue1C.json");

        std::string replica1AString;
        std::ifstream replica1Ai("../../tests/temp_data/priorityqueue1A.json");
        replica1Ai >> replica1AString;

        std::string replica1BString;
        std::ifstream replica1Bi("../../tests/temp_data/priorityqueue1B.json");
        replica1Bi >> replica1BString;

        std::string replica1CString;
        std::ifstream replica1Ci("../../tests/temp_data/priorityqueue1C.json");
        replica1Ci >> replica1CString;

        REQUIRE(replica1AString == replica1A.serialize());
		REQUIRE(replica1BString == replica1B.serialize());
		REQUIRE(replica1CString == replica1C.serialize());
	}
#endif
#ifdef LOCAL_TESTING
	SECTION("Test deserialize function reading from a file")
	{
		crdt::state::PriorityQueueMetadata<uint32_t> replica1A(0);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1B(1);
		crdt::state::PriorityQueueMetadata<uint32_t> replica1C(2);
		replica1A.push({1,2,3});
		replica1B.push({4,5,6});
		replica1C.push({7,8,9});

		replica1A.serializeFile("../../tests/temp_data/priorityqueue1A.json");
		replica1B.serializeFile("../../tests/temp_data/priorityqueue1B.json");
		replica1C.serializeFile("../../tests/temp_data/priorityqueue1C.json");

		crdt::state::PriorityQueueMetadata<uint32_t> replica2A;
		crdt::state::PriorityQueueMetadata<uint32_t> replica2B;
		crdt::state::PriorityQueueMetadata<uint32_t> replica2C;

		replica2A.deserializeFile("../../tests/temp_data/priorityqueue1A.json");
		replica2B.deserializeFile("../../tests/temp_data/priorityqueue1B.json");
		replica2C.deserializeFile("../../tests/temp_data/priorityqueue1C.json");

		REQUIRE(replica1A.serialize() == replica2A.serialize());
		REQUIRE(replica1B.serialize() == replica2B.serialize());
		REQUIRE(replica1C.serialize() == replica2C.serialize());
	}
#endif
}

TEST_CASE("Test MultiSetSB", "[classic]")
{
	SECTION("Test Insertion")
	{
		crdt::state::MultiSetSB<uint32_t> handler1(1); //Represents Server 1
		crdt::state::MultiSetMetadata<uint32_t> replica1A(0,5);
		replica1A.insert({2,6,4,6,2,16,2,1,6,7});
		crdt::state::MultiSetMetadata<uint32_t> replica1B(0,7);
		replica1B.insert({9,9,9,9});
		std::multiset<uint32_t> test1 = {1,2,2,2,4,5,6,6,6,7,9,9,9,9,16};
		handler1.addExternalReplica({replica1A,replica1B});
		REQUIRE(handler1.queryPayload() == test1);
		crdt::state::MultiSetMetadata<uint32_t> replica1C(1,2);
		replica1C.insert({10,10,10});
		handler1.addExternalReplica({replica1C});
		std::multiset<uint32_t> test2 = {2,10,10,10};
		REQUIRE(handler1.queryPayloadwithID(1) == test2);
	}
	SECTION("Test Conflict on localServer")
	{
		crdt::state::MultiSetSB<uint32_t> handler1(1); //Represents Server 1
		crdt::state::MultiSetMetadata<uint32_t> replica1A(0,5);
		crdt::state::MultiSetMetadata<uint32_t> replica1B(0);
		crdt::state::MultiSetMetadata<uint32_t> replica1C(0);
		replica1A.insert({20,25,30});
		replica1B.insert({30,35});
		replica1C.insert({40,45,30,30});
		handler1.addExternalReplica({replica1A,replica1B,replica1C});
		std::multiset<uint32_t> test1 = {5,20,25,30,30,35,40,45};
		REQUIRE(handler1.queryPayload() == test1);
	}
	SECTION("Test Conflict on multiple Servers")
	{
		crdt::state::MultiSetSB<uint32_t> handler1(2);
		crdt::state::MultiSetSB<uint32_t> handler2(2);
		crdt::state::MultiSetSB<uint32_t> handler3(3);
		crdt::state::MultiSetMetadata<uint32_t> replica1A(0);
		crdt::state::MultiSetMetadata<uint32_t> replica1B(1);
		crdt::state::MultiSetMetadata<uint32_t> replica1C(2);
		replica1A.insert({1,2,3});
		replica1B.insert({4,5,6});
		replica1C.insert({7,8,9});
		handler1.addExternalReplica({replica1A,replica1B,replica1C});
		std::multiset<uint32_t> test1 = {1,2,3,4,5,6,7,8,9};
		REQUIRE(handler1.queryPayload() == test1);
		crdt::state::MultiSetMetadata<uint32_t> replica2A(0);
		crdt::state::MultiSetMetadata<uint32_t> replica2B(1);
		crdt::state::MultiSetMetadata<uint32_t> replica2C(2);
		replica2A.insert({10,11,12});
		replica2B.insert({13,14,15});
		replica2C.insert({16,17,18});
		handler2.addExternalReplica({replica2A,replica2B,replica2C});
		std::multiset<uint32_t> test2 = {10,11,12,13,14,15,16,17,18};
		REQUIRE(handler2.queryPayload() == test2);
		crdt::state::MultiSetMetadata<uint32_t> replica3A(0);
		crdt::state::MultiSetMetadata<uint32_t> replica3B(1);
		crdt::state::MultiSetMetadata<uint32_t> replica3C(2);
		replica3A.insert({19,20,21});
		replica3B.insert({22,23,24});
		replica3C.insert({25,26,27});
		handler3.addExternalReplica({replica3A,replica3B,replica3C});
		std::multiset<uint32_t> test3 = {19,20,21,22,23,24,25,26,27};
		REQUIRE(handler3.queryPayload() == test3);
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryPayload()  == handler2.queryPayload());
		REQUIRE(handler2.queryPayload()  == handler3.queryPayload());
		crdt::state::MultiSetMetadata<uint32_t>  replica3D(3);
		replica3D.insert({100,200,300});
		handler3.addExternalReplica({replica3D});
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryPayload()  == handler2.queryPayload());
		REQUIRE(handler2.queryPayload()  == handler3.queryPayload());
		replica3D.insert({400,500});
		handler1.addExternalReplica({replica3D});
		replica3D.insert({1000,2000,564});
		handler2.addExternalReplica({replica3D});
		REQUIRE(handler1.queryPayload() != handler2.queryPayload());
		REQUIRE(handler2.queryPayload() != handler3.queryPayload());
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		std::multiset<uint32_t> test4 =  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
  		22, 23, 24, 25, 26, 27, 100, 200, 300 , 400 , 500 , 564, 1000 , 2000 }; 
		REQUIRE(handler1.queryPayload() == test4);
		REQUIRE(handler1.queryPayload() == handler2.queryPayload());
		REQUIRE(handler2.queryPayload() == handler3.queryPayload());
		std::multiset<uint32_t> test5 =  {1,2,3,10,11,12,19,20,21};
		REQUIRE(handler1.queryPayloadwithID(0) == test5);
		REQUIRE(handler1.queryPayloadwithID(0) == handler2.queryPayloadwithID(0));
		REQUIRE(handler2.queryPayloadwithID(0) == handler3.queryPayloadwithID(0));
	}

	SECTION("Test serialize function")
	{
		crdt::state::MultiSetMetadata<uint32_t> replica1A(0);
		crdt::state::MultiSetMetadata<uint32_t> replica1B(1);
		crdt::state::MultiSetMetadata<uint32_t> replica1C(2);
		replica1A.insert({1,2,3});
		replica1B.insert({4,5,6});
		replica1C.insert({7,8,9});

		REQUIRE(replica1A.serialize() == "{\"id\":0,\"payload\":[1,2,3]}");
		REQUIRE(replica1B.serialize() == "{\"id\":1,\"payload\":[4,5,6]}");
		REQUIRE(replica1C.serialize() == "{\"id\":2,\"payload\":[7,8,9]}");
	}

	SECTION("Test deserialize function")
	{
		crdt::state::MultiSetMetadata<uint32_t> replica1A(0);
		crdt::state::MultiSetMetadata<uint32_t> replica1B(1);
		crdt::state::MultiSetMetadata<uint32_t> replica1C(2);
		replica1A.insert({1,2,3});
		replica1B.insert({4,5,6});
		replica1C.insert({7,8,9});

		REQUIRE(replica1A.serialize() == "{\"id\":0,\"payload\":[1,2,3]}");
		REQUIRE(replica1B.serialize() == "{\"id\":1,\"payload\":[4,5,6]}");
		REQUIRE(replica1C.serialize() == "{\"id\":2,\"payload\":[7,8,9]}");

		crdt::state::MultiSetMetadata<uint32_t> replica2A;
		crdt::state::MultiSetMetadata<uint32_t> replica2B;
		crdt::state::MultiSetMetadata<uint32_t> replica2C;

		replica2A.deserialize(replica1A.serialize());
		replica2B.deserialize(replica1B.serialize());
		replica2C.deserialize(replica1C.serialize());

		REQUIRE(replica2A.serialize() == "{\"id\":0,\"payload\":[1,2,3]}");
		REQUIRE(replica2B.serialize() == "{\"id\":1,\"payload\":[4,5,6]}");
		REQUIRE(replica2C.serialize() == "{\"id\":2,\"payload\":[7,8,9]}");
	}
#ifdef LOCAL_TESTING
	SECTION("Test serialize function saving to a file")
	{
		crdt::state::MultiSetMetadata<uint32_t> replica1A(0);
		crdt::state::MultiSetMetadata<uint32_t> replica1B(1);
		crdt::state::MultiSetMetadata<uint32_t> replica1C(2);
		replica1A.insert({1,2,3});
		replica1B.insert({4,5,6});
		replica1C.insert({7,8,9});

		replica1A.serializeFile("../../tests/temp_data/multiset1A.json");
		replica1B.serializeFile("../../tests/temp_data/multiset1B.json");
		replica1C.serializeFile("../../tests/temp_data/multiset1C.json");

        std::string replica1AString;
        std::ifstream replica1Ai("../../tests/temp_data/multiset1A.json");
        replica1Ai >> replica1AString;

        std::string replica1BString;
        std::ifstream replica1Bi("../../tests/temp_data/multiset1B.json");
        replica1Bi >> replica1BString;

        std::string replica1CString;
        std::ifstream replica1Ci("../../tests/temp_data/multiset1C.json");
        replica1Ci >> replica1CString;

        REQUIRE(replica1AString == replica1A.serialize());
		REQUIRE(replica1BString == replica1B.serialize());
		REQUIRE(replica1CString == replica1C.serialize());
	}
#endif
#ifdef LOCAL_TESTING
	SECTION("Test deserialize function reading from a file")
	{
		crdt::state::MultiSetMetadata<uint32_t> replica1A(0);
		crdt::state::MultiSetMetadata<uint32_t> replica1B(1);
		crdt::state::MultiSetMetadata<uint32_t> replica1C(2);
		replica1A.insert({1,2,3});
		replica1B.insert({4,5,6});
		replica1C.insert({7,8,9});

		replica1A.serializeFile("../../tests/temp_data/multiset1A.json");
		replica1B.serializeFile("../../tests/temp_data/multiset1B.json");
		replica1C.serializeFile("../../tests/temp_data/multiset1C.json");

		crdt::state::MultiSetMetadata<uint32_t> replica2A;
		crdt::state::MultiSetMetadata<uint32_t> replica2B;
		crdt::state::MultiSetMetadata<uint32_t> replica2C;

		replica2A.deserializeFile("../../tests/temp_data/multiset1A.json");
		replica2B.deserializeFile("../../tests/temp_data/multiset1B.json");
		replica2C.deserializeFile("../../tests/temp_data/multiset1C.json");

		REQUIRE(replica1A.serialize() == replica2A.serialize());
		REQUIRE(replica1B.serialize() == replica2B.serialize());
		REQUIRE(replica1C.serialize() == replica2C.serialize());
	}
#endif
}

TEST_CASE("Test LWWMultiSetSB", "[classic]")
{
	SECTION("Test Insertion + Deletion")
	{
		crdt::state::LWWMultiSetSB<uint32_t> handler1(1); //Represents Server 1
		crdt::state::LWWMultiSetMetadata<uint32_t> replica1A(0,5,0); //Added at time = 0
		replica1A.insert(0,{2,6,4,6,2,16,2,1,6,7});
		crdt::state::LWWMultiSetMetadata<uint32_t> replica1B(1,7,1); //Added at time = 0
		replica1B.insert(1,{9,9,9,9});
		replica1B.insert(2,{350,360}); //time = 2
		std::multiset<uint32_t> test1 = {1, 2, 2, 2, 4, 5, 6, 6, 6, 7, 7, 9, 9, 9, 9, 16, 350 , 360};
		std::multiset<uint32_t> test2 = { 1, 2, 2, 2, 4, 5, 6, 6, 6, 7, 16 };
		std::multiset<uint32_t> test3 = { 7, 9, 9, 9, 9, 350 , 360};
		handler1.addExternalReplica({replica1A,replica1B});
		REQUIRE(handler1.queryLWWMultiSet() == test1);
		REQUIRE(handler1.queryLWWMultiSetwithID(0) == test2);
		REQUIRE(handler1.queryLWWMultiSetwithID(1) == test3);
		std::multiset<uint32_t> test4 = { 1, 4, 5, 6, 6, 6, 7, 16 };
		std::multiset<uint32_t> test5 = { 7, 9, 9, 9, 9};
		std::multiset<uint32_t> test6  = {1, 4, 5, 6, 6, 6, 7, 7, 9, 9, 9, 9, 16 }; 
		handler1.remove(1,2,{350,360});
		handler1.remove(0,3,{2,2,2});;
		REQUIRE(handler1.queryLWWMultiSetwithID(0) == test4);
		REQUIRE(handler1.queryLWWMultiSetwithID(1) == test5);
		REQUIRE(handler1.queryPayload() == test6);
	
		//Add back in 350 @time t = 4
		handler1.insert(1,4,350);
		std::multiset<uint32_t> test7 = {1, 4, 5, 6, 6, 6, 7, 7, 9, 9, 9, 9, 16, 350};
		REQUIRE(handler1.queryLWWMultiSet() == test7);
	}
	SECTION("Test Conflict on localServer")
	{
		crdt::state::LWWMultiSetSB<uint32_t> handler1(1); //Represents Server 1
		crdt::state::LWWMultiSetMetadata<uint32_t> replica1A(0);
		crdt::state::LWWMultiSetMetadata<uint32_t> replica1B(0);
		crdt::state::LWWMultiSetMetadata<uint32_t> replica1C(0);
		replica1A.insert(0,{20,25,30}); //t = 0
		replica1B.insert(1,{30,35}); // t = 1
		replica1C.insert(2, {40,45,30,30}); // t = 2
		handler1.addExternalReplica({replica1A,replica1B,replica1C});
		std::multiset<uint32_t> test1 = {30,30,40,45};
		REQUIRE(handler1.queryPayload() == test1);
		handler1.remove(0,3,{30,30});
		handler1.remove(0,4,30);
		handler1.remove(0,5,{30,30,45});
		std::multiset<uint32_t> test2 = {40};
		REQUIRE(handler1.queryPayload() == test2);
		handler1.insert(0,6,{30,30});
		std::multiset<uint32_t> test3 = {30,30,40};
		REQUIRE(handler1.queryPayload() == test3);
		handler1.remove(0,8,30);
		std::multiset<uint32_t> test4 = {30,40};
		REQUIRE(handler1.queryPayload() == test4);
	}
	SECTION("Test Conflict on multiple Servers")
	{
		crdt::state::LWWMultiSetSB<uint32_t> handler1(1);
		crdt::state::LWWMultiSetSB<uint32_t> handler2(2);
		crdt::state::LWWMultiSetSB<uint32_t> handler3(3);
		crdt::state::LWWMultiSetMetadata<uint32_t> replicaA(0,{5,5},0);
		crdt::state::LWWMultiSetMetadata<uint32_t> replicaB(0,{5,5,7},1);
		crdt::state::LWWMultiSetMetadata<uint32_t> replicaC(0,{5,10,15},2);
		crdt::state::LWWMultiSetMetadata<uint32_t> replicaD(1, {20,25}, 0);
		handler1.addExternalReplica({replicaA,replicaD});
		handler2.addExternalReplica({replicaB});
		handler3.addExternalReplica({replicaC});
		std::multiset<uint32_t> test1 = {5,10,15, 20, 25};
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryPayload() == test1);
		REQUIRE(handler1.queryPayload() == handler2.queryPayload());
		REQUIRE(handler2.queryPayload() == handler3.queryPayload());
		handler1.remove(0,4,{5,10});
		std::multiset<uint32_t> test2 = {15, 20, 25};
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler2.queryPayload() == test2);
		REQUIRE(handler1.queryPayload() == handler2.queryPayload());
		REQUIRE(handler2.queryPayload() == handler3.queryPayload());
		handler3.insert(0,5,10);
		std::multiset<uint32_t> test3 = {10, 15, 20, 25};
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler2.queryPayload() == test3);
		REQUIRE(handler1.queryPayload() == handler2.queryPayload());
		REQUIRE(handler2.queryPayload() == handler3.queryPayload());
	}

	SECTION("Test serialize function")
	{
		crdt::state::LWWMultiSetMetadata<uint32_t> replica1A(0,{5,5},0);
		crdt::state::LWWMultiSetMetadata<uint32_t> replica1B(0,{5,5,7},1);
		crdt::state::LWWMultiSetMetadata<uint32_t> replica1C(0,{5,10,15},2);

		REQUIRE(replica1A.serialize() == "{\"currentTime\":0,\"id\":0,\"payload\":{\"0\":[5,5]},\"tombstone\":null}");
		REQUIRE(replica1B.serialize() == "{\"currentTime\":1,\"id\":0,\"payload\":{\"1\":[5,5,7]},\"tombstone\":null}");
		REQUIRE(replica1C.serialize() == "{\"currentTime\":2,\"id\":0,\"payload\":{\"2\":[5,10,15]},\"tombstone\":null}");
	}

	SECTION("Test deserialize function")
	{
		crdt::state::LWWMultiSetMetadata<uint32_t> replica1A(0,{5,5},0);
		crdt::state::LWWMultiSetMetadata<uint32_t> replica1B(0,{5,5,7},1);
		crdt::state::LWWMultiSetMetadata<uint32_t> replica1C(0,{5,10,15},2);

		REQUIRE(replica1A.serialize() == "{\"currentTime\":0,\"id\":0,\"payload\":{\"0\":[5,5]},\"tombstone\":null}");
		REQUIRE(replica1B.serialize() == "{\"currentTime\":1,\"id\":0,\"payload\":{\"1\":[5,5,7]},\"tombstone\":null}");
		REQUIRE(replica1C.serialize() == "{\"currentTime\":2,\"id\":0,\"payload\":{\"2\":[5,10,15]},\"tombstone\":null}");

		crdt::state::LWWMultiSetMetadata<uint32_t> replica2A;
		crdt::state::LWWMultiSetMetadata<uint32_t> replica2B;
		crdt::state::LWWMultiSetMetadata<uint32_t> replica2C;

		replica2A.deserialize(replica1A.serialize());
		replica2B.deserialize(replica1B.serialize());
		replica2C.deserialize(replica1C.serialize());

		REQUIRE(replica2A.serialize() == "{\"currentTime\":0,\"id\":0,\"payload\":{\"0\":[5,5]},\"tombstone\":null}");
		REQUIRE(replica2B.serialize() == "{\"currentTime\":1,\"id\":0,\"payload\":{\"1\":[5,5,7]},\"tombstone\":null}");
		REQUIRE(replica2C.serialize() == "{\"currentTime\":2,\"id\":0,\"payload\":{\"2\":[5,10,15]},\"tombstone\":null}");
	}
#ifdef LOCAL_TESTING
	SECTION("Test serialize function saving to a file")
	{
		crdt::state::LWWMultiSetMetadata<uint32_t> replica1A(0,{5,5},0);
		crdt::state::LWWMultiSetMetadata<uint32_t> replica1B(0,{5,5,7},1);
		crdt::state::LWWMultiSetMetadata<uint32_t> replica1C(0,{5,10,15},2);

		replica1A.serializeFile("../../tests/temp_data/lwwmultiset1A.json");
		replica1B.serializeFile("../../tests/temp_data/lwwmultiset1B.json");
		replica1C.serializeFile("../../tests/temp_data/lwwmultiset1C.json");

        std::string replica1AString;
        std::ifstream replica1Ai("../../tests/temp_data/lwwmultiset1A.json");
        replica1Ai >> replica1AString;

        std::string replica1BString;
        std::ifstream replica1Bi("../../tests/temp_data/lwwmultiset1B.json");
        replica1Bi >> replica1BString;

        std::string replica1CString;
        std::ifstream replica1Ci("../../tests/temp_data/lwwmultiset1C.json");
        replica1Ci >> replica1CString;

        REQUIRE(replica1AString == replica1A.serialize());
		REQUIRE(replica1BString == replica1B.serialize());
		REQUIRE(replica1CString == replica1C.serialize());
	}
#endif
#ifdef LOCAL_TESTING
	SECTION("Test deserialize function reading from a file")
	{
		crdt::state::LWWMultiSetMetadata<uint32_t> replica1A(0,{5,5},0);
		crdt::state::LWWMultiSetMetadata<uint32_t> replica1B(0,{5,5,7},1);
		crdt::state::LWWMultiSetMetadata<uint32_t> replica1C(0,{5,10,15},2);

		replica1A.serializeFile("../../tests/temp_data/lwwmultiset1A.json");
		replica1B.serializeFile("../../tests/temp_data/lwwmultiset1B.json");
		replica1C.serializeFile("../../tests/temp_data/lwwmultiset1C.json");

		crdt::state::LWWMultiSetMetadata<uint32_t> replica2A;
		crdt::state::LWWMultiSetMetadata<uint32_t> replica2B;
		crdt::state::LWWMultiSetMetadata<uint32_t> replica2C;

		replica2A.deserializeFile("../../tests/temp_data/lwwmultiset1A.json");
		replica2B.deserializeFile("../../tests/temp_data/lwwmultiset1B.json");
		replica2C.deserializeFile("../../tests/temp_data/lwwmultiset1C.json");

		REQUIRE(replica1A.serialize() == replica2A.serialize());
		REQUIRE(replica1B.serialize() == replica2B.serialize());
		REQUIRE(replica1C.serialize() == replica2C.serialize());
	}
#endif
}

TEST_CASE("Test SringOB", "[classic]")
{
	SECTION("Test Conflict on localServer")
	{
		crdt::operation::StringOB<std::string> handler1(1);
		crdt::operation::StringMetaData<std::string> replicaA(0, "AB+++CDEFG");
		crdt::operation::StringMetaData<std::string> replicaB(0, "AB***CDEFG");
		handler1.addExternalReplica({replicaA,replicaB});
		REQUIRE(handler1.queryPayloadwithID(0) == "AB*+++**CDEFG");
		crdt::operation::StringMetaData<std::string> replicaC(1, "ABCDEF+++G");
		crdt::operation::StringMetaData<std::string> replicaD(1, "AB***CDEFG");
		handler1.addExternalReplica({replicaC,replicaD});
		REQUIRE(handler1.queryPayloadwithID(1) == "AB***CD+++EFG");
		crdt::operation::StringMetaData<std::string> replicaE(2, "Hello Ugly World");
		crdt::operation::StringMetaData<std::string> replicaF(2, "Hello Ugli World");
		handler1.addExternalReplica({replicaE,replicaF});
		REQUIRE(handler1.queryPayloadwithID(2) == "Hello Ugliy World");
		crdt::operation::StringMetaData<std::string> replicaG(3, "ABCDEFG");
		crdt::operation::StringMetaData<std::string> replicaH(3, "***");
		handler1.addExternalReplica({replicaG,replicaH});
		REQUIRE(handler1.queryPayloadwithID(3) == "A***BCDEFG");
		crdt::operation::StringMetaData<std::string> replicaI(4, "");
		crdt::operation::StringMetaData<std::string> replicaJ(4, "ABCDEFG");
		handler1.addExternalReplica({replicaI,replicaJ});
		REQUIRE(handler1.queryPayloadwithID(4) == "ABCDEFG");
		crdt::operation::StringMetaData<std::string> replicaK(5, "Hello    World");
		crdt::operation::StringMetaData<std::string> replicaL(5, "Hello Sun      ");
		handler1.addExternalReplica({replicaK,replicaL});
		REQUIRE(handler1.queryPayloadwithID(5) == "Hello Sun World");
		crdt::operation::StringMetaData<std::string> replicaM(6);
		crdt::operation::StringMetaData<std::string> replicaN(6);
	}
	SECTION("Test Conflict on Multiple Servers")
	{
		crdt::operation::StringOB<std::string> handler1(1);
		crdt::operation::StringOB<std::string> handler2(2);
		crdt::operation::StringOB<std::string> handler3(3);
		crdt::operation::StringMetaData<std::string> replicaA(0, "Hello           is");
		crdt::operation::StringMetaData<std::string> replicaB(0, "Hello My");
		crdt::operation::StringMetaData<std::string> replicaC(0, "Hello      name is Bob");
		handler1.addExternalReplica({replicaA});
		handler2.addExternalReplica({replicaB});
		handler3.addExternalReplica({replicaC});
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		crdt::operation::StringMetaData<std::string> replicaD(1, "I WAS NOT EXPECTING IT");
		crdt::operation::StringMetaData<std::string> replicaE(1, "I WAS EXPECTING IT");
		handler1.addExternalReplica({replicaD});
		handler2.addExternalReplica({replicaE});
		REQUIRE(handler1.queryPayloadwithID(0) == "Hello   My      name is Bob");
		REQUIRE(handler1.queryPayloadwithID(0) == handler2.queryPayloadwithID(0));
		REQUIRE(handler2.queryPayloadwithID(0) == handler3.queryPayloadwithID(0));
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler3.queryPayloadwithID(1) == "I WAS NOT EXPECTING IT");
		REQUIRE(handler1.queryPayloadwithID(1) == handler2.queryPayloadwithID(1));
		REQUIRE(handler2.queryPayloadwithID(1) == handler3.queryPayloadwithID(1));
		crdt::operation::StringMetaData<std::string> replicaM(6,"ABCDEF");
		crdt::operation::StringMetaData<std::string> replicaN(6,"ABCDEF");
		handler1.addExternalReplica({replicaM});
		handler2.addExternalReplica({replicaN});
		handler1.insert(6,2,"+++");
		handler2.insert(6,2,"***");
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryPayloadwithID(6) == "AB*+++**CDEF");
		REQUIRE(handler1.queryPayloadwithID(6) == handler2.queryPayloadwithID(6));
		REQUIRE(handler2.queryPayloadwithID(6) == handler3.queryPayloadwithID(6));
		handler1.setStringTo(6,"ABCDEFG");
		handler2.setStringTo(6,"ABCDEFG");
		handler3.clear(6);
		handler1.insert(6,6,"+++");
		handler2.insert(6,2,"***");
		REQUIRE(handler3.queryPayloadwithID(6) == "");
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler3.queryPayloadwithID(6) == "AB***CD+++EFG");
		REQUIRE(handler1.queryPayloadwithID(6) == handler2.queryPayloadwithID(6));
		REQUIRE(handler2.queryPayloadwithID(6) == handler3.queryPayloadwithID(6));
	}

	SECTION("Test serialize function")
	{
		crdt::operation::StringMetaData<std::string> replica1A(0, "Hello           is");
		crdt::operation::StringMetaData<std::string> replica1B(0, "Hello My");
		crdt::operation::StringMetaData<std::string> replica1C(0, "Hello      name is Bob");

		REQUIRE(replica1A.serialize() == "{\"id\":0,\"payload\":\"Hello           is\"}");
		REQUIRE(replica1B.serialize() == "{\"id\":0,\"payload\":\"Hello My\"}");
		REQUIRE(replica1C.serialize() == "{\"id\":0,\"payload\":\"Hello      name is Bob\"}");
	}

	SECTION("Test deserialize function")
	{
		crdt::operation::StringMetaData<std::string> replica1A(0, "Hello           is");
		crdt::operation::StringMetaData<std::string> replica1B(0, "Hello My");
		crdt::operation::StringMetaData<std::string> replica1C(0, "Hello      name is Bob");

		REQUIRE(replica1A.serialize() == "{\"id\":0,\"payload\":\"Hello           is\"}");
		REQUIRE(replica1B.serialize() == "{\"id\":0,\"payload\":\"Hello My\"}");
		REQUIRE(replica1C.serialize() == "{\"id\":0,\"payload\":\"Hello      name is Bob\"}");

		crdt::operation::StringMetaData<std::string> replica2A;
		crdt::operation::StringMetaData<std::string> replica2B;
		crdt::operation::StringMetaData<std::string> replica2C;

		replica2A.deserialize(replica1A.serialize());
		replica2B.deserialize(replica1B.serialize());
		replica2C.deserialize(replica1C.serialize());

		REQUIRE(replica2A.serialize() == "{\"id\":0,\"payload\":\"Hello           is\"}");
		REQUIRE(replica2B.serialize() == "{\"id\":0,\"payload\":\"Hello My\"}");
		REQUIRE(replica2C.serialize() == "{\"id\":0,\"payload\":\"Hello      name is Bob\"}");
	}
#ifdef LOCAL_TESTING
	SECTION("Test serialize function saving to a file")
	{
		crdt::operation::StringMetaData<std::string> replica1A(0, "Hellois");
		crdt::operation::StringMetaData<std::string> replica1B(0, "HelloMy");

		replica1A.serializeFile("../../tests/temp_data/string1A.json");
		replica1B.serializeFile("../../tests/temp_data/string1B.json");

        std::string replica1AString;
        std::ifstream replica1Ai("../../tests/temp_data/string1A.json");
        replica1Ai >> replica1AString;

        std::string replica1BString;
        std::ifstream replica1Bi("../../tests/temp_data/string1B.json");
        replica1Bi >> replica1BString;

        REQUIRE(replica1AString == replica1A.serialize());
		REQUIRE(replica1BString == replica1B.serialize());
	}
#endif
#ifdef LOCAL_TESTING
	SECTION("Test deserialize function reading from a file")
	{
		crdt::operation::StringMetaData<std::string> replica1A(0, "Hellois");
		crdt::operation::StringMetaData<std::string> replica1B(0, "HelloMy");

		replica1A.serializeFile("../../tests/temp_data/string1A.json");
		replica1B.serializeFile("../../tests/temp_data/string1B.json");

		crdt::operation::StringMetaData<std::string> replica2A;
		crdt::operation::StringMetaData<std::string> replica2B;

		replica2A.deserializeFile("../../tests/temp_data/string1A.json");
		replica2B.deserializeFile("../../tests/temp_data/string1B.json");

		REQUIRE(replica1A.serialize() == replica2A.serialize());
		REQUIRE(replica1B.serialize() == replica2B.serialize());
	}
#endif
}

TEST_CASE("Test TwoPTwoPGraphSB", "[classic]")
{
	SECTION("Test Insert Vertices Operation")
	{
		crdt::state::TwoPTwoPGraphSB<uint32_t> handler(0); //Represents Server 1
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1A(1);
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1B(2);
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1C(3);

		replica1A.insertVertice(20);
		replica1B.insertVertice(21);
		replica1C.insertVertice(22);

		std::set<uint32_t> tempA = {20};
		std::set<uint32_t> tempB = {21};
		std::set<uint32_t> tempC = {22};

		REQUIRE(replica1A.queryVertices() == tempA);
		REQUIRE(replica1B.queryVertices() == tempB);
		REQUIRE(replica1C.queryVertices() == tempC);

		handler.addExternalReplica({replica1A,replica1B,replica1C});

		std::set<uint32_t> tempHandler = {20, 21, 22};
		REQUIRE(handler.queryVertices() == tempHandler);
	}
	SECTION("Test Insert Edges Operation")
	{
		crdt::state::TwoPTwoPGraphSB<uint32_t> handler(0); //Represents Server 1
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1A(1);
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1B(2);
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1C(3);

		replica1A.insertEdge(std::pair<uint32_t, uint32_t>(20, 21));
		replica1B.insertEdge(std::pair<uint32_t, uint32_t>(21, 22));
		replica1C.insertEdge(std::pair<uint32_t, uint32_t>(22, 23));

		std::set<std::pair<uint32_t, uint32_t>> tempA = {{20, 21}};
		std::set<std::pair<uint32_t, uint32_t>> tempB = {{21, 22}};
		std::set<std::pair<uint32_t, uint32_t>> tempC = {{22, 23}};

		REQUIRE(replica1A.queryEdges() == tempA);
		REQUIRE(replica1B.queryEdges() == tempB);
		REQUIRE(replica1C.queryEdges() == tempC);

		handler.addExternalReplica({replica1A,replica1B,replica1C});
	}
	SECTION("Test Remove Vertices Operation")
	{
		crdt::state::TwoPTwoPGraphSB<uint32_t> handler(0); //Represents Server 1
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1A(1);
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1B(2);
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1C(3);

		replica1A.insertVertice(20);
		replica1A.insertVertice(21);
		replica1A.insertVertice(22);
		replica1B.insertVertice(23);
		replica1B.insertVertice(24);
		replica1B.insertVertice(25);
		replica1C.insertVertice(26);
		replica1C.insertVertice(27);
		replica1C.insertVertice(28);

		std::set<uint32_t> tempA = {20, 21, 22};
		std::set<uint32_t> tempB = {23, 24, 25};
		std::set<uint32_t> tempC = {26, 27, 28};

		REQUIRE(replica1A.queryVertices() == tempA);
		REQUIRE(replica1B.queryVertices() == tempB);
		REQUIRE(replica1C.queryVertices() == tempC);

		handler.addExternalReplica({replica1A,replica1B,replica1C});

		std::set<uint32_t> tempHandler = {20, 21, 22, 23, 24, 25, 26, 27, 28};
		REQUIRE(handler.queryVertices() == tempHandler);

		replica1A.removeVertice(20);
		replica1B.removeVertice(23);
		replica1C.removeVertice(26);

		std::set<uint32_t> tempA2 = {21, 22};
		std::set<uint32_t> tempB2 = {24, 25};
		std::set<uint32_t> tempC2 = {27, 28};

		handler.addExternalReplica({replica1A,replica1B,replica1C});

		std::set<uint32_t> tempHandler2 = {20, 21, 22, 23, 24, 25, 26, 27, 28};
		REQUIRE(handler.queryVertices() == tempHandler2);
	}
	SECTION("Test Remove Edges Operation")
	{
		crdt::state::TwoPTwoPGraphSB<uint32_t> handler(0); //Represents Server 1
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1A(1);
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1B(2);
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1C(3);

		replica1A.insertEdge(std::pair<uint32_t, uint32_t>(20, 21));
		replica1A.insertEdge(std::pair<uint32_t, uint32_t>(21, 22));
		replica1A.insertEdge(std::pair<uint32_t, uint32_t>(22, 23));
		replica1B.insertEdge(std::pair<uint32_t, uint32_t>(23, 24));
		replica1B.insertEdge(std::pair<uint32_t, uint32_t>(24, 25));
		replica1B.insertEdge(std::pair<uint32_t, uint32_t>(25, 26));
		replica1C.insertEdge(std::pair<uint32_t, uint32_t>(26, 27));
		replica1C.insertEdge(std::pair<uint32_t, uint32_t>(27, 28));
		replica1C.insertEdge(std::pair<uint32_t, uint32_t>(28, 29));

		std::set<std::pair<uint32_t, uint32_t>> tempA = {{20, 21}, {21, 22}, {22, 23}};
		std::set<std::pair<uint32_t, uint32_t>> tempB = {{23, 24}, {24, 25}, {25, 26}};
		std::set<std::pair<uint32_t, uint32_t>> tempC = {{26, 27}, {27, 28}, {28, 29}};

		REQUIRE(replica1A.queryEdges() == tempA);
		REQUIRE(replica1B.queryEdges() == tempB);
		REQUIRE(replica1C.queryEdges() == tempC);

		replica1A.removeEdge({20, 21});
		replica1B.removeEdge({23, 24});
		replica1C.removeEdge({26, 27});

		std::set<std::pair<uint32_t, uint32_t>> tempA2 = {{21, 22}, {22, 23}};
		std::set<std::pair<uint32_t, uint32_t>> tempB2 = {{24, 25}, {25, 26}};
		std::set<std::pair<uint32_t, uint32_t>> tempC2 = {{27, 28}, {28, 29}};

		handler.addExternalReplica({replica1A,replica1B,replica1C});
		std::set<std::pair<uint32_t, uint32_t>> tempHandler2 = {{21, 22}, {22, 23}, {24, 25}, {25, 26}, {27, 28}, {28, 29}};
		//REQUIRE(handler.queryEdges() == tempHandler2);
	}

	SECTION("Test serialize function")
	{
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1A(1);
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1B(2);
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1C(3);

		replica1A.insertVertice(20);
		replica1B.insertVertice(21);
		replica1C.insertVertice(22);

		replica1A.insertEdge(std::pair<uint32_t, uint32_t>(20, 21));
		replica1B.insertEdge(std::pair<uint32_t, uint32_t>(21, 22));
		replica1C.insertEdge(std::pair<uint32_t, uint32_t>(22, 23));

		REQUIRE(replica1A.serialize() == "{\"edges\":[[20,21]],\"edges_tombstone\":[],\"id\":1,\"vertices\":[20],\"vertices_tombstone\":[]}");
		REQUIRE(replica1B.serialize() == "{\"edges\":[[21,22]],\"edges_tombstone\":[],\"id\":2,\"vertices\":[21],\"vertices_tombstone\":[]}");
		REQUIRE(replica1C.serialize() == "{\"edges\":[[22,23]],\"edges_tombstone\":[],\"id\":3,\"vertices\":[22],\"vertices_tombstone\":[]}");
	}
	SECTION("Test deserialize function")
	{
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1A(1);
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1B(2);
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1C(3);

		replica1A.insertVertice(20);
		replica1B.insertVertice(21);
		replica1C.insertVertice(22);

		replica1A.insertEdge(std::pair<uint32_t, uint32_t>(20, 21));
		replica1B.insertEdge(std::pair<uint32_t, uint32_t>(21, 22));
		replica1C.insertEdge(std::pair<uint32_t, uint32_t>(22, 23));

		REQUIRE(replica1A.serialize() == "{\"edges\":[[20,21]],\"edges_tombstone\":[],\"id\":1,\"vertices\":[20],\"vertices_tombstone\":[]}");
		REQUIRE(replica1B.serialize() == "{\"edges\":[[21,22]],\"edges_tombstone\":[],\"id\":2,\"vertices\":[21],\"vertices_tombstone\":[]}");
		REQUIRE(replica1C.serialize() == "{\"edges\":[[22,23]],\"edges_tombstone\":[],\"id\":3,\"vertices\":[22],\"vertices_tombstone\":[]}");
	
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica2A;
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica2B;
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica2C;

		replica2A.deserialize(replica1A.serialize());
		replica2B.deserialize(replica1B.serialize());
		replica2C.deserialize(replica1C.serialize());

		REQUIRE(replica2A.serialize() == "{\"edges\":[[20,21]],\"edges_tombstone\":[],\"id\":1,\"vertices\":[20],\"vertices_tombstone\":[]}");
		REQUIRE(replica2B.serialize() == "{\"edges\":[[21,22]],\"edges_tombstone\":[],\"id\":2,\"vertices\":[21],\"vertices_tombstone\":[]}");
		REQUIRE(replica2C.serialize() == "{\"edges\":[[22,23]],\"edges_tombstone\":[],\"id\":3,\"vertices\":[22],\"vertices_tombstone\":[]}");
	}
#ifdef LOCAL_TESTING
	SECTION("Test serialize function saving to a file")
	{
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1A(1);
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1B(2);
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1C(3);

		replica1A.insertVertice(20);
		replica1B.insertVertice(21);
		replica1C.insertVertice(22);

		replica1A.insertEdge(std::pair<uint32_t, uint32_t>(20, 21));
		replica1B.insertEdge(std::pair<uint32_t, uint32_t>(21, 22));
		replica1C.insertEdge(std::pair<uint32_t, uint32_t>(22, 23));

		replica1A.serializeFile("../../tests/temp_data/twoptwopgraph1A.json");
		replica1B.serializeFile("../../tests/temp_data/twoptwopgraph1B.json");
		replica1C.serializeFile("../../tests/temp_data/twoptwopgraph1C.json");

        std::string replica1AString;
        std::ifstream replica1Ai("../../tests/temp_data/twoptwopgraph1A.json");
        replica1Ai >> replica1AString;

        std::string replica1BString;
        std::ifstream replica1Bi("../../tests/temp_data/twoptwopgraph1B.json");
        replica1Bi >> replica1BString;

        std::string replica1CString;
        std::ifstream replica1Ci("../../tests/temp_data/twoptwopgraph1C.json");
        replica1Ci >> replica1CString;

        REQUIRE(replica1AString == "{\"edges\":[[20,21]],\"edges_tombstone\":[],\"id\":1,\"vertices\":[20],\"vertices_tombstone\":[]}");
		REQUIRE(replica1BString == "{\"edges\":[[21,22]],\"edges_tombstone\":[],\"id\":2,\"vertices\":[21],\"vertices_tombstone\":[]}");
		REQUIRE(replica1CString == "{\"edges\":[[22,23]],\"edges_tombstone\":[],\"id\":3,\"vertices\":[22],\"vertices_tombstone\":[]}");
	}
#endif
#ifdef LOCAL_TESTING
	SECTION("Test deserialize function reading from a file")
	{
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1A(1);
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1B(2);
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica1C(3);

		replica1A.insertVertice(20);
		replica1B.insertVertice(21);
		replica1C.insertVertice(22);

		replica1A.insertEdge(std::pair<uint32_t, uint32_t>(20, 21));
		replica1B.insertEdge(std::pair<uint32_t, uint32_t>(21, 22));
		replica1C.insertEdge(std::pair<uint32_t, uint32_t>(22, 23));

		replica1A.serializeFile("../../tests/temp_data/twoptwopgraph1A.json");
		replica1B.serializeFile("../../tests/temp_data/twoptwopgraph1B.json");
		replica1C.serializeFile("../../tests/temp_data/twoptwopgraph1C.json");

		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica2A;
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica2B;
		crdt::state::TwoPTwoPGraphMetadata<uint32_t> replica2C;

		replica2A.deserializeFile("../../tests/temp_data/twoptwopgraph1A.json");
		replica2B.deserializeFile("../../tests/temp_data/twoptwopgraph1B.json");
		replica2C.deserializeFile("../../tests/temp_data/twoptwopgraph1C.json");

		REQUIRE(replica1A.serialize() == replica2A.serialize());
		REQUIRE(replica1B.serialize() == replica2B.serialize());
		REQUIRE(replica1C.serialize() == replica2C.serialize());
	}
#endif
}

// Performance Benchmark
TEST_CASE("Performance Benchmark", "[classic]")
{
	SECTION("Performance benchmark for OB-Counter")
	{
		std::cout<<"Performance benchmark for OB-Counter: \n";

		//std::clock_t start;
		long double duration = 0;

		for (int i = 0; i < 50; i++) {

			crdt::operation::CounterOB<uint32_t> obj1(1);
			crdt::operation::CounterOB<uint32_t> obj2(2);
			crdt::operation::CounterOB<uint32_t> obj3;
			crdt::operation::CounterOB<uint32_t> obj4;

			//start = std::clock();
			auto t1 = std::chrono::high_resolution_clock::now();

			obj3 = obj1 + obj2;
			obj4 = obj2 - obj1;
			
			auto t2 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			//duration += ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
			
			REQUIRE(obj3.query_payload() == 3);
			REQUIRE(obj4.query_payload() == 1);
		}
		std::cout<< "   Averge merging time: " << (duration/100.0) << " nanoseconds \n";
	}

	SECTION("Performance benchmark for G-Set")
	{
		std::cout<<"Performance benchmark for G-Set: \n";

		long double duration = 0;

		

		for (int i = 0; i < 100; i++) {

			crdt::state::GSetSB<uint32_t> handler4(4);
			crdt::state::GSetMetadata<uint32_t> replicaA(5,{3,6,9});
			crdt::state::GSetMetadata<uint32_t> replicaB(5,{2,4,6});
			crdt::state::GSetMetadata<uint32_t> replicaC(5,{1,2,3});
			
			handler4.addExternalReplica({replicaA, replicaB, replicaC});
			handler4.insert(5,{3,6,9});
			handler4.insert(5,{2,4,6});
			handler4.insert(5,{1,2,3});
			handler4.insert(2,{10,11});
			auto t1 = std::chrono::high_resolution_clock::now();
			handler4.updateInternalPayload();
			auto t2 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			std::set<uint32_t> test = {1,2,3,4,6,9};
			REQUIRE(handler4.queryPayload() == test);
			REQUIRE(handler4.queryPayloadwithID(5) == test);
		}
		std::cout<< "   Averge merging time: " << (duration/100.0) << " nanoseconds \n";
	}

	SECTION("Performance benchmark for 2P-Set")
	{
		std::cout<<"Performance benchmark for 2P-Set: \n";

		long double duration = 0;

		for (int i = 0; i < 50; i++) {

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

			auto t1 = std::chrono::high_resolution_clock::now();
			handler1.updateInternalPayload();
			auto t2 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			
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
			
			replicaB.remove(2);
			replicaE.remove(6);
			replicaE.remove(9);
			
			handler1.addExternalReplica({replicaA,replicaD});
			handler2.addExternalReplica({replicaB,replicaE});

			auto t3 = std::chrono::high_resolution_clock::now();
			handler1.updateInternalPayload();
			auto t4 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count();
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
		}
		std::cout<< "   Averge merging time: " << (duration/100.0) << " nanoseconds \n";
	}

	SECTION("Performance benchmark for Vector")
	{
		std::cout<<"Performance benchmark for Vector: \n";

		long double duration = 0;

		for (int i = 0; i < 33; i++) {

			
			crdt::state::VectorSB<uint32_t> handler1(1); //Represents Server 1
			crdt::state::VectorSB<uint32_t> handler2(2); //Represents Server 1.2
			crdt::state::VectorSB<uint32_t> handler3(3); //Represents Server 1.3
			crdt::state::VectorMetadata<uint32_t> replicaA(4,{3,6,9});
			crdt::state::VectorMetadata<uint32_t> replicaB(4,{2,4,6});
			crdt::state::VectorMetadata<uint32_t> replicaC(4,{1,2,3,6,6});
			auto t1 = std::chrono::high_resolution_clock::now();
			handler1.addExternalReplica({replicaA});
			handler2.addExternalReplica({replicaB});
			handler3.addExternalReplica({replicaC});
			auto t2 = std::chrono::high_resolution_clock::now();
			
			handler1.updateLocalExternalPayload({handler1,handler2,handler3}); // merge
			handler2.updateLocalExternalPayload({handler1,handler2,handler3});
			handler3.updateLocalExternalPayload({handler1,handler2,handler3});

			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			REQUIRE(handler1.queryPayloadwithID(4) == handler2.queryPayloadwithID(4));
			REQUIRE(handler1.queryPayloadwithID(4) == handler3.queryPayloadwithID(4));
			REQUIRE(handler2.queryPayloadwithID(4) == handler3.queryPayloadwithID(4));
			std::vector<uint32_t> test = {1,2,3,4,6,6,9};
			REQUIRE(handler1.queryPayload() == test);
			REQUIRE(handler2.queryPayload() == test);
			REQUIRE(handler3.queryPayload() == test);
		}
		std::cout<< "   Averge merging time: " << (duration/100.0) << " nanoseconds \n";
	}

	SECTION("Performance benchmark for OR-Set")
	{
		std::cout<<"Performance benchmark for OR-Set: \n";

		long double duration = 0;

		for (int i = 0; i < 20; i++) {

			crdt::state::ORSetSB<uint32_t> handler1(1); //Represents Server 1
			crdt::state::ORSetSB<uint32_t> handler2(2); //Represents Server 1.2
			crdt::state::ORSetSB<uint32_t> handler3(3); //Represents Server 1.3
			crdt::state::ORSetMetadata<uint32_t> replicaA(4,{3,6,9});
			crdt::state::ORSetMetadata<uint32_t> replicaB(4,{2,4,6});
			crdt::state::ORSetMetadata<uint32_t> replicaC(4,{1,2,3});
			crdt::state::ORSetMetadata<uint32_t> replicaD(5,{1,2,3,6,9});
			crdt::state::ORSetMetadata<uint32_t> replicaE(6,{2,3,6,9,10});
			auto t1 = std::chrono::high_resolution_clock::now();
			handler1.addExternalReplica({replicaA,replicaD});
			handler2.addExternalReplica({replicaB,replicaE});
			handler3.addExternalReplica({replicaC});
			auto t2 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			
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

			auto t3 = std::chrono::high_resolution_clock::now();
			handler1.addExternalReplica({replicaA,replicaD});
			handler2.addExternalReplica({replicaB,replicaE},{{replicaB.queryId(),removed_itemsB}, {replicaE.queryId(),removed_itemsE}});
			auto t4 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count();
			handler3.addExternalReplica({replicaC});
			
			handler1.updateLocalExternalPayload({handler1,handler2,handler3},{{replicaB.queryId(),removed_itemsB}, {replicaE.queryId(),removed_itemsE}}); // merge
			handler2.updateLocalExternalPayload({handler1,handler2,handler3},{{replicaB.queryId(),removed_itemsB}, {replicaE.queryId(),removed_itemsE}});
			handler3.updateLocalExternalPayload({handler1,handler2,handler3},{{replicaB.queryId(),removed_itemsB}, {replicaE.queryId(),removed_itemsE}});

		}
		std::cout<< "   Averge merging time: " << (duration/100.0) << " nanoseconds \n";
	}

	SECTION("Performance benchmark for G-Counter")
	{
		std::cout<<"Performance benchmark for G-Counter: \n";

		long double duration = 0;

		for (int i = 0; i < 100; i++) {

			crdt::state::GCounterSB<uint32_t> handler(1); //Represents Server 1
			/* Belongs to Server 1 */
			crdt::state::GCounterMetadata<uint32_t> replica1A(2,6);
			replica1A.updatePayload(7);
			replica1A.updatePayload(8);
			
			handler.addExternalReplica({replica1A});
			REQUIRE(handler.queryPayload() == 21);

			crdt::state::GCounterSB<uint32_t> handler2(3); //Represents Server 2
			/* Belongs to Server 2 */
			crdt::state::GCounterMetadata<uint32_t> replica2A(4,6);
			replica2A.updatePayload(3);
			replica2A.updatePayload(5);

			handler2.addExternalReplica({replica2A});
			REQUIRE(handler2.queryPayload() == 14);

			crdt::state::GCounterSB<uint32_t> handler3(5); //Represents Server 3
			crdt::state::GCounterSB<uint32_t> handler4(6); //Represents Server 4
			crdt::state::GCounterMetadata<uint32_t> replica4A(7,10);
			crdt::state::GCounterMetadata<uint32_t> replica4B(8,10);
			replica4B.updatePayload(300);
			crdt::state::GCounterMetadata<uint32_t> replica4C(9,15);
			auto t1 = std::chrono::high_resolution_clock::now();
			handler4.addExternalReplica({replica4A,replica4B,replica4C});
			auto t2 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			REQUIRE(handler4.queryPayload() == 335);
			
			handler.updateLocalExternalPayload({handler,handler2,handler3,handler4});
			handler2.updateLocalExternalPayload({handler,handler2,handler3,handler4});
			handler3.updateLocalExternalPayload({handler,handler2,handler3,handler4});
			handler4.updateLocalExternalPayload({handler,handler2,handler3,handler4});			
			
			
			REQUIRE(handler.queryPayload() == 370);
			REQUIRE(handler.queryPayload() == handler2.queryPayload());
			REQUIRE(handler3.queryPayload() == handler4.queryPayload());
			REQUIRE(handler.queryPayload() == handler4.queryPayload());

			
		}
		std::cout<< "   Averge merging time: " << (duration/100.0) << " nanoseconds \n";
	}

	SECTION("Performance benchmark for PN-Counter")
	{
		std::cout<<"Performance benchmark for PN-Counter: \n";

		long double duration = 0;

		for (int i = 0; i < 100; i++) {

			crdt::state::PNCounterSB<uint32_t> handler1(1); //Represents Server 1
			crdt::state::PNCounterSB<uint32_t> handler2(2); //Represents Server 2
			crdt::state::PNCounterSB<uint32_t> handler3(3); //Represents Server 3
			crdt::state::PNCounterMetadata<uint32_t> replica1A(1,10);
			crdt::state::PNCounterMetadata<uint32_t> replica1B(2,10);
			crdt::state::PNCounterMetadata<uint32_t> replica2A(3,10);
			crdt::state::PNCounterMetadata<uint32_t> replica2B(4,10);
			crdt::state::PNCounterMetadata<uint32_t> replica3A(5,10);
			crdt::state::PNCounterMetadata<uint32_t> replica3B(6,10);
			auto t1 = std::chrono::high_resolution_clock::now();
			handler1.addExternalReplica({replica1A,replica1B});
			handler2.addExternalReplica({replica2A,replica2B});
			handler3.addExternalReplica({replica3A,replica3B});
			auto t2 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

			replica1A.decreasePayload(5);
			replica1B.decreasePayload(5);
			handler1.addExternalReplica({replica1A,replica1B});
			replica2A.decreasePayload(5);
			replica2B.decreasePayload(5);
			handler2.addExternalReplica({replica2A,replica2B});
			replica3A.decreasePayload(5);
			replica3B.decreasePayload(5);
			handler3.addExternalReplica({replica3A,replica3B});
			
			handler1.updateLocalExternalPayload({handler1,handler2,handler3});
			handler2.updateLocalExternalPayload({handler1,handler2,handler3});
			handler3.updateLocalExternalPayload({handler1,handler2,handler3});
			
			REQUIRE(handler1.queryPayload() == 30);
			REQUIRE(handler1.queryPayload() == handler2.queryPayload());
			REQUIRE(handler2.queryPayload() == handler3.queryPayload());
			replica1A.increasePayload(20);
			handler1.addExternalReplica({replica1A});
			
			handler1.updateLocalExternalPayload({handler1,handler2,handler3});
			handler2.updateLocalExternalPayload({handler2,handler1,handler3});
			handler3.updateLocalExternalPayload({handler3,handler1,handler2});

			auto t3 = std::chrono::high_resolution_clock::now();

			handler1.updateInternalPayload();

			auto t4 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count();

			REQUIRE(handler1.queryPayload() == 50);
			REQUIRE(handler1.queryPayload() == handler2.queryPayload());
			REQUIRE(handler2.queryPayload() == handler3.queryPayload());
			replica1A.increasePayload(50);
			replica1B.increasePayload(50);
			replica3A.increasePayload(50);
			handler1.addExternalReplica({replica3A}); // Server 1 Conflict With Server 3
			handler2.addExternalReplica({replica1B}); // Server 2 Conflict with Server 1
			handler3.addExternalReplica({replica1A}); // Server 3 Conflict with Server 1
			
			handler1.updateLocalExternalPayload({handler1,handler2,handler3});
			handler2.updateLocalExternalPayload({handler1,handler2,handler3});
			handler3.updateLocalExternalPayload({handler1,handler2,handler3});
			
			auto t5 = std::chrono::high_resolution_clock::now();

			handler1.updateInternalPayload();

			auto t6 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t6 - t5).count();

			REQUIRE(handler1.queryPayloadwithID(1) == handler2.queryPayloadwithID(1));
			REQUIRE(handler2.queryPayloadwithID(1) == handler3.queryPayloadwithID(1));
			REQUIRE(handler1.queryPayload() == 200);
			REQUIRE(handler1.queryPayload() == handler2.queryPayload());
			REQUIRE(handler2.queryPayload() == handler3.queryPayload());
			
		}
		std::cout<< "   Averge merging time: " << (duration/100.0) << " nanoseconds \n";
	}

	SECTION("Performance benchmark for G-Map")
	{
		std::cout<<"Performance benchmark for G-Map: \n";

		long double duration = 0;

		for (int i = 0; i < 50; i++) {

			crdt::state::GMapSB<uint32_t, uint32_t> handler(1); //Represents Server 1
			crdt::state::GMapSB<uint32_t, uint32_t> handler2(2); //Represents Server 2
			crdt::state::GMapSB<uint32_t, uint32_t> handler3(3); //Represents Server 3
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1A(0,10,1);
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1B(0,10,2);
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1C(0,10,3);
			replica1A.insert(25,100);
			handler.addExternalReplica({replica1A});
			REQUIRE(handler.queryPayloadwithID(0,25) == 100);
			handler2.addExternalReplica({replica1B});
			handler3.addExternalReplica({replica1C});
			
			REQUIRE(handler2.queryPayloadwithID(0,10) != handler.queryPayloadwithID(0,10));
			REQUIRE(handler3.queryPayloadwithID(0,10) != handler2.queryPayloadwithID(0,10));
			
			
			handler2.updateLocalExternalPayload({handler,handler2,handler3});
			handler3.updateLocalExternalPayload({handler,handler2,handler3});

			auto t1 = std::chrono::high_resolution_clock::now();
			
			handler.updateLocalExternalPayload({handler,handler2,handler3});
			
			auto t2 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			
			REQUIRE(handler2.queryPayloadwithID(0,10) == handler.queryPayloadwithID(0,10));
			REQUIRE(handler3.queryPayloadwithID(0,10) == handler2.queryPayloadwithID(0,10));
			
			REQUIRE(handler.queryPayloadwithID(0,25) == 100);
			REQUIRE(handler2.queryPayloadwithID(0,25) == 100);
			REQUIRE(handler3.queryPayloadwithID(0,25) == 100);


			
		}
		std::cout<< "   Averge merging time: " << (duration/100.0) << " nanoseconds \n";
	}

	SECTION("Performance benchmark for G-Map (String)")
	{
		std::cout<<"Performance benchmark for G-Map (String): \n";

		long double duration = 0;

		for (int i = 0; i < 33; i++) {

			crdt::state::GMapSBString<uint32_t, std::string> handler1(1);
		crdt::state::GMapSBString<uint32_t, std::string> handler2(2); 
		crdt::state::GMapSBString<uint32_t, std::string> handler3(3);
		crdt::state::GMapMetadata<uint32_t, std::string> replica1A(0,10,"Hello");
		crdt::state::GMapMetadata<uint32_t, std::string> replica1B(0,10,"HelloMelo");
		crdt::state::GMapMetadata<uint32_t, std::string> replica1C(0,10,"Hello Hello");
		handler1.addExternalReplica({replica1A});
		handler2.addExternalReplica({replica1B});
		handler3.addExternalReplica({replica1C});
		REQUIRE(handler2.queryPayloadwithID(0,10) != handler1.queryPayloadwithID(0,10));
		REQUIRE(handler3.queryPayloadwithID(0,10) != handler2.queryPayloadwithID(0,10));
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler3.queryPayloadwithID(0,10) == "Hello Hello HelloMelo");
		REQUIRE(handler2.queryPayloadwithID(0,10) == handler1.queryPayloadwithID(0,10));
		REQUIRE(handler3.queryPayloadwithID(0,10) == handler2.queryPayloadwithID(0,10));
		replica1A.insert(1,"Hello, my name is Bob!");
		handler1.addExternalReplica({replica1A});
		auto t1 = std::chrono::high_resolution_clock::now(); //First Merge
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		auto t2 = std::chrono::high_resolution_clock::now();
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count(); //First Merge Complete
		REQUIRE(handler1.queryPayloadwithID(0,1) == handler2.queryPayloadwithID(0,1));
		REQUIRE(handler2.queryPayloadwithID(0,1) == handler3.queryPayloadwithID(0,1));
		crdt::state::GMapMetadata<uint32_t, std::string> replica1D(1,10,"ABC");
		handler3.addExternalReplica({replica1D});
		replica1D.insert(10,"DEF");
		handler2.addExternalReplica({replica1D});
		replica1D.insert(10,"U");
		handler1.addExternalReplica({replica1D});
		auto t3 = std::chrono::high_resolution_clock::now(); //Second Merge
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		auto t4 = std::chrono::high_resolution_clock::now(); //Second Merge Complete
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		
		duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count(); //Second Merge Complete
		REQUIRE(handler1.queryPayloadwithID(1,10) == "ABC DEF U");
		REQUIRE(handler1.queryPayloadwithID(1,10) == handler2.queryPayloadwithID(1,10));
		REQUIRE(handler2.queryPayloadwithID(1,10) == handler3.queryPayloadwithID(1,10));
		crdt::state::GMapMetadata<uint32_t, std::string> replica1E(2,10,"ASDHUIFDHIUSDHFUI");
		handler2.addExternalReplica({replica1E});
		auto t5 = std::chrono::high_resolution_clock::now(); //Third Merge
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		auto t6 = std::chrono::high_resolution_clock::now(); //Third Merge Complete
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		
		duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t6 - t5).count(); //Second Merge Complete

		REQUIRE(handler1.queryPayloadwithID(2,10) == handler2.queryPayloadwithID(2,10));
		REQUIRE(handler2.queryPayloadwithID(2,10) == handler3.queryPayloadwithID(2,10));
		REQUIRE(handler1.queryPayload(1) == handler2.queryPayload(1));
		REQUIRE(handler2.queryPayload(1) == handler3.queryPayload(1));
		std::vector<uint32_t> test1 = {1,10};
		REQUIRE(handler1.queryAllKeys() == test1);
		REQUIRE(handler2.queryAllKeys() == test1);
		REQUIRE(handler3.queryAllKeys() == test1);
		std::vector<std::string> test2 = { "Bob! Hello, is my name", "ABC ASDHUIFDHIUSDHFUI DEF Hello Hello HelloMelo U"};
		REQUIRE(handler1.queryAllValues() == test2);
		REQUIRE(handler2.queryAllValues() == test2);
		REQUIRE(handler3.queryAllValues() == test2);			
		}
		std::cout<< "   Averge merging time: " << (duration/100.0) << " nanoseconds \n";
	}



	SECTION("Performance benchmark for Priority Queue")
	{
		std::cout<<"Performance benchmark for Priority Queue: \n";

		long double duration = 0;

		for (int i = 0; i < 33; i++) {

			crdt::state::PriorityQueueSB<uint32_t> handler1(2);
			crdt::state::PriorityQueueSB<uint32_t> handler2(2);
			crdt::state::PriorityQueueSB<uint32_t> handler3(3);
			crdt::state::PriorityQueueMetadata<uint32_t> replica1A(0);
			crdt::state::PriorityQueueMetadata<uint32_t> replica1B(1);
			crdt::state::PriorityQueueMetadata<uint32_t> replica1C(2);
			replica1A.push({1,2,3});
			replica1B.push({4,5,6});
			replica1C.push({7,8,9});
			handler1.addExternalReplica({replica1A,replica1B,replica1C});
			std::vector<uint32_t> test1 = {9,8,7,6,5,4,3,2,1};
			REQUIRE(handler1.queryPayloadVector() == test1);
			crdt::state::PriorityQueueMetadata<uint32_t> replica2A(0);
			crdt::state::PriorityQueueMetadata<uint32_t> replica2B(1);
			crdt::state::PriorityQueueMetadata<uint32_t> replica2C(2);
			replica2A.push({10,11,12});
			replica2B.push({13,14,15});
			replica2C.push({16,17,18});
			handler2.addExternalReplica({replica2A,replica2B,replica2C});
			std::vector<uint32_t> test2 = {18,17,16,15,14,13,12,11,10};
			REQUIRE(handler2.queryPayloadVector() == test2);
			crdt::state::PriorityQueueMetadata<uint32_t> replica3A(0);
			crdt::state::PriorityQueueMetadata<uint32_t> replica3B(1);
			crdt::state::PriorityQueueMetadata<uint32_t> replica3C(2);
			replica3A.push({19,20,21});
			replica3B.push({22,23,24});
			replica3C.push({25,26,27});
			handler3.addExternalReplica({replica3A,replica3B,replica3C});
			std::vector<uint32_t> test3 = {27,26,25,24,23,22,21,20,19};
			REQUIRE(handler3.queryPayloadVector() == test3);
			
			handler1.updateLocalExternalPayload({handler1,handler2,handler3});
			handler2.updateLocalExternalPayload({handler1,handler2,handler3});
			handler3.updateLocalExternalPayload({handler1,handler2,handler3});

			auto t1 = std::chrono::high_resolution_clock::now();

			handler1.updateInternalPayload();
			
			auto t2 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			
			REQUIRE(handler1.queryPayloadVector() == handler2.queryPayloadVector());
			REQUIRE(handler2.queryPayloadVector() == handler3.queryPayloadVector());
			crdt::state::PriorityQueueMetadata<uint32_t> replica3D(3);
			replica3D.push({100,200,300});
			handler3.addExternalReplica({replica3D});
			
			handler1.updateLocalExternalPayload({handler1,handler2,handler3});
			handler2.updateLocalExternalPayload({handler1,handler2,handler3});
			handler3.updateLocalExternalPayload({handler1,handler2,handler3});
			
			auto t3 = std::chrono::high_resolution_clock::now();

			handler3.updateInternalPayload();
			
			auto t4 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count();

			REQUIRE(handler1.queryPayloadVector() == handler2.queryPayloadVector());
			REQUIRE(handler2.queryPayloadVector() == handler3.queryPayloadVector());
			replica3D.push({400,500});
			handler1.addExternalReplica({replica3D});
			replica3D.push({1000,2000,564});
			handler2.addExternalReplica({replica3D});
			
			handler1.updateLocalExternalPayload({handler1,handler2,handler3});
			handler2.updateLocalExternalPayload({handler1,handler2,handler3});
			handler3.updateLocalExternalPayload({handler1,handler2,handler3});

			auto t5 = std::chrono::high_resolution_clock::now();

			handler1.updateInternalPayload();
			
			auto t6 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t6 - t5).count();

			REQUIRE(handler1.convertPQtoVector(handler1.queryPayloadwithID(0)) == handler2.convertPQtoVector(handler2.queryPayloadwithID(0)));
			REQUIRE(handler2.convertPQtoVector(handler2.queryPayloadwithID(0)) == handler3.convertPQtoVector(handler3.queryPayloadwithID(0)));
			REQUIRE(handler1.queryPayloadVector() == handler2.queryPayloadVector());
			REQUIRE(handler2.queryPayloadVector() == handler3.queryPayloadVector());
		}
		std::cout<< "   Averge merging time: " << (duration/100.0) << " nanoseconds \n";
	}

	SECTION("Performance benchmark for Multiset")
	{
		std::cout<<"Performance benchmark for Multiset: \n";

		long double duration = 0;

		for (int i = 0; i < 33; i++) {

			crdt::state::MultiSetSB<uint32_t> handler1(2);
			crdt::state::MultiSetSB<uint32_t> handler2(2);
			crdt::state::MultiSetSB<uint32_t> handler3(3);
			crdt::state::MultiSetMetadata<uint32_t> replica1A(0);
			crdt::state::MultiSetMetadata<uint32_t> replica1B(1);
			crdt::state::MultiSetMetadata<uint32_t> replica1C(2);
			replica1A.insert({1,2,3});
			replica1B.insert({4,5,6});
			replica1C.insert({7,8,9});
			auto t1 = std::chrono::high_resolution_clock::now();
			handler1.addExternalReplica({replica1A,replica1B,replica1C});
			auto t2 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			std::multiset<uint32_t> test1 = {1,2,3,4,5,6,7,8,9};
			REQUIRE(handler1.queryPayload() == test1);

			crdt::state::MultiSetMetadata<uint32_t> replica2A(0);
			crdt::state::MultiSetMetadata<uint32_t> replica2B(1);
			crdt::state::MultiSetMetadata<uint32_t> replica2C(2);
			replica2A.insert({10,11,12});
			replica2B.insert({13,14,15});
			replica2C.insert({16,17,18});
			auto t3 = std::chrono::high_resolution_clock::now();
			handler2.addExternalReplica({replica2A,replica2B,replica2C});
			auto t4 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count();
			std::multiset<uint32_t> test2 = {10,11,12,13,14,15,16,17,18};
			REQUIRE(handler2.queryPayload() == test2);
			crdt::state::MultiSetMetadata<uint32_t> replica3A(0);
			crdt::state::MultiSetMetadata<uint32_t> replica3B(1);
			crdt::state::MultiSetMetadata<uint32_t> replica3C(2);
			replica3A.insert({19,20,21});
			replica3B.insert({22,23,24});
			replica3C.insert({25,26,27});
			auto t5 = std::chrono::high_resolution_clock::now();
			handler3.addExternalReplica({replica3A,replica3B,replica3C});
			auto t6 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t6 - t5).count();
			std::multiset<uint32_t> test3 = {19,20,21,22,23,24,25,26,27};
			REQUIRE(handler3.queryPayload() == test3);
			
			handler1.updateLocalExternalPayload({handler1,handler2,handler3});
			handler2.updateLocalExternalPayload({handler1,handler2,handler3});
			handler3.updateLocalExternalPayload({handler1,handler2,handler3});
			
			REQUIRE(handler1.queryPayload()  == handler2.queryPayload());
			REQUIRE(handler2.queryPayload()  == handler3.queryPayload());
			
		}
		std::cout<< "   Averge merging time: " << (duration/100.0) << " nanoseconds \n";
	}

	SECTION("Performance benchmark for LWW-Multiset")
	{
		std::cout<<"Performance benchmark for LWW-Multiset: \n";

		long double duration = 0;

		for (int i = 0; i < 100; i++) {

			crdt::state::LWWMultiSetSB<uint32_t> handler1(1);
			crdt::state::LWWMultiSetSB<uint32_t> handler2(2);
			crdt::state::LWWMultiSetSB<uint32_t> handler3(3);
			crdt::state::LWWMultiSetMetadata<uint32_t> replicaA(0,{5,5},0);
			crdt::state::LWWMultiSetMetadata<uint32_t> replicaB(0,{5,5,7},1);
			crdt::state::LWWMultiSetMetadata<uint32_t> replicaC(0,{5,10,15},2);
			crdt::state::LWWMultiSetMetadata<uint32_t> replicaD(1, {20,25}, 0);
			auto t1 = std::chrono::high_resolution_clock::now();
			handler1.addExternalReplica({replicaA,replicaD});
			auto t2 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			
			handler2.addExternalReplica({replicaB});
			handler3.addExternalReplica({replicaC});
			std::multiset<uint32_t> test1 = {5,10,15, 20, 25};
			
			handler1.updateLocalExternalPayload({handler1,handler2,handler3});
			handler2.updateLocalExternalPayload({handler1,handler2,handler3});
			handler3.updateLocalExternalPayload({handler1,handler2,handler3});

			//auto t1 = std::chrono::high_resolution_clock::now();

			handler1.updateInternalPayload();
			
			//auto t2 = std::chrono::high_resolution_clock::now();
			//duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			
			REQUIRE(handler1.queryPayload() == test1);
			REQUIRE(handler1.queryPayload() == handler2.queryPayload());
			REQUIRE(handler2.queryPayload() == handler3.queryPayload());
			
		}
		std::cout<< "   Averge merging time: " << (duration/100.0) << " nanoseconds \n";
	}

	SECTION("Performance benchmark for String")
	{
		std::cout<<"Performance benchmark for String: \n";

		long double duration = 0;

		for (int i = 0; i < 100; i++) {

			crdt::operation::StringOB<std::string> handler1(1);
			crdt::operation::StringOB<std::string> handler2(2);
			crdt::operation::StringOB<std::string> handler3(3);
			crdt::operation::StringMetaData<std::string> replicaA(0, "Hello           is");
			crdt::operation::StringMetaData<std::string> replicaB(0, "Hello My");
			crdt::operation::StringMetaData<std::string> replicaC(0, "Hello      name is Bob");
			
			auto t1 = std::chrono::high_resolution_clock::now();
			handler1.addExternalReplica({replicaA});
			handler2.addExternalReplica({replicaB});
			handler3.addExternalReplica({replicaC});
			auto t2 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
			
			handler1.updateLocalExternalPayload({handler1,handler2,handler3});
			handler2.updateLocalExternalPayload({handler1,handler2,handler3});
			handler3.updateLocalExternalPayload({handler1,handler2,handler3});
			
			crdt::operation::StringMetaData<std::string> replicaD(1, "I WAS NOT EXPECTING IT");
			crdt::operation::StringMetaData<std::string> replicaE(1, "I WAS EXPECTING IT");
			handler1.addExternalReplica({replicaD});
			handler2.addExternalReplica({replicaE});
			REQUIRE(handler1.queryPayloadwithID(0) == "Hello   My      name is Bob");
			REQUIRE(handler1.queryPayloadwithID(0) == handler2.queryPayloadwithID(0));
			REQUIRE(handler2.queryPayloadwithID(0) == handler3.queryPayloadwithID(0));
			
		}
		std::cout<< "   Averge merging time: " << (duration/100.0) << " nanoseconds \n";
	}
	SECTION("Performance benchmark for TwoPTwoPGraph")
	{
		std::cout<< "Performance benchmark for TwoPTwoPGraph: \n";

		long double duration = 0;

		for (int i = 0; i < 100; i++) {

			crdt::state::TwoPTwoPGraphSB<uint32_t> handler1(0);
			crdt::state::TwoPTwoPGraphSB<uint32_t> handler2(1);
			crdt::state::TwoPTwoPGraphSB<uint32_t> handler3(2);
			crdt::state::TwoPTwoPGraphMetadata<uint32_t> replicaA(20);
			crdt::state::TwoPTwoPGraphMetadata<uint32_t> replicaB(30);
			crdt::state::TwoPTwoPGraphMetadata<uint32_t> replicaC(40);

			replicaA.insertVertice(20);
			replicaA.insertVertice(21);
			replicaA.insertVertice(22);
			replicaB.insertVertice(23);
			replicaB.insertVertice(24);
			replicaB.insertVertice(25);
			replicaC.insertVertice(26);
			replicaC.insertVertice(27);
			replicaC.insertVertice(28);

			std::set<uint32_t> tempA = {20, 21, 22};
			std::set<uint32_t> tempB = {23, 24, 25};
			std::set<uint32_t> tempC = {26, 27, 28};

			auto t1 = std::chrono::high_resolution_clock::now();
			handler1.addExternalReplica({replicaA});
			handler2.addExternalReplica({replicaB});
			handler3.addExternalReplica({replicaC});
			auto t2 = std::chrono::high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
		}
		std::cout<< "   Averge merging time: " << (duration/100.0) << " nanoseconds \n";
	}
}
