#include <zmq.hpp>
#include <iostream>
#include <string>
#include <zmq_utils.h>
#include <zmq_addon.hpp>
#include <WinSock2.h>
#include <vector>
#include <Windows.h>
#include <future>

#pragma comment(lib, "ws2_32.lib") 
#pragma warning(disable : 4996)

void p2p_recv(zmq::socket_t* socket);
void Main2(char* user_name);

std::vector<std::string> split(std::string input, char delimiter)
{
	std::vector<std::string> vlist;
	std::stringstream ss(input);
	std::string temp;
	while (std::getline(ss, temp, delimiter))
	{
		vlist.push_back(temp);
	}
	return vlist;
}

std::string search_nameserver(std::string ip_mask, std::string local_ip_addr, std::string port_nameserver)
{
	zmq::context_t context(1);
	zmq::socket_t req(context, ZMQ_SUB);
	for (int last = 1; last < 255; last++)
	{
		std::string target_ip_addr = "tcp://" + ip_mask + "." + std::to_string(last) + ":" + port_nameserver;
		if (target_ip_addr != local_ip_addr || target_ip_addr == local_ip_addr)
		{
			req.connect(target_ip_addr);
		}
		req.set(zmq::sockopt::subscribe, "NAMESERVER");
		req.set(zmq::sockopt::rcvtimeo, 2000);
	}
	try
	{
		zmq::message_t res_;
		req.recv(res_, zmq::recv_flags::none);
		std::string res = res_.to_string();
		std::string res_list[2];
		std::istringstream iss(res);
		iss >> res_list[0] >> res_list[1];
		if (res_list[0] == "NAMESERVER")
		{
			return res_list[1];
		}
		else
		{
			return "None";
		}
	}
	catch (const std::exception&)
	{
		return "None";
	}
}

void beacon_nameserver(std::string local_ip_addr, std::string port_nameserver)
{
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_PUB);
	socket.bind("tcp://" + local_ip_addr + ":" + port_nameserver);
	std::cout << "local p2p name server bind to tcp://" << local_ip_addr << ":" << port_nameserver << "." << std::endl;
	while (true)
	{
		try
		{
			Sleep(1000);
			std::string message = "NAMESERVER:" + local_ip_addr;
			zmq::message_t msg(message);
			socket.send(msg, zmq::send_flags::none);
		}
		catch (const std::exception&)
		{
			break;
		}
	}
}

void user_manager_nameserver(std::string local_ip_addr, std::string port_subscribe)
{
	std::vector<std::string> user_db;
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);
	socket.bind("tcp://" + local_ip_addr + ":" + port_subscribe);
	std::cout << "local p2p db server activated at tcp://" << local_ip_addr << ":" << port_subscribe << "." << std::endl;
	while (true)
	{
		try
		{
			zmq::message_t msg;
			socket.recv(msg, zmq::recv_flags::none);
			user_db.push_back(msg.to_string());
			std::vector<std::string> user_req = split(msg.to_string(), ':');
			std::cout << "user registration '" << user_req[1] << "' from '" << user_req[0] << "'." << std::endl;
			socket.send(zmq::str_buffer("ok"));
		}
		catch (const std::exception&)
		{
			break;
		}
	}
}

void relay_server_nameserver(std::string local_ip_addr, std::string port_chat_publisher, std::string port_chat_collector)
{
	zmq::context_t context(1);
	zmq::socket_t publisher(context, ZMQ_PUB);
	publisher.bind("tcp://" + local_ip_addr + ":" + port_chat_publisher);
	zmq::socket_t collector(context, ZMQ_PULL);
	collector.bind("tcp://" + local_ip_addr + ":" + port_chat_collector);
	std::cout << "local p2p relay server activated at tcp://" << local_ip_addr << ":" << port_chat_publisher << " & " << port_chat_collector << std::endl;
	while (true)
	{
		try
		{
			zmq::message_t msg;
			collector.recv(msg, zmq::recv_flags::none);
			std::cout << "p2p-relay:<=>" << msg.to_string() << std::endl;
			std::string sendmsg = "RELAY:" + msg.to_string();
			zmq::message_t SendMessage(sendmsg);
			publisher.send(SendMessage, zmq::send_flags::none);
		}
		catch (const std::exception&)
		{
			break;
		}
	}
}

std::string get_local_ip()
{
	try
	{
		SOCKET Sersocket;
		Sersocket = socket(AF_INET, SOCK_DGRAM, 0);

		SOCKADDR_IN servaddr;
		servaddr.sin_family = AF_INET;
		const char* ip_address = "8.8.8.8";
		servaddr.sin_addr.s_addr = inet_addr(ip_address);
		servaddr.sin_port = htons(80);
		connect(Sersocket, (SOCKADDR*)&servaddr, 80);
		int len = sizeof(servaddr);
		int sockname = getsockname(Sersocket, (SOCKADDR*)&servaddr, &len);
		return std::to_string(sockname);
	}
	catch (const std::exception&)
	{
		return "127.0.0.1";
	}
}

