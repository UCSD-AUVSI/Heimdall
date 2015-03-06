#pragma once
/**
 * Interface between clustering algorithms and OpenCV.
**/

#include "SharedUtils/clustering/clustering.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "SharedUtils/SharedUtils_OpenCV.hpp"

class ClusterablePoint3D_OpenCV : public ClusterablePoint
{
public:
	myColor_3f* pixel;
	
	ClusterablePoint3D_OpenCV() : pixel(nullptr) {}
	ClusterablePoint3D_OpenCV(myColor_3f* PIXEL) : pixel(PIXEL) {}
	
	virtual double DistTo(ClusterablePoint* other)
	{
		ClusterablePoint3D_OpenCV* b = dynamic_cast<ClusterablePoint3D_OpenCV*>(other);
		return sqrt(pow((*pixel)[0]-(*b->pixel)[0],2.0) + pow((*pixel)[1]-(*b->pixel)[1],2.0) + pow((*pixel)[2]-(*b->pixel)[2],2.0));
	}
	
	virtual ClusterablePoint* MeanOf(const std::vector<ClusterablePoint*> & some_cluster)
	{
		ClusterablePoint3D_OpenCV* mean = new ClusterablePoint3D_OpenCV(nullptr);
		mean->pixel = new myColor_3f();
		
		if(some_cluster.empty() == false)
		{
			for(int i=0; i<some_cluster.size(); i++)
			{
				ClusterablePoint3D_OpenCV* b = dynamic_cast<ClusterablePoint3D_OpenCV*>(some_cluster[i]);
				assert(b != nullptr);
				(*mean->pixel)[0] += (*b->pixel)[0];
				(*mean->pixel)[1] += (*b->pixel)[1];
				(*mean->pixel)[2] += (*b->pixel)[2];
			}
			((*mean->pixel)[0]) /= ((double)(some_cluster.size()));
			((*mean->pixel)[1]) /= ((double)(some_cluster.size()));
			((*mean->pixel)[2]) /= ((double)(some_cluster.size()));
		}
		return mean;
	}
};


//------------------------------
/*class ClusterablePoint3D_better_OpenCV : public ClusterablePoint
{
public:
	myColor_3f pixel;
	int image_i;
	int image_j;
	
		...alternative implementation to using pointers; not used
};*/
//------------------------------


/// map 'ClusterablePoint's around an existing vector of colors
std::vector<ClusterablePoint*>* WrapClusterablePointsToCVColors(std::vector<myColor_3f> & cv_colors);


/// get all pixels in an image as 'ClusterablePoint's
std::vector<ClusterablePoint*>* GetSetOfPixelColors_3Df(cv::Mat* image);


/// get masked pixels in an image as 'ClusterablePoint's
std::vector<ClusterablePoint*>* GetSetOfPixelColors_WithMask_3Df(cv::Mat* image, cv::Mat* mask_CV_8U_type);


/// convert clustered pixels back to an image that displays the clusters by their colors
/// modifies the original image, so you should have made a copy of it before clustering it
cv::Mat GetClusteredImage_3Df(const std::vector<std::vector<ClusterablePoint*>> & clusters,
							std::vector<ClusterablePoint*> cluster_colors,
							cv::Mat* original_image);

/// get the mean color of each cluster
std::vector<ClusterablePoint*> GetClusterMeanColors_3Df(const std::vector<std::vector<ClusterablePoint*>> & clusters);



