#ifndef __GENERIC_SALIENCY_IMPLEMENTATION_HPP__
#define __GENERIC_SALIENCY_IMPLEMENTATION_HPP__

#include <opencv2/core/core.hpp>
#include <vector>
#include <string>

class GenericSaliencyImplementation
{
public:
	virtual void ReceivedUpdatedArgs(const std::vector<double> & newArgs) = 0;
	
	virtual void ProcessSaliency(cv::Mat fullsizeImage,
								std::vector<cv::Mat> & returnedCrops,
								std::vector<std::pair<double,double>> & returned_geolocations) = 0;
};


#endif
