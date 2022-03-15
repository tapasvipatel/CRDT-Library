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

bool start_server;
bool start_client;
std::string server_name;
std::string server_ip_address;
int server_port;

std::ofstream server_log;
std::ofstream client_log;

std::unordered_map<std::string, int> list_servers;

// List of crdts for each replica
crdt::state::TwoPSetSB<std::string> set1;
crdt::state::TwoPSetMetadata<std::string> set1Metadata;

crdt::state::PNCounterSB<int> counter1;
crdt::state::PNCounterMetadata<int> counter1Metadata;

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
		char* message = "HELLO WORLD";
		write(new_connection_socket, message, strlen(message));

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
	client_log << "CLIENT: Initiating request" << std::endl;

	// Initialize socket parameters
	int socket_client;
	struct sockaddr_in client_sockaddr;
	char response[5000];

	socket_client = socket(AF_INET, SOCK_STREAM, 0);

	// Initialize sockaddr structure
	client_sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	client_sockaddr.sin_family = AF_INET;
	client_sockaddr.sin_port = htons(server_port);

	// Connect to specified port
	connect(socket_client, (struct sockaddr*)&client_sockaddr, sizeof(client_sockaddr));

	// Receive response from server
	recv(socket_client, response, 5000, 0);

	client_log << response << std::endl;
	close(socket_client);

	client_log << "CLIENT: Finished request" << std::endl;
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

	start_server = true;
	start_client = true;

	std::string server_log_path = "/home/tapasvi/workspace/CRDT-Library/networking_application/logs/server_log_" + server_name + ".txt";
	std::string client_log_path = "/home/tapasvi/workspace/CRDT-Library/networking_application/logs/client_log_" + server_name + ".txt";
	server_log.open(server_log_path);
	client_log.open(client_log_path);

	std::thread server_thread(handle_requests);
	std::thread client_thread(client_requests);

	std::string input_command = "";

	// Initialize CRDTs
	set1Metadata.id = server_port;

	while(input_command.compare("exit"))
	{
		std::cout << ">> ";
		std::getline(std::cin, input_command);

		std::stringstream temp_string_stream(input_command);
		std::string temp;
		std::vector<std::string> list_tokens;
		while(temp_string_stream >> temp)
		{
			list_tokens.push_back(temp);
		}

		if(list_tokens[0] == "available")
		{
			std::cout << "Available CRDTs" << std::endl;
			std::cout << "> PNCounter" << std::endl;
			std::cout << "> TwoPSet" << std::endl;
		}
		else if(list_tokens[0] == "print")
		{
			std::cout << "PNCounter Value: " << std::to_string(counter1.queryPayload()) << std::endl;

			std::set<std::string> payload = set1.queryTwoPSet();
			std::string setString = "{";
			for(auto item : payload)
			{
				setString += item + ",";
			}

			setString += "}";
			std::cout << "TwoPSet Value: " << setString << std::endl;
		}
		else if(list_tokens[0] == "set")
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

			counter1.addExternalReplica({counter1Metadata});
		}
	}

	start_server = false;
	start_client = false;
	client_thread.join();

	server_log.close();
	client_log.close();
	exit(0);
	return 0;
}
