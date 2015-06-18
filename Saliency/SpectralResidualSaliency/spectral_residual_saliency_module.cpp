#include <string>
#include <vector>
#include <iostream>
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "spectral_residual_saliency_module.hpp"
#include "ProcessingClass.hpp"
#include "SharedUtils/Georeference.hpp"
#include <opencv2/highgui/highgui.hpp>
using std::cout; using std::endl;


const bool DEBUG_EXIF_INFO = false;



void SpectralResidualSaliency :: execute(imgdata_t *imdata, std::string args){
	cout << "Spectral Residual Saliency" << endl;
	
	if(imdata->image_data == nullptr) {
		std::cout << "ERROR: NOT GIVEN AN IMAGE" << std::endl;
		return;
	}
	
	cv::Mat fullsizeImage;
	std::vector<cv::Mat> foundCrops;
	std::vector<std::pair<double,double>> cropGeolocations;
	
	fullsizeImage = cv::imdecode(*imdata->image_data, CV_LOAD_IMAGE_COLOR);
	
	SpectralResidualSaliencyClass saldoer;
	saldoer.ProcessSaliency(&fullsizeImage, &foundCrops, &cropGeolocations, 0);
	
	consoleOutput.Level1() << "SpectralResidualSaliency found " << to_istring(foundCrops.size()) << " crops" << std::endl;
	
	
	//after saliency is done with the fullsize image, remove that fullsize image from the message's images vector
	imdata->image_data->clear();
	imdata->num_crops_in_this_image = foundCrops.size();
	
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
		
		int polvl = DEBUG_EXIF_INFO ? 0 : 2;
		if(DEBUG_EXIF_INFO) {
		consoleOutput.Level(polvl)<<"#####################################################################################################"<<endl;
		consoleOutput.Level(polvl)<<"target_geoloc (IN PIXELS): ([0],[1]) == ("<<std::get<0>(target_geoloc)<<", "<<std::get<1>(target_geoloc)<<")"<<endl;
		consoleOutput.Level(polvl)<<"imdata->plane: (lat, longt) and heading == ("<<imdata->planelat<<", "<<imdata->planelongt<<") and "<<imdata->planeheading<<endl;
		}
		
		target_geoloc = georeference_target_in_image(std::get<1>(target_geoloc), std::get<0>(target_geoloc), fullsizeImage.rows, fullsizeImage.cols,
				imdata->planelat, imdata->planelongt, imdata->planeheading, imdata->planealt);
		
		curr_imdata->targetlat = std::get<0>(target_geoloc);
		curr_imdata->targetlongt = std::get<1>(target_geoloc);
		
		consoleOutput.Level0()<<"curr_imdata->target: (lat, longt) == ("<<(curr_imdata->targetlat)<<", "<<(curr_imdata->targetlongt)<<")"<<endl;
		consoleOutput.Level4()<<"##################################################################################"<<endl;
		
		if(++i < foundCrops.size()){
			imgdata_t *new_imdata = new imgdata_t();
			copyIMGData(new_imdata, curr_imdata);
			
			new_imdata->next = nullptr;
			new_imdata->cropid++;
			curr_imdata->next = new_imdata;
			curr_imdata = new_imdata;
		}
	}
	setDone(imdata, SALIENCY);
}