void p2p_recv(zmq::socket_t* socket)
{
	while (true)
	{
		try
		{
			zmq::message_t msg;
			(*socket).recv(msg, zmq::recv_flags::none);
			std::vector<std::string> message = split(msg.to_string(), ':');
			std::cout << "p2p-recv::<<== " << message[1] << ":" << message[2] << std::endl;
		}
		catch (const std::exception&)
		{
			std::cout << "Recv Error!!!!!" << std::endl;
			break;	
		}
	}
}

std::string ip_addr_p2p_server = "";
std::string port_nameserver = "9001";
std::string port_chat_publisher = "9002";
std::string port_chat_collector = "9003";
std::string port_subscribe = "9004";
std::string ip_addr;
std::string ip_mask = "1";

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cout << "usage is 'P2P_ZMQ.exe _user-name_'." << std::endl;
		return 0;
	}
	else
	{
		std::cout << "starting p2p chatting program." << std::endl;

		char* user_name = argv[1];
		//ip_addr = get_local_ip();
		ip_addr = "127.0.0.1";

		std::cout << "searching for p2p server." << std::endl;

		std::string name_server_ip_addr = search_nameserver(ip_mask, ip_addr, port_nameserver);

		if (name_server_ip_addr == "None") {
			ip_addr_p2p_server = ip_addr;
			std::cout << "p2p server is not found, and p2p server mode is activated." << std::endl;
			auto beacon_thread = std::async(std::launch::async, beacon_nameserver, ip_addr, port_nameserver);
			std::cout << "p2p beacon server is activated" << std::endl;
			auto db_thread = std::async(std::launch::async, user_manager_nameserver, ip_addr, port_subscribe);
			std::cout << "p2p subsciber database server is activated." << std::endl;
			auto relay_thread = std::async(std::launch::async, relay_server_nameserver, ip_addr, port_chat_publisher, port_chat_collector);
			std::cout << "p2p message relay server is activated." << std::endl;
			auto threads = std::async(std::launch::async, Main2, user_name);
		}
		else
		{
			ip_addr_p2p_server = name_server_ip_addr;
			std::cout << "p2p server found at " << ip_addr_p2p_server << ", and p2p client mode is activated." << std::endl;
			auto threads = std::async(std::launch::async, Main2, user_name);
		}

	}
}

void Main2(char* user_name)
{
	std::cout << "starting user registration procedure." << std::endl;

	zmq::context_t db_client_context(1);
	zmq::socket_t db_client_socket(db_client_context, ZMQ_REQ);
	db_client_socket.connect("tcp://" + ip_addr_p2p_server + ":" + port_subscribe);
	std::string msg = ip_addr + ":" + user_name;
	zmq::message_t message(msg);
	db_client_socket.send(message, zmq::send_flags::none);
	db_client_socket.recv(message, zmq::recv_flags::none);
	if (message.to_string() == "ok")
	{
		std::cout << "user registration to p2p server completed." << std::endl;
	}
	else
	{
		std::cout << "user registration to p2p server failed." << std::endl;
	}
	std::cout << "starting message transfer procedure." << std::endl;

	zmq::context_t relay_client(1);
	zmq::socket_t p2p_rx(relay_client, ZMQ_SUB);
	p2p_rx.set(zmq::sockopt::subscribe, "RELAY");
	p2p_rx.connect("tcp://" + ip_addr_p2p_server + ":" + port_chat_publisher);
	zmq::socket_t p2p_tx(relay_client, ZMQ_PUSH);
	p2p_tx.connect("tcp://" + ip_addr_p2p_server + ":" + port_chat_collector);

	std::cout << "starting autonomous message transmit and receive scenario." << std::endl;


	auto recvThread = std::async(std::launch::async, p2p_recv, &p2p_rx);
	srand(time(NULL));
	while (true)
	{
		try
		{
			int rand_ = rand() % 100;

			if (rand_ < 10)
			{
				Sleep(3000);
				std::string message = "(" + std::string(user_name) + "," + ip_addr + ":ON)'";
				zmq::message_t msg(message);
				p2p_tx.send(msg, zmq::send_flags::none);
				std::cout << "p2p-send::==>>" << message << std::endl;
			}
			else if (rand_ > 90)
			{
				Sleep(3000);
				std::string message = "(" + std::string(user_name) + "," + ip_addr + ":OFF)'";
				zmq::message_t msg(message);
				p2p_tx.send(msg, zmq::send_flags::none);
				std::cout << "p2p-send::==>>" << message << std::endl;
			}
		}
		catch (const std::exception&)
		{
			break;
		}
	}

	std::cout << "closing p2p chatting program." << std::endl;

	bool global_flag_shutdown = true;
	db_client_socket.close();
	p2p_rx.close();
	p2p_tx.close();
	db_client_context.~context_t();
	relay_client.~context_t();
}