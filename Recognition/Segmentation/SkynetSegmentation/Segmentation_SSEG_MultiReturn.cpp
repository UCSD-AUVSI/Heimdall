/**
 * @file Segmentation_SSEG_MultiReturn.cpp
 * @brief The full "black box" style SSEG module: given one cropped image, return a bunch of segmentations.
 *        Runs Skynet's single-return segmentation several times, with different settings, and returns all of those results.
 * @author Jason Bunk
 */

#include "Segmentation_SSEG_MultiReturn.hpp"
#include "test_data_results_segmentation.hpp"
#include "skynet_Segmenter_SingleImageReturn.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"




Segmentation_SSEG_MultiReturn::Segmentation_SSEG_MultiReturn()
{
    my_segmenter_singleimage_algorithm = new Skynet::Segmenter_SingleImageReturn();
}
Segmentation_SSEG_MultiReturn::~Segmentation_SSEG_MultiReturn()
{
    if(my_segmenter_singleimage_algorithm != nullptr){delete my_segmenter_singleimage_algorithm;}
}


void Segmentation_SSEG_MultiReturn::DoModule(cv::Mat cropped_target_image,
    std::vector<cv::Mat>* returned_shape_segmentations,
    std::vector<cv::Scalar>* returned_shape_colors,

    std::string* folder_path_of_output_saved_images/*=nullptr*/,
    bool save_images_and_results/*=false*/,
    std::string* name_of_target_image/*=nullptr*/,
    std::vector<test_data_results_segmentation*>* optional_results_info_vec/*=nullptr*/,
    test_data_results_segmentation* master_results_info_segmentation_only/*=nullptr*/,
    std::string* correct_shape_name/*=nullptr*/,
    const char* correct_ocr_character/*=nullptr*/)
{
    if(returned_shape_segmentations != nullptr && my_segmenter_singleimage_algorithm != nullptr && returned_shape_colors != nullptr)
    {
//=======================================================================
        bool I_was_given_a_real_character_and_shape_name_to_compare = (correct_shape_name != nullptr && correct_ocr_character != nullptr);
        if(I_was_given_a_real_character_and_shape_name_to_compare)
            I_was_given_a_real_character_and_shape_name_to_compare = (*correct_ocr_character != 0 && correct_shape_name->empty()==false);

        int test_number = 0;
        if(optional_results_info_vec == nullptr || optional_results_info_vec->size() < settings.size())
        {
            test_number = -1; //don't save to optional results info
        }
        test_data_results_segmentation* all_segmentations_test_data_checker = (master_results_info_segmentation_only != nullptr) ? new test_data_results_segmentation() : nullptr;
//=======================================================================

        std::vector<Segmenter_Module_Settings>::iterator settings_iter = settings.begin();
        for(; settings_iter != settings.end(); settings_iter++)
        {
//=======================================================================
            cv::Mat foundshape_blob_returned_mask;
            cv::Mat foundshape_histogrambins_returned_for_saving;
//=======================================================================

            cv::Scalar returned_blob_color;
            cv::Mat foundshape_filled_binary = my_segmenter_singleimage_algorithm->findShape(cropped_target_image,
                            *settings_iter,
                            nullptr, //this would be a set of input colors to avoid, but we don't know what to avoid (this is for CSEG)
                            &returned_blob_color,
                            &foundshape_blob_returned_mask,
                            std::string("Shape Segmentation (SSEG)"),
                            &foundshape_histogrambins_returned_for_saving);

            if(foundshape_filled_binary.empty() == false)
            {
                returned_shape_segmentations->push_back(foundshape_filled_binary);
                returned_shape_colors->push_back(returned_blob_color);

                consoleOutput.Level3() << std::string("--color found by SSEG setting: ")
                    << to_sstring((*returned_shape_colors->rbegin())[0]) << std::string(",")
                    << to_sstring((*returned_shape_colors->rbegin())[1]) << std::string(",")
                    << to_sstring((*returned_shape_colors->rbegin())[2]) << std::string(",")
                    << to_sstring((*returned_shape_colors->rbegin())[3]) << std::string(",")
                     << std::endl;
            }

//=======================================================================
            if(test_number >= 0)
                UpdateResultsAttemptsData_SSEG(&consoleOutput.Level2(), (test_number < 0) ? nullptr : ((*optional_results_info_vec)[test_number]), I_was_given_a_real_character_and_shape_name_to_compare);

            UpdateResultsAttemptsData_SSEG(&consoleOutput.Level2(), all_segmentations_test_data_checker, I_was_given_a_real_character_and_shape_name_to_compare);


            if(foundshape_filled_binary.empty() == false)
            {
                if(test_number >= 0)
                    CheckValidityOfResults_SSEG(&consoleOutput.Level2(), (test_number < 0) ? nullptr : ((*optional_results_info_vec)[test_number]),
                                            foundshape_filled_binary.empty()==false,
                                            I_was_given_a_real_character_and_shape_name_to_compare,
                                            name_of_target_image);

                CheckValidityOfResults_SSEG(&consoleOutput.Level2(), all_segmentations_test_data_checker,
                                        foundshape_filled_binary.empty()==false,
                                        I_was_given_a_real_character_and_shape_name_to_compare,
                                        name_of_target_image);
            }
            if(test_number >= 0)
                test_number++;
#if 0
            if(save_images_and_results && folder_path_of_output_saved_images != nullptr)
            {
            saveImage(foundshape_histogrambins_returned_for_saving,
                *folder_path_of_output_saved_images + std::string("/SSEG_HISTBINS__") + (*name_of_target_image) + std::string("___SegSetting") + to_istring(test_number) + std::string(".bmp"));
            if(foundshape_filled_binary.empty() == false)
            {
            saveImage(foundshape_filled_binary,
                *folder_path_of_output_saved_images + std::string("/SSEG__") + (*name_of_target_image) + std::string("___SegSetting") + to_istring(test_number) + std::string(".jpg"));
            }
            }
#endif
//=======================================================================
        }

//=======================================================================
	if(master_results_info_segmentation_only != nullptr)
        master_results_info_segmentation_only->Update_My_SSEG_Data_DueToOther(all_segmentations_test_data_checker);
//=======================================================================



        //new: average all of the shapes, to get a smoother, cleaner shape, with less noise ("signal averaging")
        //
        if(returned_shape_segmentations->empty()==false && returned_shape_colors->empty()==false)
        {
            cv::Scalar avg_color = Average_Several_CVColors(returned_shape_colors);
            cv::Mat avg_shape = Average_Several_SingleChannel_CVMats(returned_shape_segmentations, 0.15f);

            if(avg_shape.empty()) //the averager found that the average wasn't very accurate!
                                  //this means segmentation wasn't consistent, so it probably didn't find anything useful
            {
                returned_shape_segmentations->clear();
                returned_shape_colors->clear();
                return;
            }

            cv::threshold(avg_shape, avg_shape, 100, 255, cv::THRESH_BINARY);
            avg_shape.convertTo(avg_shape, CV_8U);

            returned_shape_segmentations->clear();
            returned_shape_colors->clear();

            returned_shape_segmentations->push_back(avg_shape);
            returned_shape_colors->push_back(avg_color);


//=======================================================================
#if 0
        if(save_images_and_results && folder_path_of_output_saved_images != nullptr)
        {
            saveImage(avg_shape,
                *folder_path_of_output_saved_images + std::string("/SSEG__") + (*name_of_target_image) + std::string("___avgshape___SegSetting") + to_istring(test_number) + std::string(".jpg"));
        }
#endif
//=======================================================================
        }
    }
    else
        consoleOutput.Level0() << "SHAPE SEGMENTER WARNING: some stuff was nullptr, so didn't return any images!" << std::endl;
}

