/**
 *	author: Jason Bunk
 *	This and "clustering.hpp" sets up a generic clustering interface in C++.
**/

#include "clustering.hpp"
#include "SharedUtils/SharedUtils.hpp"


void PrintClusterPopulationCount(const std::vector<std::vector<ClusterablePoint*>> & clusters)
{
	if(clusters.empty() == false) {
		for(int i=0; i<clusters.size(); i++)
		{
			consoleOutput.Level1() << "cluster " << to_istring(i) << " had " << to_istring(clusters[i].size()) << " members!" << std::endl;
		}
	} else {
		consoleOutput.Level1() << "there are no clusters!" << std::endl;
	}
}
