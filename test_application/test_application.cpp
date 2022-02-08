#include <iostream>
#include <vector>
#include <set>

#include "../src/CrdtObject.hpp"
#include "../src/CrdtHandle.hpp"
#include "../src/Server.hpp"
#include "../src/state_based/GCounterSB.hpp"

int main(int argc, char* argv[])
{
	CrdtHandle handle;
	handle.initializeCrdtHandle("", 0);

	crdt::state::GCounterSB<uint32_t> handler(1); //Represents Server 1
	handle.registerCrdt(handler);
	handle.printRegisteredCrdtGCounterSB();

	handle.getExternalUpdates(handler);

	return 0;
}
