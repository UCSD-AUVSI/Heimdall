#include <string>
#include <vector>
#include <iostream>

#include "SharedUtils/SharedUtils.hpp"
#include "PythonSaliency_ModuleInterface.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "SharedUtils/GlobalVars.hpp"
#include "SharedUtils/Georeference.hpp"
#include "Saliency/ResultsTruthTester/SaliencyResultsTruthTester.hpp"

#include "PythonSaliency.hpp"
using std::cout;
using std::endl;

int PythonSaliency::lastLocalExperimentNum = 0;


void PythonSaliency :: execute(imgdata_t *imdata, std::string args)
{
	cout << "Python Saliency" << endl;
	bool failure = false;
	
	if(imdata->image_data == nullptr) {
		std::cout << "ERROR: NOT GIVEN AN IMAGE" << std::endl;
		failure = true;
	}
	if(args.empty()) {
		std::cout << "ERROR: NO PYTHON MODULE GIVEN TO RUN!" << std::endl;
		std::cout	<< "  When running a Heimdall client with PythonSaliency, " << std::endl
					<< "  the name of the module must be given as an argument, i.e." << std::endl
					<< "  --saliency PYTHON_SALIENCY [Canny3D]" << std::endl
					<< "  if Canny3D is the PythonSaliency module you wish to run." << std::endl;
		failure = true;
	}
	int spacepos;
	if(str_contains_char(args,' ',&spacepos)) {
		std::string second_arg = trim_chars_after_first_instance_of_delim(args, ' ', false);
		cout<<"PYTHON SALIENCY RECEIVED TWO ARGUMENTS; SECOND ARG IS: \""<<second_arg<<"\""<<endl;
		if(second_arg == "EXPERIMENT") {
			cout<<"---- SALIENCY (PYTHON) EXPERIMENTAL MODE SET"<<endl;
			
			/*globalExperimentResultsCalculatorFunc = SaliencyExperimentResultsCalculator;
			if(lastLocalExperimentNum < globalExperimentNum) {
				cout<<"NEW EXPERIMENT DETECTED: UPDATING PYTHON VARIABLES..."<<endl;
				lastLocalExperimentNum = globalExperimentNum;
			}*/
		}
	}
	
	cv::Mat fullsizeImage;
	std::vector<cv::Mat> foundCrops;
	std::vector<std::pair<double,double>> cropGeolocations;
	
	if(failure == false) {
		fullsizeImage = cv::imdecode(*imdata->image_data, CV_LOAD_IMAGE_COLOR);
		
		PythonSaliencyClass saldoer;
		saldoer.saliencyModuleFolderName = args;
		saldoer.pythonFilename = "main.py";
		saldoer.pythonFunctionName = "doSaliency";
		saldoer.ProcessSaliency(&fullsizeImage, &foundCrops, &cropGeolocations, 0);
		
		consoleOutput.Level1() << "pythonSaliency found " << to_istring(foundCrops.size()) << " crops" << std::endl;
		
		//-----------------------------------------------------------------------------------------------------------
		/*for(int jj=0; jj<foundCrops.size(); jj++) {
			std::cout<<"crop "<<jj<<" found at "<<cropGeolocations[jj].first<<","<<cropGeolocations[jj].second<<std::endl;
		}/*
		for(int jj=0; jj<foundCrops.size(); jj++) {
			cv::imshow(std::string("crop")+to_istring(jj), foundCrops[jj]);
		}
		cv::waitKey(0);
		cv::destroyAllWindows();*/
		//-----------------------------------------------------------------------------------------------------------
	}
	
	//after saliency is done with the fullsize image, remove that fullsize image from the message's images vector
	imdata->image_data->clear();
	imdata->num_crops_in_this_image = foundCrops.size();
	
	if(failure == false) {
		std::vector<int> param = std::vector<int>(2);
		param[0] = CV_IMWRITE_PNG_COMPRESSION;
		param[1] = 6; //default(3)  0-9, where 9 is smallest compressed size.
		
		imgdata_t *curr_imdata = imdata;
		int i = 0;
		while(i < foundCrops.size()) {
			std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
			cv::imencode(".png", foundCrops[i] , *newarr, param);
			delete curr_imdata->image_data; //delete whatever is already in there
			curr_imdata->image_data = newarr;
			
			std::pair<double,double> target_geoloc = cropGeolocations[i];
			
			target_geoloc = georeference_target_in_image(std::get<1>(target_geoloc), std::get<0>(target_geoloc), fullsizeImage.rows, fullsizeImage.cols,
							imdata->planelat, imdata->planelongt, imdata->planeheading, imdata->planealt);
			
			curr_imdata->targetlat = std::get<0>(target_geoloc);
			curr_imdata->targetlongt = std::get<1>(target_geoloc);
			
			
			if(++i < foundCrops.size()){
				imgdata_t *new_imdata = new imgdata_t();
				copyIMGData(new_imdata, curr_imdata);

				new_imdata->next = nullptr;
				new_imdata->cropid++;
				curr_imdata->next = new_imdata;
				curr_imdata = new_imdata;
			}
		}
	}
	
	setDone(imdata, SALIENCY);
}
