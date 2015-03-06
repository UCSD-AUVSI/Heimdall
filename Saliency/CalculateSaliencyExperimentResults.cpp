#include "TruthViewer/TruthFile.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include <iostream>
#include "OptimizeableSaliency.hpp"
#include <opencv/highgui.h>
using std::cout; using std::endl;


/*static*/
int OptimizeableSaliency_ResultsStats::DesiredTarget_MinPaddingPixels = 5;
int OptimizeableSaliency_ResultsStats::DesiredFalseP_MinPaddingPixels = 5;
double OptimizeableSaliency_ResultsStats::DesiredTarget_MaxCropLengthRatioToTargetLength = 3.5; //for Failures_TooBig
double OptimizeableSaliency_ResultsStats::DesiredFalseP_MaxCropLengthRatioToTargetLength = 3.5; //for Failures_TooBig



Optimizer_ResultsStats * OptimizeableSaliency_Output::CalculateResults()
{
	if(OptimizeableSaliency_TruthFilename == nullptr) {
		cout << "ERROR: OptimizeableSaliency_Output::CalculateResults() -- TruthFilename was NULL!!!" << endl;
		return nullptr;
	}
	
	OptimizeableSaliency_ResultsStats * results = new OptimizeableSaliency_ResultsStats();
	
	results->NumReturnedCrops = NumCrops();
	
	TruthFile loadedTruthFile;
	if(LoadTruthFile(*OptimizeableSaliency_TruthFilename, loadedTruthFile) == false) {
		consoleOutput.Level0() << "ERROR: COULD NOT LOAD TRUTH FILE \"" << (*OptimizeableSaliency_TruthFilename) << "\"" << endl;
		return results;
	}
	const int default_console_verbosity = 4;
	const int dcv = default_console_verbosity;
	bool resultsAreCompressed = (resultCropsCompressed.empty() == false);
	if(resultsAreCompressed) {
		CompressCropsSoFar();
	}
	
	// Loop over all test results' source fullsize images
	for(int ii=0; ii<numFullsizeImages(); ii++) {
		results->Total_Detections += LocationsForIdx(ii).size();
		
		
		// Loop over all possible source images in the truth file to find the corresponding truth entry
		TruthFile_ImageEntry *truthEntry = nullptr;
		for(int jj=0; jj<loadedTruthFile.images.size(); jj++) {
			if(!__stricmp(loadedTruthFile.images[jj].image_file.c_str(), ImgNameForIdx(ii).c_str())) {
				if(truthEntry != nullptr) {
					cout << "WARNING: DUPLICATE IMAGE IN TRUTH FILE?? ~~~~~~~~~~~~~~~~~~~~" << endl;
				} else {
					results->Truth_DesiredTargets += loadedTruthFile.images[jj].targets_in_image.size();
					results->Truth_DesiredFalsePs += loadedTruthFile.images[jj].falsepositives_in_image.size();
				}
				truthEntry = &(loadedTruthFile.images[jj]);
			}
		}
		if(truthEntry == nullptr) {
			cout << "WARNING: TRUTH FILE DID NOT CONTAIN IMAGE \"" << ImgNameForIdx(ii) << "\"" << endl;
			continue;
		}
		
		
		std::vector<bool> resultCropHadSomething;
		resultCropHadSomething.resize(LocationsForIdx(ii).size());
		// Loop over all result crops for this source image
		for(int jj=0; jj<LocationsForIdx(ii).size(); jj++) {
			resultCropHadSomething[jj] = false;
		}
		
		
		const int loopMaxVal = (truthEntry->targets_in_image.size() + truthEntry->falsepositives_in_image.size() - 1);
		const int loopNumTargets = truthEntry->targets_in_image.size();
		bool thisIsATarget;
		bool oneOfTheCropsFoundThisTruthObject;
		// Loop over all truth stuff for this source image (try each to see if it was captured!)
		for(int kk=0; kk<=loopMaxVal; kk++) {
			oneOfTheCropsFoundThisTruthObject = false;
			
			// Get truth target rectangle
			int tbX,tbY,tbW,tbH;
			thisIsATarget = (kk < loopNumTargets);
			if(thisIsATarget) {
				TruthFile_TargetInImage & thisTarg(truthEntry->targets_in_image[kk]);
				tbX = atoi(GetTruthEntryValue("pos_x",thisTarg).c_str());
				tbY = atoi(GetTruthEntryValue("pos_y",thisTarg).c_str());
				tbW = atoi(GetTruthEntryValue("box_min_width",thisTarg).c_str());
				tbH = atoi(GetTruthEntryValue("box_min_height",thisTarg).c_str());
			} else {
				TruthFile_FalsePositiveInImage & thisTarg(truthEntry->falsepositives_in_image[kk-loopNumTargets]);
				tbX = atoi(GetTruthEntryValue("pos_x",thisTarg).c_str());
				tbY = atoi(GetTruthEntryValue("pos_y",thisTarg).c_str());
				tbW = atoi(GetTruthEntryValue("box_min_width",thisTarg).c_str());
				tbH = atoi(GetTruthEntryValue("box_min_height",thisTarg).c_str());
			}
			
			cv::Rect thisTargRect(tbX,tbY,tbW,tbH);
			consoleOutput.Level(dcv) << "this target official rect: " << thisTargRect << endl;
			
			
			// Loop over all result crops for this source image
			for(int jj=0; jj<LocationsForIdx(ii).size(); jj++) {
		
				// ASSUME CROP LOCATION DATA IS IN PIXEL COORDINATES, NOT LATITUDE/LONGITUDE
				cv::Rect cropRect;
				cropRect.x = LocationsForIdx(ii)[jj].first;
				cropRect.y = LocationsForIdx(ii)[jj].second;
				
				if(resultsAreCompressed) {
					cv::Mat decodedim = cv::imdecode(*CompressedCropsForIdx(ii)[jj],CV_LOAD_IMAGE_UNCHANGED);
					cropRect.width  = decodedim.cols;
					cropRect.height = decodedim.rows;
				} else {
					cropRect.width  = CropsForIdx(ii)[jj].cols;
					cropRect.height = CropsForIdx(ii)[jj].rows;
				}
				
				// Intersect to find if the crop contained the true target
				cv::Rect rectIntersect = (cropRect & thisTargRect);
				
				double pixelsInCrop = ((double)(cropRect.width*cropRect.height));
				double rectIntscSize = static_cast<double>(rectIntersect.width * rectIntersect.height);
				double closeCallRatio = (rectIntscSize / pixelsInCrop);
				
				consoleOutput.Level(dcv) << "rectIntersect == " << rectIntersect << endl;
				
				
//=================================================================================================================
				
				
				// Does the crop contain a WHOLE target?
				if(rectIntersect.size() != thisTargRect.size()) {
					// Did it contain at least part of a target? Call it a "close call"
					if(closeCallRatio > 0.25 && sqrt(pixelsInCrop/((double)(tbW*tbH))) < 1.8) {
						consoleOutput.Level0() << "SALIENCY FAILURE: CLOSE CALL! (PARTIAL CROP) ------------ " << ImgNameForIdx(ii) << endl;
						if(thisIsATarget) {
							results->DesiredTarget_CloseFailures_PartialCrop++;
						} else {
							results->DesiredFalseP_CloseFailures_PartialCrop++;
						}
						resultCropHadSomething[jj] = true;
						oneOfTheCropsFoundThisTruthObject = true;
					}/* else {
						results->Total_NothingUsefulFailures++;
					}*/
				}
				else {
					resultCropHadSomething[jj] = true;
					oneOfTheCropsFoundThisTruthObject = true;
					
					// The crop contained a whole target! Get padding information
					int padX1 = (thisTargRect.x - cropRect.x);
					int padX2 = ((cropRect.x+cropRect.width) - (thisTargRect.x+thisTargRect.width));
					int padY1 = (thisTargRect.y - cropRect.y);
					int padY2 = ((cropRect.y+cropRect.height) - (thisTargRect.y+thisTargRect.height));
					
					consoleOutput.Level(dcv) << "padding: " << padX1 << ", " << padX2 << ", " << padY1 << ", " << padY2 << endl;
					
					const int minPadding = MIN(MIN(padX1,padX2), MIN(padY1,padY2));
					const int desiredPadding = (thisIsATarget ? results->DesiredTarget_MinPaddingPixels : results->DesiredFalseP_MinPaddingPixels);
					
					// Is there enough padding?
					if(minPadding < desiredPadding) {
						if(thisIsATarget) {
							results->DesiredTarget_CloseFailures_MinorPadding++;
						} else {
							results->DesiredFalseP_CloseFailures_MinorPadding++;
						}
						consoleOutput.Level0() << "SALIENCY FAILURE: NOT ENOUGH PADDING ------------ " << ImgNameForIdx(ii) << endl;
					}
					else {
						// Check if there is too much padding
						const double sizeRatio_Width = static_cast<double>(cropRect.width) / static_cast<double>(thisTargRect.width);
						const double sizeRatio_Height = static_cast<double>(cropRect.width) / static_cast<double>(thisTargRect.width);
						
						const double maxSizeRatio = MAX(sizeRatio_Width, sizeRatio_Height);
						const double desiredMaxSizeRatio = (thisIsATarget ? results->DesiredTarget_MaxCropLengthRatioToTargetLength : results->DesiredFalseP_MaxCropLengthRatioToTargetLength);
						
						if(maxSizeRatio > desiredMaxSizeRatio) {
							if(thisIsATarget) {
								results->DesiredTarget_CloseFailures_FarTooBig++;
							} else {
								results->DesiredFalseP_CloseFailures_FarTooBig++;
							}
							consoleOutput.Level0() << "SALIENCY FAILURE: CROP IS TOO BIG ------------ " << ImgNameForIdx(ii) << endl;
						}
						else {
							// Not too little padding, not too much padding, this is perfect!
							if(thisIsATarget) {
								results->DesiredTarget_Successes++;
							} else {
								results->DesiredFalseP_Successes++;
							}
						}
					}
				}
			}
			if(oneOfTheCropsFoundThisTruthObject == false) {
				if(thisIsATarget) {
					results->DesiredTargets_Missed_IncludingCloseFailures.push_back(ImgNameForIdx(ii));
				} else {
					results->DesiredFalsePs_Missed_IncludingCloseFailures.push_back(ImgNameForIdx(ii));
				}
			}
		}
		
		// Loop over all result crops for this source image
		for(int jj=0; jj<LocationsForIdx(ii).size(); jj++) {
			if(resultCropHadSomething[jj] == false) {
				results->Total_NothingUsefulFailures++;
			}
		}
	}
	return results;
}

