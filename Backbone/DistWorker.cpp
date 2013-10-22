#include <string>
#include <map>
#include <list>
#include <vector>

#include <thread>
#include <iostream>
#include <stdexcept>

#include <zmq.hpp>

#include "Backbone/Backbone.hpp"
#include "Backbone/BackStore.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Algorithm.hpp"
#include "Backbone/Algs.hpp"
#include "Backbone/Maps.hpp"

using std::cout;
using std::endl;

void doWork(const std::string server_addr, void (*func)(imgdata_t&), std::vector<zmqport_t> portList){
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
		
		imgdata_t data = *static_cast<imgdata_t*>(msg.data());

		func(data);
		
		for(zmq::socket_t *sock : pushsockets){	
			zmq::message_t sendmsg(sizeof(imgdata_t));
			memcpy(sendmsg.data(), &data, sizeof(imgdata_t));
			sock->send(sendmsg);
		}
	}
}

void dusage(){
	cout << endl << "Expects the IP of the server, followed by "
		<< "one or more of the following algorithms, separated by spaces: " << endl;

	for(auto& x: algMap){
		cout << "\t" << x.first << endl;
	}
	cout << endl;
}

int main(int argc, char* argv[]){
	if(argc < 3){
		dusage();
		return 1;
	}

	std::string addr(argv[1]); //TODO: Check that this is a valid address so ZMQ doesn't die on us

	for(int i = 2; i < argc; i++){
		std::string alg(argv[i]);
		
		std::vector<zmqport_t> portList;
		void (*algFunc)(imgdata_t&);

		try{
			alg_t newalg = strMap.at(alg);
			algFunc = algMap.at(newalg);
			portList = portMap.at(newalg);
		}
		catch(const std::out_of_range &oor){
			dusage();
			return 2;
		}

		std::thread workThr(doWork, addr, algFunc, portList);
		workThr.detach();
	}

	cout << "Press any key to exit." << endl;
	getchar();

	return 0;
}
