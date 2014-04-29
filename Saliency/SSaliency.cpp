#include <string>
#include <vector>
#include <iostream>

#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Algs.hpp"
#include "Saliency/SSaliency.hpp"
#include "Saliency/saliency_module_C_SBD.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"


/*extern*/ SaliencyModule_C_SBD* global_SSaliency_module_instance = nullptr; //declared in SSaliency.hpp


void SSaliency :: execute(imgdata_t *imdata){
	setDone(imdata, SSALIENCY);
	std::cout << "SSaliency" << std::endl;
	
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
	
	
	std::vector<std::vector<unsigned char>*>::iterator i = imdata->image_data->begin();
	while(i != imdata->image_data->end())
	{
		global_SSaliency_module_instance->do_saliency(cv::imdecode(**i, CV_LOAD_IMAGE_COLOR));
		
		//after saliency is done with the fullsize image, remove that fullsize image from the message's images vector
		delete (*i);
		i = imdata->image_data->erase(i);
	}
	
	if(imdata->image_data->empty()==false)
	{
		std::cout << "WARNING: message struct's vector of images wasn't fully cleared before saliency started pushing back crops!" << std::endl;
	}
	
	
	consoleOutput.Level1() << "Saliency found "
							<< to_istring(global_SSaliency_module_instance->returned_cropped_images.size())
							<< " crops" << std::endl;
	
	
	for(std::vector<cv::Mat>::iterator criter = global_SSaliency_module_instance->returned_cropped_images.begin();
		criter != global_SSaliency_module_instance->returned_cropped_images.end(); criter++)
	{
		//valgrind doesn't like this ("definitely lost")
		std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
		cv::imencode(".jpg", *criter, *newarr);
		imdata->image_data->push_back(newarr);
	}
	
	
	
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

