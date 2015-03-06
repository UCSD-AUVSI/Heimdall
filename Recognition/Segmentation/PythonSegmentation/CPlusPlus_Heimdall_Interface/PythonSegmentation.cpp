#include "PythonSegmentation.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "SharedUtils/PythonCVMatConvert.h"
#include "SharedUtils/GlobalVars.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/PythonUtils.hpp"
#include <fstream>
#include <boost/python.hpp>
namespace bp = boost::python;
using std::cout;
using std::endl;



static void pythonSegFunc(std::string segmentationModuleFolderName,
							std::string pythonFilename,
							std::string pythonFunctionName,
							cv::Mat inputCropImage,
							cv::Mat & returned_SSEG,
							cv::Scalar & returned_SColor,
							cv::Mat & returned_CSEG,
							cv::Scalar & returned_CColor,
							std::vector<double> *additional_args/*=nullptr*/)
{
	returned_SSEG = cv::Mat();
	returned_SColor = cv::Scalar();
	returned_CSEG = cv::Mat();
	returned_CColor = cv::Scalar();
	
	if(path_to_HeimdallBuild_directory == nullptr) {
		std::cout << "ERROR: pythonSaliency: \"path_to_HeimdallBuild_directory\" not set!" << std::endl;
		return;
	}
	
	if(PrepareForPythonStuff()) {
		aquire_py_GIL lock;
		
		// setup directories and namespaces for Python interpeter
		NDArrayConverter cvt;
		bp::object main = bp::import("__main__");
		bp::object global(main.attr("__dict__"));
		std::string dirToAdd = (*path_to_HeimdallBuild_directory)+std::string("/Recognition/Segmentation/PythonSegmentation/")+segmentationModuleFolderName+std::string("/Python");
		if(check_if_directory_exists(dirToAdd) == false) {
			std::cout << "ERROR: pythonSegFunc: couldn't find \"" << dirToAdd << "\"" << std::endl;
			return;
		}
		
		
		// load main python file into C++
		std::string loadedPyFile;
		if(read_file_contents(dirToAdd+std::string("/")+pythonFilename,loadedPyFile)==false) {
			std::cout<<"ERROR: COULD NOT FIND PYTHON FILE \""<<pythonFilename<<"\""<<std::endl;
			return;
		}
		AddPathToPythonSys(dirToAdd);
		
		
		// load main python file into the interpreter
		try {
			PyRun_SimpleString(loadedPyFile.c_str());
		}
		catch(bp::error_already_set) {
			std::cout<<"PythonSegmentation: ERROR RUNNING PYTHON FILE LOADED INTO C++: PYTHON ERROR REPORT:"<<std::endl;
			PyErr_Print(); return;
		}
		
		
		//get handle to the function
		bp::object pythoncvfunctionhandle;
		try {
			pythoncvfunctionhandle = global[pythonFunctionName];
		}
		catch(bp::error_already_set) {
			std::cout << "PythonSegmentation ERROR: function \"" << pythonFunctionName << "\" not found in python file \"" << pythonFilename << "\"! PYTHON ERROR REPORT:" << std::endl;
			PyErr_Print(); return;
		}
		//std::cout<<"done getting handle to function!"<<std::endl;
		
		
		//convert C++ cv::Mat to Python
		PyObject* givenImgCpp = cvt.toNDArray(inputCropImage);
		bp::object givenImgPyObj( bp::handle<>(bp::borrowed(givenImgCpp)) );
		
		
		//call the Python function, return data
		bp::object resultobj;
		if(additional_args != nullptr) {
			bp::list extraArgs = std_vector_to_py_list<double>(*additional_args);
			try {
				resultobj = pythoncvfunctionhandle(givenImgPyObj, extraArgs);
			}
			catch(bp::error_already_set) {
				std::cout << "PythonSegmentation ERROR: error when running python file \"" << pythonFilename << "\" WITH EXTRA ARGS! PYTHON ERROR REPORT:" << std::endl;
				PyErr_Print(); return;
			}
		}
		else {
			try {
				resultobj = pythoncvfunctionhandle(givenImgPyObj);
			}
			catch(bp::error_already_set) {
				std::cout << "PythonSegmentation ERROR: error when running python file \"" << pythonFilename << "\"! PYTHON ERROR REPORT:" << std::endl;
				PyErr_Print(); return;
			}
		}
		Py_DECREF(givenImgCpp);
		
		
		// extract results from Python to C++
		bp::tuple resultsTuple(resultobj);
		if(((int)bp::len(resultsTuple)) == 4) {
			bp::object ssegObj = boost::python::extract<boost::python::object>(resultsTuple[0])();
			returned_SSEG = cvt.toMat(ssegObj.ptr());
			
			bp::object csegObj = boost::python::extract<boost::python::object>(resultsTuple[2])();
			returned_CSEG = cvt.toMat(csegObj.ptr());
			
			bp::tuple SColorTuple(resultsTuple[1]);
			bp::tuple CColorTuple(resultsTuple[3]);
			if(((int)bp::len(SColorTuple)) != 3 || ((int)bp::len(CColorTuple)) != 3) {
				cout<<"PythonSegmentation ERROR: COLORS AREN'T 3-TUPLES"<<endl;
				cout << "PythonSegmentation ERROR: expects return value to be a 4-tuple:" << std::endl
					<< "    first the SSEG; second a 3-tuple for the shape color; third the CSEG; fourth a 3-tuple for the char color" << endl;
				return;
			}
			
			returned_SColor[0] = bp::extract<double>(SColorTuple[0]); // B
			returned_SColor[1] = bp::extract<double>(SColorTuple[1]); // G
			returned_SColor[2] = bp::extract<double>(SColorTuple[2]); // R
			
			returned_CColor[0] = bp::extract<double>(CColorTuple[0]); // B
			returned_CColor[1] = bp::extract<double>(CColorTuple[1]); // G
			returned_CColor[2] = bp::extract<double>(CColorTuple[2]); // R
		} else {
			cout << "PythonSegmentation ERROR: expects return value to be a 4-tuple:" << endl
					<< "    first the SSEG; second a 3-tuple for the shape color; third the CSEG; fourth a 3-tuple for the char color" << endl;
		}
	}
	cout<<"PS released lock"<<endl;
}


void PythonSegmentationClass::ProcessSegmentation(cv::Mat inputCropImage,
												cv::Mat & returned_SSEG,
												cv::Scalar & returned_SColor,
												cv::Mat & returned_CSEG,
												cv::Scalar & returned_CColor)
{
	pythonSegFunc(segmentationModuleFolderName,
					pythonFilename,
					pythonFunctionName,
					inputCropImage,
					returned_SSEG,
					returned_SColor,
					returned_CSEG,
					returned_CColor,
					&additional_args);
}




