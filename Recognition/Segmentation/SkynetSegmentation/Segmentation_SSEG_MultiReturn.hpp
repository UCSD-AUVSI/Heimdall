#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include "skynet_VisionUtil.hpp"
#include "SegmentationSettings.hpp"


namespace Skynet { class Segmenter_SingleImageReturn; }
class test_data_results_segmentation;


//
// This class encompasses the whole black box of shape segmentation,
// i.e. given one crop from saliency, return a bunch of shape segmentations
// (multiple because there are multiple settings)
//
class Segmentation_SSEG_MultiReturn
{
public:
    //the individual shape segmentation algorithm
    Skynet::Segmenter_SingleImageReturn* my_segmenter_singleimage_algorithm;


    std::vector<Segmenter_Module_Settings> settings;


//------
    Segmentation_SSEG_MultiReturn();
    ~Segmentation_SSEG_MultiReturn();


	void DoModule(cv::Mat cropped_target_image,
        std::vector<cv::Mat>* returned_shape_segmentations,
        std::vector<cv::Scalar>* returned_shape_colors,
        float HistSeg_CROP_RESIZE_AMOUNT,

        std::string* folder_path_of_output_saved_images=nullptr,
		bool save_images_and_results=false,
		std::string* name_of_crop=nullptr,
		std::vector<test_data_results_segmentation*>* optional_results_info_vec=nullptr,
		test_data_results_segmentation* master_results_info_segmentation_only=nullptr,
		bool this_crop_has_a_real_target=false);
};
