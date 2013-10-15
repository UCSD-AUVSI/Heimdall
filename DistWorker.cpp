#include <string>
#include <map>
#include <list>
#include <vector>

#include <thread>
#include <iostream>
#include <stdexcept>

#include <zmq.hpp>

#include "Backbone.hpp"
#include "BackStore.hpp"

#include "Orthorect.hpp"
#include "GeoRef.hpp"
#include "Saliency.hpp"
#include "Segmentation.hpp"
#include "Rec.hpp"
#include "Verif.hpp"

using std::cout;
using std::endl;

const std::map<std::string, void (*)(imgdata_t&)> algMap = 
{
	{"Orthorect", &orthorectExec},
	{"GeoRef", &geoRefExec},
	{"Saliency", &salExec},
	{"Seg", &segExec},
	{"SRec", &sRecExec},
	{"OCR", &ocrExec},
	{"Ver", &verifExec}
};

const std::map<std::string, std::vector<port_t>> portMap =
{
	{"Orthorect",	{IMAGES_PUSH, ORTHORECT_PULL}},
	{"GeoRef",		{ORTHORECT_PUSH, GEOREF_PULL}},
	{"Saliency",	{GEOREF_PUSH, SALIENCY_PULL}},
	{"Seg",			{SALIENCY_PUSH, S_SEG_PULL, C_SEG_PULL}},
	{"SRec",		{S_SEG_PUSH, TARGET_PULL}},
	{"OCR",			{C_SEG_PUSH, TARGET_PULL}},
	{"Ver",			{TARGET_PUSH, VERIFIED_PULL}}
};

void doWork(const std::string server_addr, void (*func)(imgdata_t&), std::vector<port_t> portList){
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

void usage(){
	cout << endl << "Expects the IP of the server, followed by "
		<< "one or more of the following algorithms, separated by spaces: " << endl;

	for(auto& x: algMap){
		cout << "\t" << x.first << endl;
	}
	cout << endl;
}

int main(int argc, char* argv[]){
	if(argc < 3){
		usage();
		return 1;
	}

	std::string addr(argv[1]);

	for(int i = 2; i < argc; i++){
		std::string alg(argv[i]);

		std::vector<port_t> portList;
		void (*algFunc)(imgdata_t&);

		try{
			algFunc = algMap.at(alg);
			portList = portMap.at(alg);
		}
		catch(const std::out_of_range &oor){
			usage();
			return 2;
		}

		std::thread workThr(doWork, addr, algFunc, portList);
		workThr.detach();
	}

	cout << "Press any key to exit." << endl;
	getchar();

	return 0;
}
