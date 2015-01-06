#include "PythonColorClassifier.hpp"
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



void PythonColorClassifierClass::ProcessColorClassification(unsigned char scolorR,
															unsigned char scolorG,
															unsigned char scolorB,
															std::string & returned_scolor,
															unsigned char ccolorR,
															unsigned char ccolorG,
															unsigned char ccolorB,
															std::string & returned_ccolor)
{
	returned_scolor = std::string("");
	returned_ccolor = std::string("");
	
	if(path_to_HeimdallBuild_directory == nullptr) {
		std::cout << "ERROR: PythonColorClassification: \"path_to_HeimdallBuild_directory\" not set!" << std::endl;
		return;
	}
	
	if(PrepareForPythonStuff()) {
		aquire_py_GIL lock;
		
		// setup directories and namespaces for Python interpeter
		bp::object main = bp::import("__main__");
		bp::object global(main.attr("__dict__"));
		std::string dirToAdd = (*path_to_HeimdallBuild_directory)+std::string("/Recognition/ColorClassifier/PythonColorClassifier/")+colorclassifierModuleFolderName+std::string("/Python");
		if(check_if_directory_exists(dirToAdd) == false) {
			std::cout << "ERROR: PythonColorClassification: couldn't find \"" << dirToAdd << "\"" << std::endl;
			return;
		}
		
		
		// load main python file into C++
		std::string loadedPyFile;
		if(read_file_contents(dirToAdd+std::string("/")+pythonFilename,loadedPyFile)==false) {
			std::cout<<"ERROR: PythonColorClassification: COULD NOT FIND PYTHON FILE \""<<pythonFilename<<"\""<<std::endl;
			return;
		}
		AddPathToPythonSys(dirToAdd);
		
		
		// load main python file into the interpreter
		try {
			PyRun_SimpleString(loadedPyFile.c_str());
		}
		catch(bp::error_already_set) {
			std::cout<<"PythonColorClassification: ERROR RUNNING PYTHON FILE LOADED INTO C++: PYTHON ERROR REPORT:"<<std::endl;
			PyErr_Print(); return;
		}
		
		
		//get handle to the function
		bp::object pythoncvfunctionhandle;
		try {
			pythoncvfunctionhandle = global[pythonFunctionName];
		}
		catch(bp::error_already_set) {
			std::cout << "PythonColorClassification ERROR: function \"" << pythonFunctionName << "\" not found in python file \"" << pythonFilename << "\"! PYTHON ERROR REPORT:" << std::endl;
			PyErr_Print(); return;
		}
		//std::cout<<"done getting handle to function!"<<std::endl;
		
		
		//convert colors to 3-element lists
		std::vector<double> scolorVec(3, 0.0);
		scolorVec[0] = scolorR;
		scolorVec[1] = scolorG;
		scolorVec[2] = scolorB;
		std::vector<double> ccolorVec(3, 0.0);
		ccolorVec[0] = ccolorR;
		ccolorVec[1] = ccolorG;
		ccolorVec[2] = ccolorB;
		bp::list scolorArg = std_vector_to_py_list<double>(scolorVec);
		bp::list ccolorArg = std_vector_to_py_list<double>(ccolorVec);
		
		
		//call the Python function, return data
		bp::object resultobj;
		if(additional_args.empty() == false) {
			bp::list extraArgs = std_vector_to_py_list<double>(additional_args);
			try {
				resultobj = pythoncvfunctionhandle(scolorArg, ccolorArg, extraArgs);
			}
			catch(bp::error_already_set) {
				std::cout << "PythonColorClassification ERROR: error when running python file \"" << pythonFilename << "\" WITH EXTRA ARGS! PYTHON ERROR REPORT:" << std::endl;
				PyErr_Print(); return;
			}
		}
		else {
			try {
				resultobj = pythoncvfunctionhandle(scolorArg, ccolorArg, bp::object());
			}
			catch(bp::error_already_set) {
				std::cout << "PythonColorClassification ERROR: error when running python file \"" << pythonFilename << "\"! PYTHON ERROR REPORT:" << std::endl;
				PyErr_Print(); return;
			}
		}
		
		
		// extract results from Python to C++
		bp::tuple resultsTuple(resultobj);
		if(((int)bp::len(resultsTuple)) == 2) {
			returned_scolor = bp::extract<const char*>(bp::str(resultsTuple[0]));
			returned_ccolor = bp::extract<const char*>(bp::str(resultsTuple[1]));
		} else {
			cout << "PythonColorClassification ERROR: expects return value to be a 2-tuple (two strings):" << endl
					<< "    (first the shape color, then the char color)" << endl;
		}
	}
}




