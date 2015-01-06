#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "SharedUtils/OS_FolderBrowser_tinydir.h"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"
#include "SaliencyResultsTruthTester.hpp"
#include <thread>
using std::cout;
using std::endl;

static void SingleThreadMatchTemplateToChannel(cv::Mat origImgChannel, cv::Mat templateImgChannel, float pixelsInCrop,
										float * returned_minVal, int * returned_pt_X, int * returned_pt_Y)
{
	assert(returned_minVal != nullptr);
	assert(returned_pt_X != nullptr);
	assert(returned_pt_Y != nullptr);
	
	cv::Mat mResult;
	double minVal;
	cv::Point minPt;
	
	cv::matchTemplate(origImgChannel, templateImgChannel, mResult, CV_TM_SQDIFF);
	cv::minMaxLoc(mResult, &minVal, nullptr, &minPt, nullptr);
	
	(*returned_minVal) = (sqrt(minVal) / pixelsInCrop);
	
	(*returned_pt_X) = minPt.x;
	(*returned_pt_Y) = minPt.y;
}


static bool GetCropCoordinatesWithoutFilename(cv::Mat cropImage, cv::Mat cropSourceImage, float pixelsInCrop, cv::Rect & cropRect, float & totalMinVal)
{
	std::vector<cv::Mat> crImageChannels(3);
	std::vector<cv::Mat> crSourceImageChannels(3);
	std::vector<cv::Rect> threeRects;
	threeRects.resize(3);
	cv::split(cropImage, crImageChannels);
	cv::split(cropSourceImage, crSourceImageChannels);
	
	//multithreaded template matching (three threads, one for each channel)
	static std::vector<std::thread*> threads_for_channels(3, nullptr);
	static std::vector<float> minVals(3, 0.0f);
	static std::vector<int> min_pts_x(3, 0);
	static std::vector<int> min_pts_y(3, 0);
	
	totalMinVal = 0.0f;
	
	for(int kk=0; kk<3; kk++) {
		threads_for_channels[kk] = new std::thread(SingleThreadMatchTemplateToChannel,
											crSourceImageChannels[kk],
											crImageChannels[kk],
											pixelsInCrop,
											&(minVals[kk]),
											&(min_pts_x[kk]),
											&(min_pts_y[kk]));
	}
	for(int kk=0; kk<3; kk++) {
		threads_for_channels[kk]->join();
		delete threads_for_channels[kk];
		threads_for_channels[kk] = nullptr;
		
		totalMinVal += minVals[kk];
		threeRects[kk] = cv::Rect(min_pts_x[kk], min_pts_y[kk], cropImage.cols, cropImage.rows);
	}
	
	if(totalMinVal > 0.0035f) {
		consoleOutput.Level0() << "ERROR: MINVAL > 0.35 percent !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
		consoleOutput.Level0() << "match amt: " << totalMinVal << std::endl;
		return false;
	}
	else if(threeRects[0] != threeRects[1] || threeRects[0] != threeRects[2] || threeRects[1] != threeRects[2]) {
		consoleOutput.Level0() << "ERROR: crop rectangles do not agree !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
		return false;
	}
	cropRect = threeRects[0];
	return true;
}


static bool GetCropCoordsFromFilename(std::string fname, cv::Mat cropImage, cv::Rect & cropRect)
{
	int cpos,ii;
	if(contains_substr_i(fname, std::string(",,,"), &cpos) == false)
		return false;
	if(cpos <= 1 || cpos >= (fname.size()-3))
		return false;
	
	int firstUnderscore = -1;
	for(ii=(cpos-1); ii>=0; ii--) {
		if(fname[ii] == '_') {
			firstUnderscore = ii;
			break;
		}
	}
	
	int secondUnderscore = -1;
	for(ii=(cpos+3); ii<fname.size(); ii++) {
		if(fname[ii] == '_') {
			secondUnderscore = ii;
			break;
		}
	}
	
	if(firstUnderscore < 0 || secondUnderscore < 0 || (cpos-firstUnderscore <= 1) || (secondUnderscore-cpos) <= 2)
		return false;
	
	int posx = atoi(fname.substr(firstUnderscore+1, cpos-firstUnderscore-1).c_str());
	int posy = atoi(fname.substr(cpos+3, secondUnderscore-cpos-3).c_str());
	
	if(posx < 0 || posy < 0 || posx > 7000 || posy > 7000)
		return false;
	
	cropRect = cv::Rect(posx, posy, cropImage.cols, cropImage.rows);
	
	return true;
}


