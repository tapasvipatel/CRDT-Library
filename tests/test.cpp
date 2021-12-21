#include <iostream>
#include <vector>
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
		crdt::state::GCounterSB<uint32_t> handler(1, 0); //Represents Server 1
		/* Belongs to Server 1 */
		crdt::state::GCounterMetadata<uint32_t> replica1A(2,6);
		crdt::state::GCounterMetadata<uint32_t> replica1B(3,7);
		crdt::state::GCounterMetadata<uint32_t> replica1C(4,8);
		handler.addExternalReplica({replica1A,replica1B,replica1C});
		handler.updateInternalPayload();

		crdt::state::GCounterSB<uint32_t> handler2(5, 0); //Represents Server 2
		/* Belongs to Server 2 */
		crdt::state::GCounterMetadata<uint32_t> replica2A(6,6);
		crdt::state::GCounterMetadata<uint32_t> replica2B(7,3);
		crdt::state::GCounterMetadata<uint32_t> replica2C(8,5);
		handler2.addExternalReplica({replica2A,replica2B,replica2C});
		handler2.updateInternalPayload();

		REQUIRE(handler.queryPayload() == 21);
		REQUIRE(handler2.queryPayload() == 14);
	}
	SECTION("Test Merge Operation")
	{
		crdt::state::GCounterSB<uint32_t> handler(1, 0); //Represents Server 1
		/* Belongs to Server 1 */
		crdt::state::GCounterMetadata<uint32_t> replica1A(2,6);
		replica1A.updatePayload(7);
		replica1A.updatePayload(8);
		
		handler.addExternalReplica({replica1A});
		handler.updateInternalPayload();
		REQUIRE(handler.queryPayload() == 21);

		crdt::state::GCounterSB<uint32_t> handler2(3, 0); //Represents Server 2
		/* Belongs to Server 2 */
		crdt::state::GCounterMetadata<uint32_t> replica2A(4,6);
		replica2A.updatePayload(3);
		replica2A.updatePayload(5);

		handler2.addExternalReplica({replica2A});
		handler2.updateInternalPayload();
		REQUIRE(handler.queryPayload() == 21);

		crdt::state::GCounterSB<uint32_t> handler3(5, 100); //Represents Server 3
		crdt::state::GCounterSB<uint32_t> handler4(6, 5); //Represents Server 4
		crdt::state::GCounterMetadata<uint32_t> replica4A(7,10);
		crdt::state::GCounterMetadata<uint32_t> replica4B(8,10);
		replica4B.updatePayload(300);
		crdt::state::GCounterMetadata<uint32_t> replica4C(9,15);
		handler4.addExternalReplica({replica4A,replica4B,replica4C});
		handler.updateLocalExternalPayload({handler2,handler3,handler4});
		handler2.updateLocalExternalPayload({handler,handler3,handler4});
		handler3.updateLocalExternalPayload({handler,handler2,handler4});
		handler4.updateLocalExternalPayload({handler,handler2,handler3});
		REQUIRE(handler.queryPayload() == handler2.queryPayload());
		REQUIRE(handler3.queryPayload() == handler4.queryPayload());
		REQUIRE(handler.queryPayload() == handler4.queryPayload());
	}
}
