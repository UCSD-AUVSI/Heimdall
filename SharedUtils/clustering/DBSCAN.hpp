#pragma once
#include "clustering.hpp"


std::vector<std::vector<ClusterablePoint*>> DBSCAN(std::vector<ClusterablePoint*>* keypoints,
													double eps,
													int minPts);
