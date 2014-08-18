#pragma once
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

class ClusterablePoint
{
public:
	virtual double DistTo(ClusterablePoint* other) = 0;
	
	virtual ClusterablePoint* MeanOf(const std::vector<ClusterablePoint*> & some_cluster) = 0;
};

/**
 * vector<vector<ClusterablePoint*>> CLUSTER_ALGORITHM(vector<ClusterablePoint*> *keypoints, ALGORITHM_ARGUMENTS);
**/


//example classes for use with clustering implementations

class ClusterablePoint1D : public ClusterablePoint
{
public:
	double x;
	
	ClusterablePoint1D() : x(0.0) {}
	ClusterablePoint1D(double XX) : x(XX) {}
	
	virtual double DistTo(ClusterablePoint* other)
	{
		ClusterablePoint1D* b = dynamic_cast<ClusterablePoint1D*>(other);
		return abs(x - b->x);
	}
	
	virtual ClusterablePoint* MeanOf(const std::vector<ClusterablePoint*> & some_cluster)
	{
		ClusterablePoint1D* mean = new ClusterablePoint1D(0.0);
		for(int i=0; i<some_cluster.size(); i++)
		{
			ClusterablePoint1D* b = dynamic_cast<ClusterablePoint1D*>(some_cluster[i]);
			assert(b != nullptr);
			mean->x += b->x;
		}
		mean->x /= ((double)(some_cluster.size()));
		return mean;
	}
};

class ClusterablePoint2D : public ClusterablePoint
{
public:
	double x,y;
	
	ClusterablePoint2D() : x(0.0), y(0.0) {}
	ClusterablePoint2D(double XX, double YY) : x(XX), y(YY) {}
	
	virtual double DistTo(ClusterablePoint* other)
	{
		ClusterablePoint2D* b = dynamic_cast<ClusterablePoint2D*>(other);
		return sqrt(pow(x - b->x,2.0) + pow(y - b->y,2.0));
	}
	
	virtual ClusterablePoint* MeanOf(const std::vector<ClusterablePoint*> & some_cluster)
	{
		ClusterablePoint2D* mean = new ClusterablePoint2D(0.0, 0.0);
		for(int i=0; i<some_cluster.size(); i++)
		{
			ClusterablePoint2D* b = dynamic_cast<ClusterablePoint2D*>(some_cluster[i]);
			assert(b != nullptr);
			mean->x += b->x;
			mean->y += b->y;
		}
		mean->x /= ((double)(some_cluster.size()));
		mean->y /= ((double)(some_cluster.size()));
		return mean;
	}
};

class ClusterablePoint3D : public ClusterablePoint
{
public:
	double x,y,z;
	
	ClusterablePoint3D() : x(0.0), y(0.0), z(0.0) {}
	ClusterablePoint3D(double XX, double YY, double ZZ) : x(XX), y(YY), z(ZZ) {}
	
	virtual double DistTo(ClusterablePoint* other)
	{
		ClusterablePoint3D* b = dynamic_cast<ClusterablePoint3D*>(other);
		return sqrt(pow(x - b->x,2.0) + pow(y - b->y,2.0) + pow(z - b->z,2.0));
	}
	
	virtual ClusterablePoint* MeanOf(const std::vector<ClusterablePoint*> & some_cluster)
	{
		ClusterablePoint3D* mean = new ClusterablePoint3D(0.0, 0.0, 0.0);
		for(int i=0; i<some_cluster.size(); i++)
		{
			ClusterablePoint3D* b = dynamic_cast<ClusterablePoint3D*>(some_cluster[i]);
			assert(b != nullptr);
			mean->x += b->x;
			mean->y += b->y;
			mean->z += b->z;
		}
		mean->x /= ((double)(some_cluster.size()));
		mean->y /= ((double)(some_cluster.size()));
		mean->z /= ((double)(some_cluster.size()));
		return mean;
	}
};

void PrintClusterPopulationCount(const std::vector<std::vector<ClusterablePoint*>> & clusters);

