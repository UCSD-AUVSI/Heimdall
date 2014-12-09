#include <iostream>
using std::cout;
using std::endl;
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "SharedUtils/SharedUtils.hpp"
#include "Saliency/PythonSaliency/CPlusPlus_Heimdall_Interface/PythonSaliency.hpp"
#include "SharedUtils/GlobalVars.hpp"



static std::string GetPathToTestExecutable(const char* argv0) {
	std::string fullpath = GetPathOfExecutable(argv0);
	std::string trimThis("/bin/test/testPythonCanny3Dsaliency");
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
	
	
	cout << "Test Saliency" << endl;
	if(argc < 2) {
		consoleOutput.Level0() << "usage:  [PATH TO IMAGE]" << std::endl;
		return 1;
	}
	
	cv::Mat fullsizeImage = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
	
	std::vector<cv::Mat> foundCrops;
	std::vector<std::pair<double,double>> cropGeolocations;
	pythonSaliency("Canny3D", "main.py", "doSaliency", fullsizeImage, foundCrops, cropGeolocations);
	
	if(foundCrops.empty() == false) {
		for(int ii=0; ii<foundCrops.size(); ii++) {
			cv::imshow(std::string("crop")+to_istring(ii), foundCrops[ii]);
			std::cout<<"crop"<<to_istring(ii)<<" found at geo-location: "<<cropGeolocations[ii].first<<","<<cropGeolocations[ii].second<<std::endl;
		}
		cv::waitKey(0);
	}
	else {
		std::cout << "no crops found in this image!" << std::endl;
	}
	
	return 0;
}

