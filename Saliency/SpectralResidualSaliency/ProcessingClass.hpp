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
	
	SpectralSaliencyArgs() :
		normalizeSaliencyMaps(true),
		expectedTargetLength(58.9711),
		expectedLargerTargetRatio(2.08015),
		minTargetLength(20.2645),
		maxTargetLength(205.586),
		largerGaussBlurDiamPixels(35.1689),
		centerSurroundThreshold(0.323944),
		centerSurroundThreshold_hystlow_FRACTION(0.345039),
		percentOfCropToBeTargetAfterPadding(0.346016),
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
