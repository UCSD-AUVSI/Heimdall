#ifndef PUBLISH_H
#define PUBLISH_H

#include "Backbone/IMGData.hpp"
#include "Backbone/SubClient.hpp"

class Publish : public SubClient{
	public:
        Publish(std::string server_addr, AlgClass alg) : SubClient(server_addr, alg){}
        void DoWork();
};

#endif
