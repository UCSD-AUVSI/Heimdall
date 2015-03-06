#pragma once
#include "clustering.hpp"


std::vector<std::vector<ClusterablePoint*>> KMEANS(std::vector<ClusterablePoint*>* keypoints,
													std::vector<ClusterablePoint*>* guessed_initial_cluster_cores,
													int num_iterations,
													double* return_total_distances_phi=nullptr);
