#ifndef __PYTHON_QRCODE_HPP____
#define __PYTHON_QRCODE_HPP____

#include <string>
#include <vector>
#include <opencv2/core/core.hpp>

class PythonQRCodeClass
{
public:
	std::string qrcodeModuleFolderName;
	std::string pythonFilename;
	std::string pythonFunctionName;
	std::vector<double> additional_args;
	
	
	virtual void ReceivedUpdatedArgs(const std::vector<double> & newArgs) {
		additional_args.resize(newArgs.size());
		for(int ii=0; ii<newArgs.size(); ii++) {
			additional_args[ii] = newArgs[ii];
		}
	}
	
	virtual void ProcessQRCode(cv::Mat inputCropImage,
				bool & returned_read_success,
				std::string & returned_message_if_success);
	
};

#endif
