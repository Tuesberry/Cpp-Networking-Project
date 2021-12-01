#include <zmq.hpp>
#include <string>
#include <iostream>
#include <Windows.h>

int main() {
    //  Prepare our context and socket
    zmq::context_t context(1);
    // zmq는 context 객체를 만드는 것으로 시작한다.
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");

    while (true) {
        zmq::message_t request;

        //  Wait for next request from client
        socket.recv(request, zmq::recv_flags::none);
        std::cout << "Received request: " << request.to_string() << std::endl;

        //  Do some 'work'
        Sleep(1000);

        //  Send reply back to client
        socket.send(zmq::str_buffer("b'World'"), zmq::send_flags::none);
    }
    return 0;
}