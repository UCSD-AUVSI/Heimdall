/**
 *	DBSCAN_NonEuclidean - DBSCAN = density-based clustering suitable for applications with noise.
 *
 *		"Non-Euclidean" DBSCAN: same as DBSCAN but the distance metric can be anything
 *
 *	http://en.wikipedia.org/wiki/DBSCAN
**/

#include "DBSCAN_NonEuclidean.hpp"
#include "DBSCAN.hpp"

std::vector<std::vector<int>> DBSCAN_NonEuclidean(cv::Mat * allPairwiseDistancesMatrix,
													double eps,
													int minPts)
{
	assert(allPairwiseDistancesMatrix != nullptr);
	assert(allPairwiseDistancesMatrix->empty() == false);
	
	std::vector<ClusterablePoint*> processThese;
	int ii, jj;
	
	for(ii=0; ii<allPairwiseDistancesMatrix->rows; ii++) {
		processThese.push_back(new ClusterablePoint_NonEuclidean(ii, allPairwiseDistancesMatrix));
	}
	
	std::vector<std::vector<ClusterablePoint*>> results = DBSCAN(&processThese, eps, minPts);
	std::vector<std::vector<int>> returnClusters(results.size());
	
	for(ii=0; ii<results.size(); ii++) {
		returnClusters[ii].resize(results[ii].size());
		for(jj=0; jj<results[ii].size(); jj++) {
			returnClusters[ii][jj] = dynamic_cast<ClusterablePoint_NonEuclidean*>(results[ii][jj])->myLabel;
		}
	}
	
	return returnClusters;
}
