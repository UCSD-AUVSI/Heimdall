#include <string>
#include <vector>
#include <thread>
#include <iostream>

#include <zmq.hpp>

#include "Backbone/Backbone.hpp"
#include "Backbone/BackStore.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/ServerMaps.hpp"

using std::cout;
using std::endl;

void setupPort(zmqport_t pullPort, std::vector<zmqport_t> pushPorts, zmqport_t pubPort){
	zmq::context_t context(1);

	bool send = pushPorts[0]?1:0;

	//Initialize the sockets
	zmq::socket_t pullSocket(context, ZMQ_PULL);
	zmq::socket_t pubSocket(context, ZMQ_PUB);

	std::vector<zmq::socket_t*> pushsockets;
	for(int i = 0; i < pushPorts.size(); i++){ //May push to multiple ports
		//On heap, and while true, but should be freed when process exits anyway
		zmq::socket_t *pushsocket = new zmq::socket_t(context, ZMQ_PUSH); 
		pushsockets.push_back(pushsocket);
	}

	//Setup the sockets
	std::string port = "tcp://*:" + std::to_string(pullPort);
	pullSocket.bind(port.c_str());
	if(send){
		for(int i = 0; i < pushPorts.size(); i++){
			port = "tcp://*:" + std::to_string(pushPorts[i]);
			pushsockets[i]->bind(port.c_str());
		}
	}
	if(pubPort){
		port = "tcp://*:" + std::to_string(pubPort);
		pubSocket.bind(port.c_str());
	}

	zmq::message_t msg(sizeof(imgdata_t));

	while(true){
		pullSocket.recv(&msg);
		imgdata_t* data = static_cast<imgdata_t*>(msg.data());
		if(img_update(data)){
			if(send){
				for(int i = 0; i < pushPorts.size(); i++){
					zmq::message_t sendmsg(sizeof(imgdata_t));
					memcpy(sendmsg.data(), data, sizeof(imgdata_t));
					pushsockets[i]->send(sendmsg);
				}
			}
			if(pubPort){
				pubSocket.send(msg);
			}
			if(data->verified){
				img_delete(data);	
			}
		}
	}
}

int main(int argc, char* argv[]){
	for(int i = 0; i < NUM_SERVER_THREADS; i++){
		std::thread newThread(setupPort, 
				(serverPullPortMap.at(i))[0], 
				serverPushPortMap.at(i), 
				(serverPubPortMap.at(i))[0]);
		newThread.detach();
	}

	cout << "Press any key to quit" << endl;
	getchar();

	return 0;
}
