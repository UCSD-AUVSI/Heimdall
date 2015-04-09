#include "PythonQRCode.hpp"
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



void PythonQRCodeClass::ProcessQRCode(cv::Mat inputCropImage,
				bool & returned_read_success,
				std::string & returned_message_if_success)
{
	returned_read_success = false;
	returned_message_if_success = "";
	
	if(path_to_HeimdallBuild_directory == nullptr) {
		std::cout << "ERROR: pythonQRCode: \"path_to_HeimdallBuild_directory\" not set!" << std::endl;
		return;
	}
	
	if(PrepareForPythonStuff()) {
		aquire_py_GIL lock;
		
		// setup directories and namespaces for Python interpeter
		NDArrayConverter cvt;
		bp::object main = bp::import("__main__");
		bp::object global(main.attr("__dict__"));
		std::string dirToAdd = (*path_to_HeimdallBuild_directory)+std::string("/Recognition/QRCode/PythonQRCode/")+qrcodeModuleFolderName+std::string("/Python");
		if(check_if_directory_exists(dirToAdd) == false) {
			std::cout << "ERROR: pythonQRCode: couldn't find \"" << dirToAdd << "\"" << std::endl;
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
			std::cout<<"pythonQRCode: ERROR RUNNING PYTHON FILE LOADED INTO C++: PYTHON ERROR REPORT:"<<std::endl;
			PyErr_Print(); return;
		}
		
		
		//get handle to the function
		bp::object pythoncvfunctionhandle;
		try {
			pythoncvfunctionhandle = global[pythonFunctionName];
		}
		catch(bp::error_already_set) {
			std::cout << "pythonQRCode ERROR: function \"" << pythonFunctionName << "\" not found in python file \"" << pythonFilename << "\"! PYTHON ERROR REPORT:" << std::endl;
			PyErr_Print(); return;
		}
		//std::cout<<"done getting handle to function!"<<std::endl;
		
		
		//convert C++ cv::Mat to Python
		PyObject* givenImgCpp = cvt.toNDArray(inputCropImage);
		bp::object givenImgPyObj( bp::handle<>(bp::borrowed(givenImgCpp)) );
		
		
		//call the Python function, return data
		bp::object resultobj;
		if(additional_args.empty() == false) {
			bp::list extraArgs = std_vector_to_py_list<double>(additional_args);
			try {
				resultobj = pythoncvfunctionhandle(givenImgPyObj, extraArgs);
			}
			catch(bp::error_already_set) {
				std::cout << "pythonQRCode ERROR: error when running python file \"" << pythonFilename << "\" WITH EXTRA ARGS! PYTHON ERROR REPORT:" << std::endl;
				PyErr_Print(); return;
			}
		}
		else {
			try {
				resultobj = pythoncvfunctionhandle(givenImgPyObj, bp::object());
			}
			catch(bp::error_already_set) {
				std::cout << "pythonQRCode ERROR: error when running python file \"" << pythonFilename << "\"! PYTHON ERROR REPORT:" << std::endl;
				PyErr_Print(); return;
			}
		}
		Py_DECREF(givenImgCpp);
		
		
		// now extract results from Python to C++
		//
		returned_message_if_success.clear();
		
		bp::tuple resultsTuple(resultobj);
		if(((int)bp::len(resultsTuple)) == 1) {
			returned_read_success = bp::extract<bool>(resultsTuple[0]);
			if(returned_read_success) {
				cout << "PythonQRCode FATAL ERROR: a successfully read QR code message needs to return a 2-tuple:" << endl
						<< "      (successful read? (boolean), QR code string)" << endl;
			}
		} else if(((int)bp::len(resultsTuple)) == 2) {
			returned_read_success = bp::extract<bool>(resultsTuple[0]);
			returned_message_if_success = bp::extract<const char*>(bp::str(resultsTuple[1]));
			if(returned_read_success == false) {
				returned_message_if_success.clear();
			} else {
				cout << "C++ confirms successfully read QR code message: \"" << returned_message_if_success << "\"" << endl;
			}
		} else {
			cout << "PythonQRCode ERROR: expects return value to be a 2-tuple:" << endl
					<< "    (successful read? (boolean), QR code string (or empty string if wasnt read))" << endl;
		}
	}
	consoleOutput.Level3()<<"PYQRC released GIL lock"<<endl;
}




