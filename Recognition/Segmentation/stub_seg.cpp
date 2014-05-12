#include <iostream>

#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "stub_seg.hpp"

using std::cout;
using std::endl;

void StubSeg :: execute(imgdata_t *imdata, std::string args) {
	cout << "Stub Segmentation" << endl;
	
	//copy the input image directly into both the CSEG and SSEG slots in the imdata struct
	//the input image should already be segmented for this to be useful
	
	if(imdata->image_data->empty() == false)
	{
		cv::Mat cropped_input_image = cv::imdecode(**(imdata->image_data->begin()), CV_LOAD_IMAGE_COLOR);
		
		std::vector<int> param = std::vector<int>(2);
		param[0] = CV_IMWRITE_PNG_COMPRESSION;
		param[1] = 6; //default(3)  0-9, where 9 is smallest compressed size.
		
		std::vector<unsigned char> *newarr_s = new std::vector<unsigned char>();
		cv::imencode(".png", cropped_input_image, *newarr_s, param);
		imdata->sseg_image_data->push_back(newarr_s);
		
		std::vector<unsigned char> *newarr_c = new std::vector<unsigned char>();
		cv::imencode(".png", cropped_input_image, *newarr_c, param);
		imdata->cseg_image_data->push_back(newarr_c);
	}
	
	setDone(imdata, SEG);
}
