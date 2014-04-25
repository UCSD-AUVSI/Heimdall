#ifndef MAPS_H
#define MAPS_H

#include <map>
#include <string>
#include <vector>

#include "Backbone/Backbone.hpp"

//Mapping between strings and algorithm classes
extern const std::map<std::string, AlgClass> alg_class_str_map;

//Dependencies between algorithm classes. Used to decide port routing
extern const std::map<std::vector<AlgClass>, std::vector<AlgClass>> alg_class_dependency_map;

#endif
