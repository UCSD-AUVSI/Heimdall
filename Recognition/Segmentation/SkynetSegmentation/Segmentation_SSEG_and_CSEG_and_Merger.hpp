#ifndef ___SKYNET_SSEG_AND_CSEG_AND_MERGER_H___
#define ___SKYNET_SSEG_AND_CSEG_AND_MERGER_H___

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "test_data_results_segmentation.hpp"


class Segmentation_SSEG_MultiReturn;
class Segmentation_CSEG_MultiReturn;
class test_data_results_segmentation;


class Segmentation_SSEG_and_CSEG_and_Merger
{
protected:
    Segmentation_SSEG_MultiReturn* my_sseg_module;
    Segmentation_CSEG_MultiReturn* my_cseg_module;

    void SetupDefaultSettings();

public:
    std::vector<test_data_results_segmentation*> my_test_results_vec;



    Segmentation_SSEG_and_CSEG_and_Merger();
    ~Segmentation_SSEG_and_CSEG_and_Merger();


	void DoModule(cv::Mat cropped_target_image,
                    std::vector<cv::Mat>* returned_SSEGs,
                    std::vector<cv::Scalar>* returned_SSEG_colors,
                    std::vector<cv::Mat>* returned_CSEGs,
                    std::vector<cv::Scalar>* returned_CSEG_colors,

                    std::string* folder_path_of_output_saved_images=nullptr,
                    bool save_images_and_results=false,
                    std::string* name_of_target_image=nullptr,
                    std::vector<test_data_results_segmentation*>* optional_results_info_vec=nullptr,
                    test_data_results_segmentation* master_results_info_segmentation_only=nullptr,
                    bool this_crop_has_a_real_target=false);
};


#endif
