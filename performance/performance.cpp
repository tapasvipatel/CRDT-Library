#include <ostream>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <vector>
#include <set>

#include "../src/state_based/PNCounterSB.hpp"
#include "../src/state_based/GCounterSB.hpp"
#include "../src/state_based/GMapSB.hpp"
#include "../src/state_based/GSetSB.hpp"
#include "../src/state_based/MultiSetSB.hpp"
#include "../src/state_based/ORSetSB.hpp"
#include "../src/state_based/PriorityQueueSB.hpp"
#include "../src/state_based/TwoPSetSB.hpp"
#include "../src/state_based/VectorSB.hpp"
#include "../src/operation_based/StringOB.hpp"
#include "../src/state_based/LWWMultiSetSB.hpp"

std::string filePath = "/home/tapasvi/workspace/CRDT-Library/performance/results/";

void PNCounterPerformance()
{
	std::cout << "------------------------------------------------------" << std::endl;
	std::cout << "PNCOUNTER" << std::endl;
	std::vector<int> replicas = {1, 2, 4, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072};
	std::ofstream o(filePath + "PNCounter.csv");

	for(auto num : replicas)
	{
		std::vector<std::string> serializedStrings;
		int numReplicas = num;

		for(int i = 0; i < numReplicas; i++)
		{
			crdt::state::PNCounterMetadata<uint32_t> replica1A(i,i);
			serializedStrings.push_back(replica1A.serialize());
		}

		// deserialize, then merge
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		std::vector<crdt::state::PNCounterMetadata<uint32_t>> deserializeMetadata;

		for(auto s : serializedStrings)
		{
			crdt::state::PNCounterMetadata<uint32_t> metadata;
			metadata.deserialize(s);
			deserializeMetadata.push_back(metadata);
		}

		crdt::state::PNCounterSB<uint32_t> replicaMaster;
		replicaMaster.addExternalReplica(deserializeMetadata);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		std::cout << numReplicas << " : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    	o << numReplicas << "," << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	}
	std::cout << "------------------------------------------------------" << std::endl;
}

void GCounterPerformance()
{
	std::cout << "------------------------------------------------------" << std::endl;
	std::cout << "GCounter" << std::endl;
	std::vector<int> replicas = {1, 2, 4, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072};
	std::ofstream o(filePath + "GCounter.csv");

	for(auto num : replicas)
	{
		std::vector<std::string> serializedStrings;
		int numReplicas = num;

		for(int i = 0; i < numReplicas; i++)
		{
			crdt::state::GCounterMetadata<uint32_t> replica1A(i,i);
			serializedStrings.push_back(replica1A.serialize());
		}

		// deserialize, then merge
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		std::vector<crdt::state::GCounterMetadata<uint32_t>> deserializeMetadata;

		for(auto s : serializedStrings)
		{
			crdt::state::GCounterMetadata<uint32_t> metadata;
			metadata.deserialize(s);
			deserializeMetadata.push_back(metadata);
		}

		crdt::state::GCounterSB<uint32_t> replicaMaster;
		replicaMaster.addExternalReplica(deserializeMetadata);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		std::cout << numReplicas << " : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
		o << numReplicas << "," << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	}
	std::cout << "------------------------------------------------------" << std::endl;
}

void GMapPerformance()
{
	std::cout << "------------------------------------------------------" << std::endl;
	std::cout << "GMap" << std::endl;
	std::vector<int> replicas = {1, 2, 4, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072};
	std::ofstream o(filePath + "GMap.csv");

	for(auto num : replicas)
	{
		int key = 0;
		std::vector<std::string> serializedStrings;
		int numReplicas = num;

		for(int i = 0; i < numReplicas; i++)
		{
			crdt::state::GMapMetadata<uint32_t, uint32_t> replica1A(i,key,i);
			serializedStrings.push_back(replica1A.serialize());
			key++;
		}

		// deserialize, then merge
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		std::vector<crdt::state::GMapMetadata<uint32_t, uint32_t>> deserializeMetadata;

		for(auto s : serializedStrings)
		{
			crdt::state::GMapMetadata<uint32_t, uint32_t> metadata;
			metadata.deserialize(s);
			deserializeMetadata.push_back(metadata);
		}

		crdt::state::GMapSB<uint32_t, uint32_t> replicaMaster;
		replicaMaster.addExternalReplica(deserializeMetadata);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		std::cout << numReplicas << " : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
		o << numReplicas << "," << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	}
	std::cout << "------------------------------------------------------" << std::endl;
}

void LWWMultiSetPerformance()
{
	std::cout << "------------------------------------------------------" << std::endl;
	std::cout << "LWWMultiSet" << std::endl;
	std::vector<int> replicas = {1, 2, 4, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072};
	std::ofstream o(filePath + "LWWMultiSet.csv");

	for(auto num : replicas)
	{
		int key = 0;
		std::vector<std::string> serializedStrings;
		int numReplicas = num;

		for(int i = 0; i < numReplicas; i++)
		{
			crdt::state::LWWMultiSetMetadata<std::string> replica1A(i,std::to_string(key),i);
			serializedStrings.push_back(replica1A.serialize());
			key++;
		}

		// deserialize, then merge
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		std::vector<crdt::state::LWWMultiSetMetadata<std::string>> deserializeMetadata;

		for(auto s : serializedStrings)
		{
			crdt::state::LWWMultiSetMetadata<std::string> metadata;
			metadata.deserialize(s);
			deserializeMetadata.push_back(metadata);
		}

		crdt::state::LWWMultiSetSB<std::string> replicaMaster;
		replicaMaster.addExternalReplica(deserializeMetadata);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		std::cout << numReplicas << " : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
		o << numReplicas << "," << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	}
	std::cout << "------------------------------------------------------" << std::endl;
}

void MultiSetPerformance()
{
	std::cout << "------------------------------------------------------" << std::endl;
	std::cout << "MultiSet" << std::endl;
	std::vector<int> replicas = {1, 2, 4, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072};
	std::ofstream o(filePath + "MultiSet.csv");

	for(auto num : replicas)
	{
		std::vector<std::string> serializedStrings;
		int numReplicas = num;

		for(int i = 0; i < numReplicas; i++)
		{
			crdt::state::MultiSetMetadata<uint32_t> replica1A(i,i);
			serializedStrings.push_back(replica1A.serialize());
		}

		// deserialize, then merge
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		std::vector<crdt::state::MultiSetMetadata<uint32_t>> deserializeMetadata;

		for(auto s : serializedStrings)
		{
			crdt::state::MultiSetMetadata<uint32_t> metadata;
			metadata.deserialize(s);
			deserializeMetadata.push_back(metadata);
		}

		crdt::state::MultiSetSB<uint32_t> replicaMaster;
		replicaMaster.addExternalReplica(deserializeMetadata);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		std::cout << numReplicas << " : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
		o << numReplicas << "," << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	}
	std::cout << "------------------------------------------------------" << std::endl;
}

void ORSetPerformance()
{
	std::cout << "------------------------------------------------------" << std::endl;
	std::cout << "ORSet" << std::endl;
	std::vector<int> replicas = {1, 2, 4, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072};
	std::ofstream o(filePath + "ORSet.csv");

	for(auto num : replicas)
	{
		std::vector<std::string> serializedStrings;
		int numReplicas = num;

		for(int i = 0; i < numReplicas; i++)
		{
			crdt::state::ORSetMetadata<uint32_t> replica1A(i,i);
			serializedStrings.push_back(replica1A.serialize());
		}

		// deserialize, then merge
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		std::vector<crdt::state::ORSetMetadata<uint32_t>> deserializeMetadata;

		for(auto s : serializedStrings)
		{
			crdt::state::ORSetMetadata<uint32_t> metadata;
			metadata.deserialize(s);
			deserializeMetadata.push_back(metadata);
		}

		crdt::state::ORSetSB<uint32_t> replicaMaster;
		replicaMaster.addExternalReplica(deserializeMetadata);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		std::cout << numReplicas << " : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
		o << numReplicas << "," << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	}
	std::cout << "------------------------------------------------------" << std::endl;
}

void PriorityQueuePerformance()
{
	std::cout << "------------------------------------------------------" << std::endl;
	std::cout << "PriorityQueue" << std::endl;
	std::vector<int> replicas = {1, 2, 4, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072};
	std::ofstream o(filePath + "PriorityQueue.csv");

	for(auto num : replicas)
	{
		std::vector<std::string> serializedStrings;
		int numReplicas = num;

		for(int i = 0; i < numReplicas; i++)
		{
			crdt::state::PriorityQueueMetadata<uint32_t> replica1A(i,i);
			serializedStrings.push_back(replica1A.serialize());
		}

		// deserialize, then merge
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		std::vector<crdt::state::PriorityQueueMetadata<uint32_t>> deserializeMetadata;

		for(auto s : serializedStrings)
		{
			crdt::state::PriorityQueueMetadata<uint32_t> metadata;
			metadata.deserialize(s);
			deserializeMetadata.push_back(metadata);
		}

		crdt::state::PriorityQueueSB<uint32_t> replicaMaster;
		replicaMaster.addExternalReplica(deserializeMetadata);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		std::cout << numReplicas << " : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
		o << numReplicas << "," << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	}
	std::cout << "------------------------------------------------------" << std::endl;
}

void TwoPSetPerformance()
{
	std::cout << "------------------------------------------------------" << std::endl;
	std::cout << "TwoPSet" << std::endl;
	std::vector<int> replicas = {1, 2, 4, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072};
	std::ofstream o(filePath + "TwoPSet.csv");

	for(auto num : replicas)
	{
		std::vector<std::string> serializedStrings;
		int numReplicas = num;

		for(int i = 0; i < numReplicas; i++)
		{
			crdt::state::TwoPSetMetadata<uint32_t> replica1A(i,i);
			serializedStrings.push_back(replica1A.serialize());
		}

		// deserialize, then merge
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		std::vector<crdt::state::TwoPSetMetadata<uint32_t>> deserializeMetadata;

		for(auto s : serializedStrings)
		{
			crdt::state::TwoPSetMetadata<uint32_t> metadata;
			metadata.deserialize(s);
			deserializeMetadata.push_back(metadata);
		}

		crdt::state::TwoPSetSB<uint32_t> replicaMaster;
		replicaMaster.addExternalReplica(deserializeMetadata);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		std::cout << numReplicas << " : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
		o << numReplicas << "," << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	}
	std::cout << "------------------------------------------------------" << std::endl;
}

void GSetPerformance()
{
	std::cout << "------------------------------------------------------" << std::endl;
	std::cout << "GSet" << std::endl;
	std::vector<int> replicas = {1, 2, 4, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072};
	std::ofstream o(filePath + "GSet.csv");

	for(auto num : replicas)
	{
		std::vector<std::string> serializedStrings;
		int numReplicas = num;

		for(int i = 0; i < numReplicas; i++)
		{
			crdt::state::GSetMetadata<uint32_t> replica1A(i,i);
			serializedStrings.push_back(replica1A.serialize());
		}

		// deserialize, then merge
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		std::vector<crdt::state::GSetMetadata<uint32_t>> deserializeMetadata;

		for(auto s : serializedStrings)
		{
			crdt::state::GSetMetadata<uint32_t> metadata;
			metadata.deserialize(s);
			deserializeMetadata.push_back(metadata);
		}

		crdt::state::GSetSB<uint32_t> replicaMaster;
		replicaMaster.addExternalReplica(deserializeMetadata);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		std::cout << numReplicas << " : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
		o << numReplicas << "," << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	}
	std::cout << "------------------------------------------------------" << std::endl;
}

void StringPerformance()
{
	std::cout << "------------------------------------------------------" << std::endl;
	std::cout << "String" << std::endl;
	std::vector<int> replicas = {1, 2, 4, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072};
	std::ofstream o(filePath + "String.csv");

	for(auto num : replicas)
	{
		std::vector<std::string> serializedStrings;
		int numReplicas = num;

		for(int i = 0; i < numReplicas; i++)
		{
			crdt::operation::StringMetaData<std::string> replica1A(i,"aa");
			serializedStrings.push_back(replica1A.serialize());
		}

		// deserialize, then merge
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		std::vector<crdt::operation::StringMetaData<std::string>> deserializeMetadata;

		for(auto s : serializedStrings)
		{
			crdt::operation::StringMetaData<std::string> metadata;
			metadata.deserialize(s);
			deserializeMetadata.push_back(metadata);
		}

		crdt::operation::StringOB<std::string> replicaMaster;
		replicaMaster.addExternalReplica(deserializeMetadata);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		std::cout << numReplicas << " : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
		o << numReplicas << "," << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	}
	std::cout << "------------------------------------------------------" << std::endl;
}

void VectorPerformance()
{
	std::cout << "------------------------------------------------------" << std::endl;
	std::cout << "Vector" << std::endl;
	std::vector<int> replicas = {1, 2, 4, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072};
	std::ofstream o(filePath + "Vector.csv");

	for(auto num : replicas)
	{
		std::vector<std::string> serializedStrings;
		int numReplicas = num;

		for(int i = 0; i < numReplicas; i++)
		{
			crdt::state::VectorMetadata<uint32_t> replica1A(i,i);
			serializedStrings.push_back(replica1A.serialize());
		}

		// deserialize, then merge
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		std::vector<crdt::state::VectorMetadata<uint32_t>> deserializeMetadata;

		for(auto s : serializedStrings)
		{
			crdt::state::VectorMetadata<uint32_t> metadata;
			metadata.deserialize(s);
			deserializeMetadata.push_back(metadata);
		}

		crdt::state::VectorSB<uint32_t> replicaMaster;
		replicaMaster.addExternalReplica(deserializeMetadata);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		std::cout << numReplicas << " : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
		o << numReplicas << "," << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	}
	std::cout << "------------------------------------------------------" << std::endl;
}

int main(int argc, char* argv[])
{
	// performance
	//PNCounterPerformance();
	//GCounterPerformance();
	//GMapPerformance();
	LWWMultiSetPerformance();
	MultiSetPerformance();
	ORSetPerformance();
	PriorityQueuePerformance();
	TwoPSetPerformance();
	GSetPerformance();
	//StringPerformance();
	//VectorPerformance();

	return 0;
}
