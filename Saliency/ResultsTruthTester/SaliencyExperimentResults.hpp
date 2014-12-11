#ifndef __SALIENCY_EXPERIMENT_RESULTS_HPP__
#define __SALIENCY_EXPERIMENT_RESULTS_HPP__

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>


class SaliencyExperimentResults
{
public:
static int desiredMinPaddingPixels;
static double desiredMaxCropLengthRatioToTargetLength; //for Failures_TooBig


	int numFailures_totalDetectionFailures;
	int numFailures_PaddingFailures;
	int numFailures_TooBig;
	int numCloseCallsForCroppingFailures;
	int numSuccesses;
	int numDetectedThings;
	int numTruthTargets;
	
	SaliencyExperimentResults() :
		numFailures_totalDetectionFailures(0),
		numFailures_PaddingFailures(0),
		numFailures_TooBig(0),
		numCloseCallsForCroppingFailures(0),
		numSuccesses(0),
		numDetectedThings(0),
		numTruthTargets(0) {}
	
	int TruePositives() {return numSuccesses;}
	int FalsePositives() {return (numDetectedThings - numSuccesses);}
	int FalseNegatives() {if(numTruthTargets<=0){return 0;} return (numTruthTargets - numSuccesses);}
};


class SaliencyOutput
{
public:
	void clear();
	~SaliencyOutput() {clear();}
	
	/*
		Maps from original-image-filename to the results Saliency got from them
		
		Crops format:	std::vector< cv::Mat >
		
		Locations format: std::vector< std::pair<double,double> >
	*/
	std::map<std::string, std::vector<std::vector<unsigned char>*>*> resultCropsCompressed;
	std::map<std::string, std::vector<cv::Mat>*> resultCrops;
	std::map<std::string, std::vector<std::pair<double,double>>*> resultLocations;
	
	/*
		Get results by source image filename
	*/
	std::vector<cv::Mat>&
		CropsForImage(std::string imgFilename);
	
	std::vector<std::pair<double,double>>&
		LocationsForImage(std::string imgFilename);
	
	std::vector<std::vector<unsigned char>*>&
		CompressedCropsForImage(std::string imgFilename);
	
	/*
		Compress crops to save memory
	*/
	void clearCompressedCrops();
	void CompressCropsSoFar();
	
	/*
		Get results by the index when looping over all source images
	*/
	int numFullsizeImages() const;
	
	std::string ImgNameForIdx(int idx);
	
	std::vector<cv::Mat>&
		CropsForIdx(int idx);
	
	std::vector<std::pair<double,double>>&
		LocationsForIdx(int idx);
		
	std::vector<std::vector<unsigned char>*>&
		CompressedCropsForIdx(int idx);
};


SaliencyExperimentResults GetSaliencyExperimentResults(std::string truthFilename,
														/*const*/ SaliencyOutput & testOutput);



#endif
