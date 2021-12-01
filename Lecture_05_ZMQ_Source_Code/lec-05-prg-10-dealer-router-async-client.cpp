#include <zmq.hpp>
#include <iostream>
#include <time.h>
#include <Windows.h>
#include <future>

#pragma warning(disable : 4996)

int main(int argc, char** argv)
{
	std::string client_id = "None";
	if (argc > 1)client_id = argv[1];

	std::string identity = "b'" + client_id + "'";

	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_DEALER);
	const int size = identity.length();
	socket.setsockopt(ZMQ_IDENTITY, identity.c_str(), size);
	socket.connect("tcp://localhost:5570");
	std::cout << "Client " << identity << " started" << std::endl;

	int reqs = 0;
	while (true)
	{
		reqs++;
		std::cout << "Req #" << reqs << " sent.." << std::endl;
		std::string msg = "b'request #" + std::to_string(reqs) + "'";
		zmq::message_t message(msg);
		socket.send(message, zmq::send_flags::none);

		Sleep(1000);

		socket.recv(message, zmq::recv_flags::none);
		std::cout << identity << " received: " << message.to_string() << std::endl;
	}
}