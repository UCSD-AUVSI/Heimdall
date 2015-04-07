#include <string>
#include <vector>
#include <iostream>

#include "SharedUtils/SharedUtils.hpp"
#include "PythonQRCode_ModuleInterface.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "PythonQRCode.hpp"
using std::cout; using std::endl;


void PythonQRCodeModuleInterface :: execute(imgdata_t *imdata, std::string args)
{
	cout << "Python QR Code (starting with C++)" << endl;
	bool failure = false;
	
	if(imdata->image_data == nullptr) {
		cout << "ERROR: NOT GIVEN AN IMAGE" << std::endl;
		failure = true;
	}
	if(args.empty()) {
		std::cout << "ERROR: NO PYTHON MODULE GIVEN TO RUN!" << std::endl;
		std::cout	<< "  When running a Heimdall client with PythonQRCode, " << std::endl
					<< "  the name of the module must be given as an argument, i.e." << std::endl
					<< "  --seg PYTHON_QRCODE [ExampleStub]" << std::endl
					<< "  if ExampleStub is the PythonQRCode module you wish to run." << std::endl;
		failure = true;
	}
	
	
	cv::Mat inputCropImage;
	bool read_succeeded_bool;
	std::string message_if_read;
	
	
	if(failure == false) {
		inputCropImage = cv::imdecode(*imdata->image_data, CV_LOAD_IMAGE_COLOR);
		
		PythonQRCodeClass qrcdoer;
		qrcdoer.qrcodeModuleFolderName = args;
		qrcdoer.pythonFilename = "main.py";
		qrcdoer.pythonFunctionName = "tryQRCodeRead";
		qrcdoer.ProcessQRCode(inputCropImage,
									read_succeeded_bool,
									message_if_read);
		
		if(read_succeeded_bool) {
			if(message_if_read.empty() == false) {
				imdata->qrCodeMessage = message_if_read;
			} else {
				cout << "C++ reports ERROR: you successfully read a QR code but didnt return the message??" << endl;
			}
		}
	}
	
	setDone(imdata, QRCODE);
}





