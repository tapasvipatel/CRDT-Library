#include <iostream>
#include <vector>
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>

#include "../src/operation_based/CounterOB.hpp"
#include "../src/state_based/GCounterSB.hpp"
#include "../src/state_based/PNCounterSB.hpp"

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
		REQUIRE(handler2.queryPayload() == 14);

		crdt::state::GCounterSB<uint32_t> handler3(5, 100); //Represents Server 3
		crdt::state::GCounterSB<uint32_t> handler4(6, 5); //Represents Server 4
		crdt::state::GCounterMetadata<uint32_t> replica4A(7,10);
		crdt::state::GCounterMetadata<uint32_t> replica4B(8,10);
		replica4B.updatePayload(300);
		crdt::state::GCounterMetadata<uint32_t> replica4C(9,15);
		handler4.addExternalReplica({replica4A,replica4B,replica4C});
		handler4.updateInternalPayload();
		auto server1 = handler;
		auto server2 = handler2;
		auto server3 = handler3;
		auto server4 = handler4;
		handler.updateLocalExternalPayload({server1,server2,server3,server4});
		handler2.updateLocalExternalPayload({server1,server2,server3,server4});
		handler3.updateLocalExternalPayload({server1,server2,server3,server4});
		handler4.updateLocalExternalPayload({server1,server2,server3,server4});
		INFO(handler.queryPayload());
		REQUIRE(handler.queryPayload() == handler2.queryPayload());
		REQUIRE(handler3.queryPayload() == handler4.queryPayload());
		REQUIRE(handler.queryPayload() == handler4.queryPayload());

		// replica on server A decides to update
		replica1A.updatePayload(7);
		handler.addExternalReplica({replica1A});
		handler.updateInternalPayload();
		//30 s have passed and now we poll from all servers
		server1 = handler;
		server2 = handler2;
		server3 = handler3;
		server4 = handler4;
		handler.updateLocalExternalPayload({server1,server2,server3,server4});
		handler2.updateLocalExternalPayload({server1,server2,server3,server4});
		handler3.updateLocalExternalPayload({server1,server2,server3,server4});
		handler4.updateLocalExternalPayload({server1,server2,server3,server4});
		INFO(handler.queryPayload());
		REQUIRE(handler.queryPayload() == handler2.queryPayload());
		REQUIRE(handler3.queryPayload() == handler4.queryPayload());
		REQUIRE(handler.queryPayload() == handler4.queryPayload());
		replica2A.updatePayload(3);
		
		handler.addExternalReplica({replica2A}); //Server one will now cause conflict with server two
		handler.updateInternalPayload();
		//30 s have passed and now we poll from all servers
		server1 = handler;
		server2 = handler2;
		server3 = handler3;
		server4 = handler4;
		handler.updateLocalExternalPayload({server1,server2,server3,server4});
		handler2.updateLocalExternalPayload({server1,server2,server3,server4});
		handler3.updateLocalExternalPayload({server1,server2,server3,server4});
		handler4.updateLocalExternalPayload({server1,server2,server3,server4});
		INFO(handler.queryPayload());
		REQUIRE(handler.queryPayload() == handler2.queryPayload());
		REQUIRE(handler3.queryPayload() == handler4.queryPayload());
		REQUIRE(handler.queryPayload() == handler4.queryPayload());
	}
}

