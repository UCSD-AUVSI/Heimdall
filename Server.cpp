#include <string>
#include <thread>
#include <iostream>

#include <zmq.hpp>

#include "Backbone.hpp"

using std::cout;
using std::endl;

void setupPort(int pullPort, int pushPort, int pubPort, bool send){
	zmq::context_t context(1);

	zmq::socket_t pullSocket(context, ZMQ_PULL);
	zmq::socket_t pushSocket(context, ZMQ_PUSH);
	zmq::socket_t pubSocket(context, ZMQ_PUB);

	std::string port = "tcp://*:" + std::to_string(pullPort);
	pullSocket.bind(port.c_str());

	if(send){
		std::string port = "tcp://*:" + std::to_string(pushPort);
		pushSocket.bind(port.c_str());
	}

	if(pubPort){
		port = "tcp://*:" + std::to_string(pubPort);
		pubSocket.bind(port.c_str());
	}

	zmq::message_t msg;

	while(true){
		if(!pullSocket.recv(&msg)){
			cout << "Failed Receive" << endl;
		}
		if(send){
			if(!pushSocket.send(msg)){
				cout << "Failed" << endl;
			}
		}
		if(pubPort){
			pubSocket.send(msg);
		}
	}
}

int main(int argc, char* argv[]){
	std::thread imageThread(setupPort, IMAGES_PULL, IMAGES_PUSH, IMAGES_PUB, true);
	std::thread orthoThread(setupPort, ORTHORECT_PULL, ORTHORECT_PUSH, NO_PORT, true);
	std::thread geoThread(setupPort, GEOREF_PULL, GEOREF_PUSH, NO_PORT, true);
	std::thread salThread(setupPort, SALIENCY_PULL, SALIENCY_PUSH, SALIENCY_PUB, true);
	std::thread ssegThread(setupPort, S_SEG_PULL, S_SEG_PUSH, NO_PORT, true);
	std::thread csegThread(setupPort, C_SEG_PULL, C_SEG_PUSH, NO_PORT, true);
	std::thread targetThread(setupPort, TARGET_PULL, TARGET_PUSH, TARGET_PUB, true);
	std::thread verThread(setupPort, VERIFIED_PULL, NO_PORT, VERIFIED_PUB, false);

	imageThread.detach();
	orthoThread.detach();
	geoThread.detach();
	salThread.detach();
	ssegThread.detach();
	csegThread.detach();
	targetThread.detach();
	verThread.detach();

	cout << "Press any key to quit" << endl;
	getchar();

	return 0;
}
