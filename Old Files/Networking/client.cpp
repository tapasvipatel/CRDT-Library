#include <iostream>
#include <boost/asio.hpp>
#include <unordered_map>
#include <utility>
#include <string>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

class g_Set  //Grow-Only Set
{
public:
    std::set<int> set;
    int id;
    
    g_Set() {
    }
    
    //Give each object unique id
    g_Set(int id) {
        this->id = id;
    }
    
    void set_id(int id) {
        this->id = id;
    }
    
    int get_id() {
        return this->id;
    }
    
    void add(int e) {
        set.insert(e);
    }
    
    bool lookup(int e) {
        if (set.count(e)) {
            return true;
        }
        else {
            return false;
        }
    }
    
    bool compare(g_Set S, g_Set T) {
        return includes(S.set.begin(), S.set.end(), T.set.begin(), T.set.end());
    }
    
    void merge(std::vector<g_Set> replicas) {
        for (auto replica: replicas){
            set.insert(replica.set.begin(), replica.set.end());
        }
    }
    
    void print() {
        int gSet_size = set.size(); 
        cout << "GSet with ID=" << get_id();
        cout << " (";
        for (auto const &e: set) {
            if (gSet_size == 1){
                std::cout << e;
            } else{
                std::cout << e << ' ';
            }
            gSet_size--;   
        }
        cout << ")" << endl;
    }
    
    std::string serialize()
    {
		std::string serialized_string = "";
		std::set<int>::iterator it;
		
		for(it = set.begin(); it != set.end(); it++)
		{
			serialized_string += std::to_string(*it) + ",";
		}
		
		return serialized_string;
	}
	
	void deserialize(std::string s)
	{
		// split string and get elements
		g_Set received_set;
		
		std::string token;
		size_t pos = 0;
		
		while((pos = s.find(",")) != std::string::npos)
		{
			token = s.substr(0, pos);
			received_set.add(std::stoi(token));
			s.erase(0, pos + 1);
		}
		
		this->merge({received_set});
	}
};

class Client
{
public:
	std::vector<std::pair<string, int>> servers;
	
	Client(boost::asio::io_service& io_service)
	{
		servers.push_back(std::pair<string, int>("127.0.0.1", 1234));
		servers.push_back(std::pair<string, int>("127.0.0.1", 5678));
		servers.push_back(std::pair<string, int>("127.0.0.1", 9112));
	}
	
	void get_updates(boost::asio::io_service& io_service, g_Set& replica_gSet_B)
	{
		string serialized = "";
		
		for(int i = 0; i < servers.size(); i++)
		{
			tcp::socket socket(io_service);
			string host_name = servers[i].first;
			int port = servers[i].second;
			cout << port << endl;
			
			socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(host_name), port));
			
			// message from client
			const string msg = "Hello from client\n";
			boost::system::error_code error;
			boost::asio::write(socket, boost::asio::buffer(msg), error);
			
			//boost::system::error_code error;
			boost::asio::streambuf receive_buffer;
			boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), error);
			
			if(error && error != boost::asio::error::eof)
			{
				cout << "receive failed" << error.message() << endl;
			}
			else
			{
				const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
				serialized = data;
				//cout << data << endl;
			}
			socket.close();
			
			replica_gSet_B.deserialize(serialized);
		}
		
		
		
		//return serialized;
	}
};

int main()
{
	g_Set replica_gSet_B(1);
	//replica_gSet_B.add(6);
    replica_gSet_B.add(3);
    replica_gSet_B.add(4);
    replica_gSet_B.print();
    
	boost::asio::io_service io_service;
	Client my_client(io_service);
	my_client.get_updates(io_service, replica_gSet_B);
	
	//replica_gSet_B.deserialize(serialized);
    replica_gSet_B.print();
	
	return 0;
}
