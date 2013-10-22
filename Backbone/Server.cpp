#include <string>
#include <thread>
#include <iostream>

#include <zmq.hpp>

#include "Backbone/Backbone.hpp"
#include "Backbone/BackStore.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Algorithm.hpp"

using std::cout;
using std::endl;

void setupPort(int pullPort, int pushPort, int pubPort, bool send){
	zmq::context_t context(1);

	//Initialize the three sockets
	zmq::socket_t pullSocket(context, ZMQ_PULL);
	zmq::socket_t pushSocket(context, ZMQ_PUSH);
	zmq::socket_t pubSocket(context, ZMQ_PUB);

	//Setup the three sockets
	std::string port = "tcp://*:" + std::to_string(pullPort);
	pullSocket.bind(port.c_str());
	if(send){
		port = "tcp://*:" + std::to_string(pushPort);
		pushSocket.bind(port.c_str());
	}
	if(pubPort){
		port = "tcp://*:" + std::to_string(pubPort);
		pubSocket.bind(port.c_str());
	}

	zmq::message_t msg(sizeof(imgdata_t));

	while(true){
		pullSocket.recv(&msg);
		imgdata_t data = *static_cast<imgdata_t*>(msg.data());
		if(img_update(data)){
			if(send){
				pushSocket.send(msg);
			}
			if(pubPort){
				pubSocket.send(msg);
			}
		}
		if(data.verified){
			img_delete(data);	
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
