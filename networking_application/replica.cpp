/*
Sample code taken from: www.binarytides.com/socket-programming-c-linux-tutorial/
Select code was taken on how to generate and use sockets
ALL CREDITS GIVEN TO THIS WEBSITE AND ITS AUTHORS FOR SIMPLIFYING THE PROCESS TO CREATE AND GENERATE SOCKETS
*/

#include <iostream>
#include <vector>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <sstream>

#include "../src/state_based/PNCounterSB.hpp"
#include "../src/state_based/TwoPSetSB.hpp"
#include "../src/state_based/GCounterSB.hpp"
#include "../src/state_based/GMapSB.hpp"
#include "../src/state_based/GSetSB.hpp"
#include "../src/state_based/MultiSetSB.hpp"
#include "../src/state_based/LWWMultiSetSB.hpp"
#include "../src/state_based/ORSetSB.hpp"
#include "../src/state_based/VectorSB.hpp"

bool start_server;
bool start_client;
std::string server_name;
std::string server_ip_address;
int server_port;

std::ofstream server_log;
std::ofstream client_log;

std::vector<int> list_servers;

// List of crdts for each replica
crdt::state::TwoPSetSB<std::string> set1;
crdt::state::TwoPSetMetadata<std::string> set1Metadata;

crdt::state::PNCounterSB<int> counter1;
crdt::state::PNCounterMetadata<int> counter1Metadata;

crdt::state::GCounterSB<int> gcounter1;
crdt::state::GCounterMetadata<int> gcounter1Metadata;

crdt::state::GMapSB<int, int> gmap1;
crdt::state::GMapMetadata<int, int> gmap1Metadata;

crdt::state::GSetSB<std::string> gset1;
crdt::state::GSetMetadata<std::string> gset1Metadata;

crdt::state::MultiSetSB<int> multiset1;
crdt::state::MultiSetMetadata<int> multiset1Metadata;

crdt::state::LWWMultiSetSB<int> lwwmultiset1;
crdt::state::LWWMultiSetMetadata<int> lwwmultiset1Metadata;

crdt::state::ORSetSB<std::string> orset1;
crdt::state::ORSetMetadata<std::string> orset1Metadata;

crdt::state::VectorSB<std::string> vector1;
crdt::state::VectorMetadata<std::string> vector1Metadata;

void handle_requests()
{
	server_log << "SERVER: Starting server" << std::endl;

	// Initialize server parameters
	int socket_server;
	struct sockaddr_in server_sockaddr, client;
	socket_server = socket(AF_INET, SOCK_STREAM, 0);

	// Initialize sockaddr structure
	server_sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(server_port);

	// Bind the server to the socket
	bind(socket_server, (struct sockaddr*)&server_sockaddr, sizeof(server_sockaddr));
	listen(socket_server, 3);
	server_log << "SERVER: Waiting for connection" << std::endl;

	int new_connection_socket;
	int size_of_sockaddr_in = sizeof(struct sockaddr_in);

	// Handle new connection requests
	while((new_connection_socket = accept(socket_server, (struct sockaddr*)&client, (socklen_t*)&size_of_sockaddr_in)))
	{
		server_log << "SERVER: Accepted connection" << std::endl;
		server_log << "SERVER: Processing request" << std::endl;

		std::string message;
		message += counter1Metadata.serialize();
		message += "\n";
		message += set1Metadata.serialize();
		message += "\n";
		message += gcounter1Metadata.serialize();
		message += "\n";
		message += gmap1Metadata.serialize();
		message += "\n";
		message += gset1Metadata.serialize();
		message += "\n";
		message += multiset1Metadata.serialize();
		message += "\n";
		message += lwwmultiset1Metadata.serialize();
		message += "\n";
		message += orset1Metadata.serialize();
		message += "\n";
		message += vector1Metadata.serialize();
		message += "\n";

		write(new_connection_socket, (char*)&message[0], strlen((char*)&message[0]));

		if(!start_server)
		{
			close(socket_server);
			return;
		}

		server_log << "SERVER: Waiting for connection" << std::endl;
	}

	close(socket_server);
	server_log << "SERVER: Stopped server" << std::endl;
}

