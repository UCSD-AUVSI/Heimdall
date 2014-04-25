#include <string>
#include <iostream>

#include "Verification/Verif.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Algs.hpp"
#include "opencv2/opencv.hpp"

using std::cout;
using std::endl;

void Stub_Verify :: execute(imgdata_t *imdata, std::string args){
	cv::startWindowThread();
	cv::namedWindow("Image", CV_WINDOW_NORMAL);
	for(std::vector<std::vector<unsigned char>*>::iterator i = imdata->image_data->begin();
			i < imdata->image_data->end(); ++i){
		cv::imshow("Image",	cv::imdecode(**i, CV_LOAD_IMAGE_COLOR));
		cv::waitKey(0);
	}

	for(std::vector<std::vector<unsigned char>*>::iterator i = imdata->sseg_image_data->begin();
			i < imdata->sseg_image_data->end(); ++i){
		cv::imshow("Image",	cv::imdecode(**i, CV_LOAD_IMAGE_COLOR));
		cv::waitKey(0);
	}

	for(std::vector<std::vector<unsigned char>*>::iterator i = imdata->cseg_image_data->begin();
			i < imdata->cseg_image_data->end(); ++i){
		cv::imshow("Image",	cv::imdecode(**i, CV_LOAD_IMAGE_COLOR));
		cv::waitKey(0);
	}
	cv::destroyWindow("Image");

	setDone(imdata, VERIF);
	cout << "Stub Verification" << endl << endl;
}
