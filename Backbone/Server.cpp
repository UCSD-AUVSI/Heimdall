#include <string>
#include <vector>
#include <thread>
#include <iostream>

#include <zmq.hpp>

#include "Backbone/Backbone.hpp"
#include "Backbone/MessageHandling.hpp"
#include "Backbone/BackStore.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/ServerMaps.hpp"

#include "opencv2/opencv.hpp"

using std::cout;
using std::endl;

void setupPort(zmqport_t pullPort, std::vector<zmqport_t> pushPorts, zmqport_t pubPort){
	zmq::context_t context(1);

	bool send = pushPorts[0]?1:0;

	//Initialize the sockets
	zmq::socket_t pull_socket(context, ZMQ_PULL);
	zmq::socket_t pubSocket(context, ZMQ_PUB);

	std::vector<zmq::socket_t*> pushsockets;
	for(int i = 0; i < pushPorts.size(); i++){ //May push to multiple ports
		//On heap, and while true, but should be freed when process exits anyway
		zmq::socket_t *pushsocket = new zmq::socket_t(context, ZMQ_PUSH); 
		pushsockets.push_back(pushsocket);
	}

	//Setup the sockets
	std::string port = "tcp://*:" + std::to_string(pullPort);
	pull_socket.bind(port.c_str());
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

	imgdata_t imdata;
	while(true){
		zmq::message_t *msg = new zmq::message_t();
		pull_socket.recv(msg);
		
		unpackMessageData(&imdata, msg);
	
		if(img_update(&imdata)){
			if(send){
				for(zmq::socket_t *sock : pushsockets){	
					zmq::message_t sendmsg(messageSizeNeeded(&imdata));
					packMessageData(&sendmsg, &imdata);
					sock->send(sendmsg);
				}
			}
			if(pubPort){
				pubSocket.send(*msg);
			}
			if(imdata.verified){
				img_delete(&imdata);	
			}
		}

		clearIMGData(&imdata);
		delete msg;
	}
}

int main(int argc, char* argv[]){
	// TODO: Allow for multiple publish ports
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
