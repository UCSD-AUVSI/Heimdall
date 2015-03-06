#ifndef DIST_MAPS_H
#define DIST_MAPS_H

#include <map>
#include <vector>

#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"

//Mapping between classes of algorithms and actual implementations
extern const std::map<std::string, std::vector<std::string>> alg_choice_map;

//Mapping between algorithms, and the actual classes that implement them
extern const std::map<std::string, void (*)(imgdata_t *, std::string)> alg_func_map;

#endif
