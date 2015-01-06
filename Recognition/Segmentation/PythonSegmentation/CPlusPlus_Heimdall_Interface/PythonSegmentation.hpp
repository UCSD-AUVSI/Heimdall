#ifndef __PYTHON_SEGMENTATION_HPP____
#define __PYTHON_SEGMENTATION_HPP____

#include <string>
#include <vector>
#include <opencv2/core/core.hpp>

class PythonSegmentationClass
{
public:
	std::string segmentationModuleFolderName;
	std::string pythonFilename;
	std::string pythonFunctionName;
	std::vector<double> additional_args;
	
	
	virtual void ReceivedUpdatedArgs(const std::vector<double> & newArgs) {
		additional_args.resize(newArgs.size());
		for(int ii=0; ii<newArgs.size(); ii++) {
			additional_args[ii] = newArgs[ii];
		}
	}
	
	
	virtual void ProcessSegmentation(cv::Mat inputCropImage,
									cv::Mat & returned_SSEG,
									cv::Scalar & returned_SColor,
									cv::Mat & returned_CSEG,
									cv::Scalar & returned_CColor);
	
};

#endif
