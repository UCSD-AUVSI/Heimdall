#ifndef __PYTHON_COLOR_CLASSIFICATION_HPP____
#define __PYTHON_COLOR_CLASSIFICATION_HPP____

#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class PythonColorClassifierClass
{
public:
	std::string colorclassifierModuleFolderName;
	std::string pythonFilename;
	std::string pythonFunctionName;
	std::vector<double> additional_args;
	
	
	virtual void ReceivedUpdatedArgs(const std::vector<double> & newArgs) {
		additional_args.resize(newArgs.size());
		for(int ii=0; ii<newArgs.size(); ii++) {
			additional_args[ii] = newArgs[ii];
		}
	}
	
	
	virtual void ProcessColorClassification(unsigned char scolorR,
											unsigned char scolorG,
											unsigned char scolorB,
											std::string & returned_scolor,
											unsigned char ccolorR,
											unsigned char ccolorG,
											unsigned char ccolorB,
											std::string & returned_ccolor);
	
};

#endif