void generate_requests()
{
	for(auto server_info : list_servers)
	{
		if(server_info != server_port)
		{
			std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
			client_log << "CLIENT: Initiating request" << std::endl;

			// Initialize socket parameters
			int socket_client;
			struct sockaddr_in client_sockaddr;
			char response[5000];

			socket_client = socket(AF_INET, SOCK_STREAM, 0);

			// Initialize sockaddr structure
			client_sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
			client_sockaddr.sin_family = AF_INET;
			client_sockaddr.sin_port = htons(server_info);

			// Connect to specified port
			connect(socket_client, (struct sockaddr*)&client_sockaddr, sizeof(client_sockaddr));
			client_log << "CLIENT: Connected to server (127.0.0.1," << std::to_string(server_info) << ")" << std::endl;

			// Receive response from server
			recv(socket_client, response, 5000, 0);

			std::string response_string(response);
			std::vector<std::string> serialized_strings;
			std::stringstream serialized_string_stream(response_string);
			std::string temp;

			while(getline(serialized_string_stream, temp, '\n'))
			{
				serialized_strings.push_back(temp);
			}

			// Merge pncounter
			crdt::state::PNCounterMetadata<int> new_counter;
			new_counter.deserialize(serialized_strings[0]);
			counter1.addExternalReplica({counter1Metadata, new_counter});

			// Merge twopset
			crdt::state::TwoPSetMetadata<std::string> new_set;
			new_set.deserialize(serialized_strings[1]);
			set1.addExternalReplica({set1Metadata, new_set});

			// Merge gcounter
			crdt::state::GCounterMetadata<int> new_gcounter;
			new_gcounter.deserialize(serialized_strings[2]);
			gcounter1.addExternalReplica({gcounter1Metadata, new_gcounter});

			// Merge gmap
			crdt::state::GMapMetadata<int, int> new_gmap;
			new_gmap.deserialize(serialized_strings[3]);
			gmap1.addExternalReplica({gmap1Metadata, new_gmap});

			// Merge gset
			crdt::state::GSetMetadata<std::string> new_gset;
			new_gset.deserialize(serialized_strings[4]);
			gset1.addExternalReplica({gset1Metadata, new_gset});

			// Merge multiset
			crdt::state::MultiSetMetadata<int> new_multiset;
			new_multiset.deserialize(serialized_strings[5]);
			multiset1.addExternalReplica({multiset1Metadata, new_multiset});

			// Merge lwwmultiset
			crdt::state::LWWMultiSetMetadata<int> new_lwwmultiset;
			new_lwwmultiset.deserialize(serialized_strings[6]);
			lwwmultiset1.addExternalReplica({lwwmultiset1Metadata, new_lwwmultiset});

			// Merge orset
			crdt::state::ORSetMetadata<std::string> new_orset;
			new_orset.deserialize(serialized_strings[7]);
			orset1.addExternalReplica({orset1Metadata, new_orset});

			// Merge vector
			crdt::state::VectorMetadata<std::string> new_vector;
			new_vector.deserialize(serialized_strings[8]);
			vector1.addExternalReplica({vector1Metadata, new_vector});

			close(socket_client);
			client_log << "CLIENT: Disconnected from server (127.0.0.1," << std::to_string(server_info) << ")" << std::endl;
			client_log << "CLIENT: Finished request" << std::endl;

			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			std::cout << "Network Latency (microseconds): " << " : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
		}
	}
}

void client_requests()
{
	while(start_client)
	{
		generate_requests();
		sleep(15);
	}
}

