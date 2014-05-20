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
    float HistSeg_CROP_RESIZE_AMOUNT,

    std::string* folder_path_of_output_saved_images/*=nullptr*/,
    bool save_images_and_results/*=false*/,
    std::string* name_of_crop/*=nullptr*/,
    std::vector<test_data_results_segmentation*>* optional_results_info_vec/*=nullptr*/,
    test_data_results_segmentation* master_results_info_segmentation_only/*=nullptr*/,
    bool this_crop_has_a_real_target/*=false*/)
{
    if(returned_shape_segmentations != nullptr && my_segmenter_singleimage_algorithm != nullptr && returned_shape_colors != nullptr)
    {
//=======================================================================
        int test_number = 0;
        if(optional_results_info_vec == nullptr || optional_results_info_vec->size() < settings.size()) {
            test_number = -1; //don't save to optional results info
        }
        test_data_results_segmentation* all_segmentations_test_data_checker = (master_results_info_segmentation_only != nullptr) ? new test_data_results_segmentation() : nullptr;
		int setting_number = -1;
//=======================================================================
		
        std::vector<Segmenter_Module_Settings>::iterator settings_iter = settings.begin();
        for(; settings_iter != settings.end(); settings_iter++)
        {
//=======================================================================
			setting_number++;
            cv::Mat foundshape_blob_returned_mask;
            cv::Mat findshape_returned_histogrambins_for_saving;
            cv::Mat findshape_returned_preprocessed_for_saving;
            consoleOutput.Level4() << "=================================================================================================" << std::endl;
            consoleOutput.Level2() << std::string("--------------------- starting SSEG setting: ") << to_istring(setting_number) << std::endl;
//=======================================================================

            cv::Scalar returned_blob_color;
            cv::Mat foundshape_filled_binary = my_segmenter_singleimage_algorithm->findShape(cropped_target_image,
                            *settings_iter,
                            HistSeg_CROP_RESIZE_AMOUNT,
                            nullptr, //this would be a set of input colors to avoid, but we don't know what to avoid (this is for CSEG)
                            &returned_blob_color,
                            &foundshape_blob_returned_mask,
                            std::string("Shape Segmentation (SSEG)"),
                            &findshape_returned_histogrambins_for_saving,
                            &findshape_returned_preprocessed_for_saving);

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
            if(test_number >= 0) {
                UpdateResultsAttemptsData_SSEG(&consoleOutput.Level3(), (test_number < 0) ? nullptr : ((*optional_results_info_vec)[test_number]), this_crop_has_a_real_target);
			}
            UpdateResultsAttemptsData_SSEG(&consoleOutput.Level3(), all_segmentations_test_data_checker, this_crop_has_a_real_target);

            if(foundshape_filled_binary.empty() == false) {
                if(test_number >= 0)
                    CheckValidityOfResults_SSEG(&consoleOutput.Level3(), (test_number < 0) ? nullptr : ((*optional_results_info_vec)[test_number]),
                                            foundshape_filled_binary.empty()==false,
                                            this_crop_has_a_real_target,
                                            name_of_crop);
                CheckValidityOfResults_SSEG(&consoleOutput.Level3(), all_segmentations_test_data_checker,
                                        foundshape_filled_binary.empty()==false,
                                        this_crop_has_a_real_target,
                                        name_of_crop);
            }
            if(test_number >= 0)
                test_number++;
#if 0
            if(save_images_and_results && folder_path_of_output_saved_images != nullptr)
            {
#if 0
				saveImage(findshape_returned_histogrambins_for_saving,
					*folder_path_of_output_saved_images + std::string("/") +
					(*name_of_crop) + std::string("___SSegSetting") + to_istring(setting_number) + std::string("_HistBin.bmp"));
				/*saveImage(findshape_returned_preprocessed_for_saving,
					*folder_path_of_output_saved_images + std::string("/") +
					(*name_of_crop) + std::string("___SSegSetting") + to_istring(setting_number) + std::string("_aPreprocessed.bmp"));*/
#endif
				if(foundshape_filled_binary.empty() == false)
				{
				saveImage(foundshape_filled_binary,
					*folder_path_of_output_saved_images + std::string("/") +
					(*name_of_crop) + std::string("___SSegSetting") + to_istring(setting_number) + std::string(".png"));
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
            cv::Mat avg_shape = Average_Several_SingleChannel_CVMats(returned_shape_segmentations, 0.06f, "SSEG at the end of MultiReturn");

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
        if(save_images_and_results && folder_path_of_output_saved_images != nullptr) {
            saveImage(avg_shape,
                *folder_path_of_output_saved_images + std::string("/") +
                (*name_of_crop) + std::string("__SSEG.png"));
        }
#endif
//=======================================================================
        }
    }
    else
        consoleOutput.Level0() << "SHAPE SEGMENTER WARNING: some stuff was nullptr, so didn't return any images!" << std::endl;
}

