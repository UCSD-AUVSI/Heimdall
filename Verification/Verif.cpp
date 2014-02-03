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

void Stub_Verify :: execute(imgdata_t *data){
	cv::startWindowThread();
	cv::namedWindow("Image", CV_WINDOW_NORMAL);
	cv::imshow("Image",	cv::imdecode(*(data->image_data->at(0)), CV_LOAD_IMAGE_COLOR));
	cv::waitKey(0);
	cv::imshow("Image",	cv::imdecode(*(data->sseg_image_data->at(0)), CV_LOAD_IMAGE_COLOR));
	cv::waitKey(0);
	cv::imshow("Image",	cv::imdecode(*(data->cseg_image_data->at(0)), CV_LOAD_IMAGE_COLOR));
	cv::waitKey(0);
	cv::destroyWindow("Image");

	setDone(data, STUB_VERIF);
	cout << "Stub Verification" << endl << endl;
}
