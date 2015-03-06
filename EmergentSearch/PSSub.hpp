#ifndef PSSUB_H
#define PSSUB_H

#include "Backbone/IMGData.hpp"
#include "Backbone/SubClient.hpp"

class PSSub : public SubClient{
	public:
        PSSub(std::string server_addr, AlgClass alg) : SubClient(server_addr, alg){}
        void DoWork();
};

#endif
