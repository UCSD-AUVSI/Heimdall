#include <string>
#include <vector>
#include <iostream>

#include "SharedUtils/SharedUtils.hpp"
#include "PythonSegmentation_ModuleInterface.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "PythonSegmentation.hpp"
using std::cout;
using std::endl;


void PythonSegmentation :: execute(imgdata_t *imdata, std::string args)
{
	cout << "Python Segmentation" << endl;
	bool failure = false;
	
	if(imdata->image_data == nullptr) {
		std::cout << "ERROR: NOT GIVEN AN IMAGE" << std::endl;
		failure = true;
	}
	if(args.empty()) {
		std::cout << "ERROR: NO PYTHON MODULE GIVEN TO RUN!" << std::endl;
		std::cout	<< "  When running a Heimdall client with PythonSegmentation, " << std::endl
					<< "  the name of the module must be given as an argument, i.e." << std::endl
					<< "  --seg PYTHON_SEG [ExampleStub]" << std::endl
					<< "  if ExampleStub is the PythonSegmentation module you wish to run." << std::endl;
		failure = true;
	}
	
	
	cv::Mat inputCropImage;
	cv::Mat returned_SSEG;
	cv::Scalar returned_SColor;
	cv::Mat returned_CSEG;
	cv::Scalar returned_CColor;
	
	
	if(failure == false) {
		inputCropImage = cv::imdecode(*imdata->image_data, CV_LOAD_IMAGE_COLOR);
		
		PythonSegmentationClass segdoer;
		segdoer.segmentationModuleFolderName = args;
		segdoer.pythonFilename = "main.py";
		segdoer.pythonFunctionName = "doSegmentation";
		segdoer.ProcessSegmentation(inputCropImage,
									returned_SSEG,
									returned_SColor,
									returned_CSEG,
									returned_CColor);
		
		std::vector<int> param = std::vector<int>(2);
		param[0] = CV_IMWRITE_PNG_COMPRESSION;
		param[1] = 6; //default(3)  0-9, where 9 is smallest compressed size.
		
		if(returned_SSEG.empty() == false) {
			std::vector<unsigned char> *newarr_s = new std::vector<unsigned char>();
			cv::imencode(".png", returned_SSEG, *newarr_s, param);
			imdata->sseg_image_data->push_back(newarr_s);
		}
		else {
			consoleOutput.Level1() << "warning: in PythonSeg, empty SSEG cv::Mat returned" << endl;
		}
		
		if(returned_CSEG.empty() == false) {
			std::vector<unsigned char> *newarr_c = new std::vector<unsigned char>();
			cv::imencode(".png", returned_CSEG, *newarr_c, param);
			imdata->cseg_image_data->push_back(newarr_c);
		}
		else {
			consoleOutput.Level1() << "warning: in PythonSeg, empty CSEG cv::Mat returned" << endl;
		}
		
		imdata->scolorB = returned_SColor[0];
		imdata->scolorG = returned_SColor[1];
		imdata->scolorR = returned_SColor[2];
		
		imdata->ccolorB = returned_CColor[0];
		imdata->ccolorG = returned_CColor[1];
		imdata->ccolorR = returned_CColor[2];
	}
	
	setDone(imdata, SEG);
}
