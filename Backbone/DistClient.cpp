#include <string>
#include <map>
#include <vector>

#include <thread>
#include <iostream>
#include <iomanip>

#include <zmq.hpp>

#include "Backbone/Backbone.hpp"
#include "Backbone/Maps.hpp"
#include "Backbone/DistMaps.hpp"
#include "Backbone/MessageHandling.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/DistClient.hpp"
#include "Backbone/PortHandling.hpp"

#include "Verification/DisplayVerif/display_verif.hpp" //for closing the results text file

using std::cout;
using std::endl;

DistClient :: DistClient(std::string addr, void (*workfunc)(imgdata_t*, std::string), AlgClass alg_class, std::string arguments){
    func = workfunc;
    alg = alg_class;
    server_addr = addr;
    args = arguments;
}

void DistClient :: run(){
    std::thread workThr(&DistClient::work, this);
    workThr.detach();   
}

void DistClient :: work(){
    zmq::context_t context(1);

    std::vector<int> pull_port_list = getDistPullPorts(alg);
    std::vector<int> push_port_list = getDistPushPorts(alg);

    bool pull_needed = (pull_port_list.size() > 0);

    //Initialize sockets
    zmq::socket_t pull_socket(context, ZMQ_PULL);
    zmq::socket_t push_socket(context, ZMQ_PUSH);

    if(pull_needed){
        //Connect pull socket
        std::string port_str = "tcp://" + server_addr + ":" + std::to_string(pull_port_list.front());
        pull_socket.connect(port_str.c_str());
    }

    //Connect push socket
    std::string port_str = "tcp://" + server_addr + ":" + std::to_string(push_port_list.front());
    push_socket.connect(port_str.c_str());

    while(true){
        imgdata_t *imdata = new imgdata_t();
        initEmptyIMGData(imdata);
        if(pull_needed){
            zmq::message_t *msg = new zmq::message_t();
            pull_socket.recv(msg);

            unpackMessageData(imdata, msg);
            delete msg;
        }

        func(imdata, args);

        imgdata_t *curr_send_imdata = imdata;
        // Send one message per new image crop in imgdata
        while(curr_send_imdata != nullptr){
            if(messageSizeNeeded(curr_send_imdata) > 0 &&
                    curr_send_imdata->image_data != nullptr &&
                    curr_send_imdata->image_data->size()) { //If we need to send a message
                // Need to allocate on heap, too large for stack
                zmq::message_t *sendmsg = new zmq::message_t(messageSizeNeeded(curr_send_imdata));
                packMessageData(sendmsg, curr_send_imdata);
                push_socket.send(*sendmsg);
                delete sendmsg;
            }

            clearIMGData(curr_send_imdata);
            imgdata_t *temp = curr_send_imdata;
            curr_send_imdata = curr_send_imdata->next;
            delete temp;
        }
    }
}

void DistClient :: usage(){
    cout << "Usage: ./DistClient [OPTION]..."  << endl;
    cout << "Starts multiple clients running individual algorithms in separate threads.\n" << endl;

    cout << "Command Line Options: \n" << endl;
    cout << "\t--server\t\tServer IP Address\n" << endl;
    cout << "\tAlgorithm Options: \n" << endl;

    for(auto& x: alg_choice_map){
        cout << "\t--" << x.first << endl;
        for(std::string alg : x.second){
            cout << "\t\t" << alg << endl;
        }
        cout << endl;
    }
    cout << endl;
    cout << "\t--clear\n\t\tSet all algorithms to \"NONE\"" << endl;
    cout << "\t--stubs\n\t\tSet all algorithms to use stubs" << endl;
    cout << endl;
    cout << "Arguments to the clients can be included in square brackets ([]) after the algorithm is specified" << endl;
    cout << "E.g. --ocr TESS_OCR [\"--arguments value --arguments_2 value_2\"]" << endl << endl;
}

