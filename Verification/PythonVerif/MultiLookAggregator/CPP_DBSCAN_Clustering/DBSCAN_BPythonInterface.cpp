#include <iostream>
#include <stdint.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/python.hpp>
#include "SharedUtils/PythonCVMatConvert.h"
#include "SharedUtils/PythonUtils.hpp"
#include "SharedUtils/clustering/DBSCAN_NonEuclidean.hpp"
namespace bp = boost::python;
using std::cout; using std::endl;


bp::object TargetClusterDBSCAN(PyObject *distanceMatrixForClustering, int DBSCAN_minPts, double DBSCAN_epsilon)
{
	NDArrayConverter cvt;
    cv::Mat srcImg = cvt.toMat(distanceMatrixForClustering);
    if(srcImg.type() != CV_32F && srcImg.type() != CV_32FC1) {
		srcImg.convertTo(srcImg, CV_32F);
    }
    
    //You only need (and only SHOULD) give this function half of a matrix (one of the diagonal corners),
    //	and the rest should be zero. (Distances should be symmetric: D(A,B) == D(B,A))
    //This will duplicate all entries across the diagonal to accomodate that.
    cv::Mat srcTranspose;
    cv::transpose(srcImg, srcTranspose);
    srcImg += srcTranspose;
    
	std::vector<std::vector<int>> clusteredPts = DBSCAN_NonEuclidean(&srcImg,
																		DBSCAN_epsilon,
																		DBSCAN_minPts);
	
	bp::list returnedListOfLists;
	for(int ii=0; ii<clusteredPts.size(); ii++) {
		returnedListOfLists.append(std_vector_to_py_list<int>(clusteredPts[ii]));
	}
	return returnedListOfLists;
}


static void init()
{
    Py_Initialize();
    import_array();
}

BOOST_PYTHON_MODULE(pydbscancpplib)
{
    init();
    bp::def("TargetClusterDBSCAN", TargetClusterDBSCAN);
}



