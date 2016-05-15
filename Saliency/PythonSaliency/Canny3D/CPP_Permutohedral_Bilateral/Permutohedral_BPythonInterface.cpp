#include <iostream>
#include <stdint.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/python.hpp>
#include "SharedUtils/PythonCVMatConvert.h"
#include "permutohedral.h"
namespace bp = boost::python;

//#include "opencv2/core/internal.hpp"


PyObject* PermutohedralBilateralFilter(PyObject *srcImgPy, float sigmaSpace, float sigmaColor)
{
    NDArrayConverter cvt;
    cv::Mat srcImg = cvt.toMat(srcImgPy);
    CV_Assert(srcImg.channels() == 3);
    CV_Assert(srcImg.type() == CV_32FC3);
    
    float invSpatialStdev = 1.0f/sigmaSpace;
    float invColorStdev = 1.0f/sigmaColor;
    
    // copy input to format used by permutohedral
    PHImage input(1, srcImg.cols, srcImg.rows, 3);
    for (int y = 0; y < input.height; y++) {
	for (int x = 0; x < input.width; x++) {
            input(x,y)[0] = srcImg.at<cv::Vec3f>(y,x)[0];
            input(x,y)[1] = srcImg.at<cv::Vec3f>(y,x)[1];
            input(x,y)[2] = srcImg.at<cv::Vec3f>(y,x)[2];
        }
    }
    
    // Construct the position vectors out of x, y, r, g, and b.
    PHImage positions(1, srcImg.cols, srcImg.rows, 5);
    for (int y = 0; y < input.height; y++) {
	for (int x = 0; x < input.width; x++) {
	    positions(x, y)[0] = invSpatialStdev * x;
	    positions(x, y)[1] = invSpatialStdev * y;
	    positions(x, y)[2] = invColorStdev * input(x, y)[0];
	    positions(x, y)[3] = invColorStdev * input(x, y)[1];
	    positions(x, y)[4] = invColorStdev * input(x, y)[2];
	}
    }
    
    // Filter the input with respect to the position vectors. (see permutohedral.h)
    PHImage out = PermutohedralLattice::filter(input, positions);
    //PHImage out = input;
    
    cv::Mat returned(srcImg.rows, srcImg.cols, CV_32FC3);
    for (int y = 0; y < input.height; y++) {
	for (int x = 0; x < input.width; x++) {
            returned.at<cv::Vec3f>(y,x)[0] = out(x,y)[0];
            returned.at<cv::Vec3f>(y,x)[1] = out(x,y)[1];
            returned.at<cv::Vec3f>(y,x)[2] = out(x,y)[2];
        }
    }
    return cvt.toNDArray(returned);
}

static void init()
{
    Py_Initialize();
    import_array();
}
BOOST_PYTHON_MODULE(pypermutohedralfiltercpplib)
{
    init();
    bp::def("PermutohedralBilateralFilter", PermutohedralBilateralFilter);
}
