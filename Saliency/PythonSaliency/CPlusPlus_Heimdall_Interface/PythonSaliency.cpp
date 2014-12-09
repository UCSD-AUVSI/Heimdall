#include "PythonSaliency.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "SharedUtils/PythonCVMatConvert.h"
#include "SharedUtils/GlobalVars.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include <fstream>
#include <boost/python.hpp>
namespace bp = boost::python;
using std::cout;
using std::endl;


static bool read_file_contents(std::string filename, std::string & returnedFileContents)
{
	returnedFileContents.clear();
	std::ifstream myfile(filename);
	if(myfile.is_open() && myfile.good()) {
		std::string line;
		while(std::getline(myfile,line)) {
			returnedFileContents = (returnedFileContents + std::string("\n") + line);
		}
		myfile.close();
		return true;
	}
	return false;
}


static void AddPathToPythonSys(std::string path)
{
	// now insert the current working directory into the python path so module search can take advantage
	// this must happen after python has been initialised
	
	//boost::filesystem::path workingDir = boost::filesystem::absolute("./").normalize();
	
	std::cout << "adding to python path: \"" << path << "\"" << std::endl;
	
	PyObject* sysPath = PySys_GetObject("path");
	PyList_Insert( sysPath, 0, PyString_FromString(path.c_str()));
	//Py_DECREF(sysPath); -- DONT DO THIS
	
	//print python's search paths to confirm that it was added
	/*PyRun_SimpleString(	"import sys\n"
						"from pprint import pprint\n"
						"pprint(sys.path)\n");*/
	
	//alternate methods, tried and had trouble with?
	/*PyObject* sysPath = PySys_GetObject((char*)"path");
	PyObject* prognam1 = PyString_FromString("/mywork/AUVSI/Heimdall/build");
	PyList_Append(sysPath, prognam1);
	Py_DECREF(prognam1);
	/*Py_Initialize();
	PyObject* sysPath = PySys_GetObject((char*)"path");
	PyList_Append(sysPath, PyString_FromString("."));*/
}


void pythonSaliency(std::string saliencyModuleFolderName,
					std::string pythonFilename,
					std::string pythonFunctionName,
					cv::Mat fullsizeImage,
					std::vector<cv::Mat> & returnedCrops,
					std::vector<std::pair<double,double>> & returned_geolocations)
{
	returnedCrops.clear();
	returned_geolocations.clear();
	
	if(path_to_HeimdallBuild_directory == nullptr) {
		std::cout << "ERROR: pythonSaliency: \"path_to_HeimdallBuild_directory\" not set!" << std::endl;
		return;
	}
	
	//initialize embedded Python interpreter
	if(!Py_IsInitialized()) {
		Py_Initialize();
	}
	if(!Py_IsInitialized()) {
		cout << "ERROR: pythonSaliency: COULD NOT INITIALIZE PYTHON INTERPRETER: Python Saliency" << endl;
	}
	else {
		// setup directories and namespaces for Python interpeter
		NDArrayConverter cvt;
		bp::object main = bp::import("__main__");
		bp::object global(main.attr("__dict__"));
		std::string dirToAdd = (*path_to_HeimdallBuild_directory)+std::string("/Saliency/PythonSaliency/")+saliencyModuleFolderName+std::string("/Python");
		if(check_if_directory_exists(dirToAdd) == false) {
			std::cout << "ERROR: pythonSaliency: couldn't find \"" << dirToAdd << "\"" << std::endl;
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
			std::cout<<"PYTHON SALIENCY: ERROR RUNNING PYTHON FILE LOADED INTO C++: PYTHON ERROR REPORT:"<<std::endl;
			PyErr_Print(); return;
		}
		
		
		/*NOTE: DON'T UNCOMMENT: THIS METHOD HAD MANY MORE PROBLEMS THAN JUST LOADING THE FILE INTO C++ AND USING PyRun_SimpleString()
		try { bp::object execfile = bp::exec_file(pythonFilename.c_str(), global, global);
		} catch(std::invalid_argument) {
			std::cout << "PYTHON SALIENCY ERROR: PYTHON FILE \"" << pythonFilename << "\" NOT FOUND! PYTHON INTERPRETER ERROR REPORT:" << std::endl;
			PyErr_Print(); return;
		} catch(bp::error_already_set) {
			std::cout << "PYTHON SALIENCY ERROR: ERROR WHILE LOADING PYTHON FILE \"" << pythonFilename << "\"! PYTHON INTERPRETER ERROR REPORT:" << std::endl;
			PyErr_Print(); return;
		}*/
		
		
		//get handle to the function
		bp::object pythoncvfunctionhandle;
		try {
			pythoncvfunctionhandle = global[pythonFunctionName];
		}
		catch(bp::error_already_set) {
			std::cout << "PYTHON SALIENCY ERROR: function \"" << pythonFunctionName << "\" not found in python file \"" << pythonFilename << "\"! PYTHON ERROR REPORT:" << std::endl;
			PyErr_Print(); return;
		}
		//std::cout<<"done getting handle to function!"<<std::endl;
		
		
		//convert C++ cv::Mat to Python
		PyObject* givenImgCpp = cvt.toNDArray(fullsizeImage);
		bp::object givenImgPyObj( bp::handle<>(bp::borrowed(givenImgCpp)) );
		
		//call the Python function, return a list of crops
		bp::object resultobj;
		try {
			resultobj = pythoncvfunctionhandle(givenImgPyObj);
		}
		catch(bp::error_already_set) {
			std::cout << "PYTHON SALIENCY ERROR: error when running python file \"" << pythonFilename << "\"! PYTHON ERROR REPORT:" << std::endl;
			PyErr_Print(); return;
		}
		
		
		// extract each crop from Python to C++
		bp::list resultBPList(resultobj);
		int numElementsInReturnedList = ((int)bp::len(resultBPList));
		for(int ii=0; ii<numElementsInReturnedList; ii++)
		{
			bp::object thisImgObj = boost::python::extract<boost::python::object>(resultBPList[ii])();
			returnedCrops.push_back(cvt.toMat(thisImgObj.ptr()));
			
			returned_geolocations.push_back(std::pair<double,double>(0.0,0.0)); //TODO
		}
	}
}
