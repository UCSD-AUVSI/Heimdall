#ifndef DISTCLIENT_H
#define DISTCLIENT_H

#include <zmq.hpp>
#include <string>
#include <vector>

#include "Backbone/IMGData.hpp"
#include "Backbone/Client.hpp"

class DistClient : public Client
{
	public:
		DistClient(std::string addr, void (*workfunc)(imgdata_t*), std::vector<zmqport_t> portList);
		void run();
	    static void usage();
	
	private:
		std::string server_addr;
		void (*func)(imgdata_t*);
		std::vector<zmqport_t> portList;
		void work();
};

#endif