int main(int argc, char* argv[]){
    int i = 1;

    std::string addr = "localhost"; //Default Server Address 


    std::map<std::string, std::string> local_alg_map;
    std::map<std::string, std::string> local_args_map;

    bool clear_map = false, set_stub_map = false;
    for(int i = 1; i < argc; i++){
        if(std::string(argv[i]) == "--clear"){
            clear_map = true;
        }
        else if(std::string(argv[i]) == "--stubs"){
            set_stub_map = true;
        }
    }

    for(auto& x: alg_choice_map){
        if(clear_map){
            //Map is supposed to be clear, add "NONE" for every algorithm
            local_alg_map.insert(std::pair<std::string, std::string>(x.first, "NONE"));
        }
        else if(set_stub_map){
            //Find what the stub algorithm is for this alg class, and add it 
            for(std::string alg_name : x.second){
                if(alg_name.find("STUB") != std::string::npos){
                    local_alg_map.insert(std::pair<std::string, std::string>(x.first, alg_name));
                    break;
                }
            }
            //If we couldn't find a stub algorithm, just set it to none
            if(local_alg_map.find(x.first) == local_alg_map.end()){
                local_alg_map.insert(std::pair<std::string, std::string>(x.first, "NONE"));
            }
        }
        else{
            local_alg_map.insert(std::pair<std::string, std::string>(x.first, x.second[0]));
        }

        local_args_map.insert(std::pair<std::string, std::string>(x.first, ""));
    }

    // Process arguments
    for(int i = 1; i < argc; i++){
        // We have already dealt with these cases
        if(std::string(argv[i]) == "--clear" || std::string(argv[i]) == "--stubs"){
            continue;
        }

        // Display Help
        if(std::string(argv[i]) == "--help"){
            DistClient::usage();
            return -1;
        }
        // Check for server
        else if(std::string(argv[i]) == "--server"){
            if(++i >= argc){
                DistClient::usage();
                return -1;
            }
            //TODO: Check if addr is a valid address
            addr = argv[i];
        }
        // Check for user-specified algorithm choices
        else{
            // Get Algorithm Class
            std::string alg_class = std::string(argv[i]).substr(2); //Strip off two dashes

            std::vector<std::string> alg_list;
            try{
                alg_list = alg_choice_map.at(alg_class);
            }   
            catch(const std::out_of_range &oor){
                DistClient :: usage();
                return -1;
            }

            if(++i >= argc){
                DistClient::usage();
                return -1;
            }

            // Get Algorithm Implementation
            std::string alg = std::string(argv[i]);

            if(std::find(alg_list.begin(), alg_list.end(), alg) == alg_list.end()){
                DistClient :: usage();
                return -1;
            }

            // We now have an algorithm that is indeed one of the possible ones
            // Put this in our local map
            local_alg_map[alg_class] = alg;

            // Check if next string in argument list is an argument for the DistClient
            if(i + 1 < argc && std::string(argv[i+1]).front() == '[' && std::string(argv[i+1]).back() == ']'){
                std::string arg_str(argv[i+1]);
                // Cut off brackets
                arg_str = arg_str.substr(1, arg_str.size()-2);
                local_args_map[alg_class] = arg_str;

                i++;
            }
        }
    }

    DistClient* dc = 0;   
    std::locale loc;

    // Now spin off individual threads for each algorithm
    for(auto& x: local_alg_map){
        // User could specifically want to run no algorithm
        // for a certain algorithm class
        // Print what algorithm has been chosen
        std::string alg_class_str = x.first;
        std::string alg_args = local_args_map[alg_class_str];

        std::transform(alg_class_str.begin(), alg_class_str.end(), alg_class_str.begin(), toupper);
        cout << "Algorithm for ";
        cout << std::setfill('.') << std::setw(15) << std::left << alg_class_str;
        cout << std::setfill(' ') << std::setw(15) << std::left << x.second << endl;
        cout << "With arguments: " << alg_args << endl;

        if(x.second != "NONE"){
            AlgClass alg_class;
            try{
                alg_class = alg_class_str_map.at(x.first);
            }
            catch(const std::out_of_range &oor){
                // The perils of using strings for keys instead of stuff that will be caught at compile time
                cout << "\nThere is an inconsistancy between alg_choice_map and alg_class_str_map in DistMaps.cpp. ";
                cout << x.first << " is not in alg_class_str_map.\n" << endl;
                return -1;
            }

            try{
                dc = new DistClient(addr, alg_func_map.at(x.second), alg_class, alg_args);
            }
            catch(const std::out_of_range &oor){
                cout << "\nAlgorithm " << x.second << " is not mapped correctly. Check DistMaps.cpp\n" << endl;
                return -1;
            }
            dc->run();  
        }
    }
    cout << endl;

    cout << "Press any key to exit." << endl;
    getchar();

    if(outfile_verif_results != nullptr) {
        outfile_verif_results->close();
        delete outfile_verif_results;
        outfile_verif_results = nullptr;
    }

    //std::chrono::milliseconds dura(60000);
    //std::this_thread::sleep_for(dura);

    return 0;
}
