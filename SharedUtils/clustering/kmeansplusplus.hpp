#pragma once
#include "clustering.hpp"
#include "SharedUtils/SharedUtils_RNG.hpp"


std::vector<std::vector<ClusterablePoint*>> KMEANSPLUSPLUS(std::vector<ClusterablePoint*>* keypoints,
													RNG* random_NG,
													int k__num_cluster_cores,
													int num_lloyd_iterations,
													int num_kmeanspp_iterations);
