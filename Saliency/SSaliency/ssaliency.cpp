#include <string>
#include <vector>
#include <iostream>

#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include "ssaliency.hpp"
#include "saliency_module_C_SBD.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"


/*extern*/ SaliencyModule_C_SBD* global_SSaliency_module_instance = nullptr; //declared in SSaliency.hpp


using std::cout;
using std::endl;

void SSaliency :: execute(imgdata_t *imdata, std::string args){
	cout << "SSaliency, ID: " << imdata->id  << ", CropID: " << imdata->cropid << endl;
	
	if(global_SSaliency_module_instance == nullptr)
	{
		global_SSaliency_module_instance = new SaliencyModule_C_SBD();
		
		global_SSaliency_module_instance->write_output_to_folder = false;
		
		bool environment_is_desert = false;
		
		if(environment_is_desert) {
			global_SSaliency_module_instance->settings.push_back(SaliencySettings());
			global_SSaliency_module_instance->settings.rbegin()->Canny_low_threshold = 50;
			global_SSaliency_module_instance->settings.rbegin()->Canny_high_threshold = 120;
			global_SSaliency_module_instance->settings.rbegin()->ColorConvert_CV_color_space = CV_BGR2Luv;
			
			global_SSaliency_module_instance->settings.push_back(SaliencySettings());
			global_SSaliency_module_instance->settings.rbegin()->Canny_low_threshold = 50;
			global_SSaliency_module_instance->settings.rbegin()->Canny_high_threshold = 120;
			global_SSaliency_module_instance->settings.rbegin()->ColorConvert_CV_color_space = CV_BGR2Lab;
			
			/*global_SSaliency_module_instance->settings.push_back(SaliencySettings());
			global_SSaliency_module_instance->settings.rbegin()->Canny_low_threshold = 5;
			global_SSaliency_module_instance->settings.rbegin()->Canny_high_threshold = 250;
			global_SSaliency_module_instance->settings.rbegin()->ColorConvert_CV_color_space = COLORSPACE_CONVERSIONTYPE_KEEPRGB;
			global_SSaliency_module_instance->settings.rbegin()->ColorConvert_desired_CV_color_channels[0] = true;
			global_SSaliency_module_instance->settings.rbegin()->ColorConvert_desired_CV_color_channels[1] = true;
			global_SSaliency_module_instance->settings.rbegin()->ColorConvert_desired_CV_color_channels[2] = true;*/
		} else {
			global_SSaliency_module_instance->settings.push_back(SaliencySettings());
		}
		
		global_SSaliency_module_instance->output_folder_to_save_crops = std::string();
		global_SSaliency_module_instance->write_ALL_output_not_just_the_crops = false;
	}
	
	
    global_SSaliency_module_instance->do_saliency(cv::imdecode(*imdata->image_data, CV_LOAD_IMAGE_COLOR));
    
    //after saliency is done with the fullsize image, remove that fullsize image from the message's images vector
    imdata->image_data->clear();
	
	consoleOutput.Level1() << "Saliency found "
							<< to_istring(global_SSaliency_module_instance->returned_cropped_images.size())
							<< " crops" << std::endl;
	
	imdata->num_crops_in_this_image = global_SSaliency_module_instance->returned_cropped_images.size();
	
    imgdata_t *curr_imdata = imdata;
    int i = 0;
    while(i < global_SSaliency_module_instance->returned_cropped_images.size())
    {
		//valgrind doesn't like this ("definitely lost")
		std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
		cv::imencode(".jpg", global_SSaliency_module_instance->returned_cropped_images[i], *newarr);
		delete curr_imdata->image_data;
		curr_imdata->image_data = newarr;
        
        if(++i < global_SSaliency_module_instance->returned_cropped_images.size()){
            imgdata_t *new_imdata = new imgdata_t();
            copyIMGData(new_imdata, curr_imdata);

            new_imdata->next = nullptr;
            new_imdata->cropid++;
            curr_imdata->next = new_imdata;
            curr_imdata = new_imdata;
        }
	}
	
	setDone(imdata, SALIENCY);
	
#if 0
	std::cout << "starting image encoding test" << std::endl;
	
    cv::Mat immm = cv::Mat::zeros(400,400, CV_8U);
	immm.at<unsigned char>(299,299) = 255;
	immm.at<unsigned char>(299,300) = 255;
	immm.at<unsigned char>(300,299) = 255;
	immm.at<unsigned char>(300,300) = 255;
	immm.at<unsigned char>(300,301) = 255;
	immm.at<unsigned char>(300,302) = 255;
	immm.at<unsigned char>(300,303) = 255;
	immm.at<unsigned char>(300,304) = 255;
	immm.at<unsigned char>(300,305) = 255;
	
	cv::imshow("before encoding", immm);
	cv::waitKey(0);
	
    /*
	std::vector<unsigned char> buff;
	std::vector<int> param = std::vector<int>(2);
    param[0] = CV_IMWRITE_PNG_COMPRESSION;
    param[1] = 3; //default(3)  0-9.
    cv::imencode(".png", immm, buff, param);*/
	
	std::cout << "sizeof buff: " << to_istring(buff.size()) << std::endl;
	
	// hangs
	cv::Mat immm2 = cv::imdecode(buff, CV_LOAD_IMAGE_ANYDEPTH);
	
	cv::imshow("after decoding", immm2);
	cv::waitKey(0);
	
	std::cout << "done with the image encoding test" << std::endl;
#endif
}

