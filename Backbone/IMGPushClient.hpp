#ifndef PUSHCLIENT_H
#define PUSHCLIENT_H

#include <zmq.hpp>
#include <string>
#include <vector>

#include "Backbone/Client.hpp"
#include "Backbone/IMGData.hpp"

class IMGPushClient : public Client{
    public:
        IMGPushClient(std::string addr, std::string imageloc);
        void run();
        static void usage();
    
    private:
        std::string server_addr;
        std::string image;
        void work();
};

#endif
