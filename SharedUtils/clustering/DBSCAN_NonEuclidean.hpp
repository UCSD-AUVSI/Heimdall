#pragma once
#include "clustering.hpp"
#include <iostream>
#include <opencv2/core/core.hpp>


class ClusterablePoint_NonEuclidean : public ClusterablePoint
{
public:
	cv::Mat * allPairwiseDistancesMatrix;
	int myLabel;
	
	ClusterablePoint_NonEuclidean() : myLabel(-1), allPairwiseDistancesMatrix(nullptr) {}
	ClusterablePoint_NonEuclidean(int newLabel, cv::Mat * parentMat) : myLabel(newLabel), allPairwiseDistancesMatrix(parentMat) {}
	
	virtual double DistTo(ClusterablePoint* other)
	{
		ClusterablePoint_NonEuclidean* b = dynamic_cast<ClusterablePoint_NonEuclidean*>(other);
		//std::cout<<"DISTTO: ("<<myLabel<<" --> "<<(b->myLabel)<<") == "<<(allPairwiseDistancesMatrix->at<double>(myLabel, b->myLabel))<<std::endl;
		return (double)(allPairwiseDistancesMatrix->at<float>(myLabel, b->myLabel));
	}
	
	virtual ClusterablePoint* MeanOf(const std::vector<ClusterablePoint*> & some_cluster)
	{
		std::cout<<"Error: ClusterablePoint_NonEuclidean() cant calculate the mean of a cluster!"<<std::endl;
		std::cout<<"It should only be used for DBSCAN, or other clustering algorithms that need only RELATIVE distances between points."<<std::endl;
		assert(false);
		return nullptr;
	}
};


std::vector<std::vector<int>> DBSCAN_NonEuclidean(cv::Mat * allPairwiseDistancesMatrix,
													double eps,
													int minPts);
