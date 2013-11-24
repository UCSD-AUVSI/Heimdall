#ifndef MAPS_H
#define MAPS_H

#include <map>
#include <vector>

#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"

//
// GENERAL MAPS
//

//Mapping between classes of algorithms and actual implementations
extern const std::map<std::string, std::vector<std::string>> selectMap;

//Mapping between strings referring to algorithms
//and in code references to algorithms
extern const std::map<std::string, alg_t> algStrMap; 

//Mapping between strings and algorithm classes
extern const std::map<std::string, algclass_t> algClassStrMap;

//
// DISTRIBUTED CLIENT SIDE MAPS
//

//Mapping between algorithms, and what ports they use from client-side
//First port in list is what it pulls from
//Rest are what they push to
extern const std::map<alg_t, std::vector<zmqport_t>> distPortMap;

//Mapping between algorithms, and the actual classes that implement them
extern const std::map<alg_t, void (*)(imgdata_t *)> algFuncMap;

#endif
