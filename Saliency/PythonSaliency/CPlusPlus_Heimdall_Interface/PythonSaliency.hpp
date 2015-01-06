#ifndef __PYTHON_SALIENCY_HPP____
#define __PYTHON_SALIENCY_HPP____

#include "Saliency/GenericSaliencyImplementation.hpp"

class PythonSaliencyClass : public GenericSaliencyImplementation
{
public:
	std::string saliencyModuleFolderName;
	std::string pythonFilename;
	std::string pythonFunctionName;
	std::vector<double> additional_args;
	
	
	virtual void ReceivedUpdatedArgs(const std::vector<double> & newArgs) {
		additional_args.resize(newArgs.size());
		for(int ii=0; ii<newArgs.size(); ii++) {
			additional_args[ii] = newArgs[ii];
		}
	}
	
	virtual void ProcessSaliency(cv::Mat fullsizeImage,
								std::vector<cv::Mat> & returnedCrops,
								std::vector<std::pair<double,double>> & returned_geolocations);
	
};

#endif
