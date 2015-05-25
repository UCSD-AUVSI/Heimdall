#include <iostream>
#include <stdint.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/python.hpp>
#include "SharedUtils/PythonCVMatConvert.h"
#include "Shaperec_ModuleMain.hpp"
namespace bp = boost::python;

bp::object doBPyShapeRec(bp::object SSEGsListObj)
{
	bp::list ListOfSSEGs(SSEGsListObj);
	int numElementsInList = ((int)bp::len(ListOfSSEGs));
	std::vector<cv::Mat> ssegs_vec;
	NDArrayConverter cvt;
	
	for(int ii=0; ii<numElementsInList; ii++) {
		bp::object SrcImg(ListOfSSEGs[ii]);
		ssegs_vec.push_back(cvt.toMat(SrcImg.ptr()));
	}
	
	ShapeRecModule_Main polygonShapeRecInstance("shaperec_reference_shapes");
	polygonShapeRecInstance.DoModule(&ssegs_vec);
	
	std::string shapename("");
	if(polygonShapeRecInstance.last_obtained_results.empty() == false) {
		ShapeRec_Result &bestresult = (*polygonShapeRecInstance.last_obtained_results.results.begin());
		shapename = std::string(bestresult.reference_shape_name);
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
