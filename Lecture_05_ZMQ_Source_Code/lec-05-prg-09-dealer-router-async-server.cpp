#include <zmq.hpp>
#include <iostream>
#include <time.h>
#include <Windows.h>
#include <future>
#include <vector>
#include <zmq_addon.hpp>

void ServerWorker(zmq::context_t* context, int id);

int main(int argc, char** argv)
{
	int num_server = 5;
	if (argc > 1)num_server = int(argv[1]);
	
	zmq::context_t context(1);
	zmq::socket_t frontend (context, ZMQ_ROUTER);
	frontend.bind("tcp://*:5570");

	zmq::socket_t backend(context, ZMQ_DEALER);
	backend.bind("inproc://backend");

	std::future<void>* workers = new std::future<void>[num_server];
	for (int i = 0; i < num_server; i++)
	{
		workers[i] = std::async(std::launch::async, ServerWorker, &context, i);
	}
	
	zmq::proxy(frontend, backend);

	frontend.close();
	backend.close();
}

void ServerWorker(zmq::context_t* context, int id)
{
	zmq::socket_t worker = zmq::socket_t(*context, ZMQ_DEALER);
	
	worker.connect("inproc://backend");

	std::cout << "Worker#" << id << " started" << std::endl;

	std::string ident, msg;

	while (true)
	{
		std::vector<zmq::message_t> message;
		zmq::recv_multipart(worker, std::back_inserter(message), zmq::recv_flags::none);
		ident = message[0].to_string();
		msg = message[1].to_string();
		std::cout << "Worker#" << id << " received " << msg << " from " << ident << std::endl;

		std::array<zmq::message_t, 2> send_msgs = {
			zmq::message_t(ident),
			zmq::message_t(msg)
		};
		zmq::send_multipart(worker, send_msgs);
	}

	worker.close();
	
}

