#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include "skynet_VisionUtil.hpp"
#include "SegmentationSettings.hpp"


namespace Skynet { class Segmenter_SingleImageReturn; }
class test_data_results_segmentation;


//
// This class encompasses the whole black box of character segmentation
//
class Segmentation_CSEG_MultiReturn
{
public:
    //the individual character segmentation algorithm
    Skynet::Segmenter_SingleImageReturn* my_segmenter_singleimage_algorithm;


    std::vector<Segmenter_Module_Settings> settings;


//------
    Segmentation_CSEG_MultiReturn();


	void DoModule(cv::Mat cropped_target_image,
        std::vector<cv::Mat>* input_SSEGs,
        std::vector<cv::Scalar>* input_colors_to_ignore,
        std::vector<cv::Mat>* returned_char_segmentations,
        std::vector<cv::Scalar>* returned_char_colors,

        std::string* folder_path_of_output_saved_images=nullptr,
		bool save_images_and_results=false,
		std::string* name_of_target_image=nullptr,
		std::vector<test_data_results_segmentation*>* optional_results_info_vec=nullptr,
		test_data_results_segmentation* master_results_info_segmentation_only=nullptr,
		std::string* correct_shape_name=nullptr,
		const char* correct_ocr_character=nullptr);
};
