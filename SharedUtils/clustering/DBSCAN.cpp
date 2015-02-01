/**
 *	DBSCAN - density-based clustering suitable for applications with noise.
 *
 *			This algorithm is significantly slower than KMEANS or KMEANS++,
 *			because DBSCAN is complexity O(N^2) in the number of points N.
 *			KMEANS is order O(k*N) where k is number of clusters (usually small compared to N),
 *			and KMEANS++ is order O(k*p*N) where p is number of loops before choosing optimum (also usually small).
 *		
 *		When used for image color clustering: this isn't great for clustering by color alone,
 *		because there can be a "bridge" of intermediate colors that causes clusters to combine.
 *		See "dbscan_bridging_problem.png"
 *		One solution to this is to use 5-D clustering (3 color + 2 space), but KMEANS++ is still much better.
 *
 *	http://en.wikipedia.org/wiki/DBSCAN
**/

#include "DBSCAN.hpp"


static std::vector<int> DBSCAN_RegionQuery(std::vector<ClusterablePoint*>* keypoints, ClusterablePoint* keypoint, double eps)
{
	double dist;
	std::vector<int> retKeys;
	for(int i=0; i<keypoints->size(); i++)
	{
		dist = keypoint->DistTo(keypoints->at(i));
		if(dist <= eps && dist > 0.0)
		{
		    retKeys.push_back(i);
		}
	}
	return retKeys;
}


std::vector<std::vector<ClusterablePoint*>> DBSCAN(std::vector<ClusterablePoint*>* keypoints,
													double eps,
													int minPts)
{
	const int numKeyPts = keypoints->size();
	
	std::vector<std::vector<ClusterablePoint*>> clusters;
	std::vector<int> noise;
	std::vector<int> neighborPts;
	std::vector<int> neighborPts_;
	
	//init clustered and visited
	std::vector<bool> clustered(numKeyPts, false);
	std::vector<bool> visited(numKeyPts, false);
	
	//c = 0
	int c = -1; //array indexing trick

	//for each unvisted point P in dataset keypoints
	for(int i=0; i<numKeyPts; i++)
	{
		if(visited[i] == false)
		{
		    //Mark P as visited
		    visited[i] = true;
		    neighborPts = DBSCAN_RegionQuery(keypoints, keypoints->at(i), eps);
		    if(neighborPts.size() < minPts)
			{
		        //Mark P as Noise
		        noise.push_back(i);
			}
			else
		    {
				//c = next cluster
		        clusters.push_back(std::vector<ClusterablePoint*>());
		        c++;
				
		        //expandCluster()
				{
				    // add P to cluster c
					clustered[i] = true;
				    clusters[c].push_back(keypoints->at(i));
				    
					//for each point P' in neighborPts
				    for(int j=0; j<neighborPts.size(); j++)
				    {
				        //if P' is not visited
				        if(visited[neighborPts[j]] == false)
				        {
				            //Mark P' as visited
				            visited[neighborPts[j]] = true;
				            neighborPts_ = DBSCAN_RegionQuery(keypoints, keypoints->at(neighborPts[j]), eps);
				            if(neighborPts_.size() >= minPts)
				            {
								//NeighborPts = NeighborPts joined with NeighborPts'
				                neighborPts.insert(neighborPts.end(),neighborPts_.begin(),neighborPts_.end());
				            }
				        }
				        // if P' is not yet a member of any cluster
				        if(clustered[neighborPts[j]] == false)
						{
							// add P' to cluster c
				            clusters[c].push_back(keypoints->at(neighborPts[j]));
							clustered[neighborPts[j]] = true;
						}
				    }
				}
		    }

		}
	}
	
	if(clusters.empty()) {
		clusters.push_back(std::vector<ClusterablePoint*>());
	}
	
	return clusters;
}

