#include <iostream>
#include <stdint.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/python.hpp>
#include "SharedUtils/PythonCVMatConvert.h"
#include "SharedUtils/PythonUtils.hpp"
#include "SharedUtils/clustering/kmeansplusplus.hpp"
#include "SharedUtils/clustering_opencv.hpp"
namespace bp = boost::python;
using std::cout; using std::endl;


static RNG_rand_r myrng;



/*
	"num_lloyd_iterations" is the number of KMEANS iterations within each KMEANS step until
				that step is considered converged... good numbers are at least 6 to 10
				higher numbers will (may) produce (slightly) more accurate clusters
	
	"num_kmeanspp_iterations" is the number of times KMEANS is called until we decide
					to stop and pick the result that had the lowest potential...
					should probably do about 15 to 20
					higher numbers will produce more consistent (and slightly better) results
*/

bp::object ClusterKmeansPPwithMask(PyObject *filteredCropImage, PyObject *maskForClustering,
				int k_num_cores, int num_lloyd_iterations, int num_kmeanspp_iterations)
{
	NDArrayConverter cvt;
	cv::Mat srcCropImage = cvt.toMat(filteredCropImage);
	cv::Mat srcMaskImage = cvt.toMat(maskForClustering);
	
	if(srcCropImage.empty()) {
		cout<<"ClusterKmeansPPwithMask() -- error: srcCropImage was empty"<<endl<<std::flush; return bp::object();
	}
	if(srcCropImage.channels() != 3) {
		cout<<"ClusterKmeansPPwithMask() -- error: srcCropImage was not a 3-channel image"<<endl<<std::flush; return bp::object();
	}
	if(srcCropImage.type() != CV_32FC3) {
		srcCropImage.convertTo(srcCropImage, CV_32FC3);
	}
	
	//----------------------------------------------------
	// clustering / processing
	
	// get clusterable pixels
	std::vector<ClusterablePoint*>* clusterablePixels(GetSetOfPixelColors_WithMask_3Df(&srcCropImage, &srcMaskImage));
	
	// do kmeans++
	std::vector<std::vector<ClusterablePoint*>> resultsClusters(KMEANSPLUSPLUS(clusterablePixels, &myrng, k_num_cores, num_lloyd_iterations, num_kmeanspp_iterations));
	
	// get the color of each cluster
	std::vector<ClusterablePoint*> clusterColors(GetClusterMeanColors_3Df(resultsClusters));
	
	// get the binary masks of each cluster
	std::vector<cv::Mat> returnedMasksCPP(GetClusterMasks_3Df(resultsClusters, clusterColors, srcCropImage.rows, srcCropImage.cols));
	
	// draw the result to a color-clustered image
	cv::Mat drawnClusters(GetClusteredImage_3Df(resultsClusters, clusterColors, srcCropImage.rows, srcCropImage.cols));
	
	//----------------------------------------------------
	// now prepare to ship results to Python from C++
	
	int numClustersFound = (int)resultsClusters.size();
	
	bp::list returnedClusterColors; //will be a list of 3-element-lists
	bp::list returnedClusterMasks; //will be a list of cv2 numpy images
	
	for(int ii=0; ii<numClustersFound; ii++) {
		//get color of each cluster
		std::vector<double> thisClustersColors(3);
		ClusterablePoint3D_OpenCV* thiscluster_pixelcolor = dynamic_cast<ClusterablePoint3D_OpenCV*>(clusterColors[ii]);
		thisClustersColors[0] = thiscluster_pixelcolor->GetPixel()[0];
		thisClustersColors[1] = thiscluster_pixelcolor->GetPixel()[1];
		thisClustersColors[2] = thiscluster_pixelcolor->GetPixel()[2];
		returnedClusterColors.append(std_vector_to_py_list<double>(thisClustersColors));
		
		//get mask of each cluster
		PyObject* thisImgCPP = cvt.toNDArray(returnedMasksCPP[ii]);
		returnedClusterMasks.append(bp::object(bp::handle<>(bp::borrowed(thisImgCPP))));
	}
	
	//get mask of each cluster
	PyObject* drawnClustersCPP = cvt.toNDArray(drawnClusters);
	bp::object drawnClustersPython(bp::handle<>(bp::borrowed(drawnClustersCPP)));
	
	return bp::make_tuple(drawnClustersPython, returnedClusterColors, returnedClusterMasks);
}


static void init()
{
    Py_Initialize();
    import_array();
}

BOOST_PYTHON_MODULE(pykmeansppcpplib)
{
    init();
    bp::def("ClusterKmeansPPwithMask", ClusterKmeansPPwithMask);
}



