#include <string>
#include <map>
#include <vector>
#include <list>

#include <thread>
#include <iostream>

#include <zmq.hpp>

#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Maps.hpp"
#include "Backbone/DistClient.hpp"

using std::cout;
using std::endl;

DistClient :: DistClient(std::string addr, void (*workfunc)(imgdata_t*), std::vector<zmqport_t> ports){
	func = workfunc;
	portList = ports;
	server_addr = addr;	
}

void DistClient :: run(){
	std::thread workThr(&DistClient::work, this);
	workThr.detach();	
}

void DistClient :: work(){
	zmq::context_t context(1);

	zmq::socket_t pullsocket(context, ZMQ_PULL);
	std::string addr = "tcp://" + server_addr + ":" + std::to_string(portList[0]); //Always pulling from one port
	pullsocket.connect(addr.c_str());

	std::list<zmq::socket_t*> pushsockets;

	for(int i = 1; i < portList.size(); i++){ //May push to multiple ports
		//On heap, and while true, but should be freed when process exits anyway
		zmq::socket_t *pushsocket = new zmq::socket_t(context, ZMQ_PUSH); 
		addr = "tcp://" + server_addr + ":" + std::to_string(portList[i]);
		pushsocket->connect(addr.c_str());
		pushsockets.push_back(pushsocket);
	}

	zmq::message_t msg(sizeof(imgdata_t));
	while(true){
		pullsocket.recv(&msg);
		
		imgdata_t* data = static_cast<imgdata_t*>(msg.data());
		
		func(data);
		
		for(zmq::socket_t *sock : pushsockets){	
			zmq::message_t sendmsg(sizeof(imgdata_t));
			memcpy(sendmsg.data(), data, sizeof(imgdata_t));
			sock->send(sendmsg);
		}
	}
}

void DistClient :: usage(){
	cout << "Usage: ./DistClient [OPTION]..."  << endl;
	cout << "Starts multiple clients running individual algorithms in separate threads.\n" << endl;

	cout << "Command Line Options: \n" << endl;
	cout << "\t--server\t\tServer IP Address\n" << endl;
	cout << "\tAlgorithm Options: \n" << endl;

	for(auto& x: selectMap){
		cout << "\t--" << x.first << endl;
		for(std::string alg : x.second){
			cout << "\t\t" << alg << endl;
		}
		cout << endl;
	}
	cout << endl;
}
