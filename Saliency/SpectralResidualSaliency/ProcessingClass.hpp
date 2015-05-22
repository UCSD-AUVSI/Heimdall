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

//score: 0.720114, TruePositives: 62, CloseErrors: 6, FalsePositives: 45, FalseNegatives: 15
//num crops: 113
//params: (137.585, 3.09141, 16.6311, 513.349, 48.1391, 0.331574, 0.58696, 0.420527)
missed target in image "IMG_9207_geotag_shopped.JPG"
missed target in image "circle_F_IMG_0605.jpg"
missed target in image "diamond_IMG_0578.jpg"
missed target in image "square_2012_IMG_3133.jpg"
missed target in image "square_2012_IMG_3154.jpg"
missed target in image "triangle_1725425557_geotag.jpg"
*/
	SpectralSaliencyArgs() :
		normalizeSaliencyMaps(true),
		expectedTargetLength(137.585),
		expectedLargerTargetRatio(3.09141),
		minTargetLength(16.6311),
		maxTargetLength(513.349),
		largerGaussBlurDiamPixels(48.1391),
		centerSurroundThreshold(0.331574),
		centerSurroundThreshold_hystlow_FRACTION(0.58696),
		percentOfCropToBeTargetAfterPadding(0.420527),
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
