#include <iostream>
using std::cout;
using std::endl;
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "SharedUtils/SharedUtils.hpp"
#include "Recognition/ColorClassifier/PythonColorClassifier/CPlusPlus_Heimdall_Interface/PythonColorClassifier.hpp"
#include "SharedUtils/GlobalVars.hpp"



static std::string GetPathToTestExecutable(const char* argv0) {
	std::string fullpath = GetPathOfExecutable(argv0);
	std::string trimThis("/bin/test/testPythonExampleColorClassifier");
	if(fullpath.substr(fullpath.size()-trimThis.size()) != trimThis) {
		std::cout<<"WARNING: TEST EXECUTABLE NOT FOUND ???? error finding path to executable"<<std::endl;
	}
	fullpath.erase(fullpath.size()-trimThis.size());
	return fullpath;
}


int main(int argc, char** argv)
{
	if(argc < 1) { std::cout<<"?? no arguments for executable??"<<std::endl; return 1; }
	path_to_HeimdallBuild_directory = new std::string(GetPathToTestExecutable(argv[0]));
	
	cout << "Test Python Example Color Classifier" << endl;
	
	
	std::string returned_scolor;
	std::string returned_ccolor;
	
	PythonColorClassifierClass ccdoer;
	ccdoer.colorclassifierModuleFolderName = "ExampleStub";
	ccdoer.pythonFilename = "main.py";
	ccdoer.pythonFunctionName = "doColorClassification";
	ccdoer.ProcessColorClassification(	255,0,0, //red
										returned_scolor,
										0,255,0, //green
										returned_ccolor);
	
	cout << "Shape color (supposed to be red) -- \"" << returned_scolor << "\"" << endl;
	cout << "Char. color (supposed to be green) -- \"" << returned_ccolor << "\"" << endl;
	
	return 0;
}

