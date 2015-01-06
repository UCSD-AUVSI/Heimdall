#include <string>
#include <vector>
#include <iostream>
using std::cout; using std::endl;
#include "spectral_residual_saliency_module.hpp"
#include "ProcessingClass.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include <opencv2/highgui/highgui.hpp>


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
	saldoer.ProcessSaliency(fullsizeImage, foundCrops, cropGeolocations);
	
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
	setDone(imdata, SALIENCY);
}



