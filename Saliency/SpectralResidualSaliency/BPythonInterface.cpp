#include <iostream>
#include <stdint.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/python.hpp>
#include "SharedUtils/PythonCVMatConvert.h"
#include "modifiedSpectralResidualSaliency.hpp"
namespace bp = boost::python;

PyObject* doBPySpectralResidualSaliency(bp::object FullsizedImage)
{
	NDArrayConverter cvt;
	cv::Mat srcImgCPP = cvt.toMat(FullsizedImage.ptr());
	
	modified_StaticSaliencySpectralResidual mssaliency;
	
	cv::Mat saliencyMap;
	mssaliency.computeSaliencyImpl_multiChannel(srcImgCPP, saliencyMap);
	
	return cvt.toNDArray(saliencyMap);
}

static void init()
{
    Py_Initialize();
    import_array();
}

BOOST_PYTHON_MODULE(bpSRsaliencylib)
{
    init();
    bp::def("doBPySpectralResidualSaliency", doBPySpectralResidualSaliency);
}
