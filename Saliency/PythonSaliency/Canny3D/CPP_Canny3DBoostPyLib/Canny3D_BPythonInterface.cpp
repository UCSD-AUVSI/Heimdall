#include <iostream>
#include <stdint.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/python.hpp>
#include "SharedUtils/PythonCVMatConvert.h"
#include "canny_modified_from_v2_4_9.hpp"
namespace bp = boost::python;


PyObject* CannyNewFuncRGB(PyObject *srcImgPy, double threshLow, double threshHigh, int kernelSize)
{
    NDArrayConverter cvt;
    cv::Mat srcImg = cvt.toMat(srcImgPy);
	cv::Mat returnedEdges;
	
	cppCannyBunk_RGB(srcImg, returnedEdges, threshLow, threshHigh, kernelSize);
	
    return cvt.toNDArray(returnedEdges);
}

PyObject* CannyNewFuncCIELAB(PyObject *srcImgPy, double threshLow, double threshHigh, int kernelSize, double colorspacescalar)
{
    NDArrayConverter cvt;
    cv::Mat srcImg = cvt.toMat(srcImgPy);
	cv::Mat returnedEdges;
	
	cppCannyBunk_CIELAB(srcImg, returnedEdges, threshLow, threshHigh, kernelSize, colorspacescalar);
	
    return cvt.toNDArray(returnedEdges);
}

PyObject* CannyVanilla(PyObject *srcImgPy, double threshLow, double threshHigh, int kernelSize)
{
    NDArrayConverter cvt;
    cv::Mat srcImg = cvt.toMat(srcImgPy);
	cv::Mat returnedEdges;
	if(srcImg.channels() > 1) {
		cv::cvtColor(srcImg, srcImg, CV_BGR2GRAY);
	}
	
	cv::Canny(srcImg, returnedEdges, threshLow, threshHigh, kernelSize, true);
	
    return cvt.toNDArray(returnedEdges);
}

static void init()
{
    Py_Initialize();
    import_array();
}

BOOST_PYTHON_MODULE(pycanny3dcpplib)
{
    init();
    bp::def("RGB", CannyNewFuncRGB);
    bp::def("CIELAB", CannyNewFuncCIELAB);
    bp::def("vanilla", CannyVanilla);
}
