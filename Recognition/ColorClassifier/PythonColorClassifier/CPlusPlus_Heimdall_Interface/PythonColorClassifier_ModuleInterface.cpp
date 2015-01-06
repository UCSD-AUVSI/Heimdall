#include <string>
#include <vector>
#include <iostream>

#include "SharedUtils/SharedUtils.hpp"
#include "PythonColorClassifier_ModuleInterface.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "PythonColorClassifier.hpp"
using std::cout;
using std::endl;


void PythonColorClassifier :: execute(imgdata_t *imdata, std::string args)
{
	cout << "Python Color Classifier" << endl;
	bool failure = false;
	
	if(imdata->image_data == nullptr) {
		std::cout << "ERROR: NOT GIVEN AN IMAGE" << std::endl;
		failure = true;
	}
	if(args.empty()) {
		std::cout << "ERROR: NO PYTHON MODULE GIVEN TO RUN!" << std::endl;
		std::cout	<< "  When running a Heimdall client with PythonColorClassifier, " << std::endl
					<< "  the name of the module must be given as an argument, i.e." << std::endl
					<< "  --seg PYTHON_COLORCLASS [ExampleStub]" << std::endl
					<< "  if ExampleStub is the PythonColorClassifier module you wish to run." << std::endl;
		failure = true;
	}
	
	
	if(failure == false) {
		std::string returned_scolor;
		std::string returned_ccolor;
		
		PythonColorClassifierClass ccdoer;
		ccdoer.colorclassifierModuleFolderName = args;
		ccdoer.pythonFilename = "main.py";
		ccdoer.pythonFunctionName = "doColorClassification";
		ccdoer.ProcessColorClassification(	imdata->scolorR,
											imdata->scolorG,
											imdata->scolorB,
											returned_scolor,
											imdata->ccolorR,
											imdata->ccolorG,
											imdata->ccolorB,
											returned_ccolor);
		
		imdata->scolor = returned_scolor;
		imdata->ccolor = returned_ccolor;
	}
	
	setDone(imdata, COLORCLASS);
}
