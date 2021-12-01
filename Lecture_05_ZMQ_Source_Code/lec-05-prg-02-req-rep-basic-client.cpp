#include <zmq.hpp>
#include <iostream>
#include <string>

int main()
{
	zmq::context_t context(1);
	
	// Socket to talk to server
	std::cout << "Connection to hello world server..." << std::endl;

	zmq::socket_t requester(context, ZMQ_REQ);
	requester.connect("tcp://localhost:5555");

	// Do 10 requests, waiting each time for a response
	zmq::message_t message;
	for (int request = 0; request < 10; request++)
	{
		std::cout << "Sending request " << request << " ..." << std::endl;
		requester.send(zmq::str_buffer("b'Hello'"), zmq::send_flags::none);

		// Get the reply.
		requester.recv(message, zmq::recv_flags::none);
		std::cout << "Received reply " << request << " [ " << message.to_string() << " ]" << std::endl;
	}
}