#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "SharedUtils/OS_FolderBrowser_tinydir.h"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"
#include "TruthViewer/TruthFile.hpp"
#include "SaliencyResultsTruthTester.hpp"
#include <thread>

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


static bool GetCropCoordinatesWithoutFilename(cv::Mat cropImage, cv::Mat cropOrigImage, float pixelsInCrop, cv::Rect & cropRect, float & totalMinVal)
{
	std::vector<cv::Mat> crImageChannels(3);
	std::vector<cv::Mat> crOrigImageChannels(3);
	std::vector<cv::Rect> threeRects;
	threeRects.resize(3);
	cv::split(cropImage, crImageChannels);
	cv::split(cropOrigImage, crOrigImageChannels);
	
	//multithreaded template matching (three threads, one for each channel)
	static std::vector<std::thread*> threads_for_channels(3, nullptr);
	static std::vector<float> minVals(3, 0.0f);
	static std::vector<int> min_pts_x(3, 0);
	static std::vector<int> min_pts_y(3, 0);
	
	totalMinVal = 0.0f;
	
	for(int kk=0; kk<3; kk++) {
		threads_for_channels[kk] = new std::thread(SingleThreadMatchTemplateToChannel,
											crOrigImageChannels[kk],
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


ResultsData TestFolderWithCrops(std::string truthFilename, std::string truthImageFileFolder, std::string folderWithCrops,
						int desiredMinPaddingPixels, double desiredMaxCropLengthRatioToTargetLength,
						bool cropCoordinatesAreInFilename)
{
	const int default_console_verbosity = 4;
	
	
	TruthFile loadedTruthFile;
	if(LoadTruthFile(truthFilename, loadedTruthFile) == false) {
		consoleOutput.Level0() << "ERROR: COULD NOT LOAD TRUTH FILE \"" << truthFilename << "\"" << std::endl;
		return ResultsData();
	}
	
	const int dcv = default_console_verbosity;
	ResultsData results;
	
	for(int ii=0; ii<loadedTruthFile.images.size(); ii++) {
		results.numTruthTargets += loadedTruthFile.images[ii].targets_in_image.size();
	}
	
	tinydir_dir dir;
	std::string filename,filenameExtension;
	std::string cropOriginalImageFile;
	
	tinydir_open(&dir, folderWithCrops.c_str());
	
	/*
		Loop over the crops in the folder
		For each crop, get its corresponding original image, and figure out where in the image it came from
		Then see if where it came from matches a target!
	*/
	while(dir.has_next) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if(file.is_dir == false) {
			filename = std::string(file.name);
			if(filename.size() > 4) {
				filenameExtension = eliminate_extension_from_filename(filename);
				if(filename_extension_is_image_type(filenameExtension)) {
					cropOriginalImageFile = std::string(file.name);
					trim_chars_after_first_instance_of_delim(cropOriginalImageFile,'.',false);
					
					cropOriginalImageFile = (cropOriginalImageFile + std::string(".jpg"));
					consoleOutput.Level(dcv) << "this crop: \"" << filename << "\", original image: \"" << cropOriginalImageFile << "\"" << std::endl;
					
					cv::Mat cropImage = cv::imread(file.path);
					cv::Mat cropOrigImage = cv::imread(truthImageFileFolder + cropOriginalImageFile);
					float pixelsInCrop = static_cast<float>(cropImage.rows*cropImage.cols);
					
					if(cropOrigImage.empty() == false && cropImage.empty() == false)
					{
						results.numDetectedThings++;
						
						float totalMinVal;
						bool cropFileIsGood = false;
						cv::Rect cropRect;
						
						if(cropCoordinatesAreInFilename == false) {
							if(GetCropCoordinatesWithoutFilename(cropImage, cropOrigImage, pixelsInCrop, cropRect, totalMinVal)) {
								cropFileIsGood = true;
							}
							else {
								results.numFileErrors++;
							}
						}
						else {
							if(GetCropCoordsFromFilename(std::string(file.name), cropImage, cropRect)) {
								cropFileIsGood = true;
								totalMinVal = 0.0f;
							}
							else {
								results.numFileErrors++;
							}
						}
						
						if(cropFileIsGood)
						{
							consoleOutput.Level(dcv) << "match amt: " << totalMinVal << ", location: " << cropRect << std::endl;
							bool foundTheImage = false;
							for(int ii=0; ii<loadedTruthFile.images.size(); ii++) {
								if(!__stricmp(loadedTruthFile.images[ii].image_file.c_str(), cropOriginalImageFile.c_str())) {
									if(foundTheImage == true) {
										consoleOutput.Level0() << "ERROR: DUPLICATE IMAGE IN TRUTH FILE?? ~~~~~~~~~~~~~~~~~~~~" << std::endl;
										results.numFileErrors++;
									}
									foundTheImage = true;
									for(int jj=0; jj<loadedTruthFile.images[ii].targets_in_image.size(); jj++) {
										TruthFile_TargetInImage & thisTarg(loadedTruthFile.images[ii].targets_in_image[jj]);
										int tbX = atoi(GetTruthEntryValue("pos_x",thisTarg).c_str());
										int tbY = atoi(GetTruthEntryValue("pos_y",thisTarg).c_str());
										int tbW = atoi(GetTruthEntryValue("box_min_width",thisTarg).c_str());
										int tbH = atoi(GetTruthEntryValue("box_min_height",thisTarg).c_str());
										cv::Rect thisTargRect(tbX,tbY,tbW,tbH);
										consoleOutput.Level(dcv) << "this target official rect: " << thisTargRect << std::endl;
										
										cv::Rect rectIntersect = (cropRect & thisTargRect);
										
										double rectIntscSize = static_cast<double>(rectIntersect.width * rectIntersect.height);
										double closeCallRatio = (rectIntscSize / static_cast<double>(pixelsInCrop));
										
										consoleOutput.Level(dcv) << "rectIntersect == " << rectIntersect << std::endl;
										if(rectIntersect.size() != thisTargRect.size()) {
											consoleOutput.Level(dcv) << "SALIENCY FAILURE: DETECTION FAILURE ------------------------------------------------" << std::endl;
											results.numFailures_DetectionFailures++;
											
											if(closeCallRatio > 0.3) {
												results.numCloseCallsForCroppingFailures++;
											}
										}
										else {
											int padX1 = (thisTargRect.x - cropRect.x);
											int padX2 = ((cropRect.x+cropRect.width) - (thisTargRect.x+thisTargRect.width));
											int padY1 = (thisTargRect.y - cropRect.y);
											int padY2 = ((cropRect.y+cropRect.height) - (thisTargRect.y+thisTargRect.height));
											
											consoleOutput.Level(dcv) << "padding: " << padX1 << ", " << padX2 << ", " << padY1 << ", " << padY2 << std::endl;
											
											int minPadding = MIN(MIN(padX1,padX2), MIN(padY1,padY2));
											
											if(minPadding < desiredMinPaddingPixels) {
												results.numFailures_PaddingFailures++;
												consoleOutput.Level0() << "SALIENCY FAILURE: NOT ENOUGH PADDING ------------ " << filename << std::endl;
											}
											else {
												double sizeRatio_Width = static_cast<double>(cropRect.width) / static_cast<double>(thisTargRect.width);
												double sizeRatio_Height = static_cast<double>(cropRect.width) / static_cast<double>(thisTargRect.width);
												
												double maxSizeRatio = MAX(sizeRatio_Width, sizeRatio_Height);
												
												if(maxSizeRatio > desiredMaxCropLengthRatioToTargetLength) {
													results.numFailures_TooBig++;
													consoleOutput.Level0() << "SALIENCY FAILURE: CROP IS TOO BIG ------------ " << filename << std::endl;
												}
												else {
													results.numSuccesses++;
												}
	
											}
										}
									}
								}
							}
						}
						consoleOutput.Level(dcv) << "checked " << results.numDetectedThings << " images so far" << std::endl;
					}
					else {
						consoleOutput.Level0() << "ERROR READING CROP IMAGE, OR THE ORIGINAL IMAGE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
						results.numFileErrors++;
					}
				}
			}
		}
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
	
	return results;
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