SaliencyOutput
	ReadFolderWithCrops(std::string truthImageFileFolder,
						std::string folderWithCrops,
						bool cropCoordinatesAreInFilename,
						bool compress/*=true*/)
{
	const int default_console_verbosity = 4;
	const int dcv = default_console_verbosity;
	
	if(truthImageFileFolder[truthImageFileFolder.size()-1] != '/') {
		truthImageFileFolder = (truthImageFileFolder + std::string("/"));
	}
	
	SaliencyOutput salOut;
	tinydir_dir dir;
	std::string filename,filenameExtension;
	std::string cropSourceImageFile;
	int numFileErrors = 0;
	int numFilesScanned = 0;
	
	tinydir_open(&dir, folderWithCrops.c_str());
	
	/*
		Loop over the crops in the folder
		For each crop, get its corresponding original image, and figure out where in the image it came from
	*/
	while(dir.has_next) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if(file.is_dir == false && filename_extension_is_image_type(get_extension_from_filename(file.name))) {
			cropSourceImageFile = std::string(file.name);
			trim_chars_after_first_instance_of_delim(cropSourceImageFile,'.',false);
			cropSourceImageFile = (cropSourceImageFile + std::string(".jpg"));
			
			consoleOutput.Level(dcv) << "this crop: \"" << filename << "\", original image: \"" << cropSourceImageFile << "\"" << std::endl;
			
			cv::Mat cropImage = cv::imread(file.path);
			
			if(check_if_file_exists(truthImageFileFolder+cropSourceImageFile) && cropImage.empty() == false)
			{
				numFilesScanned++;
				float totalMinVal;
				bool cropFileIsGood = false;
				cv::Rect cropRect;
				
				if(cropCoordinatesAreInFilename == false) {
					cv::Mat cropSourceImage = cv::imread(truthImageFileFolder+cropSourceImageFile);
					if(GetCropCoordinatesWithoutFilename(cropImage, cropSourceImage, (float)(cropImage.rows*cropImage.cols), cropRect, totalMinVal)) {
						cropFileIsGood = true;
					}
					else {
						numFileErrors++;
					}
				}
				else {
					if(GetCropCoordsFromFilename(std::string(file.name), cropImage, cropRect)) {
						cropFileIsGood = true;
						totalMinVal = 0.0f;
					}
					else {
						numFileErrors++;
					}
				}
				
				if(cropFileIsGood)
				{
					salOut.CropsForImage(cropSourceImageFile).push_back(cropImage);
					salOut.LocationsForImage(cropSourceImageFile).push_back(std::pair<double,double>(cropRect.x,cropRect.y));
					
					consoleOutput.Level(dcv) << "match amt: " << totalMinVal << ", location: " << cropRect << endl;
					
					if(compress) {
						salOut.CompressCropsSoFar();
					}
				}
				consoleOutput.Level(dcv) << "scanned " << numFilesScanned << " images so far" << endl;
			}
			else {
				consoleOutput.Level(0) << "ERROR READING CROP IMAGE, OR THE ORIGINAL IMAGE WAS NOT FOUND ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
				numFileErrors++;
			}
		}
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
	
	if(numFileErrors > 0) {
		consoleOutput.Level(0) << "WARNING: THERE WERE " << numFileErrors
					<< " ERRORS WHEN READING THE SALIENCY CROPS IN THAT FOLDER" << endl;
	}
	
	return salOut;
}

/*
Filename Specifications for crops with coordinates in file name:
=================================================================
example
=================================

filename string:	x_12,,,34_x.png
index of chars:		012345678901234
length: 15 chars
coordinates of upper left corner of crop: (12,34)


commasPosition = cpos = 4

firstUnderscore = fUnd = 1
	
		looped from 3 to 0

seconUnderscore = sUnd = 9
		
		looped from 7 to 14

firstNumber atoi(substr(fUnd+1, cpos-fUnd-1))

seconNumber atoi(substr(cpos+3, sUnd-cpos-3))

*/


void SaliencyExperimentResultsCalculator(std::vector<imgdata_t*> imgResults,
										ExperimentResultsData* calculatedResults)
{
	cout<<"TODO: CALCULATING SALIENCY RESULTS"<<endl;
	cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<endl;
	cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<endl;
	//todo: generate "SaliencyOutput" from all of the imgResults
}




