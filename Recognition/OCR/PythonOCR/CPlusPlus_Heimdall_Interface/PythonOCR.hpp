#ifndef __PYTHON_OCR_HPP____
#define __PYTHON_OCR_HPP____

#include <string>
#include <vector>
#include <opencv2/core/core.hpp>

class PythonOCRClass
{
public:
	std::string pythonOCRModuleFolderName;
	std::string pythonFilename;
	std::string pythonFunctionName;
	std::vector<double> additional_args;
	
	
	virtual void ReceivedUpdatedArgs(const std::vector<double> & newArgs) {
		additional_args.resize(newArgs.size());
		for(int ii=0; ii<newArgs.size(); ii++) {
			additional_args[ii] = newArgs[ii];
		}
	}
	
	
	virtual void ProcessOCR(cv::Mat givenCSEG,
							char & char1, double char1_conf, double char1_angle,
							char & char2, double char2_conf, double char2_angle);
	
};

#endif
