#include <string>
#include <map>
#include <vector>
#include <list>

#include <thread>
#include <iostream>

#include <zmq.hpp>

#include "Backbone/Backbone.hpp"
#include "Backbone/MessageHandling.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/DistMaps.hpp"
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

	imgdata_t data;
	while(true){
		zmq::message_t msg;
		pullsocket.recv(&msg);

		unpackMessageData(&data, &msg);

		func(&data);
		
		for(zmq::socket_t *sock : pushsockets){	
			while(data.image_data->size() > 0){
				zmq::message_t sendmsg(messageSizeNeeded(&data));
				packMessageData(&sendmsg, &data);
				sock->send(sendmsg);
				data.image_data->back()->clear();
				data.image_data->pop_back();
			}
		}

		clearIMGData(&data);
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

int main(int argc, char* argv[]){
	int i = 1;

	std::string addr = "localhost"; //Default Server Address

	std::string alg;
	std::vector<std::string> alglist;

	std::map<std::string, alg_t> local_algStrMap;

	for(auto& x: selectMap){
		local_algStrMap.insert(std::pair<std::string, alg_t>(x.first, algStrMap.at(x.second[0])));
	}

	for(int i = 1; i < argc; i++){
		if(std::string(argv[i]) == "--server"){
			if(++i >= argc){
				DistClient::usage();
				return -1;
			}
			addr = argv[i];
		}
		else{
			try{
				std::string algtype = std::string(argv[i]);
				algtype = algtype.substr(2); //Strip off two dashes

				alglist = selectMap.at(algtype);

				if(++i >= argc){
					DistClient::usage();
					return -1;
				}
				alg = std::string(argv[i]);

				if(std::find(alglist.begin(), alglist.end(), alg) == alglist.end()){
					DistClient :: usage();
					return -1;
				}

				local_algStrMap[algtype] = algStrMap.at(alg);
			}	
			catch(const std::out_of_range &oor){
				DistClient :: usage();
				return -1;
			}
		}
	}

	for(auto& x: local_algStrMap){
		if(x.second != NONE){
			DistClient* dc = new DistClient(addr, algFuncMap.at(x.second), distPortMap.at(x.second));
			dc->run();	
		}
	}

	cout << "Press any key to exit." << endl;
	getchar();

	return 0;
}
