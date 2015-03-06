#ifndef BDSUB_H
#define BDSUB_H

#include "Backbone/IMGData.hpp"
#include "Backbone/SubClient.hpp"

class BDSub : public SubClient{
	public:
        BDSub(std::string server_addr, AlgClass alg) : SubClient(server_addr, alg){}
        void DoWork();
};

#endif
