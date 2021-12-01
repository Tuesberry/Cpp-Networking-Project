#include <zmq.hpp>
#include <iostream>
#include <time.h>
#include <Windows.h>
#include <future>

void SubscriberThread(zmq::context_t* context);

int main()
{
	zmq::context_t context(1);
	
    auto thread = std::async(std::launch::async, SubscriberThread, &context);
	zmq::socket_t publisher(context, ZMQ_PUSH);
	publisher.connect("tcp://localhost:5558");

	srand(time(NULL));
	zmq::message_t message;

	while (true)
	{
		int rand_ = rand() % 100;
			
		if (rand_ < 10)
		{
            std::string message = "b'"+std::to_string(rand_)+"'";
            zmq::message_t msg(message);
			publisher.send(msg, zmq::send_flags::none);
			std::cout << "I: sending message " <<rand_ << std::endl;
            Sleep(100);
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
        std::cout << "I: received message " << message.to_string() << std::endl;
    }
}
