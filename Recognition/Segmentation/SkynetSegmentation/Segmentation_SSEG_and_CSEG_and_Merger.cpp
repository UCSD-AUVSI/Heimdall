#include "Segmentation_SSEG_and_CSEG_and_Merger.hpp"
#include "Segmentation_SSEG_MultiReturn.hpp"
#include "Segmentation_CSEG_MultiReturn.hpp"
#include "Segmentation_CSEG_SSEG_Merger.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"
#include <iostream>
#include <string>


Segmentation_SSEG_and_CSEG_and_Merger::Segmentation_SSEG_and_CSEG_and_Merger()
{
    my_sseg_module = new Segmentation_SSEG_MultiReturn();
    my_cseg_module = new Segmentation_CSEG_MultiReturn();
	SetupDefaultSettings();
}
Segmentation_SSEG_and_CSEG_and_Merger::~Segmentation_SSEG_and_CSEG_and_Merger()
{
    if(my_sseg_module   != nullptr){delete my_sseg_module;}
    if(my_cseg_module   != nullptr){delete my_cseg_module;}

    if(my_test_results_vec.empty() == false) {
        for(int bbb=0; bbb<my_test_results_vec.size(); bbb++) {
            delete my_test_results_vec[bbb];
        }
    }
}


void Segmentation_SSEG_and_CSEG_and_Merger::DoModule(cv::Mat cropped_target_image,
    std::vector<cv::Mat>* returned_SSEGs,
    std::vector<cv::Scalar>* returned_SSEG_colors,
    std::vector<cv::Mat>* returned_CSEGs,
    std::vector<cv::Scalar>* returned_CSEG_colors,

    std::string* folder_path_of_output_saved_images/*=nullptr*/,
    bool save_images_and_results/*=false*/,
    std::string* name_of_target_image/*=nullptr*/,
    std::vector<test_data_results_segmentation*>* optional_results_info_vec/*=nullptr*/,
    test_data_results_segmentation* master_results_info_segmentation_only/*=nullptr*/,
    bool this_crop_has_a_real_target/*=false*/)
{
    if(my_sseg_module==nullptr || my_cseg_module==nullptr)
    {
        consoleOutput.Level0() << "Error: here was a problem with Segmentation_SSEG_and_CSEG_and_Merger having null ptrs!" << std::endl;
        return;
    }

    if(returned_SSEGs != nullptr && returned_SSEG_colors != nullptr && returned_CSEGs != nullptr && returned_CSEG_colors != nullptr)
    {
        returned_SSEGs->clear();
        returned_SSEG_colors->clear();
        returned_CSEGs->clear();
        returned_CSEG_colors->clear();
    
        //----------------------
        //resize the input crop!
        float HistSeg_CROP_RESIZE_AMOUNT = 2.0f;
		cv::Mat resized_mat;
		if(HistSeg_CROP_RESIZE_AMOUNT > 1.0) {
			cv::resize(cropped_target_image, resized_mat, cv::Size(0.0,0.0), HistSeg_CROP_RESIZE_AMOUNT, HistSeg_CROP_RESIZE_AMOUNT, cv::INTER_LINEAR);
		} else {
			cropped_target_image.copyTo(resized_mat);
		}
		//---
		


        my_sseg_module->DoModule(resized_mat,
                                    returned_SSEGs,
                                    returned_SSEG_colors,
                                    HistSeg_CROP_RESIZE_AMOUNT,
                                    //optional
                                    folder_path_of_output_saved_images,
                                    save_images_and_results,
                                    name_of_target_image/*,
                                    optional_results_info_vec,
                                    master_results_info_segmentation_only,
                                    this_crop_has_a_real_target*/);


        my_cseg_module->DoModule(resized_mat,
                                    returned_SSEGs,
                                    returned_SSEG_colors, //avoid segmenting these colors
                                    returned_CSEGs,
                                    returned_CSEG_colors,
                                    HistSeg_CROP_RESIZE_AMOUNT,
                                    //optional
                                    folder_path_of_output_saved_images,
                                    save_images_and_results,
                                    name_of_target_image/*,
                                    optional_results_info_vec,
                                    master_results_info_segmentation_only,
                                    this_crop_has_a_real_target*/);


        Segmentation_CSEG_SSEG_Merger::DoModule(//resized_mat,
                                    returned_SSEGs,
                                    returned_SSEG_colors,
                                    returned_CSEGs,
                                    returned_CSEG_colors,
                                    HistSeg_CROP_RESIZE_AMOUNT
                                    );
		
		
		
		if(returned_SSEGs->empty() == false && returned_SSEGs->begin()->type() != CV_8U) {
			consoleOutput.Level0() << "WARNING: SEGMENTATION'S SSEGS AREN'T IN \'CV_8U\' FORMAT" << std::endl;
		}
		if(returned_CSEGs->empty() == false && returned_CSEGs->begin()->type() != CV_8U) {
			consoleOutput.Level0() << "WARNING: SEGMENTATION'S CSEGS AREN'T IN \'CV_8U\' FORMAT" << std::endl;
		}
		
    }
    else
        consoleOutput.Level0() << "warning: segmentation wasn't given a place to put SSEGs or CSEGs, so it didn't return any" << std::endl;
}




