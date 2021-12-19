#include <iostream>

#include "../src/operation_based/CounterOB.hpp"

int main(int argc, char** argv)
{
	crdt::operation::CounterOB<uint32_t> obj1(5);
	std::cout << obj1.query() << std::endl;
}
