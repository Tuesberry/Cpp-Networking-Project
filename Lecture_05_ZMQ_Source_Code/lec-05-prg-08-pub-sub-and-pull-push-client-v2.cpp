#include <zmq.hpp>
#include <iostream>
#include <time.h>
#include <Windows.h>
#include <future>

void SubscriberThread(zmq::context_t* context);
std::string clientID;

int main(int argc, char** argv)
{
	zmq::context_t context(1);

	auto thread = std::async(std::launch::async, SubscriberThread, &context);
	zmq::socket_t publisher(context, ZMQ_PUSH);
	publisher.connect("tcp://localhost:5558");

	srand(time(NULL));
	zmq::message_t message;

	if (argc > 1)
	{
		clientID = argv[1];
	}
	else
	{
		clientID = "None";
	}

	while (true)
	{
		int rand_ = rand() % 100;

		if (rand_ < 10)
		{
			Sleep(1000);
			std::string message = "b'(" + clientID + ":ON)'";
			zmq::message_t msg(message);
			publisher.send(msg, zmq::send_flags::none);
			std::cout << clientID <<": send status - activated" << std::endl;
		}
		else if (rand_ > 90)
		{
			Sleep(1000);
			std::string message = "b'(" + clientID + ":OFF)'";
			zmq::message_t msg(message);
			publisher.send(msg, zmq::send_flags::none);
			std::cout << clientID << ": send status - deactivated" << std::endl;
		}
	}
}

void SubscriberThread(zmq::context_t* context)
{
	zmq::socket_t subscriber(*context, ZMQ_SUB);
	subscriber.set(zmq::sockopt::subscribe, "b");
	subscriber.connect("tcp://localhost:5557");

	while (true)
	{
		zmq::message_t message;
		subscriber.recv(message, zmq::recv_flags::none);
		std::cout << clientID <<" : receive status => " << message.to_string() << std::endl;
	}
}
