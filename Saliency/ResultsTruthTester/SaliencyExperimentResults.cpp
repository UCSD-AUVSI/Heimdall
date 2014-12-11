#include "SaliencyExperimentResults.hpp"
#include "TruthViewer/TruthFile.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include <iostream>
using std::cout;
using std::endl;


/*static*/
int SaliencyExperimentResults::desiredMinPaddingPixels = 5;
double SaliencyExperimentResults::desiredMaxCropLengthRatioToTargetLength = 3.5; //for Failures_TooBig


SaliencyExperimentResults GetSaliencyExperimentResults(std::string truthFilename,
														/*const*/ SaliencyOutput & testOutput)
{
	SaliencyExperimentResults results;
	
	TruthFile loadedTruthFile;
	if(LoadTruthFile(truthFilename, loadedTruthFile) == false) {
		consoleOutput.Level0() << "ERROR: COULD NOT LOAD TRUTH FILE \"" << truthFilename << "\"" << std::endl;
		return results;
	}
	const int default_console_verbosity = 4;
	const int dcv = default_console_verbosity;
	bool resultsAreCompressed = (testOutput.resultCropsCompressed.empty() == false);
	if(resultsAreCompressed) {
		testOutput.CompressCropsSoFar();
	}
	
	// Loop over all test results' source fullsize images
	for(int ii=0; ii<testOutput.numFullsizeImages(); ii++) {
		results.numDetectedThings += testOutput.LocationsForIdx(ii).size();
		
		// Loop over all possible source images in the truth file to find the corresponding truth entry
		TruthFile_ImageEntry *truthEntry = nullptr;
		for(int jj=0; jj<loadedTruthFile.images.size(); jj++) {
			if(!__stricmp(loadedTruthFile.images[jj].image_file.c_str(), testOutput.ImgNameForIdx(ii).c_str())) {
				if(truthEntry != nullptr) {
					cout << "WARNING: DUPLICATE IMAGE IN TRUTH FILE?? ~~~~~~~~~~~~~~~~~~~~" << std::endl;
				} else {
					results.numTruthTargets += loadedTruthFile.images[jj].targets_in_image.size();
				}
				truthEntry = &(loadedTruthFile.images[jj]);
			}
		}
		if(truthEntry == nullptr) {
			cout << "WARNING: TRUTH FILE DID NOT CONTAIN IMAGE \"" << testOutput.ImgNameForIdx(ii) << "\"" << endl;
			continue;
		}
		
		// Loop over all result crops for this source image
		for(int jj=0; jj<testOutput.LocationsForIdx(ii).size(); jj++) {
			// Loop over all truth targets for this source image
			for(int kk=0; kk<truthEntry->targets_in_image.size(); kk++) {
				
				if(truthEntry->targets_in_image.size() > 1) {
					cout << "TODO: WHAT IF A TRUTH IMAGE CONTAINS MORE THAN ONE TARGET ???" << endl;
				}
				
				// ASSUME CROP LOCATION DATA IS IN PIXEL COORDINATES, NOT LATITUDE/LONGITUDE
				cv::Rect cropRect;
				cropRect.x = testOutput.LocationsForIdx(ii)[jj].first;
				cropRect.y = testOutput.LocationsForIdx(ii)[jj].second;
				
				if(resultsAreCompressed) {
					cv::Mat decodedim = cv::imdecode(*testOutput.CompressedCropsForIdx(ii)[jj],CV_LOAD_IMAGE_UNCHANGED);
					cropRect.width  = decodedim.cols;
					cropRect.height = decodedim.rows;
				} else {
					cropRect.width  = testOutput.CropsForIdx(ii)[jj].cols;
					cropRect.height = testOutput.CropsForIdx(ii)[jj].rows;
				}
				
				// Get truth target rectangle
				TruthFile_TargetInImage & thisTarg(truthEntry->targets_in_image[kk]);
				int tbX = atoi(GetTruthEntryValue("pos_x",thisTarg).c_str());
				int tbY = atoi(GetTruthEntryValue("pos_y",thisTarg).c_str());
				int tbW = atoi(GetTruthEntryValue("box_min_width",thisTarg).c_str());
				int tbH = atoi(GetTruthEntryValue("box_min_height",thisTarg).c_str());
				cv::Rect thisTargRect(tbX,tbY,tbW,tbH);
				consoleOutput.Level(dcv) << "this target official rect: " << thisTargRect << std::endl;
				
				// Intersect to find if the crop contained the true target
				cv::Rect rectIntersect = (cropRect & thisTargRect);
				
				double pixelsInCrop = ((double)(cropRect.width*cropRect.height));
				double rectIntscSize = static_cast<double>(rectIntersect.width * rectIntersect.height);
				double closeCallRatio = (rectIntscSize / pixelsInCrop);
				
				consoleOutput.Level(dcv) << "rectIntersect == " << rectIntersect << std::endl;
				
				
				
				// Does the crop contain a WHOLE target?
				if(rectIntersect.size() != thisTargRect.size()) {
					consoleOutput.Level(dcv) << "SALIENCY FAILURE: DETECTION FAILURE ------------------------------------------------" << std::endl;
					results.numFailures_totalDetectionFailures++;
					
					// Did it contain at least 10% of a target? Call it a "close call"
					if(closeCallRatio > 0.14 && sqrt(pixelsInCrop/((double)(tbW*tbH))) < 1.8) {
						consoleOutput.Level0() << "SALIENCY FAILURE: CLOSE CALL! (PARTIAL CROP) ------------ " << testOutput.ImgNameForIdx(ii) << std::endl;
						results.numCloseCallsForCroppingFailures++;
					}
				}
				else {
					// The crop contained a whole target! Get padding information
					int padX1 = (thisTargRect.x - cropRect.x);
					int padX2 = ((cropRect.x+cropRect.width) - (thisTargRect.x+thisTargRect.width));
					int padY1 = (thisTargRect.y - cropRect.y);
					int padY2 = ((cropRect.y+cropRect.height) - (thisTargRect.y+thisTargRect.height));
					
					consoleOutput.Level(dcv) << "padding: " << padX1 << ", " << padX2 << ", " << padY1 << ", " << padY2 << std::endl;
					
					int minPadding = MIN(MIN(padX1,padX2), MIN(padY1,padY2));
					
					// Is there enough padding?
					if(minPadding < results.desiredMinPaddingPixels) {
						results.numFailures_PaddingFailures++;
						consoleOutput.Level0() << "SALIENCY FAILURE: NOT ENOUGH PADDING ------------ " << testOutput.ImgNameForIdx(ii) << std::endl;
					}
					else {
						// Check if there is too much padding
						double sizeRatio_Width = static_cast<double>(cropRect.width) / static_cast<double>(thisTargRect.width);
						double sizeRatio_Height = static_cast<double>(cropRect.width) / static_cast<double>(thisTargRect.width);
						
						double maxSizeRatio = MAX(sizeRatio_Width, sizeRatio_Height);
						
						if(maxSizeRatio > results.desiredMaxCropLengthRatioToTargetLength) {
							results.numFailures_TooBig++;
							consoleOutput.Level0() << "SALIENCY FAILURE: CROP IS TOO BIG ------------ " << testOutput.ImgNameForIdx(ii) << std::endl;
						}
						else {
							// Not too little padding, not too much padding, this is perfect!
							results.numSuccesses++;
						}
					}
				}
			}
		}
	}
	return results;
}


