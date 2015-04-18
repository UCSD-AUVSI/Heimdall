#ifndef __PYTHON_VERIF_HPP____
#define __PYTHON_VERIF_HPP____

#include <string>
#include <vector>
#include <opencv2/core/core.hpp>

class PythonVerifClass
{
public:
	std::string verifModuleFolderName;
	std::string pythonFilename;
	std::string pythonFunctionName;
	std::vector<double> additional_args;
	
	
	virtual void ReceivedUpdatedArgs(const std::vector<double> & newArgs) {
		additional_args.resize(newArgs.size());
		for(int ii=0; ii<newArgs.size(); ii++) {
			additional_args[ii] = newArgs[ii];
		}
	}
	
	
	virtual void ProcessVerification(unsigned char scolorR,
									unsigned char scolorG,
									unsigned char scolorB,
									std::string shape_color,
									std::string shape_name,
									unsigned char ccolorR,
									unsigned char ccolorG,
									unsigned char ccolorB,
									std::string character_color,
									std::string character_name,
									double target_latitude,
									double target_longitude,
									double target_orientation,
									std::string original_image_filename,
									cv::Mat inputCropImage);
	
};

#endif
