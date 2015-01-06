#include <string>
#include <vector>
#include <iostream>

#include "SharedUtils/SharedUtils.hpp"
#include "PythonVerif_ModuleInterface.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "PythonVerif.hpp"
using std::cout;
using std::endl;


void PythonVerify :: execute(imgdata_t *imdata, std::string args)
{
	cout << "Python Verif" << endl;
	bool failure = false;
	
	if(args.empty()) {
		std::cout << "ERROR: NO PYTHON MODULE GIVEN TO RUN!" << std::endl;
		std::cout	<< "  When running a Heimdall client with PythonVerif, " << std::endl
					<< "  the name of the module must be given as an argument, i.e." << std::endl
					<< "  --seg PYTHON_VERIF [ExampleStub]" << std::endl
					<< "  if ExampleStub is the PythonVerif module you wish to run." << std::endl;
		failure = true;
	}
	
	
	if(failure == false) {
		PythonVerifClass vdoer;
		vdoer.verifModuleFolderName = args;
		vdoer.pythonFilename = "main.py";
		vdoer.pythonFunctionName = "doVerif";
		vdoer.ProcessVerification(	imdata->scolorR,
									imdata->scolorG,
									imdata->scolorB,
									imdata->scolor,
									imdata->shape,
									imdata->ccolorR,
									imdata->ccolorG,
									imdata->ccolorB,
									imdata->ccolor,
									imdata->character,
									imdata->targetlat,
									imdata->targetlongt,
									imdata->targetorientation);
	}
	
	setDone(imdata, VERIF);
}
