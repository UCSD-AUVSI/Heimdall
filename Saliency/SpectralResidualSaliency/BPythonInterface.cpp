#include <iostream>
#include <stdint.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/python.hpp>
#include "SharedUtils/PythonCVMatConvert.h"
#include "spectral_residual_saliency_module.hpp"
#include "ProcessingClass.hpp"
namespace bp = boost::python;

bp::object doBPySpectralResidualSaliency(bp::object FullsizedImage)
{
	NDArrayConverter cvt;
	cv::Mat srcImgCPP = cvt.toMat(FullsizedImage.ptr());
	
	std::vector<cv::Mat> foundCrops;
	std::vector<std::pair<double,double>> cropGeolocations;
	SpectralResidualSaliencyClass saldoer;
	saldoer.ProcessSaliency(&srcImgCPP, &foundCrops, &cropGeolocations, 0);
	consoleOutput.Level1() << "SpectralResidualSaliency found " << to_istring(foundCrops.size()) << " crops" << std::endl;
	
	std::vector<bp::object> foundCropsPy;
	for(int ii=0; ii<foundCrops.size(); ii++) {
		foundCropsPy.append(bp::object(cvt.toNDArray()));
	}
	
	return bp::str(shapename.c_str());
}

static void init()
{
    Py_Initialize();
    import_array();
}

BOOST_PYTHON_MODULE(libpypolyshaperec)
{
    init();
    bp::def("doBPyShapeRec", doBPyShapeRec);
}
