#pragma once
/**
 * Interface between clustering algorithms and OpenCV.
**/

#include "SharedUtils/clustering/clustering.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "SharedUtils/SharedUtils_OpenCV.hpp"

class ClusterablePoint_OpenCV : public ClusterablePoint
{
	myColor_3f originalPixelColor;
	double row, col; //speed up distance calculations by saving as floating point
	
public:
	myColor_3f& GetPixelRef() {return originalPixelColor;}
	myColor_3f  GetPixel() {return originalPixelColor;}
	int GetRow() {return (int)row;}
	int GetCol() {return (int)col;}
	double RowD() {return row;}
	double ColD() {return col;}
	double& RefRowD() {return row;}
	double& RefColD() {return col;}
	
	ClusterablePoint_OpenCV() : row(0), col(0), originalPixelColor(myColor_3f(0.0f, 0.0f, 0.0f)) {}
	ClusterablePoint_OpenCV(int ROW, int COL, myColor_3f GIVENCOLOR) : row((double)ROW), col((double)COL), originalPixelColor(GIVENCOLOR) {}
};

class ClusterablePoint_3D_OpenCV : public ClusterablePoint_OpenCV
{
public:
    ClusterablePoint_3D_OpenCV() : ClusterablePoint_OpenCV() {}
    ClusterablePoint_3D_OpenCV(int ROW, int COL, myColor_3f GIVENCOLOR) : ClusterablePoint_OpenCV(ROW, COL, GIVENCOLOR) {}
    
	virtual double DistTo(ClusterablePoint* other) {
		ClusterablePoint_OpenCV* b = dynamic_cast<ClusterablePoint_OpenCV*>(other);
		return pow(GetPixel()[0]-b->GetPixel()[0],2.0) + pow(GetPixel()[1]-b->GetPixel()[1],2.0) + pow(GetPixel()[2]-b->GetPixel()[2],2.0);
	}
	virtual ClusterablePoint* MeanOf(const std::vector<ClusterablePoint*> & some_cluster) {
		ClusterablePoint_3D_OpenCV* mean = new ClusterablePoint_3D_OpenCV();
		if(some_cluster.empty() == false) {
			for(int i=0; i<some_cluster.size(); i++) {
				ClusterablePoint_OpenCV* b = dynamic_cast<ClusterablePoint_OpenCV*>(some_cluster[i]);
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

class ClusterablePoint_5D_OpenCV : public ClusterablePoint_OpenCV
{
public:
    double spatialScale; //normalization of spatial component, e.g. if set to numRows then row-distances will be at most 1.0
    
    ClusterablePoint_5D_OpenCV() : ClusterablePoint_OpenCV(), spatialScale(1.0) {}
    ClusterablePoint_5D_OpenCV(int ROW, int COL, myColor_3f GIVENCOLOR) : ClusterablePoint_OpenCV(ROW, COL, GIVENCOLOR), spatialScale(1.0) {}
    ClusterablePoint_5D_OpenCV(int ROW, int COL, myColor_3f GIVENCOLOR, double spScale) : ClusterablePoint_OpenCV(ROW, COL, GIVENCOLOR), spatialScale(spScale) {}
    
	virtual double DistTo(ClusterablePoint* other) {
		ClusterablePoint_OpenCV* b = dynamic_cast<ClusterablePoint_OpenCV*>(other);
		return pow(GetPixel()[0]-b->GetPixel()[0],2.0) + pow(GetPixel()[1]-b->GetPixel()[1],2.0) + pow(GetPixel()[2]-b->GetPixel()[2],2.0) + pow((RowD()-b->RowD())/spatialScale,2.0) + pow((ColD()-b->ColD())/spatialScale,2.0);
	}
	virtual ClusterablePoint* MeanOf(const std::vector<ClusterablePoint*> & some_cluster) {
		ClusterablePoint_5D_OpenCV* mean = new ClusterablePoint_5D_OpenCV();
		if(some_cluster.empty() == false) {
			for(int i=0; i<some_cluster.size(); i++) {
				ClusterablePoint_OpenCV* b = dynamic_cast<ClusterablePoint_OpenCV*>(some_cluster[i]);
				assert(b != nullptr);
				mean->GetPixelRef()[0] += b->GetPixel()[0];
				mean->GetPixelRef()[1] += b->GetPixel()[1];
				mean->GetPixelRef()[2] += b->GetPixel()[2];
				mean->RefRowD() += b->RowD();
				mean->RefColD() += b->ColD();
			}
			double some_cluster_size = ((double)(some_cluster.size()));
			mean->GetPixelRef()[0] /= some_cluster_size;
			mean->GetPixelRef()[1] /= some_cluster_size;
			mean->GetPixelRef()[2] /= some_cluster_size;
			mean->RefRowD() /= some_cluster_size;
			mean->RefColD() /= some_cluster_size;
		}
		return mean;
	}
};




/// get all pixels in an image as 'ClusterablePoint's
std::vector<ClusterablePoint*>* GetSetOfPixelColors_3Df(cv::Mat* image, double if_5D_then_scale = -1.0);


/// get masked pixels in an image as 'ClusterablePoint's
std::vector<ClusterablePoint*>* GetSetOfPixelColors_WithMask_3Df(cv::Mat* image, cv::Mat* mask_CV_8U_type, double if_5D_then_scale = -1.0);


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



