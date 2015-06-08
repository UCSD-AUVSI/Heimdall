/**
 *	author: Jason Bunk
 *	Utility functions for clustering with OpenCV.
**/

#include "SharedUtils/clustering_opencv.hpp"
#include <iostream>
using std::cout; using std::endl;



/// get all pixels in an image as 'ClusterablePoint's
std::vector<ClusterablePoint*>* GetSetOfPixelColors_3Df(cv::Mat* image, double if_5D_then_scale/*=-1.0*/)
{
	std::vector<ClusterablePoint*>* retset = new std::vector<ClusterablePoint*>(image->rows*image->cols, nullptr);
    ClusterablePoint* newclusterpt = nullptr;
	
	if(image->type() == CV_32FC3)
	{
		int n=0;
		for(int i=0; i<image->rows; i++)
		{
			for(int j=0; j<image->cols; j++)
			{
                newclusterpt = nullptr;
                
                if(if_5D_then_scale > 0.0) {
                    newclusterpt = new ClusterablePoint_5D_OpenCV(i, j, image->at<myColor_3f>(i,j), if_5D_then_scale);
                } else {
                    newclusterpt = new ClusterablePoint_3D_OpenCV(i, j, image->at<myColor_3f>(i,j));
                }
                
                (*retset)[n] = newclusterpt;
				n++;
			}
		}
	}
	else {
		//todo? support more image formats, e.g. 1-channel or 2-channel images; perhaps non-floating-point
		cout << "error: invalid image format for clustering! needs to be 3-channel floating point CV_32FC3" << endl;
	}
	return retset;
}


/// get ClusterablePoints for input to the kmeans++, with masking
std::vector<ClusterablePoint*>* GetSetOfPixelColors_WithMask_3Df(cv::Mat* image, cv::Mat* mask_CV_8U_type, double if_5D_then_scale/*=-1.0*/)
{
	assert(image != nullptr && mask_CV_8U_type != nullptr);
	assert(image->empty()==false && mask_CV_8U_type->empty()==false);
	assert(image->cols == mask_CV_8U_type->cols && image->rows == mask_CV_8U_type->rows);
	assert(mask_CV_8U_type->type() == CV_8U);
	
	std::vector<ClusterablePoint*>* retset = new std::vector<ClusterablePoint*>();
    ClusterablePoint* newclusterpt = nullptr;
	
	if(image->type() == CV_32FC3)
	{
		for(int i=0; i<image->rows; i++)
		{
			for(int j=0; j<image->cols; j++)
			{
				if(mask_CV_8U_type->at<uint8_t>(i,j) > 0)
				{
                    newclusterpt = nullptr;
                    
                    if(if_5D_then_scale > 0.0) {
                        newclusterpt = new ClusterablePoint_5D_OpenCV(i, j, image->at<myColor_3f>(i,j), if_5D_then_scale);
                    } else {
                        newclusterpt = new ClusterablePoint_3D_OpenCV(i, j, image->at<myColor_3f>(i,j));
                    }
                    
                    retset->push_back(newclusterpt);
				}
			}
		}
	}
	else {
		//todo? support more image formats, e.g. 1-channel or 2-channel images; perhaps non-floating-point
		cout << "error: invalid image format for clustering! needs to be 3-channel floating point CV_32FC3" << endl;
	}
	return retset;
}


/// produces a reduced-color image that visualizes the clusters (num colors in image == num clusters)
cv::Mat GetClusteredImage_3Df(const std::vector<std::vector<ClusterablePoint*>> & clusters,
							std::vector<ClusterablePoint*> cluster_colors,
							int original_img_rows, int original_img_cols)
{
	assert(clusters.size() == cluster_colors.size());
	
	ClusterablePoint_OpenCV* thiscluster_pixelcolor = nullptr;
	ClusterablePoint_OpenCV* thispixel = nullptr;
	cv::Mat returnedMat = cv::Mat::zeros(original_img_rows, original_img_cols, CV_32FC3); //three-channel floating point image
	
	for(int i=0; i<clusters.size(); i++)
	{
		thiscluster_pixelcolor = dynamic_cast<ClusterablePoint_OpenCV*>(cluster_colors[i]);
		
		for(int j=0; j<clusters[i].size(); j++)
		{
			thispixel = dynamic_cast<ClusterablePoint_OpenCV*>(clusters[i][j]);
			returnedMat.at<myColor_3f>(thispixel->GetRow(), thispixel->GetCol()) = thiscluster_pixelcolor->GetPixel();
		}
	}
	return returnedMat;
}


/// get mask of each cluster; return a vector of binary uint8 masks
std::vector<cv::Mat> GetClusterMasks_3Df(const std::vector<std::vector<ClusterablePoint*>> & clusters,
							std::vector<ClusterablePoint*> cluster_colors,
							int original_img_rows, int original_img_cols)
{
	assert(clusters.size() == cluster_colors.size());
	assert(original_img_rows > 0 && original_img_cols > 0);
	
	ClusterablePoint_OpenCV* thispixel = nullptr;
	std::vector<cv::Mat> returnedMasks(clusters.size());
	
	for(int i=0; i<clusters.size(); i++)
	{
		returnedMasks[i] = cv::Mat::zeros(original_img_rows, original_img_cols, CV_8U);
		
		for(int j=0; j<clusters[i].size(); j++)
		{
			thispixel = dynamic_cast<ClusterablePoint_OpenCV*>(clusters[i][j]);
			
			assert(thispixel->GetRow() >= 0 && thispixel->GetRow() < original_img_rows);
			assert(thispixel->GetCol() >= 0 && thispixel->GetCol() < original_img_cols);
			
			returnedMasks[i].at<uint8_t>(thispixel->GetRow(), thispixel->GetCol()) = 1;
		}
	}
	return returnedMasks;
}


/// get the mean color of each cluster
std::vector<ClusterablePoint*> GetClusterMeanColors_3Df(const std::vector<std::vector<ClusterablePoint*>> & clusters)
{
	std::vector<ClusterablePoint*> retcolors;
	
	ClusterablePoint_OpenCV* thispixel = nullptr;
		
	for(int i=0; i<clusters.size(); i++)
	{
		if(clusters[i].empty())
		{
			retcolors.push_back(new ClusterablePoint_3D_OpenCV(0, 0, myColor_3f(0.0f, 0.0f, 0.0f)));
		}
		else
		{
			retcolors.push_back(clusters[i][0]->MeanOf(clusters[i]));
		}
	}
	return retcolors;
}


