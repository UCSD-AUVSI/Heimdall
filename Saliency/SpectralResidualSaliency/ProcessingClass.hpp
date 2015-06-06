#ifndef __PROCESSING_SALIENCYSPECTRALRESIDUAL_HPP
#define __PROCESSING_SALIENCYSPECTRALRESIDUAL_HPP

#include "Saliency/OptimizeableSaliency.hpp"


class SpectralSaliencyArgs
{
public:
	bool normalizeSaliencyMaps;
	double expectedTargetLength;
	double expectedLargerTargetRatio;
	double minTargetLength;
	double maxTargetLength;
	int largerGaussBlurDiamPixels;
	double centerSurroundThreshold;
	double centerSurroundThreshold_hystlow_FRACTION;
	double percentOfCropToBeTargetAfterPadding;
	std::string save_output_to_this_folder;

/*
Dataset: 2015 April 31 Training Set  (48 images)
score: 0.754096
params: (102.283, 1.88841, 27.2135, 456.142, 43.4693, 0.22672, 0.149246, 0.679372)
num crops: 123
num successes: targets: 41
num successes: falseps: 29
*/
	SpectralSaliencyArgs() :
		normalizeSaliencyMaps(true),
		expectedTargetLength(94.335),
		expectedLargerTargetRatio(2.1608),
		minTargetLength(45.7863),
		maxTargetLength(398.458),
		largerGaussBlurDiamPixels(71.6563),
		centerSurroundThreshold(0.22061),
		centerSurroundThreshold_hystlow_FRACTION(0.104754),
		percentOfCropToBeTargetAfterPadding(0.794506),
		/*expectedTargetLength(80.0),
		minTargetLength(12.0),
		maxTargetLength(200.0),
		largerGaussBlurDiamPixels(27),
		centerSurroundThreshold(0.47),//3.9e6),
		centerSurroundThreshold_hystlow(0.39),
		percentOfCropToBeTargetAfterPadding(0.3),*/
		save_output_to_this_folder("")
		{}
	
	void GetVec(std::vector<double> & putHere);
	static SpectralSaliencyArgs GetArgs(const std::vector<double> & argsvec);
};


class SpectralResidualSaliencyClass : public GenericSaliencyImplementation
{
public:
	SpectralSaliencyArgs args;
	
//------------------------------------------------------------
	bool saveIntermediateResults;
	cv::Mat last_saliency_map;
	cv::Mat last_centersurround_map;
	cv::Mat last_binary_map;
//------------------------------------------------------------
	
	SpectralResidualSaliencyClass() : GenericSaliencyImplementation(), saveIntermediateResults(false) {}
	
	virtual void ReceivedUpdatedArgs(const std::vector<double> & newArgs) {
		args = SpectralSaliencyArgs::GetArgs(newArgs);
	}
	
	virtual void ProcessSaliency(cv::Mat * fullsizeImage,
								std::vector<cv::Mat> * returnedCrops,
								std::vector<std::pair<double,double>> * returned_geolocations,
								int threadNumForDebugging);
};


#endif
