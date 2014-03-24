#include <iostream>
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "SkynetSeg_BackboneInterface.hpp"
#include "Segmentation_SSEG_and_CSEG_and_Merger.hpp"
#include "SharedUtils/SharedUtils.hpp"

/*extern*/ Segmentation_SSEG_and_CSEG_and_Merger* global_SkynetSegmentation_module_instance = new Segmentation_SSEG_and_CSEG_and_Merger();

void SkynetSeg :: execute(imgdata_t *imdata)
{
	std::cout << "NUAUVSI SkynetSeg module beginning" << std::endl;

	if(imdata->image_data->empty() == false)
	{
		cv::Mat cropped_input_image = cv::imdecode(**(imdata->image_data->begin()), CV_LOAD_IMAGE_COLOR);

		std::vector<cv::Mat>* returned_SSEGs = new std::vector<cv::Mat>();
		std::vector<cv::Scalar>* returned_SSEG_colors = new std::vector<cv::Scalar>();
		std::vector<cv::Mat>* returned_CSEGs = new std::vector<cv::Mat>();
		std::vector<cv::Scalar>* returned_CSEG_colors = new std::vector<cv::Scalar>();

		//these two are optional; they tell the module where to save photos, and what name to assign this crop
		std::string folder_to_save_SSEGs_and_CSEGs("./");
		std::string name_of_input_crop(to_istring(rand()));


		global_SkynetSegmentation_module_instance->DoModule(cropped_input_image,
										                    returned_SSEGs,
										                    returned_SSEG_colors,
										                    returned_CSEGs,
										                    returned_CSEG_colors,
															//optional: save SSEGs and CSEGs
															&folder_to_save_SSEGs_and_CSEGs,
															true,
															&name_of_input_crop);


		//pack found images (if any) into the message struct
		for(std::vector<cv::Mat>::iterator iter_sseg = returned_SSEGs->begin();
				iter_sseg != returned_SSEGs->end(); iter_sseg++)
		{
			std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
			cv::imencode(".jpg", *iter_sseg, *newarr);
			imdata->sseg_image_data->push_back(newarr);
		}

		for(std::vector<cv::Mat>::iterator iter_cseg = returned_CSEGs->begin();
				iter_cseg != returned_CSEGs->end(); iter_cseg++)
		{
			std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
			cv::imencode(".jpg", *iter_cseg, *newarr);
			imdata->cseg_image_data->push_back(newarr);
		}


        //TODO: give color information to the message
        //this may require writing a function that can convert a table of numeric RGB color values to their names as strings


        //now that we've packed away this info into the imdata message, we can delete these
		delete returned_SSEGs;
		returned_SSEGs=nullptr;

        delete returned_SSEG_colors;
        returned_SSEG_colors=nullptr;

		delete returned_CSEGs;
		returned_CSEGs=nullptr;

		delete returned_CSEG_colors;
		returned_CSEG_colors=nullptr;
	}
	else
		std::cout << "warning: SkynetSeg module wasn't given a crop from saliency!" << std::endl;

	setDone(imdata, SKYNET_SEG);
}
