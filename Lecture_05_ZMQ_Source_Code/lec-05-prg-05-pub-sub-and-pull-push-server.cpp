#include <zmq.hpp>
#include <iostream>
#include <Windows.h>

int main()
{
	zmq::context_t context(1);
	zmq::socket_t publisher(context, ZMQ_PUB);
	publisher.bind("tcp://*:5557");
	zmq::socket_t collector(context, ZMQ_PULL);
	collector.bind("tcp://*:5558");

	while (true)
	{
		zmq::message_t message;
		collector.recv(message, zmq::recv_flags::none);
		std::cout << "I: publishing update " << message.to_string() << std::endl;   
        publisher.send(message, zmq::send_flags::none);
	}
}