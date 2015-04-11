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
#include "Saliency/ResultsTruthTester/SaliencyResultsTruthTester.hpp"

#include "PythonSaliency.hpp"
using std::cout;
using std::endl;

int PythonSaliency::lastLocalExperimentNum = 0;



// Calculate pixels per feet (resolution) of given image, AFTER scaling
double mycalculate_px_per_feet(double horiz_cols, double altitude, double scalefactor){
    cout << "alt: " << altitude << endl;
    double focal_length = 35, crop_factor = 1.6; 
    double equiv_foc_len = focal_length * crop_factor;

    double h_fov = 2 * atan(36/(2*equiv_foc_len)); 

    double h_ground = 2 * altitude * tan(h_fov/2);
    double px_per_feet = horiz_cols/h_ground;

    return px_per_feet * scalefactor;
}

std::pair<double, double> myfind_target_geoloc(int targetrow, int targetcol, int imrows, int imcols, double planelat, double planelongt, double planeheading, double pxperfeet){
    planeheading = planeheading - kPI; //Gimbal is reversed in plane

    double rowdiff = targetrow - (double)imrows/2; //row diff from center (and center of plane)
    double coldiff = targetcol - (double)imcols/2; //col diff from center (and center of plane)

    // Tranlate to polar coordinates, in feet
    double centerdiff = sqrt(pow(rowdiff, 2) + pow(coldiff, 2));
    double centerfeetdiff = centerdiff / pxperfeet;
    double centerangle = atan(coldiff/-rowdiff) + (((-rowdiff)>0)?0.0:kPI);
    
    // Project to Lat/Long
    double latfeetdiff = centerfeetdiff * cos(planeheading + centerangle);
    double longtfeetdiff = centerfeetdiff * sin(planeheading + centerangle);

    // Convert Lat/Long feet differences into degrees to get final lat/long
    double target_lat = planelat + latfeetdiff/365221.43; //365221 feet in 1 degree of latitude arc, small angle assumptions for field; 
    double longt_deg_to_feet = kPI * 20898855.01138578 * cos(to_radians(target_lat)) / 180; //Radius of circle at this lat, (PI*R)/(180)
    double target_longt = planelongt + longtfeetdiff/longt_deg_to_feet;
    
    //cout << "heading: " << planeheading << endl;
    //cout << "imrows: " << imrows << " imcols: " << imcols << endl;
    //cout << "rd: " << rowdiff << endl;
    //cout << "cd: " << coldiff << endl;
    //cout << "ppf: " << pxperfeet << endl;
    //cout << "centerdiff: " << centerdiff << endl;
    //cout << "centerfeetdiff: " << centerfeetdiff << endl;
    //cout << "centerangle: " << centerangle << endl;
    //printf("lat diff: %.7f\tdeg, %.3f feet\n", latfeetdiff/365221, latfeetdiff);
    //printf("long diff: %.7f\tdeg, %.3f feet\n", longtfeetdiff/longt_deg_to_feet, longtfeetdiff);

    return std::pair<double, double>(target_lat, target_longt);
}



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
			
			target_geoloc = myfind_target_geoloc(std::get<1>(target_geoloc), std::get<0>(target_geoloc), fullsizeImage.rows, fullsizeImage.cols,
                    imdata->planelat, imdata->planelongt, imdata->planeheading, mycalculate_px_per_feet(fullsizeImage.cols, imdata->planealt, 1));
			
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