void SaliencyOutput::clear()
{
	auto cropsiter = resultCrops.begin();
	while(cropsiter != resultCrops.end()) {
		if(cropsiter->second != nullptr) {
			cropsiter->second->clear();
			delete cropsiter->second;
		}
		cropsiter = resultCrops.erase(cropsiter);
	}
	auto locationsiter = resultLocations.begin();
	while(locationsiter != resultLocations.end()) {
		if(locationsiter->second != nullptr) {
			locationsiter->second->clear();
			delete locationsiter->second;
		}
		locationsiter = resultLocations.erase(locationsiter);
	}
	clearCompressedCrops();
}

void SaliencyOutput::clearCompressedCrops()
{
	auto compiter = resultCropsCompressed.begin();
	while(compiter != resultCropsCompressed.end()) {
		if(compiter->second != nullptr) {
			for(int ii=0; ii<compiter->second->size(); ii++) {
				delete compiter->second->at(ii);
			}
			compiter->second->clear();
			delete compiter->second;
		}
		compiter = resultCropsCompressed.erase(compiter);
	}
}

void SaliencyOutput::CompressCropsSoFar()
{
	if(resultCrops.empty()==false) {
		std::vector<unsigned char> *newarr_writehere;
		std::vector<int> pngparam = std::vector<int>(2);
		pngparam[0] = CV_IMWRITE_PNG_COMPRESSION; pngparam[1] = 3; //0-9... 9 is smallest compressed size
		auto itimg = resultCrops.begin();
		while(itimg != resultCrops.end()) {
			if(itimg->second != nullptr) {
				CompressedCropsForImage(itimg->first); //add a slot in the map for this image, if it doesn't already exist
				if(itimg->second->empty()==false) {
					auto itcr = itimg->second->begin(); //iter for std::vector< cv::Mat >
					while(itcr != itimg->second->end()) {
						newarr_writehere = new std::vector<unsigned char>();
						cv::imencode(".png", *itcr, *newarr_writehere, pngparam);
						CompressedCropsForImage(itimg->first).push_back(newarr_writehere);
						itcr = itimg->second->erase(itcr);
					}
				}
			} else {
				cout<<"WARNING: INVALID IMAGE IN SALIENCY OUTPUT CROPS LIST"<<endl;
			}
			itimg = resultCrops.erase(itimg);
		}
	}
}

