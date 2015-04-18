#include "PythonVerif.hpp"
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



void PythonVerifClass::ProcessVerification(	unsigned char scolorR,
											unsigned char scolorG,
											unsigned char scolorB,
											std::string shape_color,
											std::string shape_name,
											unsigned char ccolorR,
											unsigned char ccolorG,
											unsigned char ccolorB,
											std::string character_color,
											std::string character_name,
											double target_latitude,
											double target_longitude,
											double target_orientation,
											std::string original_image_filename)
{
	if(path_to_HeimdallBuild_directory == nullptr) {
		std::cout << "ERROR: PythonVerif: \"path_to_HeimdallBuild_directory\" not set!" << std::endl;
		return;
	}
	
	if(PrepareForPythonStuff()) {
		aquire_py_GIL lock;
		
		// setup directories and namespaces for Python interpeter
		bp::object main = bp::import("__main__");
		bp::object global(main.attr("__dict__"));
		std::string dirToAdd = (*path_to_HeimdallBuild_directory)+std::string("/Verification/PythonVerif/")+verifModuleFolderName+std::string("/Python");
		if(check_if_directory_exists(dirToAdd) == false) {
			std::cout << "ERROR: PythonVerif: couldn't find \"" << dirToAdd << "\"" << std::endl;
			return;
		}
		
		
		// load main python file into C++
		std::string loadedPyFile;
		if(read_file_contents(dirToAdd+std::string("/")+pythonFilename,loadedPyFile)==false) {
			std::cout<<"ERROR: PythonVerif: COULD NOT FIND PYTHON FILE \""<<pythonFilename<<"\""<<std::endl;
			return;
		}
		AddPathToPythonSys(dirToAdd);
		
		
		// load main python file into the interpreter
		try {
			PyRun_SimpleString(loadedPyFile.c_str());
		}
		catch(bp::error_already_set) {
			std::cout<<"PythonVerif: ERROR RUNNING PYTHON FILE LOADED INTO C++: PYTHON ERROR REPORT:"<<std::endl;
			PyErr_Print(); return;
		}
		
		
		//get handle to the function
		bp::object pythoncvfunctionhandle;
		try {
			pythoncvfunctionhandle = global[pythonFunctionName];
		}
		catch(bp::error_already_set) {
			std::cout << "PythonVerif ERROR: function \"" << pythonFunctionName << "\" not found in python file \"" << pythonFilename << "\"! PYTHON ERROR REPORT:" << std::endl;
			PyErr_Print(); return;
		}
		//std::cout<<"done getting handle to function!"<<std::endl;
		
		
		//prepare arguments
		std::vector<double> ShapeColorValsVec(3, 0.0);
		ShapeColorValsVec[0] = scolorR;
		ShapeColorValsVec[1] = scolorG;
		ShapeColorValsVec[2] = scolorB;
		std::vector<double> CharColorValsVec(3, 0.0);
		CharColorValsVec[0] = ccolorR;
		CharColorValsVec[1] = ccolorG;
		CharColorValsVec[2] = ccolorB;
		bp::list bpShapeColorVals = std_vector_to_py_list<double>(ShapeColorValsVec);
		bp::list bpCharColorVals  = std_vector_to_py_list<double>(CharColorValsVec);
		
		bp::str bpShapeColorStr(shape_color.c_str());
		bp::str bpShapeName(shape_name.c_str());
		
		bp::str bpCharColorStr(character_color.c_str());
		bp::str bpCharName(character_name.c_str());
		
		bp::object bpTargetLat(target_latitude);
		bp::object bpTargetLong(target_longitude);
		bp::object bpTargetOrientation(target_orientation);
		
		bp::str bpOrigImageFilename(original_image_filename.c_str());
		
		
		//call the Python function, return data
		bp::object resultobj;
		if(additional_args.empty() == false) {
			bp::list extraArgs = std_vector_to_py_list<double>(additional_args);
			try {
				pythoncvfunctionhandle(	bpShapeColorVals, bpShapeColorStr, bpShapeName,
										bpCharColorVals,  bpCharColorStr,  bpCharName,
										bpTargetLat, bpTargetLong, bpTargetOrientation,
										bpOrigImageFilename,
										extraArgs);
			}
			catch(bp::error_already_set) {
				std::cout << "PythonVerif ERROR: error when running python file \"" << pythonFilename << "\" WITH EXTRA ARGS! PYTHON ERROR REPORT:" << std::endl;
				PyErr_Print(); return;
			}
		}
		else {
			try {
				pythoncvfunctionhandle(	bpShapeColorVals, bpShapeColorStr, bpShapeName,
										bpCharColorVals,  bpCharColorStr,  bpCharName,
										bpTargetLat, bpTargetLong, bpTargetOrientation,
										bpOrigImageFilename,
										bp::object());
			}
			catch(bp::error_already_set) {
				std::cout << "PythonVerif ERROR: error when running python file \"" << pythonFilename << "\"! PYTHON ERROR REPORT:" << std::endl;
				PyErr_Print(); return;
			}
		}
	}
}




