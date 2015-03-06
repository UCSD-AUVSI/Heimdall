#ifndef SUB_CLIENT_H_
#define SUB_CLIENT_H_

#include <string>

#include <zmq.hpp>

#include "Backbone/IMGData.hpp"
#include "Backbone/Backbone.hpp"

class SubClient {
    public:
        SubClient(std::string server_addr, AlgClass my_alg);
        virtual void DoWork() =0;
        ~SubClient();

    protected:
        imgdata_t *GetNext();

    private:
        zmq::socket_t *sub_socket;
        zmq::context_t *context;
};

#endif
