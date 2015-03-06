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

PyObject* ClusterKmeansPPwithMask(PyObject *filteredCropImage, PyObject *maskForClustering,
				int k_num_cores, int num_lloyd_iterations, int num_kmeanspp_iterations)
{
	NDArrayConverter cvt;
	cv::Mat srcCropImage = cvt.toMat(filteredCropImage);
	cv::Mat srcMaskImage = cvt.toMat(maskForClustering);
	
	if(srcCropImage.empty()) {
		cout<<"ClusterKmeansPPwithMask() -- error: srcCropImage was empty"<<endl<<std::flush; return nullptr;
	}
	if(srcCropImage.channels() != 3) {
		cout<<"ClusterKmeansPPwithMask() -- error: srcCropImage was not a 3-channel image"<<endl<<std::flush; return nullptr;
	}
	if(srcCropImage.type() != CV_32FC3) {
		srcCropImage.convertTo(srcCropImage, CV_32FC3);
	}
	
	/*
		Because of how the current "clustering_opencv" code works,
		this modifies the pixels of "srcCropImage"... which probably doesn't modify the original Python image
	*/
	std::vector<ClusterablePoint*>* clusterablePixels = GetSetOfPixelColors_WithMask_3Df(&srcCropImage, &srcMaskImage);
	
	std::vector<std::vector<ClusterablePoint*>> resultsClusters = KMEANSPLUSPLUS(clusterablePixels, &myrng, k_num_cores, num_lloyd_iterations, num_kmeanspp_iterations);
	
	std::vector<ClusterablePoint*> clusterColors = GetClusterMeanColors_3Df(resultsClusters);
	
	memset(srcCropImage.data, 0, sizeof(float)*3*srcCropImage.rows*srcCropImage.cols);
	//srcCropImage = cv::Mat::zeros(srcCropImage.size(), srcCropImage.type());
	cv::Mat resultMat = GetClusteredImage_3Df(resultsClusters, clusterColors, &srcCropImage);
	
	return cvt.toNDArray(resultMat);
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



