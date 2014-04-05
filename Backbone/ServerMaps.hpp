#ifndef MAPS_H
#define MAPS_H

#include <map>
#include <vector>

#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"

//
// SERVER SIDE MAPS
// MODIFICATIONS UNNECESSARY FOR MODULE ADDITION
//
	
const static int NUM_SERVER_THREADS = 6;

extern const std::map<int, std::vector<zmqport_t>> serverPullPortMap;

extern const std::map<int, std::vector<zmqport_t>> serverPushPortMap;

extern const std::map<int, std::vector<zmqport_t>> serverPubPortMap;

#endif
