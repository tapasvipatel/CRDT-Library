#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>

using namespace boost::asio;
using ip::tcp;
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

class con_handler: public boost::enable_shared_from_this<con_handler>
{
public:
	tcp::socket sock;
	std::string message = "Hello From Server!\n";
	enum {max_length = 1024};
	char data[1024];
	
	typedef boost::shared_ptr<con_handler> pointer;
	con_handler(boost::asio::io_service& io_service): sock(io_service)
	{
		;
	}
	
	static pointer create(boost::asio::io_service& io_service)
	{
		return pointer(new con_handler(io_service));
	}
	
	tcp::socket& socket()
	{
		return sock;
	}
	
	void start()
	{
		sock.async_read_some(
			boost::asio::buffer(data, 1024),
			boost::bind(&con_handler::handle_read,
			shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
		
		sock.async_write_some(
			boost::asio::buffer(message, max_length),
			boost::bind(&con_handler::handle_write,
			shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
	
	void handle_read(const boost::system::error_code& err, size_t bytes_transferred)
	{
		if(!err)
		{
			cout << data << endl;
		}
		else
		{
			std::cerr << "error: " << err.message() << std::endl;
			sock.close();
		}
	}
	
	void handle_write(const boost::system::error_code& err, size_t bytes_transferred)
	{
		if(!err)
		{
			cout << "Server sent Hello message" << endl;
		}
		else
		{
			std::cerr << "error: " << err.message() << std::endl;
			sock.close();
		}
	}
};

class Server
{
private:
	tcp::acceptor acceptor_;
	void start_accept(std::string message)
	{
		// socket
		con_handler::pointer connection = con_handler::create(acceptor_.get_io_service());
		acceptor_.async_accept(connection->socket(),
		boost::bind(&Server::handle_accept, this, connection, 
		boost::asio::placeholders::error));
		connection->message = message;
	}

public:
	std::string message;
	
	// constructor for accepting connection from client
	Server(boost::asio::io_service& io_service, int port, std::string message): acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
	{
		this->message = message;
		start_accept(message);
	}
	
	void handle_accept(con_handler::pointer connection, const boost::system::error_code& err)
	{
		if(!err)
		{
			connection->start();
		}
		start_accept(this->message);
	}
};

int main(int argc, char** argv)
{
	g_Set replica_gSet_A(0);
    
    //g_Set replica_gSet_B(1);
	//replica_gSet_B.add(6);
    //replica_gSet_B.add(3);
    //replica_gSet_B.add(4);
    
    //cout << serialized << endl;
    //replica_gSet_B.deserialize(serialized);
    //replica_gSet_B.print();
	
	int port = std::stoi(argv[1]);
	
	if(port == 1234)
	{
		replica_gSet_A.add(6);
	}
	else if(port == 5678)
	{
		replica_gSet_A.add(7);
	}
	else if(port == 9112)
	{
		replica_gSet_A.add(8);
	}
	
	std::string serialized = replica_gSet_A.serialize();
	
	replica_gSet_A.print();
	
	boost::asio::io_service io_service;
	Server server(io_service, port, serialized);
	io_service.run();
	
	return 0;
}
