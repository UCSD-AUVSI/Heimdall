/**
 *	author: Jason Bunk
 *	Utility functions for clustering with OpenCV.
**/

#include "SharedUtils/clustering_opencv.hpp"
#include <iostream>
using std::cout; using std::endl;


/// map 'ClusterablePoint's around an existing vector of colors
std::vector<ClusterablePoint*>* WrapClusterablePointsToCVColors(std::vector<myColor_3f> & cv_colors)
{
	std::vector<ClusterablePoint*>* retval = new std::vector<ClusterablePoint*>(cv_colors.size(), nullptr);
	
	for(int i=0; i<cv_colors.size(); i++)
	{
		retval->at(i) = new ClusterablePoint3D_OpenCV(&cv_colors[i]);
	}
	return retval;
}


/// get all pixels in an image as 'ClusterablePoint's
std::vector<ClusterablePoint*>* GetSetOfPixelColors_3Df(cv::Mat* image)
{
	std::vector<ClusterablePoint*>* retset = new std::vector<ClusterablePoint*>(image->rows*image->cols, nullptr);
	
	if(image->type() == CV_32FC3)
	{
		int n=0;
		for(int i=0; i<image->rows; i++)
		{
			for(int j=0; j<image->cols; j++)
			{
				(*retset)[n] = new ClusterablePoint3D_OpenCV(&(image->at<myColor_3f>(i,j)));
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


std::vector<ClusterablePoint*>* GetSetOfPixelColors_WithMask_3Df(cv::Mat* image, cv::Mat* mask_CV_8U_type)
{
	assert(image != nullptr && mask_CV_8U_type != nullptr);
	assert(image->empty()==false && mask_CV_8U_type->empty()==false);
	assert(image->cols == mask_CV_8U_type->cols && image->rows == mask_CV_8U_type->rows);
	assert(mask_CV_8U_type->type() == CV_8U);
	
	std::vector<ClusterablePoint*>* retset = new std::vector<ClusterablePoint*>();
	
	if(image->type() == CV_32FC3)
	{
		for(int i=0; i<image->rows; i++)
		{
			for(int j=0; j<image->cols; j++)
			{
				if(mask_CV_8U_type->at<uint8_t>(i,j) > 0)
				{
					retset->push_back(new ClusterablePoint3D_OpenCV(&(image->at<myColor_3f>(i,j))));
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


/// convert clustered pixels back to an image that displays the clusters by their colors
/// modifies the original image, so you should have made a copy of it before clustering it
cv::Mat GetClusteredImage_3Df(const std::vector<std::vector<ClusterablePoint*>> & clusters,
							std::vector<ClusterablePoint*> cluster_colors,
							cv::Mat* original_image)
{
	assert(clusters.size() == cluster_colors.size());
	
	ClusterablePoint3D_OpenCV* thispixel = nullptr;
	ClusterablePoint3D_OpenCV* thiscluster_pixelcolor = nullptr;
	
	if(original_image->type() == CV_32FC3)
	{
		for(int i=0; i<clusters.size(); i++)
		{
			thiscluster_pixelcolor = dynamic_cast<ClusterablePoint3D_OpenCV*>(cluster_colors[i]);
			
			for(int j=0; j<clusters[i].size(); j++)
			{
				thispixel = dynamic_cast<ClusterablePoint3D_OpenCV*>(clusters[i][j]);
				(*thispixel->pixel)[0] = (*thiscluster_pixelcolor->pixel)[0];
				(*thispixel->pixel)[1] = (*thiscluster_pixelcolor->pixel)[1];
				(*thispixel->pixel)[2] = (*thiscluster_pixelcolor->pixel)[2];
			}
		}
	}
	cv::Mat retval;
	original_image->copyTo(retval);
	return retval;
}


/// get the mean color of each cluster
std::vector<ClusterablePoint*> GetClusterMeanColors_3Df(const std::vector<std::vector<ClusterablePoint*>> & clusters)
{
	std::vector<ClusterablePoint*> retcolors;
	
	ClusterablePoint3D_OpenCV* thispixel = nullptr;
		
	for(int i=0; i<clusters.size(); i++)
	{
		if(clusters[i].empty())
		{
			myColor_3f* blackcolor = new myColor_3f(0.0f, 0.0f, 0.0f);
			retcolors.push_back(new ClusterablePoint3D_OpenCV(blackcolor));
		}
		else
		{
			retcolors.push_back(clusters[i][0]->MeanOf(clusters[i]));
		}
	}
	return retcolors;
}


