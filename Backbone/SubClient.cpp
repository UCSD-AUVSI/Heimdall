#include <iostream>

#include <zmq.hpp>

#include "Backbone/PortHandling.hpp"
#include "Backbone/MessageHandling.hpp"
#include "Backbone/SubClient.hpp"

using std::cout;
using std::endl;

SubClient :: SubClient(std::string server_addr, AlgClass alg) {
    int sub_port = getClientSubPorts(alg)[0];
    
    context = new zmq::context_t(1);

    // Initialize sockets
    sub_socket = new zmq::socket_t(*context, ZMQ_SUB);
    sub_socket->setsockopt(ZMQ_SUBSCRIBE, 0, 0);

    // Connect socket
    std::string port_str = "tcp://" + server_addr + ":" + std::to_string(sub_port);
    cout << port_str << endl;
    sub_socket->connect(port_str.c_str());
}

imgdata_t * SubClient :: GetNext(){
    // imgdata_t to hold data
    imgdata_t *imdata = new imgdata_t();
    initEmptyIMGData(imdata);

    // Receive message
    zmq::message_t *msg = new zmq::message_t();
    sub_socket->recv(msg);
    unpackMessageData(imdata, msg);
    delete msg;

    return imdata;
}

SubClient :: ~SubClient() {
    delete context;
    delete sub_socket;
}
