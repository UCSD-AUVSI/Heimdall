/**
 *	author: Jason Bunk
 *
 *	KMEANS - One of the simplest clustering algorithms.
 *			Needs initial guesses, then converges to the local optimum.
 *				- Each point is assigned its closest cluster core
 *				- The mean (average) of each resulting cluster is taken as the new cluster core,
 *				- then the algorithm is run again (until desired # runs; will eventually stop changing)
 *			The result depends on the initial guesses. Will eventually converge to local optimum,
 *			which is usually not the global optimum (not the best clustering) unless the
 *			initial guesses are chosen very carefully.
 *				The goodness of the clustering is measured by "phi" which is the sum of the
 *				distances of each point to their corresponding cluster core.
 *			See kmeans++ for an attempt to find good initial guesses, to try to reach the global optimum.
 *
 *	http://en.wikipedia.org/wiki/K-means
**/

#include "kmeans.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include <iostream>


static std::vector<std::vector<ClusterablePoint*>> KMEANS_cluster_once(std::vector<ClusterablePoint*>* keypoints,
													std::vector<ClusterablePoint*>* guessed_cluster_cores,
													double* return_total_distances_phi)
{
	int numKeyPts = keypoints->size();
	int numClusterCores = guessed_cluster_cores->size();
	if(return_total_distances_phi != nullptr) {
		(*return_total_distances_phi) = 0.0;
	}
	
	std::vector<std::vector<ClusterablePoint*>> clusters(numClusterCores);
	double closestClusterDist, thisClusterDist;
	int closestClusterIndex;
	
	for(int i=0; i<numKeyPts; i++)
	{
		closestClusterDist = 0.0;
		closestClusterIndex = -1;
		
		for(int j=0; j<numClusterCores; j++)
		{
			thisClusterDist = keypoints->at(i)->DistTo(guessed_cluster_cores->at(j));
			if(thisClusterDist < closestClusterDist || closestClusterIndex < 0)
			{
				closestClusterDist = thisClusterDist;
				closestClusterIndex = j;
			}
		}
		
		clusters[closestClusterIndex].push_back(keypoints->at(i));
		
		if(return_total_distances_phi != nullptr) {
			(*return_total_distances_phi) += closestClusterDist;
		}
	}
	
	return clusters;
}

std::vector<std::vector<ClusterablePoint*>> KMEANS(std::vector<ClusterablePoint*>* keypoints,
													std::vector<ClusterablePoint*>* guessed_initial_cluster_cores,
													int num_iterations,
													double* return_total_distances_phi/*=nullptr*/)
{
	std::vector<std::vector<ClusterablePoint*>> clusters = KMEANS_cluster_once(keypoints, guessed_initial_cluster_cores, return_total_distances_phi);
	
	if(num_iterations > 1)
	{
		int numClusterCores = guessed_initial_cluster_cores->size();
		num_iterations--; //we already did one step
		
		std::vector<ClusterablePoint*>* new_guesses_cluster_cores = new std::vector<ClusterablePoint*>();
		
		for(int i=0; i<num_iterations; i++)
		{
			//get means of each cluster
			for(int c=0; c<numClusterCores; c++)
			{
				new_guesses_cluster_cores->push_back(keypoints->front()->MeanOf(clusters[c]));
			}
			clusters = KMEANS_cluster_once(keypoints, new_guesses_cluster_cores, return_total_distances_phi);
			
			new_guesses_cluster_cores->clear();
		}
		
		delete new_guesses_cluster_cores;
	}
	
	return clusters;
}
