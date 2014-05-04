#ifndef PUSHCLIENT_H
#define PUSHCLIENT_H

#include <zmq.hpp>
#include <string>
#include <vector>

#include "Backbone/Client.hpp"
#include "Backbone/IMGData.hpp"

void OpenAndPushAllImagesInDir(std::string dirpath, int subdir_recursion_depth_limit, std::string& server_address);
void CreatePushClientForImage(std::string server_address, std::string image_filename);

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
