#include <iostream>
#include <chrono>
#include <thread>

#include <zmq.hpp>

#include "Backbone.hpp"

using std::cout;
using std::endl;

void usage(){
	cout << "Expects one argument, the IP of the server" << endl;
}

int main(int argc, char* argv[]){
	if(argc != 2){
		usage();
		return 1;
	}

	std::string server_addr (argv[1]);

	zmq::context_t context(1);

	zmq::socket_t pushsocket(context, ZMQ_PUSH);
	std::string addr = "tcp://" + server_addr + ":" + std::to_string(IMAGES_PULL);
	pushsocket.connect(addr.c_str());

	zmq::message_t msg;
	
	while(true){
		pushsocket.send(msg);
		std::chrono::milliseconds dura(1000);
		std::this_thread::sleep_for(dura);
	}
}
