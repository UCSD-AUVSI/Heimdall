#ifndef PORT_HANDLING_H_
#define PORT_HANDLING_H_

#include <vector>
#include "Backbone/Backbone.hpp"

std::vector<int> getServerPullPorts(std::vector<AlgClass> alg_list);
std::vector<int> getServerPushPorts(std::vector<AlgClass> alg_list);
std::vector<int> getServerPubPorts(std::vector<AlgClass> alg_list);
std::vector<int> getDistPullPorts(AlgClass alg);
std::vector<int> getDistPushPorts(AlgClass alg);
std::vector<int> getClientSubPorts(AlgClass alg);

#endif
