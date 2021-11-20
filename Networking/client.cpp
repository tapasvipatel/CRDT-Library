#include <iostream>
#include <boost/asio.hpp>
#include <unordered_map>
#include <utility>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

class Client
{
public:
	std::unordered_map<string, int> servers;
	
	Client(boost::asio::io_service& io_service)
	{
		servers.insert(std::pair<string, int>("127.0.0.1", 1234));
	}
	
	void get_updates(tcp::socket& socket)
	{
		cout << "inside get updates" << endl;
		std::unordered_map<string, int>::iterator servers_it;
		
		for(servers_it = servers.begin(); servers_it != servers.end(); servers_it++)
		{
			cout << "inside foor loop" << endl;
			string host_name = servers_it->first;
			int port = servers_it->second;
			
			socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234));
			
			// message from client
			const string msg = "Hello from client\n";
			boost::system::error_code error;
			boost::asio::write(socket, boost::asio::buffer(msg), error);
			
			if(!error)
			{
				cout << "Client sent hello message!" << endl;
			}
			
			cout << "after connect" << endl;
			//boost::system::error_code error;
			boost::asio::streambuf receive_buffer;
			boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), error);
			cout << "after read" << endl;
			
			if(error && error != boost::asio::error::eof)
			{
				cout << "receive failed" << error.message() << endl;
			}
			else
			{
				cout << "right before printing data" << endl;
				const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
				cout << data << endl;
			}
		}
	}
};

int main()
{
	boost::asio::io_service io_service;
	tcp::socket socket(io_service);
	Client my_client(io_service);
	my_client.get_updates(socket);
	
	return 0;
}
