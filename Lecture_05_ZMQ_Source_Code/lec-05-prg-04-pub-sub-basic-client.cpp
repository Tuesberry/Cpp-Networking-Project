/*
*  Weather update client
*  connects SUB socket to tcp://localhost:5556
*  Collects weather updates and finds avg temp in zipcode
*/

#include <zmq.hpp>
#include <iostream>
#include <string>
#pragma warning(disable : 4996)

int main(int argc, char* argv[])
{
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_SUB);

	std::cout << "Collecting updates from weather server..." << std::endl;
	socket.connect("tcp://localhost:5556");

	// Subscribe to zipcode, default is NYC, 10001
	const char* zip_filter = (argc > 1) ? argv[1] : "10001";
	socket.setsockopt(ZMQ_SUBSCRIBE, zip_filter, strlen(zip_filter));

	// Process 5 updates
	int total_temp = 0;
	zmq::message_t string;
	int zipcode, temperature, relhumidity, update_nbr;

	for (update_nbr = 0; update_nbr < 20; update_nbr++)
	{
		socket.recv(string, zmq::recv_flags::none);

		std::istringstream iss(static_cast<char*>(string.data()));
		iss >> zipcode >> temperature >> relhumidity;

		total_temp += temperature;

		//Added from the original code
		std::cout << "Receive temperature for zipcode '" << zip_filter 
			<< "' was " << temperature << " F" << std::endl;
	}
	
	std::cout << "Average temperature for zipcode '" << zip_filter << "' was "
		<< (int)(total_temp / update_nbr) << std::endl;

	return 0;
}