#include <iostream>
#include <vector>
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>

#include "../src/operation_based/CounterOB.hpp"
#include "../src/state_based/GCounterSB.hpp"
#include "../src/state_based/PNCounterSB.hpp"
#include "../src/state_based/GMapSB.hpp"
#include "../src/state_based/PriorityQueueSB.hpp"

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
		replica1A.updatePayload(7);
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
		REQUIRE(handler.queryPayload() == handler2.queryPayload());
		REQUIRE(handler3.queryPayload() == handler4.queryPayload());
		REQUIRE(handler.queryPayload() == handler4.queryPayload());
	}
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
		REQUIRE(handler1.queryPayload() == 200);
		REQUIRE(handler1.queryPayload() == handler2.queryPayload());
		REQUIRE(handler2.queryPayload() == handler3.queryPayload());
	}
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
			REQUIRE(handler.queryPayload(2,1) == 5);
			replica1B.insert(1,25);
			handler.addExternalReplica({replica1A,replica1B,replica1C});
			REQUIRE(handler.queryPayload(2,1) == 25);
			replica1C.insert(4,20);
			replica1C.insert(0,52);
			replica1C.insert(2,-500);
			handler.addExternalReplica({replica1A,replica1B,replica1C});
			REQUIRE(handler.queryPayload(3,4) == 20);
			REQUIRE(handler.queryPayload(3,0) == 52);
			REQUIRE(handler.queryPayload(3,2) == -500);
			handler2.addExternalReplica({replica1D});
			handler3.addExternalReplica({replica1E});
			REQUIRE(handler2.queryPayload(4,'a') == 99);
			REQUIRE(handler3.queryPayload(5,0) == "Hello World");
	}
	SECTION("Test Conflict on localServer")
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
			REQUIRE(handler.queryPayload(0,10) == 4);
			REQUIRE(handler2.queryPayload(0,10) == "Hello World how are ya");
			crdt::state::GMapMetadata<uint32_t, std::string> replica1L(0,10,"Z");
			handler2.addExternalReplica({replica1I,replica1J,replica1K,replica1L});
			handler3.addExternalReplica({replica1I,replica1J,replica1K,replica1L});
			REQUIRE(handler2.queryPayload(0,10) == "Z");
			REQUIRE(handler3.queryPayload(0,10) == "Hello World Z are how ya");
			replica1I.insert(11,"dfojsfsdojfiod");
			replica1J.insert(11,"fgklfgkfd;");
			replica1K.insert(11,"zzzzzzzzzzzz");
			replica1L.insert(11,"#$#%#$%$^$^^");
			handler2.addExternalReplica({replica1I,replica1J,replica1K,replica1L});
			handler3.addExternalReplica({replica1I,replica1J,replica1K,replica1L});
			REQUIRE(handler2.queryPayload(0,11) == "zzzzzzzzzzzz");
			REQUIRE(handler3.queryPayload(0,11) == "#$#%#$%$^$^^ dfojsfsdojfiod fgklfgkfd; zzzzzzzzzzzz");
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
			REQUIRE(handler.queryPayload(0,25) == 100);
			handler2.addExternalReplica({replica1B});
			handler3.addExternalReplica({replica1C});
			
			REQUIRE(handler2.queryPayload(0,10) != handler.queryPayload(0,10));
			REQUIRE(handler3.queryPayload(0,10) != handler2.queryPayload(0,10));
			handler.updateLocalExternalPayload({handler,handler2,handler3});
			handler2.updateLocalExternalPayload({handler,handler2,handler3});
			handler3.updateLocalExternalPayload({handler,handler2,handler3});
			REQUIRE(handler2.queryPayload(0,10) == handler.queryPayload(0,10));
			REQUIRE(handler3.queryPayload(0,10) == handler2.queryPayload(0,10));
			
			REQUIRE(handler.queryPayload(0,25) == 100);
			REQUIRE(handler2.queryPayload(0,25) == 100);
			REQUIRE(handler3.queryPayload(0,25) == 100);
			replica1B.insert(3,200);
			replica1B.insert(75,350);
			handler2.addExternalReplica({replica1B});
			handler.updateLocalExternalPayload({handler,handler2,handler3});
			handler2.updateLocalExternalPayload({handler,handler2,handler3});
			handler3.updateLocalExternalPayload({handler,handler2,handler3});
			REQUIRE(handler.queryPayload(0,3) == 200);
			REQUIRE(handler3.queryPayload(0,75) == 350);
			REQUIRE(handler.queryPayload(0,3) == handler3.queryPayload(0,3));
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1D(1,0,5000);
			handler3.addExternalReplica({replica1D});
			handler.updateLocalExternalPayload({handler,handler2,handler3});
			handler2.updateLocalExternalPayload({handler,handler2,handler3});
			handler3.updateLocalExternalPayload({handler,handler2,handler3});
			REQUIRE(handler3.queryPayload(1,0) == 5000);
			REQUIRE(handler2.queryPayload(1,0) == handler.queryPayload(1,0));
			REQUIRE(handler3.queryPayload(1,0) == handler2.queryPayload(1,0));
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
		REQUIRE(handler2.queryPayload(0,10) != handler1.queryPayload(0,10));
		REQUIRE(handler3.queryPayload(0,10) != handler2.queryPayload(0,10));
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler3.queryPayload(0,10) == "Hello HelloMelo");
		REQUIRE(handler2.queryPayload(0,10) == handler1.queryPayload(0,10));
		REQUIRE(handler3.queryPayload(0,10) == handler2.queryPayload(0,10));
		replica1A.insert(1,"Hello, my name is Bob!");
		handler1.addExternalReplica({replica1A});
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryPayload(0,1) == handler2.queryPayload(0,1));
		REQUIRE(handler2.queryPayload(0,1) == handler3.queryPayload(0,1));
		crdt::state::GMapMetadata<uint32_t, std::string> replica1D(1,10,"ABC");
		handler3.addExternalReplica({replica1D});
		replica1D.insert(10,"DEF");
		handler2.addExternalReplica({replica1D});
		replica1D.insert(10,"U");
		handler1.addExternalReplica({replica1D});
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryPayload(1,10) == "ABC DEF U");
		REQUIRE(handler1.queryPayload(1,10) == handler2.queryPayload(1,10));
		REQUIRE(handler2.queryPayload(1,10) == handler3.queryPayload(1,10));
		crdt::state::GMapMetadata<uint32_t, std::string> replica1E(2,10,"ASDHUIFDHIUSDHFUI");
		handler2.addExternalReplica({replica1E});
		handler1.updateLocalExternalPayload({handler1,handler2,handler3});
		handler2.updateLocalExternalPayload({handler1,handler2,handler3});
		handler3.updateLocalExternalPayload({handler1,handler2,handler3});
		REQUIRE(handler1.queryPayload(2,10) == handler2.queryPayload(2,10));
		REQUIRE(handler2.queryPayload(2,10) == handler3.queryPayload(2,10));
	}
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

	SECTION("Test Delta Merge")
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
		REQUIRE(handler1.queryPayloadVector() == handler2.queryPayloadVector());
		REQUIRE(handler2.queryPayloadVector() == handler3.queryPayloadVector());
	}
}