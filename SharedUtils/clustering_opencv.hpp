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
	myColor_3f originalPixelColor;
	int row, col;
	
public:
	myColor_3f& GetPixelRef() {return originalPixelColor;}
	myColor_3f  GetPixel() {return originalPixelColor;}
	int GetRow() {return row;}
	int GetCol() {return col;}
	
	ClusterablePoint3D_OpenCV() : row(0), col(0), originalPixelColor(myColor_3f(0.0f, 0.0f, 0.0f)) {}
	ClusterablePoint3D_OpenCV(int ROW, int COL, myColor_3f GIVENCOLOR) : row(ROW), col(COL), originalPixelColor(GIVENCOLOR) {}
	
	virtual double DistTo(ClusterablePoint* other)
	{
		ClusterablePoint3D_OpenCV* b = dynamic_cast<ClusterablePoint3D_OpenCV*>(other);
		return sqrt(pow(GetPixel()[0]-b->GetPixel()[0],2.0) + pow(GetPixel()[1]-b->GetPixel()[1],2.0) + pow(GetPixel()[2]-b->GetPixel()[2],2.0));
	}
	
	virtual ClusterablePoint* MeanOf(const std::vector<ClusterablePoint*> & some_cluster)
	{
		ClusterablePoint3D_OpenCV* mean = new ClusterablePoint3D_OpenCV();
		
		if(some_cluster.empty() == false)
		{
			for(int i=0; i<some_cluster.size(); i++)
			{
				ClusterablePoint3D_OpenCV* b = dynamic_cast<ClusterablePoint3D_OpenCV*>(some_cluster[i]);
				assert(b != nullptr);
				mean->GetPixelRef()[0] += b->GetPixel()[0];
				mean->GetPixelRef()[1] += b->GetPixel()[1];
				mean->GetPixelRef()[2] += b->GetPixel()[2];
			}
			mean->GetPixelRef()[0] /= ((double)(some_cluster.size()));
			mean->GetPixelRef()[1] /= ((double)(some_cluster.size()));
			mean->GetPixelRef()[2] /= ((double)(some_cluster.size()));
		}
		return mean;
	}
};





/// get all pixels in an image as 'ClusterablePoint's
std::vector<ClusterablePoint*>* GetSetOfPixelColors_3Df(cv::Mat* image);


/// get masked pixels in an image as 'ClusterablePoint's
std::vector<ClusterablePoint*>* GetSetOfPixelColors_WithMask_3Df(cv::Mat* image, cv::Mat* mask_CV_8U_type);


/// produces a reduced-color image that visualizes the clusters (num colors in image == num clusters)
cv::Mat GetClusteredImage_3Df(const std::vector<std::vector<ClusterablePoint*>> & clusters,
							std::vector<ClusterablePoint*> cluster_colors,
							int original_img_rows, int original_img_cols);

/// get mask of each cluster; return a vector of binary uint8 masks
std::vector<cv::Mat> GetClusterMasks_3Df(const std::vector<std::vector<ClusterablePoint*>> & clusters,
							std::vector<ClusterablePoint*> cluster_colors,
							int original_img_rows, int original_img_cols);

/// get the mean color of each cluster
std::vector<ClusterablePoint*> GetClusterMeanColors_3Df(const std::vector<std::vector<ClusterablePoint*>> & clusters);



