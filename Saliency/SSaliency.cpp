#include <string>
#include <vector>
#include <iostream>

#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Algs.hpp"
#include "Saliency/SSaliency.hpp"
#include "Saliency/saliency_module_C_SBD.hpp"


/*extern*/ SaliencyModule_C_SBD* global_SSaliency_module_instance = nullptr; //declared in SSaliency.hpp


using std::cout;
using std::endl;

void SSaliency :: execute(imgdata_t *imdata){
	setDone(imdata, SSALIENCY);
	cout << "SSaliency" << endl;
	
	if(global_SSaliency_module_instance == nullptr)
	{
		global_SSaliency_module_instance = new SaliencyModule_C_SBD();
		
		global_SSaliency_module_instance->write_output_to_folder = false;
		global_SSaliency_module_instance->settings.push_back(SaliencySettings()); //defaults to CieLAB color space
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
		cout << "WARNING: message struct's vector of images wasn't fully cleared before saliency started pushing back crops!" << std::endl;
	}
	
	
	for(std::vector<cv::Mat>::iterator criter = global_SSaliency_module_instance->returned_cropped_images.begin();
		criter != global_SSaliency_module_instance->returned_cropped_images.end(); criter++)
	{
		std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
		cv::imencode(".jpg", *criter, *newarr);
		imdata->image_data->push_back(newarr);
	}
	
}
