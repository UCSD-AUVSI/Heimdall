#include <iostream>
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "SkynetSeg_BackboneInterface.hpp"
#include "Segmentation_SSEG_and_CSEG_and_Merger.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include <unistd.h>//for sleep()

// /*extern*/ Segmentation_SSEG_and_CSEG_and_Merger* global_SkynetSegmentation_module_instance = new Segmentation_SSEG_and_CSEG_and_Merger();
/*extern*/ bool skynetseg_is_currently_running = false;
static volatile bool skynetseg_volatile_is_currently_running = false;


void SkynetSeg :: execute(imgdata_t *imdata)
{
    while(skynetseg_is_currently_running || skynetseg_volatile_is_currently_running)
    {
        sleep(100);
        std::cout << "waiting for SkynetSeg..." << std::endl;
    }
    skynetseg_is_currently_running = true;
    skynetseg_volatile_is_currently_running = true;


	std::cout << "SkynetSeg" << std::endl;

	if(imdata->image_data->empty() == false)
	{
        Segmentation_SSEG_and_CSEG_and_Merger SkynetSegmentation_module_instance;

		//valgrind doesn't like this ("possibly lost")
		cv::Mat cropped_input_image = cv::imdecode(**(imdata->image_data->begin()), CV_LOAD_IMAGE_COLOR);

		std::vector<cv::Mat> returned_SSEGs;
		std::vector<cv::Scalar> returned_SSEG_colors;
		std::vector<cv::Mat> returned_CSEGs;
		std::vector<cv::Scalar> returned_CSEG_colors;

		//these two are optional; they tell the module where to save photos, and what name to assign this crop
		std::string folder_to_save_SSEGs_and_CSEGs("./");
		std::string name_of_input_crop(to_istring(rand()));
		bool save_ssegs_and_csegs = false;


		SkynetSegmentation_module_instance.DoModule(cropped_input_image,
										                    &returned_SSEGs,
										                    &returned_SSEG_colors,
										                    &returned_CSEGs,
										                    &returned_CSEG_colors,
															//optional: save SSEGs and CSEGs
															&folder_to_save_SSEGs_and_CSEGs,
															save_ssegs_and_csegs,
															&name_of_input_crop);


		//pack found images (if any) into the message struct
		for(std::vector<cv::Mat>::iterator iter_sseg = returned_SSEGs.begin();
				iter_sseg != returned_SSEGs.end(); iter_sseg++)
		{
			/*std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
			cv::imencode(".jpg", *iter_sseg, *newarr);
			imdata->sseg_image_data->push_back(newarr);*/
			
			std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
			std::vector<int> param = std::vector<int>(2);
			param[0] = CV_IMWRITE_PNG_COMPRESSION;
			param[1] = 3; //default(3)  0-9, where 9 is smallest compressed size.
			cv::imencode(".png", *iter_sseg, *newarr, param);
			imdata->sseg_image_data->push_back(newarr);
		}

		for(std::vector<cv::Mat>::iterator iter_cseg = returned_CSEGs.begin();
				iter_cseg != returned_CSEGs.end(); iter_cseg++)
		{
			/*std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
			cv::imencode(".jpg", *iter_cseg, *newarr);
			imdata->cseg_image_data->push_back(newarr);*/
			
			std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
			std::vector<int> param = std::vector<int>(2);
			param[0] = CV_IMWRITE_PNG_COMPRESSION;
			param[1] = 3; //default(3)  0-9, where 9 is smallest compressed size.
			cv::imencode(".png", *iter_cseg, *newarr, param);
			imdata->cseg_image_data->push_back(newarr);
		}


        //TODO: give color information to the message (imdata)
        //this may require writing a function that can convert a table of numeric RGB color values to their names as strings
        if(returned_SSEGs.empty() == false)
        {
            for(std::vector<cv::Scalar>::iterator iter_sseg_color = returned_SSEG_colors.begin();
                    iter_sseg_color != returned_SSEG_colors.end(); iter_sseg_color++)
            {
                std::cout << std::string("color found by SSEG in \'BGR\' colorspace: ")
                    << to_sstring((*iter_sseg_color)[0]) << std::string(", ")
                    << to_sstring((*iter_sseg_color)[1]) << std::string(", ")
                    << to_sstring((*iter_sseg_color)[2]) << std::endl;
            }
        }
        if(returned_CSEGs.empty() == false)
        {
            for(std::vector<cv::Scalar>::iterator iter_cseg_color = returned_CSEG_colors.begin();
                    iter_cseg_color != returned_CSEG_colors.end(); iter_cseg_color++)
            {
                std::cout << std::string("color found by CSEG in \'BGR\' colorspace: ")
                    << to_sstring((*iter_cseg_color)[0]) << std::string(", ")
                    << to_sstring((*iter_cseg_color)[1]) << std::string(", ")
                    << to_sstring((*iter_cseg_color)[2]) << std::endl;
            }
        }
	}
	else
		std::cout << "SkynetSeg module wasn't given a crop from saliency!" << std::endl;

	setDone(imdata, SKYNET_SEG);
	skynetseg_is_currently_running = false;
	skynetseg_volatile_is_currently_running = false;
}
