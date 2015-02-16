/**
 *	author: Jason Bunk
 *
 *	KMEANS++ - The "++" means an improved formulation of initial conditions
 *				for KMEANS to try to get closer to the optimal clustering.
 *				Steps 1-5 (and 7) are the "++" and step 6 is plain KMEANS.
 *				
 *		
 *		1. pick a random point to be the first keypoint
 *		
 *		2. generate a vector of distances to this first point, and add up all of these distances
 *		
 *		3. pick a value from 0 to the sum of all the distances, initialize a value to this distance,
 *		and iterate through (subtracting each distance in the vector from the value) until you get to the new point
 *						(when the value == 0), skipping points already chosen (which have distance 0 anyway)
 *		
 *		4. go through the vector again, possibly replacing each distance with the distance to the 2nd point if
 *		that distance is shorter (just pick the min of the 2 values); and re-adding all the distances
 *		
 *		5. repeat 3 and 4 again and again until you have k__num_cluster_cores
 *		
 *		6. call the KMEANS algorithm with these cluster cores
 *		
 *		7. evaluate the potential (how well the clusters clustered), repeat steps 1-6 until potential is low enough
 *
 *	http://en.wikipedia.org/wiki/K-means++
**/

#include "kmeans.hpp"
#include "kmeansplusplus.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include <algorithm>

#include <iostream>
#define PRINTDEBUGINFO(stuffTOPRINT) 
									//std::cout<<stuffTOPRINT<<std::endl;
									//this was an ugly macro, don't do this



static std::vector<ClusterablePoint*> KMEANSPLUSPLUS_get_cluster_cores(std::vector<ClusterablePoint*>* keypoints,
													RNG* random_NG,
													int k__num_cluster_cores)
{
	assert(random_NG != nullptr);
	
	//initialize stuff
	int num_keypoints = keypoints->size();
	int num_keypoints_minus1 = (num_keypoints - 1);
	std::vector<ClusterablePoint*> initial_cluster_cores;
	std::vector<double> min_euclid_distances(num_keypoints, 1.8e16); //should be positive infinity
	std::vector<double> running_total_min_euclid_distances(num_keypoints, 0.0);
	double curr_euclid_dist = 0.0;
	
	
	//step 1
	int last_core_idx = random_NG->rand_int(0, num_keypoints_minus1);
	initial_cluster_cores.push_back((*keypoints)[last_core_idx]);
	
	PRINTDEBUGINFO("INITIAL CORE IDX (index of first chosen point): " << last_core_idx);
	
	
	//this loop explained as step 5
	while(initial_cluster_cores.size() < k__num_cluster_cores)
	{
		
		//step 2 or 4
		for(int i=0; i<num_keypoints; i++) {
			curr_euclid_dist = ((*keypoints)[i])->DistTo((*keypoints)[last_core_idx]);
			min_euclid_distances[i] = std::min(curr_euclid_dist, min_euclid_distances[i]);
			if(i == 0) {
				running_total_min_euclid_distances[0] = min_euclid_distances[i];
			} else {
				running_total_min_euclid_distances[i] = (min_euclid_distances[i] + running_total_min_euclid_distances[i-1]);
			}
			
			PRINTDEBUGINFO("min_euclid_distances[" << i << "] == " << min_euclid_distances[i]);
		}
		PRINTDEBUGINFO("sum of euclid distances: " << running_total_min_euclid_distances.back());
		
		
		//step 3
		//choose a point randomly, but weighted by distance,
		//so points further away from all existing clusters are preferred
		double chosen_sum_euclid_distances_rand = random_NG->rand_double(0.0, running_total_min_euclid_distances.back());
		
		//binary search to quickly get the chosen point
		std::vector<double>::iterator foundit = std::upper_bound(running_total_min_euclid_distances.begin(),
																running_total_min_euclid_distances.end(),
																chosen_sum_euclid_distances_rand);
		
		//the array was not only sorted ascending, but each point corresponds 1-to-1 with the original array of keypoints
		//so we can get keypoint index as the distance between the pointer of foundit and the start of the array
		last_core_idx = (&(*foundit)) - (&(running_total_min_euclid_distances[0]));
		
		
		PRINTDEBUGINFO("randomly chosen new last_core_idx: " << last_core_idx);
		
		
		//fix if the randomly chosen point was already a guessed cluster core
		if(min_euclid_distances[last_core_idx] == 0.0)
		{
			int starting_idx = last_core_idx;
			while(last_core_idx < num_keypoints_minus1 && min_euclid_distances[last_core_idx] == 0.0) {
				last_core_idx++;
			}
			if(min_euclid_distances[last_core_idx] == 0.0) {
				last_core_idx = starting_idx;
				while(last_core_idx > 0 && min_euclid_distances[last_core_idx] == 0.0) {
					last_core_idx--;
				}
				if(min_euclid_distances[last_core_idx] == 0.0) {
					std::cout << "ERROR IN KMEANS++: K (NUM CLUSTERS) >= NUM POINTS" << std::endl;
					return initial_cluster_cores;
				}
			}
		}
		
		PRINTDEBUGINFO("updated new last_core_idx (in case it was already in cluster): " << last_core_idx);
		
		
		//now add the new guessed cluster core
		initial_cluster_cores.push_back((*keypoints)[last_core_idx]);
	}
	
	return initial_cluster_cores;
}



std::vector<std::vector<ClusterablePoint*>> KMEANSPLUSPLUS(std::vector<ClusterablePoint*>* keypoints,
													RNG* random_NG,
													int k__num_cluster_cores,
													int num_lloyd_iterations,
													int num_kmeanspp_iterations)
{
	//call "KMEANSPLUSPLUS_run_once" "num_kmeanspp_iterations" times
	//return the cluster-set with the lowest potential
	
	std::vector<ClusterablePoint*> clustercores;
	std::vector<std::vector<ClusterablePoint*>> best_clusters;
	double min_total_dist_potential = 1.8e16; //should be positive infinity
	double last_total_dist_potential = 0.0;
	
	
	for(int iii=0; iii<num_kmeanspp_iterations; iii++)
	{
		clustercores.clear();
		
		
		//use KMEANS++ to get good initial guesses for cluster cores
		clustercores = KMEANSPLUSPLUS_get_cluster_cores(keypoints, random_NG, k__num_cluster_cores);
		
		
		//then call plain KMEANS (step 6)
		std::vector<std::vector<ClusterablePoint*>> last_clusters = KMEANS(keypoints, &clustercores, num_lloyd_iterations, &last_total_dist_potential);
		
		
		consoleOutput.Level3() << "last potential: " << last_total_dist_potential << std::endl;
		
		
		//keep the result with the lowest potential (the best clustering) (step 7)
		if(last_total_dist_potential < min_total_dist_potential) {
			min_total_dist_potential = last_total_dist_potential;
			best_clusters = last_clusters;
		}
	}
	
	
	consoleOutput.Level3() << "kmeans++ finished... lowest found potential: " << min_total_dist_potential << std::endl;
	
	return best_clusters;
}






