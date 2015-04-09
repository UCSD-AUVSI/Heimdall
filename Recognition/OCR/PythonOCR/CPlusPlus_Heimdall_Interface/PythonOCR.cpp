#include "PythonOCR.hpp"
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



void PythonOCRClass::ProcessOCR(cv::Mat givenCSEG,
							char & char1, double char1_conf, double char1_angle,
							char & char2, double char2_conf, double char2_angle)
{
	char1 = '\0'; char1_conf = 0.0; char1_angle = 0.0;
	char2 = '\0'; char2_conf = 0.0; char2_angle = 0.0;
	
	if(path_to_HeimdallBuild_directory == nullptr) {
		std::cout << "ERROR: PythonOCR: \"path_to_HeimdallBuild_directory\" not set!" << std::endl;
		return;
	}
	
	if(PrepareForPythonStuff()) {
		aquire_py_GIL lock;
		
		// setup directories and namespaces for Python interpeter
		NDArrayConverter cvt;
		bp::object main = bp::import("__main__");
		bp::object global(main.attr("__dict__"));
		std::string dirToAdd = (*path_to_HeimdallBuild_directory)+std::string("/Recognition/OCR/PythonOCR/")+pythonOCRModuleFolderName+std::string("/Python");
		if(check_if_directory_exists(dirToAdd) == false) {
			std::cout << "ERROR: PythonOCR: couldn't find \"" << dirToAdd << "\"" << std::endl;
			return;
		}
		
		
		// load main python file into C++
		std::string loadedPyFile;
		if(read_file_contents(dirToAdd+std::string("/")+pythonFilename,loadedPyFile)==false) {
			std::cout<<"ERROR: PythonOCR: COULD NOT FIND PYTHON FILE \""<<pythonFilename<<"\""<<std::endl;
			return;
		}
		AddPathToPythonSys(dirToAdd);
		
		
		// load main python file into the interpreter
		try {
			PyRun_SimpleString(loadedPyFile.c_str());
		}
		catch(bp::error_already_set) {
			std::cout<<"PythonOCR: ERROR RUNNING PYTHON FILE LOADED INTO C++: PYTHON ERROR REPORT:"<<std::endl;
			PyErr_Print(); return;
		}
		
		
		//get handle to the function
		bp::object pythoncvfunctionhandle;
		try {
			pythoncvfunctionhandle = global[pythonFunctionName];
		}
		catch(bp::error_already_set) {
			std::cout << "PythonOCR ERROR: function \"" << pythonFunctionName << "\" not found in python file \"" << pythonFilename << "\"! PYTHON ERROR REPORT:" << std::endl;
			PyErr_Print(); return;
		}
		//std::cout<<"done getting handle to function!"<<std::endl;
		
		
		//convert C++ cv::Mat to Python
		PyObject* givenImgCpp = cvt.toNDArray(givenCSEG);
		bp::object givenImgPyObj( bp::handle<>(bp::borrowed(givenImgCpp)) );
		
		
		//call the Python function, return data
		bp::object resultobj;
		if(additional_args.empty() == false) {
			bp::list extraArgs = std_vector_to_py_list<double>(additional_args);
			try {
				resultobj = pythoncvfunctionhandle(givenImgPyObj, extraArgs);
			}
			catch(bp::error_already_set) {
				std::cout << "PythonOCR ERROR: error when running python file \"" << pythonFilename << "\" WITH EXTRA ARGS! PYTHON ERROR REPORT:" << std::endl;
				PyErr_Print(); return;
			}
		}
		else {
			try {
				resultobj = pythoncvfunctionhandle(givenImgPyObj, bp::object());
			}
			catch(bp::error_already_set) {
				std::cout << "PythonOCR ERROR: error when running python file \"" << pythonFilename << "\"! PYTHON ERROR REPORT:" << std::endl;
				PyErr_Print(); return;
			}
		}
		Py_DECREF(givenImgCpp);
		
		
		// now extract results from Python to C++
		//
		bp::tuple resultsTuple(resultobj);
		if(((int)bp::len(resultsTuple)) == 6) {
			char1 = bp::extract<char>(resultsTuple[0]);
			char1_conf = bp::extract<double>(resultsTuple[1]);
			char1_angle = bp::extract<double>(resultsTuple[2]);
			char2 = bp::extract<char>(resultsTuple[3]);
			char2_conf = bp::extract<double>(resultsTuple[4]);
			char2_angle = bp::extract<double>(resultsTuple[5]);
		} else {
			cout << "PythonOCR ERROR: expects return value to be a 6-tuple (tuple with 6 elements):" << endl
					<< "    (char1, char1_confidence, char1_angle, char2, char2_confidence, char2_angle)" << endl;
		}
	}
}




