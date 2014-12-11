#include <iostream>
using std::cout;
using std::endl;
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/OS_FolderBrowser_tinydir.h"
#include "Saliency/PythonSaliency/CPlusPlus_Heimdall_Interface/PythonSaliency.hpp"
#include "SharedUtils/GlobalVars.hpp"



static std::string GetPathToTestExecutable(const char* argv0) {
	std::string fullpath = GetPathOfExecutable(argv0);
	std::string trimThis("/bin/test/testOptimizeCanny3Dsaliency");
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
	
	cout << "Optimize-Canny3D-Saliency" << endl;
	if(argc < 2) {
		consoleOutput.Level0() << "usage:  [PATH TO FOLDER WITH IMAGES]" << endl;
		return 1;
	}
	if(!check_if_directory_exists(argv[1])) {
		consoleOutput.Level0() << "ERROR: path \""<<argv[1]<<"\" not found!!" << endl;
		return 1;
	}
	
	
	std::vector<std::string> tImgFnames;
	std::vector<cv::Mat> tImgs;
	
	tinydir_dir dir;
	tinydir_open(&dir, argv[1]);
	while(dir.has_next) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if(file.is_dir == false && file.name[0] != '.' && filename_extension_is_image_type(get_extension_from_filename(file.name))) {
			tImgFnames.push_back(file.name);
			tImgs.push_back(cv::imread(file.path,CV_LOAD_IMAGE_COLOR));
		}
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
	assert(tImgs.size() == tImgFnames.size());
	
	cout << "found " << tImgs.size() << " images to test with!" << endl;
	
	/*
		Maps from original-image-filename to the results PythonSaliency got from them
	*/
	std::map<std::string, std::vector<cv::Mat>*> resultCrops;
	std::map<std::string, std::vector<std::pair<double,double>>*> resultLocations;
	for(int jj=0; jj<tImgFnames.size(); jj++) {
		resultCrops[    tImgFnames[jj]] = new std::vector<cv::Mat>();
		resultLocations[tImgFnames[jj]] = new std::vector<std::pair<double,double>>();
	}
	
	
	for(int jj=0; jj<tImgs.size(); jj++)
	{
		pythonSaliency("Canny3D", "main.py", "doSaliency", tImgs[jj], *resultCrops[tImgFnames[jj]], *resultLocations[tImgFnames[jj]]);
		
		if(resultCrops[tImgFnames[jj]]->empty() == false) {
			for(int ii=0; ii<resultCrops[tImgFnames[jj]]->size(); ii++) {
				//cv::imshow(tImgs[jj].first+std::string(" crop")+to_istring(ii), foundCrops[jj][ii]);
				std::cout<<tImgFnames[jj]<<" crop"<<to_istring(ii)<<" found at geo-location: "<<
					(*resultLocations[tImgFnames[jj]])[ii].first
					<<","<<
					(*resultLocations[tImgFnames[jj]])[ii].second
					<<std::endl;
			}
			//cv::waitKey(0);
		}
		else {
			std::cout << "no crops found in this image!" << std::endl;
		}
	}
	
	return 0;
}

