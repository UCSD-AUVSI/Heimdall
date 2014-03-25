#include "Segmentation_CSEG_MultiReturn.hpp"
#include "test_data_results_segmentation.hpp"
#include "skynet_Segmenter_SingleImageReturn.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"




Segmentation_CSEG_MultiReturn::Segmentation_CSEG_MultiReturn()
{
    my_segmenter_singleimage_algorithm = new Skynet::Segmenter_SingleImageReturn();
}


void Segmentation_CSEG_MultiReturn::DoModule(cv::Mat cropped_target_image,
    std::vector<cv::Mat>* input_SSEGs,
    std::vector<cv::Scalar>* input_colors_to_ignore,
    std::vector<cv::Mat>* returned_char_segmentations,
    std::vector<cv::Scalar>* returned_char_colors,

    std::string* folder_path_of_output_saved_images/*=nullptr*/,
    bool save_images_and_results/*=false*/,
    std::string* name_of_target_image/*=nullptr*/,
    std::vector<test_data_results_segmentation*>* optional_results_info_vec/*=nullptr*/,
    test_data_results_segmentation* master_results_info_segmentation_only/*=nullptr*/,
    std::string* correct_shape_name/*=nullptr*/,
    const char* correct_ocr_character/*=nullptr*/)
{
    if(input_SSEGs != nullptr && input_colors_to_ignore != nullptr && returned_char_segmentations != nullptr && returned_char_colors != nullptr && my_segmenter_singleimage_algorithm != nullptr)
    {
		if(input_SSEGs->empty())
		{
			consoleOutput.Level2() << "CSEG module given no SSEGs, so it didn't do anything" << std::endl;
			return;
		}
//=======================================================================
        consoleOutput.Level3() << std::endl << std::endl << "doing character segmentation" << std::endl << std::endl;
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

        cv::Scalar mean_color_of_all_input_SSEGs;
        cv::Mat avg_input_sseg;

        if(input_SSEGs->size() > 1)
        {
            avg_input_sseg = Average_Several_SingleChannel_CVMats(input_SSEGs);
            mean_color_of_all_input_SSEGs = Average_Several_CVColors(input_colors_to_ignore);


//=======================================================================
#if 0
            saveImage(avg_input_sseg,
                *folder_path_of_output_saved_images + std::string("/CSEG__") + (*name_of_target_image) + std::string("__AVGd__SegSetting") + to_istring(test_number) + std::string(".bmp"));
#endif
//=======================================================================

            cv::threshold(avg_input_sseg, avg_input_sseg, 127, 255, cv::THRESH_BINARY);
            avg_input_sseg.convertTo(avg_input_sseg, CV_8U);

//=======================================================================
#if 0
            saveImage(avg_input_sseg,
                *folder_path_of_output_saved_images + std::string("/CSEG__") + (*name_of_target_image) + std::string("__AVGd222__SegSetting") + to_istring(test_number) + std::string(".bmp"));
#endif
//=======================================================================
        }
        else
        {
            input_SSEGs->begin()->copyTo(avg_input_sseg);
            mean_color_of_all_input_SSEGs = (*input_colors_to_ignore->begin());
        }
//=======================================================================


        std::vector<std::vector<cv::Point>>* contours_in_avg =
                        GetContoursOfMinimumSize__DestroysInputImage(avg_input_sseg, 0.003f);

        std::vector<cv::Point> all_points_in_all_contours;
        std::vector<std::vector<cv::Point>>::iterator contours_in_avg_iter = contours_in_avg->begin();
        for(; contours_in_avg_iter != contours_in_avg->end(); contours_in_avg_iter++)
        {
            all_points_in_all_contours.insert(all_points_in_all_contours.end(),
                    contours_in_avg_iter->begin(), contours_in_avg_iter->end());
        }
        avg_input_sseg.setTo(0);
        cv::drawContours(avg_input_sseg, *contours_in_avg, -1, cv::Scalar(255), CV_FILLED, 8);



        cv::Rect RectToCropTo = cv::boundingRect(all_points_in_all_contours);
        cv::Mat cropped_region(cropped_target_image, RectToCropTo);

//
        cv::Mat cseg_copied_back_to_fullsize(cv::Size(avg_input_sseg.cols, avg_input_sseg.rows), CV_8U, cv::Scalar(0));
        cv::Mat submatrix_of_fullsize_that_represents_crop = cseg_copied_back_to_fullsize(RectToCropTo);



//=======================================================================
#if 0
        if(save_images_and_results && folder_path_of_output_saved_images != nullptr)
        {
        saveImage(cropped_region,
            *folder_path_of_output_saved_images + std::string("/CSEG_CROPPED__") + (*name_of_target_image) + std::string("___SegSetting") + to_istring(test_number) + std::string(".bmp"));
        }
#endif
//=======================================================================



        std::vector<Segmenter_Module_Settings>::iterator settings_iter = settings.begin();
        for(; settings_iter != settings.end(); settings_iter++)
        {
//=======================================================================
            cv::Mat foundshape_blob_returned_mask;
            cv::Mat foundshape_histogrambins_returned_for_saving;
            consoleOutput.Level2() << std::string("-----character segmentation setting number: ") << to_istring(test_number+1) << std::endl;
//=======================================================================

            cv::Scalar returned_blob_color;
            cv::Mat foundshape_filled_binary = my_segmenter_singleimage_algorithm->findShape(cropped_region,
                            *settings_iter,
                            &mean_color_of_all_input_SSEGs,
                            &returned_blob_color,
                            &foundshape_blob_returned_mask,
                            std::string("Character Segmentation (CSEG)"),
                            &foundshape_histogrambins_returned_for_saving);

            if(foundshape_filled_binary.empty() == false)
            {
                returned_char_segmentations->push_back(foundshape_filled_binary);
                returned_char_colors->push_back(returned_blob_color);

                consoleOutput.Level3() << std::string("--color found by CSEG setting: ")
                    << to_sstring((*returned_char_colors->rbegin())[0]) << std::string(",")
                    << to_sstring((*returned_char_colors->rbegin())[1]) << std::string(",")
                    << to_sstring((*returned_char_colors->rbegin())[2]) << std::string(",")
                    << to_sstring((*returned_char_colors->rbegin())[3]) << std::string(",")
                     << std::endl;
            }

//=======================================================================
            if(test_number >= 0)
                UpdateResultsAttemptsData_CSEG(&consoleOutput.Level2(), (test_number < 0) ? nullptr : ((*optional_results_info_vec)[test_number]), I_was_given_a_real_character_and_shape_name_to_compare);

            UpdateResultsAttemptsData_CSEG(&consoleOutput.Level2(), all_segmentations_test_data_checker, I_was_given_a_real_character_and_shape_name_to_compare);


            if(foundshape_filled_binary.empty() == false)
            {
                if(test_number >= 0)
                    CheckValidityOfResults_CSEG(&consoleOutput.Level2(), (test_number < 0) ? nullptr : ((*optional_results_info_vec)[test_number]),
                                            foundshape_filled_binary.empty()==false,
                                            I_was_given_a_real_character_and_shape_name_to_compare,
                                            name_of_target_image);

                CheckValidityOfResults_CSEG(&consoleOutput.Level2(), all_segmentations_test_data_checker,
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
                *folder_path_of_output_saved_images + std::string("/CSEG_HISTBINS__") + (*name_of_target_image) + std::string("___SegSetting") + to_istring(test_number) + std::string(".bmp"));
            if(foundshape_filled_binary.empty() == false)
            {
            saveImage(foundshape_filled_binary,
                *folder_path_of_output_saved_images + std::string("/CSEG__") + (*name_of_target_image) + std::string("___SegSetting") + to_istring(test_number) + std::string(".jpg"));
            }
            }
#endif
//=======================================================================
        }

//=======================================================================
	if(master_results_info_segmentation_only != nullptr)
        master_results_info_segmentation_only->Update_My_CSEG_Data_DueToOther(all_segmentations_test_data_checker);
//=======================================================================



        //new: average all of the character shapes, to get a smoother, cleaner character, with less noise ("signal averaging")
        //
        if(returned_char_segmentations->empty()==false && returned_char_colors->empty()==false)
        {
            cv::Scalar avg_color = Average_Several_CVColors(returned_char_colors);
            cv::Mat avg_shape = Average_Several_SingleChannel_CVMats(returned_char_segmentations, 0.15f);

            if(avg_shape.empty()) //the averager found that the average wasn't very accurate!
                                  //this means segmentation wasn't consistent, so it probably didn't find anything useful
            {
                returned_char_segmentations->clear();
                returned_char_colors->clear();
                return;
            }

            cv::threshold(avg_shape, avg_shape, 127, 255, cv::THRESH_BINARY);
            avg_shape.convertTo(avg_shape, CV_8U);


            //right now, "avg_shape" contains the character blob, but in a small cropped section of the size of the input shape segmentations
            //we want to paste this back into a mat of the same size as the input shape segmentations,
            //so that we can combine CSEG and SSEG back into SSEG to fill in the original shape
            avg_shape.copyTo(submatrix_of_fullsize_that_represents_crop);
            avg_shape.release();
            cseg_copied_back_to_fullsize.copyTo(avg_shape);


            /*
            //actually modify the input SSEG; we've eliminated small specks of noise from it, which is useful
            //also,
            //combine the SSEG and the CSEG back into the SSEG; the letter should be contained within the shape anyway!
            cv::max(avg_input_sseg, cseg_copied_back_to_fullsize, *input_SSEGs->begin());
            */


            returned_char_segmentations->clear();
            returned_char_colors->clear();

            returned_char_segmentations->push_back(avg_shape);
            returned_char_colors->push_back(avg_color);


#if 0
            if(save_images_and_results && folder_path_of_output_saved_images != nullptr)
            {
            saveImage(avg_shape,
                *folder_path_of_output_saved_images + std::string("/CSEG__") + (*name_of_target_image) + std::string("___avgchar___SegSetting") + to_istring(test_number) + std::string(".jpg"));
            }
#endif
        }
    }
    else
        consoleOutput.Level0() << "CHARACTER SEGMENTER WARNING: some stuff was nullptr, so didn't return any images!" << std::endl;
}