void Segmentation_SSEG_and_CSEG_and_Merger::SetupDefaultSettings()
{
    my_sseg_module->settings.resize(9);
    my_cseg_module->settings.resize(11);
    /*   my_test_results_vec.resize(10,nullptr);
    for(int bbb=0; bbb<my_test_results_vec.size(); bbb++)
    {
        my_test_results_vec[bbb] = new test_data_results_segmentation();
        my_test_results_vec[bbb]->my_test_type = testtype__per_segmentation;
    }*/



	int COLORSPACE_SETTING_RGB_11 =     0;
	int COLORSPACE_SETTING_RGB_22 =     1;
	int COLORSPACE_SETTING_CIELAB_11 =  2;
	int COLORSPACE_SETTING_CIELAB_22 =  3;
	int COLORSPACE_SETTING_CIELAB_33 =  4;
	int COLORSPACE_SETTING_CIELUV_11 =  5;
	int COLORSPACE_SETTING_CIELUV_22 =  6;
	int COLORSPACE_SETTING_CIELUV_33 =  7;
	
	int COLORSPACE_SETTING_CIELAB_L  =  8;
	int COLORSPACE_SETTING_CIELAB_LA =  9;
	int COLORSPACE_SETTING_CIELAB_LB = 10;


	int COLORSPACE_SETTING_CIELAB_44 =  10;
	int COLORSPACE_SETTING_CIELUV_44 =  11;

	int COLORSPACE_SETTING_HSV_11 =     12;
	int COLORSPACE_SETTING_HSV_22 =     13;



	my_sseg_module->settings[COLORSPACE_SETTING_RGB_11].preprocess_CV_conversion_type = COLORSPACE_CONVERSIONTYPE_KEEPRGB;

	my_sseg_module->settings[COLORSPACE_SETTING_RGB_22].preprocess_CV_conversion_type = COLORSPACE_CONVERSIONTYPE_KEEPRGB;
    my_sseg_module->settings[COLORSPACE_SETTING_RGB_22].HistSeg_BLUR_PREPROCESS_RADIUS_PIXELS = 6;

	my_sseg_module->settings[COLORSPACE_SETTING_CIELAB_11].preprocess_CV_conversion_type = CV_BGR2Lab;
	my_sseg_module->settings[COLORSPACE_SETTING_CIELAB_11].preprocess_channels_to_keep[0] = true;

	my_sseg_module->settings[COLORSPACE_SETTING_CIELAB_22].preprocess_CV_conversion_type = CV_BGR2Lab;
	my_sseg_module->settings[COLORSPACE_SETTING_CIELAB_22].preprocess_channels_to_keep[0] = false;

	my_sseg_module->settings[COLORSPACE_SETTING_CIELAB_33].preprocess_CV_conversion_type = CV_BGR2Lab;
	my_sseg_module->settings[COLORSPACE_SETTING_CIELAB_33].HistSeg_BLUR_PREPROCESS_RADIUS_PIXELS = 6;
	my_sseg_module->settings[COLORSPACE_SETTING_CIELAB_33].preprocess_channels_to_keep[0] = true;

	my_sseg_module->settings[COLORSPACE_SETTING_CIELUV_11].preprocess_CV_conversion_type = CV_BGR2Luv;
	my_sseg_module->settings[COLORSPACE_SETTING_CIELUV_11].preprocess_channels_to_keep[0] = true;

	my_sseg_module->settings[COLORSPACE_SETTING_CIELUV_22].preprocess_CV_conversion_type = CV_BGR2Luv;
	my_sseg_module->settings[COLORSPACE_SETTING_CIELUV_22].preprocess_channels_to_keep[0] = false;

	my_sseg_module->settings[COLORSPACE_SETTING_CIELUV_33].preprocess_CV_conversion_type = CV_BGR2Luv;
	my_sseg_module->settings[COLORSPACE_SETTING_CIELUV_33].HistSeg_BLUR_PREPROCESS_RADIUS_PIXELS = 6;
	my_sseg_module->settings[COLORSPACE_SETTING_CIELUV_33].preprocess_channels_to_keep[0] = true;
	
	my_sseg_module->settings[COLORSPACE_SETTING_CIELAB_L].preprocess_CV_conversion_type = CV_BGR2Lab;
	my_sseg_module->settings[COLORSPACE_SETTING_CIELAB_L].HistSeg_BLUR_PREPROCESS_RADIUS_PIXELS = 6;
	my_sseg_module->settings[COLORSPACE_SETTING_CIELAB_L].preprocess_channels_to_keep[0] = true;
	my_sseg_module->settings[COLORSPACE_SETTING_CIELAB_L].preprocess_channels_to_keep[1] = false;
	my_sseg_module->settings[COLORSPACE_SETTING_CIELAB_L].preprocess_channels_to_keep[2] = false;




	my_cseg_module->settings[COLORSPACE_SETTING_RGB_11].preprocess_CV_conversion_type = COLORSPACE_CONVERSIONTYPE_KEEPRGB;

	my_cseg_module->settings[COLORSPACE_SETTING_RGB_22].preprocess_CV_conversion_type = COLORSPACE_CONVERSIONTYPE_KEEPRGB;
    my_cseg_module->settings[COLORSPACE_SETTING_RGB_22].HistSeg_BLUR_PREPROCESS_RADIUS_PIXELS = 6;

	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_11].preprocess_CV_conversion_type = CV_BGR2Lab;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_11].preprocess_channels_to_keep[0] = true;

	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_22].preprocess_CV_conversion_type = CV_BGR2Lab;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_22].preprocess_channels_to_keep[0] = false;

	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_33].preprocess_CV_conversion_type = CV_BGR2Lab;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_33].HistSeg_BLUR_PREPROCESS_RADIUS_PIXELS = 6;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_33].preprocess_channels_to_keep[0] = true;

	my_cseg_module->settings[COLORSPACE_SETTING_CIELUV_11].preprocess_CV_conversion_type = CV_BGR2Luv;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELUV_11].preprocess_channels_to_keep[0] = true;

	my_cseg_module->settings[COLORSPACE_SETTING_CIELUV_22].preprocess_CV_conversion_type = CV_BGR2Luv;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELUV_22].preprocess_channels_to_keep[0] = false;

	my_cseg_module->settings[COLORSPACE_SETTING_CIELUV_33].preprocess_CV_conversion_type = CV_BGR2Luv;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELUV_33].HistSeg_BLUR_PREPROCESS_RADIUS_PIXELS = 6;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELUV_33].preprocess_channels_to_keep[0] = true;

	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_L].preprocess_CV_conversion_type = CV_BGR2Lab;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_L].HistSeg_BLUR_PREPROCESS_RADIUS_PIXELS = 6;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_L].preprocess_channels_to_keep[0] = true;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_L].preprocess_channels_to_keep[1] = false;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_L].preprocess_channels_to_keep[2] = false;
	
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_LA].preprocess_CV_conversion_type = CV_BGR2Lab;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_LA].HistSeg_BLUR_PREPROCESS_RADIUS_PIXELS = 6;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_LA].preprocess_channels_to_keep[0] = true;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_LA].preprocess_channels_to_keep[1] = true;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_LA].preprocess_channels_to_keep[2] = false;
	
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_LB].preprocess_CV_conversion_type = CV_BGR2Lab;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_LB].HistSeg_BLUR_PREPROCESS_RADIUS_PIXELS = 6;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_LB].preprocess_channels_to_keep[0] = true;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_LB].preprocess_channels_to_keep[1] = false;
	my_cseg_module->settings[COLORSPACE_SETTING_CIELAB_LB].preprocess_channels_to_keep[2] = true;

	for(std::vector<Segmenter_Module_Settings>::iterator csegset_iter = my_cseg_module->settings.begin();
                    csegset_iter != my_cseg_module->settings.end(); csegset_iter++)
    {
        csegset_iter->HistSeg_NUM_VALID_COLORS = 12;
        csegset_iter->HistSeg_NUM_BINS = 14;
        csegset_iter->HistSeg_MERGE_COLOR_DISTANCE = 25.0f;
        csegset_iter->HistSeg_COLOR_DISTANCE_TO_PREVIOUSLYFOUND_THRESHOLD = 41.0f;//26.0f;
        csegset_iter->HistSeg_COLOR_DISTANCE_THRESHOLD = 250.0f;
        //csegset_iter->HistSeg_MINIMUM_BLOB_SIZE_THRESHOLD *= 0.5f;
        csegset_iter->HistSeg_PERCENT_OF_CROP_EDGE_TOUCHED_ACCEPTABLE = 0.15f;
        csegset_iter->HistSeg_FILL_IN_SHAPE_BLOB_BEFORE_RETURNING = false;
    }
}