//------------------------------------------------------------------

std::vector<cv::Mat>&
	SaliencyOutput::CropsForImage(std::string imgFilename)
{
	if(resultCrops.find(imgFilename) == resultCrops.end()) {
		resultCrops[imgFilename] = new std::vector<cv::Mat>();
	}
	return *resultCrops[imgFilename];
}

std::vector<std::pair<double,double>>&
	SaliencyOutput::LocationsForImage(std::string imgFilename)
{
	if(resultLocations.find(imgFilename) == resultLocations.end()) {
		resultLocations[imgFilename] = new std::vector<std::pair<double,double>>();
	}
	return *resultLocations[imgFilename];
}


std::vector<std::vector<unsigned char>*>&
	SaliencyOutput::CompressedCropsForImage(std::string imgFilename)
{
	if(resultCropsCompressed.find(imgFilename) == resultCropsCompressed.end()) {
		resultCropsCompressed[imgFilename] = new std::vector<std::vector<unsigned char>*>();
	}
	return *resultCropsCompressed[imgFilename];
}


int SaliencyOutput::numFullsizeImages() const {
	if(resultCrops.size()!=resultLocations.size() && resultCropsCompressed.size()!=resultLocations.size()) {
		std::cout<<"WARNING IN SALIENCY_EXPERIMENT_RESULTS: INCONSISTENT SIZE"<<std::endl;
	}
	return resultLocations.size();
}

std::string SaliencyOutput::ImgNameForIdx(int idx) {
	if(resultCrops.size()==resultLocations.size()) {
		if(idx >= 0 && idx < resultCrops.size()) {
			auto it = resultCrops.begin();
			if(idx == 0) return it->first;
			return std::next(it, idx)->first;
		}
	} else if(resultCropsCompressed.size()==resultLocations.size()) {
		if(idx >= 0 && idx < resultCropsCompressed.size()) {
			auto it = resultCropsCompressed.begin();
			if(idx == 0) return it->first;
			return std::next(it, idx)->first;
		}
	}
	std::cout<<"WARNING IN SALIENCY_EXPERIMENT_RESULTS: INCONSISTENT SIZE"<<std::endl;
	return "";
}

std::vector<cv::Mat>&
	SaliencyOutput::CropsForIdx(int idx)
{
	if(idx >= 0 && idx < resultCrops.size()) {
		auto it = resultCrops.begin();
		if(idx == 0) return *(it->second);
		return *(std::next(it, idx)->second);
	}
	std::cout<<"FATAL ERROR IN SALIENCY EXPERIMENT RESULTS INDEXING FOR CROPS"<<std::endl;
	assert(false);
	return *(new std::vector<cv::Mat>());
}

std::vector<std::pair<double,double>>&
	SaliencyOutput::LocationsForIdx(int idx)
{
	if(idx >= 0 && idx < resultLocations.size()) {
		auto it = resultLocations.begin();
		if(idx == 0) return *(it->second);
		return *(std::next(it, idx)->second);
	}
	std::cout<<"FATAL ERROR IN SALIENCY EXPERIMENT RESULTS INDEXING FOR LOCATIONS"<<std::endl;
	assert(false);
	return *(new std::vector<std::pair<double,double>>());
}

std::vector<std::vector<unsigned char>*>&
	SaliencyOutput::CompressedCropsForIdx(int idx)
{
	if(idx >= 0 && idx < resultCropsCompressed.size()) {
		auto it = resultCropsCompressed.begin();
		if(idx == 0) return *(it->second);
		return *(std::next(it, idx)->second);
	}
	std::cout<<"FATAL ERROR IN SALIENCY EXPERIMENT RESULTS INDEXING FOR COMPRESSED CROPS"<<std::endl;
	assert(false);
	return *(new std::vector<std::vector<unsigned char>*>());
}
