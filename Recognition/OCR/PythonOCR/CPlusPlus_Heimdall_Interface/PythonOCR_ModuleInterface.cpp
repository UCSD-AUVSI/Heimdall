#include <string>
#include <vector>
#include <iostream>
#include "SharedUtils/SharedUtils.hpp"
#include "PythonOCR_ModuleInterface.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "PythonOCR.hpp"
using std::cout; using std::endl;


void PythonOCRModuleInterface :: execute(imgdata_t *imdata, std::string args)
{
	cout << "Python OCR (starting with C++)" << endl;
	bool failure = false;
	
	if(imdata->cseg_image_data->empty()) {
		std::cout << "ERROR: NOT GIVEN A CSEG" << std::endl;
		failure = true;
	}
	if(args.empty()) {
		std::cout << "ERROR: NO PYTHON MODULE GIVEN TO RUN!" << std::endl;
		std::cout	<< "  When running a Heimdall client with PythonOCR, " << std::endl
					<< "  the name of the module must be given as an argument, i.e." << std::endl
					<< "  --ocr PYTHON_OCR [ExampleStub]" << std::endl
					<< "  if ExampleStub is the PythonOCR module you wish to run." << std::endl;
		failure = true;
	}
	
	
	if(failure == false) {
		PythonOCRClass ocrdoer;
		ocrdoer.pythonOCRModuleFolderName = args;
		ocrdoer.pythonFilename = "main.py";
		ocrdoer.pythonFunctionName = "doOCR";
		
		int num_csegs_done = 0;
        std::vector<cv::Mat> given_CSEGs;
        std::vector<std::vector<unsigned char>*>::iterator cseg_iter;
        for(cseg_iter = imdata->cseg_image_data->begin();
            cseg_iter != imdata->cseg_image_data->end();
            cseg_iter++)
        {
			if(num_csegs_done > 0) {
				cout << "NOTICE: THERE WAS MORE THAN ONCE CSEG, so far there have been "<<num_csegs_done<<" !!!"<<endl;
			}
			char char1, char2;
			double char1conf, char2conf, char1angle, char2angle;
			ocrdoer.ProcessOCR(cv::imdecode(**cseg_iter, CV_LOAD_IMAGE_GRAYSCALE),
										char1, char1conf, char1angle,
										char2, char2conf, char2angle);
			imdata->character1 = char_to_string(char1);
			imdata->char1confidence = char1conf;
			imdata->character2 = char_to_string(char2);
			imdata->char2confidence = char2conf;
			num_csegs_done++;
        }
	}
	
	setDone(imdata, OCR);
}
