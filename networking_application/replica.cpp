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

bool start_server;
bool start_client;
std::string server_name;
std::string server_ip_address;
int server_port;

std::unordered_map<std::string, int> list_servers;

std::ofstream server_log;
std::ofstream client_log;

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
	client_sockaddr.sin_port = htons(8080);

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
		sleep(5);
	}
}

int main(int argc, char* argv[])
{
	server_name = argv[1];
	server_ip_address = argv[2];
	server_port = std::stoi(argv[3]);

	std::cout << server_name << std::endl;
	std::cout << server_ip_address << std::endl;
	std::cout << server_port << std::endl;

	start_server = true;
	start_client = true;

	std::string server_log_path = "/home/tapasvi/workspace/CRDT-Library/networking_application/logs/server_log_" + server_name + ".txt";
	std::string client_log_path = "/home/tapasvi/workspace/CRDT-Library/networking_application/logs/client_log_" + server_name + ".txt";
	server_log.open(server_log_path);
	client_log.open(client_log_path);

	std::thread server_thread(handle_requests);
	std::thread client_thread(client_requests);

	std::string input_command = "";

	while(input_command.compare("exit"))
	{
		std::cout << ">> ";
		std::cin >> input_command;
		std::cout << input_command << std::endl;
	}

	start_server = false;
	start_client = false;
	client_thread.join();

	server_log.close();
	client_log.close();
	exit(0);
	return 0;
}
