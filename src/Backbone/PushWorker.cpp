#include <iostream>
#include <chrono>
#include <thread>

#include <zmq.hpp>

#include "Backbone.hpp"
#include "IMGData.hpp"

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

	int count = 0;
	while(true){
		imgdata_t data;
		cout << "Sending " << sizeof(imgdata_t) << " bytes" << endl;
		zmq::message_t msg(sizeof(imgdata_t));
		data.id = count++;
		memcpy(msg.data(), &data, sizeof(imgdata_t));
		pushsocket.send(msg);
		std::chrono::milliseconds dura(1000);
		std::this_thread::sleep_for(dura);
	}
}