TEST_CASE("Test PNCounterSB", "[classic]")
{
	SECTION("Test Insert Operation")
	{
			crdt::state::PNCounterSB<uint32_t> handler(1, 0); //Represents Server 1
			/* Belongs to Server 1 */
			crdt::state::PNCounterMetadata<uint32_t> replica1A(2,6);
			crdt::state::PNCounterMetadata<uint32_t> replica1B(3,7);
			crdt::state::PNCounterMetadata<uint32_t> replica1C(4,8);
			handler.addExternalReplica({replica1A,replica1B,replica1C});
			handler.updateInternalPayload();

			crdt::state::PNCounterSB<uint32_t> handler2(5, 0); //Represents Server 2
			/* Belongs to Server 2 */
			crdt::state::PNCounterMetadata<uint32_t> replica2A(6,6);
			crdt::state::PNCounterMetadata<uint32_t> replica2B(7,3);
			crdt::state::PNCounterMetadata<uint32_t> replica2C(8,5);
			handler2.addExternalReplica({replica2A,replica2B,replica2C});
			handler2.updateInternalPayload();

			REQUIRE(handler.queryPayload() == 21);
			REQUIRE(handler2.queryPayload() == 14);
	}
	SECTION("Test Add/Subtract Operation")
	{
		crdt::state::PNCounterSB<uint32_t> handler(1, 0); //Represents Server 1
		crdt::state::PNCounterMetadata<uint32_t> replica1A(2,6);
		handler.addExternalReplica({replica1A});
		replica1A.increasePayload(7);
		replica1A.increasePayload(16);
		handler.addExternalReplica({replica1A});
		handler.updateInternalPayload();
		REQUIRE(handler.queryPayload() == 29);
		replica1A.increasePayload(2);
		replica1A.decreasePayload(8);
		handler.addExternalReplica({replica1A});
		handler.updateInternalPayload();
		REQUIRE(handler.queryPayload() == 23);
	}
	SECTION("Test Merge on 1 Server")
	{
		crdt::state::PNCounterSB<uint32_t> handler(1, 0); //Represents Server 1
		crdt::state::PNCounterMetadata<uint32_t> replica1A(2,0);
		crdt::state::PNCounterMetadata<uint32_t> replica1B(3,0);
		handler.addExternalReplica({replica1A,replica1B});
		handler.updateInternalPayload();
		REQUIRE(handler.queryPayload() == 0);
		replica1A.increasePayload(6);
		replica1A.increasePayload(7);
		replica1A.increasePayload(8);
		replica1B.increasePayload(6);
		replica1B.increasePayload(3);
		replica1B.increasePayload(5);
		handler.addExternalReplica({replica1A,replica1B}); // Fetch the local replicas that got updated
		handler.updateInternalPayload(); //Update the server
		REQUIRE(handler.queryPayload() == 35);
		replica1A.decreasePayload(6);
		handler.addExternalReplica({replica1A}); 
		handler.updateInternalPayload();
		REQUIRE(handler.queryPayload() == 29);
		replica1B.decreasePayload(5);
		handler.addExternalReplica({replica1B}); 
		handler.updateInternalPayload();
		REQUIRE(handler.queryPayload() == 24);
		crdt::state::PNCounterMetadata<uint32_t> replica1C(4,200);
		replica1A.decreasePayload(4);
		replica1B.decreasePayload(20);
		handler.addExternalReplica({replica1A,replica1B,replica1C}); 
		handler.updateInternalPayload();
		REQUIRE(handler.queryPayload() == 200);	
	}
	SECTION("Test Merge on Multiple Server")
	{
		crdt::state::PNCounterSB<uint32_t> handler1(1, 0); //Represents Server 1
		crdt::state::PNCounterSB<uint32_t> handler2(2, 0); //Represents Server 2
		crdt::state::PNCounterSB<uint32_t> handler3(3, 0); //Represents Server 3
		crdt::state::PNCounterMetadata<uint32_t> replica1A(2,10);
		crdt::state::PNCounterMetadata<uint32_t> replica1B(3,10);
		crdt::state::PNCounterMetadata<uint32_t> replica2A(4,10);
		crdt::state::PNCounterMetadata<uint32_t> replica2B(5,10);
		crdt::state::PNCounterMetadata<uint32_t> replica3A(6,10);
		crdt::state::PNCounterMetadata<uint32_t> replica3B(7,10);
		handler1.addExternalReplica({replica1A,replica1B});
		handler1.updateInternalPayload();
		handler2.addExternalReplica({replica2A,replica2B});
		handler2.updateInternalPayload();
		handler3.addExternalReplica({replica3A,replica3B});
		handler3.updateInternalPayload();
		replica1A.decreasePayload(5);
		replica1B.decreasePayload(5);
		handler1.addExternalReplica({replica1A,replica1B});
		handler1.updateInternalPayload();
		replica2A.decreasePayload(5);
		replica2B.decreasePayload(5);
		handler2.addExternalReplica({replica2A,replica2B});
		handler2.updateInternalPayload();
		replica3A.decreasePayload(5);
		replica3B.decreasePayload(5);
		handler3.addExternalReplica({replica3A,replica3B});
		handler3.updateInternalPayload();
		auto server1 = handler1;
		auto server2 = handler2;
		auto server3 = handler3;
		handler1.updateLocalExternalPayload({server1,server2,server3});
		handler2.updateLocalExternalPayload({server1,server2,server3});
		handler3.updateLocalExternalPayload({server1,server2,server3});
		REQUIRE(handler1.queryPayload() == handler2.queryPayload());
		REQUIRE(handler2.queryPayload() == handler3.queryPayload());
		INFO(handler1.queryPayload());
	}
}