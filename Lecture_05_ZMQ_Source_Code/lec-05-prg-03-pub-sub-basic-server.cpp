/*
*  Weather update server
*  Binds PUB socket to tcp://*:5556
*  Publishes random weather updates
*/

#include <zmq.hpp>
#include <iostream>
#include <time.h>
#include <stdlib.h>

int main()
{
	std::cout << "Publishing updates at weather server..." << std::endl;

	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_PUB);
	socket.bind("tcp://*:5556");

	int zipcode, temperature, relhumidity;
	
	srand(time(NULL));

	while (true)
	{
		zipcode = rand() % 99999 + 1;
		temperature = rand() % 215 - 80;
		relhumidity = rand() % 50 + 10;

		zmq::message_t message(20);
		snprintf((char*)message.data(), 20, "%05d %d %d", zipcode, temperature, relhumidity);
		socket.send(message, zmq::send_flags::none);
	}

	return 0;
}