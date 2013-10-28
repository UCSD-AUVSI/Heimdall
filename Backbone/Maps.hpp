#ifndef MAPS_H
#define MAPS_H

#include <map>
#include <vector>

#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

//Mapping between strings referring to algorithms
//and in code references to algorithms
extern const std::map<std::string, alg_t> strMap;

//Mapping between algorithms, and what ports they use
//First port in list is what it pulls from
//Rest are what they push to
extern const std::map<alg_t, std::vector<zmqport_t>> portMap;

extern const std::map<alg_t, void (*)(imgdata_t &)> algMap;

#endif
