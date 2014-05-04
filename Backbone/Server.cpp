#include <string>
#include <vector>
#include <thread>
#include <iostream>

#include <zmq.hpp>

#include "Backbone/Backbone.hpp"
#include "Backbone/Maps.hpp"
#include "Backbone/MessageHandling.hpp"
#include "Backbone/BackStore.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/PortHandling.hpp"

#include "opencv2/opencv.hpp"

using std::cout;
using std::endl;

void startServerPort(std::vector<AlgClass> alg_class_list){
    zmq::context_t context(1);

    std::vector<int> pull_port_list = getServerPullPorts(alg_class_list);
    std::vector<int> push_port_list = getServerPushPorts(alg_class_list);
    std::vector<int> pub_port_list  = getServerPubPorts(alg_class_list);

    bool push_msgs = (push_port_list.size() > 0);
    bool pub_msgs = (pub_port_list.size() > 0);

    //Initialize the sockets
    zmq::socket_t pull_socket(context, ZMQ_PULL);
    zmq::socket_t publish_socket(context, ZMQ_PUB);

    std::vector<zmq::socket_t*> push_sockets;
    for(int i = 0; i < push_port_list.size(); i++){ //May push to multiple ports
        push_sockets.push_back(new zmq::socket_t(context, ZMQ_PUSH));
    }

    //Setup the sockets
    std::string port_str = "tcp://*:" + std::to_string(pull_port_list.front());
    pull_socket.bind(port_str.c_str());

    if(push_msgs){
        int count = 0;
        for(int port : push_port_list){
            port_str = "tcp://*:" + std::to_string(port);
            push_sockets[count++]->bind(port_str.c_str());
        }
    }

    if(pub_msgs){
        port_str = "tcp://*:" + std::to_string(pub_port_list.front());
        publish_socket.bind(port_str.c_str());
    }

    imgdata_t imdata;
    while(true){
        zmq::message_t *msg = new zmq::message_t();
        pull_socket.recv(msg);

        unpackMessageData(&imdata, msg);

        if(img_update(&imdata)){
            if(imdata.verified){
 //               img_delete(&imdata);    
            }
            for(zmq::socket_t *sock : push_sockets){
                zmq::message_t *sendmsg = new zmq::message_t(messageSizeNeeded(&imdata));
                packMessageData(sendmsg, &imdata);
                sock->send(*sendmsg);
                delete sendmsg;
            }
            if(pub_msgs){
                publish_socket.send(*msg);
            }
        }

        clearIMGData(&imdata);
        delete msg;
    }

    for(zmq::socket_t *sock : push_sockets){
        delete sock;
    }
}

int main(int argc, char* argv[]){
    for(auto& x: alg_class_dependency_map){
        std::thread newThread(startServerPort, x.first);
        newThread.detach();
    }

    cout << "Press any key to quit" << endl;
    getchar();
    
    //std::chrono::milliseconds dura(60000);
    //std::this_thread::sleep_for(dura);

    return 0;
}