int main(int argc, char* argv[])
{
	server_name = argv[1];
	server_ip_address = argv[2];
	server_port = std::stoi(argv[3]);

	// Initialize CRDTs
	set1Metadata.id = server_port;
	counter1Metadata.id = server_port;

	// Initialize servers
	list_servers.push_back(3020);
	list_servers.push_back(3021);
	list_servers.push_back(3022);

	start_server = true;
	start_client = true;

	std::string server_log_path = "/home/tapasvi/workspace/CRDT-Library/networking_application/logs/server_log_" + server_name + ".txt";
	std::string client_log_path = "/home/tapasvi/workspace/CRDT-Library/networking_application/logs/client_log_" + server_name + ".txt";
	server_log.open(server_log_path);
	client_log.open(client_log_path);

	std::thread server_thread(handle_requests);
	//std::thread client_thread(client_requests);

	std::chrono::steady_clock::time_point referenceTime = std::chrono::steady_clock::now();

	std::string input_command = "";

	while(input_command.compare("exit"))
	{
		std::cout << ">> ";
		std::getline(std::cin, input_command);

		if(input_command.length() == 0)
		{
			continue;
		}

		std::stringstream temp_string_stream(input_command);
		std::string temp;
		std::vector<std::string> list_tokens;
		while(temp_string_stream >> temp)
		{
			list_tokens.push_back(temp);
		}

		if(list_tokens[0] == "available")
		{
			std::cout << "------------------------------------------" << std::endl;
			std::cout << "Available CRDTs" << std::endl;
			std::cout << "> PNCounter<int>" << std::endl;
			std::cout << "> TwoPSet<string>" << std::endl;
			std::cout << "> GCounter<int>" << std::endl;
			std::cout << "> GMap<int, int>" << std::endl;
			std::cout << "> GSet<string>" << std::endl;
			std::cout << "> MultiSet<int>" << std::endl;
			std::cout << "> LWWMultiSet<int>" << std::endl;
			std::cout << "> ORSet<string>" << std::endl;
			std::cout << "> Vector<string>" << std::endl;
			std::cout << "------------------------------------------" << std::endl;
		}
		else if(list_tokens[0] == "print")
		{
			std::cout << "------------------------------------------" << std::endl;
			
			// GCounter
			std::cout << "GCounter Value: " << std::to_string(gcounter1.queryPayload()) << std::endl;

			// PNCounter
			std::cout << "PNCounter Value: " << std::to_string(counter1.queryPayload()) << std::endl;

			// TwoPSet
			std::set<std::string> payload = set1.queryTwoPSet();
			std::string setString = "{";
			for(auto item : payload)
			{
				setString += item + ",";
			}
			setString += "}";
			std::cout << "TwoPSet Value: " << setString << std::endl;

			// GMap
			std::map<int, int> gmap_payload = gmap1.getTotalPayload();
			std::string gmapString = "{";
			for(auto item : gmap_payload)
			{
				gmapString += "(" + std::to_string(item.first) + "," + std::to_string(item.second) + ")" + ",";
			}
			gmapString += "}";
			std::cout << "GMap Value: " << gmapString << std::endl;

			// GSet
			std::set<std::string> gset_payload = gset1.queryPayload();
			std::string gsetString = "{";
			for(auto item : gset_payload)
			{
				gsetString += item + ",";
			}
			gsetString += "}";
			std::cout << "GSet Value: " << gsetString << std::endl;

			// Multiset
			std::multiset<int> multiset_payload = multiset1.queryPayload();
			std::string multisetString = "{";
			for(auto item : multiset_payload)
			{
				multisetString += std::to_string(item) + ",";
			}
			multisetString += "}";
			std::cout << "MultiSet Value: " << multisetString << std::endl;

			// LWWMultiset
			std::multiset<int> lwwmultiset_payload = lwwmultiset1.queryPayload();
			std::string lwwmultisetString = "{";
			for(auto item : lwwmultiset_payload)
			{
				lwwmultisetString += std::to_string(item) + ",";
			}
			lwwmultisetString += "}";
			std::cout << "LWWMultiset Value: " << lwwmultisetString << std::endl;

			// ORSet
			std::vector<std::string> orset_payload = orset1.queryPayload();
			std::string orsetString = "{";
			for(auto item : orset_payload)
			{
				orsetString += item + ",";
			}
			orsetString += "}";
			std::cout << "ORSet Value: " << orsetString << std::endl;

			// Vector
			std::vector<std::string> vector_payload = vector1.queryPayload();
			std::string vectorString = "{";
			for(auto item : vector_payload)
			{
				vectorString += item + ",";
			}
			vectorString += "}";
			std::cout << "Vector Value: " << vectorString << std::endl;

			std::cout << "------------------------------------------" << std::endl;
		}
		else if(list_tokens[0] == "sync")
		{
			generate_requests();
		}
		else if(list_tokens[0] == "twopset")
		{
			if(list_tokens[1] == "add")
			{
				set1Metadata.insert(list_tokens[2]);
			}
			else if(list_tokens[1] == "remove")
			{
				set1Metadata.remove(list_tokens[2]);
			}
			else if(list_tokens[1] == "serialize")
			{
				std::cout << set1Metadata.serialize() << std::endl;
			}
			else if(list_tokens[1] == "help")
			{
				std::cout << "[add, remove, serialize]" << std::endl;
			}

			set1.addExternalReplica({set1Metadata});
		}
		else if(list_tokens[0] == "pncounter")
		{
			if(list_tokens[1] == "increase")
			{
				counter1Metadata.increasePayload(std::stoi(list_tokens[2]));
			}
			else if(list_tokens[1] == "decrease")
			{
				counter1Metadata.decreasePayload(std::stoi(list_tokens[2]));
			}
			else if(list_tokens[1] == "serialize")
			{
				std::cout << counter1Metadata.serialize() << std::endl;
			}
			else if(list_tokens[1] == "help")
			{
				std::cout << "[increase, decrease, serialize]" << std::endl;
			}

			counter1.addExternalReplica({counter1Metadata});
		}
		else if(list_tokens[0] == "gcounter")
		{
			if(list_tokens[1] == "increase")
			{
				gcounter1Metadata.updatePayload(std::stoi(list_tokens[2]));
			}
			else if(list_tokens[1] == "serialize")
			{
				std::cout << gcounter1Metadata.serialize() << std::endl;
			}
			else if(list_tokens[1] == "help")
			{
				std::cout << "[increase, serialize]" << std::endl;
			}

			gcounter1.addExternalReplica({gcounter1Metadata});
		}
		else if(list_tokens[0] == "gmap")
		{
			if(list_tokens[1] == "insert")
			{
				gmap1Metadata.insert(std::stoi(list_tokens[2]), std::stoi(list_tokens[3]));
			}
			else if(list_tokens[1] == "serialize")
			{
				std::cout << gmap1Metadata.serialize() << std::endl;
			}
			else if(list_tokens[1] == "help")
			{
				std::cout << "[insert, serialize]" << std::endl;
			}

			gmap1.addExternalReplica({gmap1Metadata});
		}
		else if(list_tokens[0] == "gset")
		{
			if(list_tokens[1] == "insert")
			{
				gset1Metadata.insert(list_tokens[2]);
			}
			else if(list_tokens[1] == "serialize")
			{
				std::cout << gset1Metadata.serialize() << std::endl;
			}

			else if(list_tokens[1] == "help")
			{
				std::cout << "[insert, serialize]" << std::endl;
			}

			gset1.addExternalReplica({gset1Metadata});
		}
		else if(list_tokens[0] == "multiset")
		{
			if(list_tokens[1] == "insert")
			{
				multiset1Metadata.insert(std::stoi(list_tokens[2]));
			}
			else if(list_tokens[1] == "serialize")
			{
				std::cout << multiset1Metadata.serialize() << std::endl;
			}
			else if(list_tokens[1] == "help")
			{
				std::cout << "[insert, serialize]" << std::endl;
			}

			multiset1.addExternalReplica({multiset1Metadata});
		}
		else if(list_tokens[0] == "lwwmultiset")
		{
			if(list_tokens[1] == "insert")
			{
				std::chrono::steady_clock::time_point insertionTime = std::chrono::steady_clock::now();
				long long int metadataInsertionTime = (long long int) (std::chrono::duration_cast<std::chrono::seconds>(insertionTime - referenceTime).count());
				lwwmultiset1Metadata.insert(metadataInsertionTime, std::stoi(list_tokens[2]));
			}
			else if(list_tokens[1] == "serialize")
			{
				std::cout << lwwmultiset1Metadata.serialize() << std::endl;
			}
			else if(list_tokens[1] == "help")
			{
				std::cout << "[insert, serialize]" << std::endl;
			}

			lwwmultiset1.addExternalReplica({lwwmultiset1Metadata});
		}
		else if(list_tokens[0] == "orset")
		{
			if(list_tokens[1] == "insert")
			{
				orset1Metadata.insert(list_tokens[2]);
			}
			else if(list_tokens[1] == "serialize")
			{
				std::cout << orset1Metadata.serialize() << std::endl;
			}
			else if(list_tokens[1] == "help")
			{
				std::cout << "[insert, serialize]" << std::endl;
			}

			orset1.addExternalReplica({orset1Metadata});
		}
		else if(list_tokens[0] == "vector")
		{
			if(list_tokens[1] == "insert")
			{
				vector1Metadata.push_back(list_tokens[2]);
			}
			else if(list_tokens[1] == "serialize")
			{
				std::cout << vector1Metadata.serialize() << std::endl;
			}
			else if(list_tokens[1] == "help")
			{
				std::cout << "[insert, serialize]" << std::endl;
			}

			vector1.addExternalReplica({vector1Metadata});
		}
	}

	start_server = false;
	start_client = false;
	//client_thread.join();

	server_log.close();
	client_log.close();
	exit(0);
	return 0;
}
