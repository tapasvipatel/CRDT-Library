#include <iostream>
#include <vector>
#include <set>

#include "../src/state_based/PNCounterSB.hpp"

int main(int argc, char* argv[])
{
	std::string filePath = "/home/tapasvi/workspace/CRDT-Library/test_application/json/temp.json";

	// PN COUNTER
	std::cout << "HELLOOOO" << std::endl;
	crdt::state::PNCounterMetadata<uint32_t> replica1A(1,10);
	crdt::state::PNCounterMetadata<uint32_t> replica1B(2,20);
	
	std::cout << replica1A.serialize() << std::endl;
	std::cout << replica1B.serialize() << std::endl;
	replica1B.serializeFile(filePath);

	crdt::state::PNCounterMetadata<uint32_t> replica1C;
	replica1C.deserializeFile(filePath);
	std::cout << replica1C.queryId() << std::endl;
	std::cout << replica1C.queryPayloadP() << std::endl;
	std::cout << replica1C.queryPayloadN() << std::endl;

	// PN COUNTER END

	return 0;
}

/*
	Testing JSON related functionality


	json j;

	j["pi"] = 123;
	j["name"] = "taps";

	std::cout << j << std::endl;

	std::ofstream o("sample.json");
	o << j << std::endl;
	return 0;

*/
