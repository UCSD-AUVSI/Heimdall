#include <string>
#include <map>
#include <vector>
#include <exception>
#include <mutex>

#include "Backbone/Backbone.hpp"
#include "Backbone/Maps.hpp"
#include "Backbone/PortHandling.hpp"

const int kBasePort = 1820;

std::mutex port_lock;

bool initialized = false;

std::map<std::vector<AlgClass>, std::vector<int>> server_pull_port_map = {};
std::map<std::vector<AlgClass>, std::vector<int>> server_push_port_map = {};
std::map<std::vector<AlgClass>, std::vector<int>> server_pub_port_map  = {};
std::map<AlgClass, std::vector<int>> dist_pull_port_map = {};
std::map<AlgClass, std::vector<int>> dist_push_port_map = {};
std::map<AlgClass, std::vector<int>> client_sub_port_map = {};

void initializePortMaps(){
    int curr_port = kBasePort;

    for(auto& alg_class_list : alg_class_dependency_map){
        // ALG -> SERVER, PUSH -> PULL
        for(auto& alg_class : alg_class_list.first){
            dist_push_port_map.insert(std::pair<AlgClass, std::vector<int>>(alg_class, {curr_port}));
        }
        server_pull_port_map.insert(std::pair<std::vector<AlgClass>, std::vector<int>>(alg_class_list.first, {curr_port}));
        curr_port++;

        //SERVER -> ALGS, PUSH -> PULL
        std::vector<int> server_push_port_list = {};
        for(auto& alg_class : alg_class_list.second){
            server_push_port_list.push_back(curr_port);

            dist_pull_port_map.insert(std::pair<AlgClass, std::vector<int>>(alg_class, {curr_port}));

            curr_port++;
        }
        server_push_port_map.insert(std::pair<std::vector<AlgClass>, std::vector<int>>(alg_class_list.first, server_push_port_list));

        //SERVER -> ALGS, PUB -> SUB 
        server_pub_port_map.insert(std::pair<std::vector<AlgClass>, std::vector<int>>(alg_class_list.first, {curr_port}));
        for(auto& alg_class : alg_class_list.first){
            client_sub_port_map.insert(std::pair<AlgClass, std::vector<int>>(alg_class, {curr_port}));
        }
        curr_port++;
    }

    initialized = true;
}

std::vector<int> getServerPullPorts(std::vector<AlgClass> alg_list){
    port_lock.lock();
    if(!initialized){
        initializePortMaps();
    }
    port_lock.unlock();
    try{
        return ::server_pull_port_map.at(alg_list);
    }
    catch(std::out_of_range& oor){
        return {};
    }
}

std::vector<int> getServerPushPorts(std::vector<AlgClass> alg_list){
    port_lock.lock();
    if(!initialized){
        initializePortMaps();
    }
    port_lock.unlock();
    try{
        return ::server_push_port_map.at(alg_list);
    }
    catch(std::out_of_range& oor){
        return {};
    }
}

std::vector<int> getServerPubPorts(std::vector<AlgClass> alg_list){
    port_lock.lock();
    if(!initialized){
        initializePortMaps();
    }
    port_lock.unlock();
    try{
        return ::server_pub_port_map.at(alg_list);
    }
    catch(std::out_of_range& oor){
        return {};
    }
}

std::vector<int> getDistPullPorts(AlgClass alg){
    port_lock.lock();
    if(!initialized){
        initializePortMaps();
    }
    port_lock.unlock();
    try{
        return ::dist_pull_port_map.at(alg);
    }
    catch(std::out_of_range& oor){
        return {};
    }
}

std::vector<int> getDistPushPorts(AlgClass alg){
    port_lock.lock();
    if(!initialized){
        initializePortMaps();
    }
    port_lock.unlock();
    try{
        return ::dist_push_port_map.at(alg);
    }
    catch(std::out_of_range& oor){
        return {};
    }
}

std::vector<int> getClientSubPorts(AlgClass alg){
    port_lock.lock();
    if(!initialized){
        initializePortMaps();
    }
    port_lock.unlock();
    try{
        return ::client_sub_port_map.at(alg);
    }
    catch(std::out_of_range& oor){
        return {};
    }
}
