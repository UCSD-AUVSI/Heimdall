#include "Segmentation_CSEG_MultiReturn.hpp"
#include "test_data_results_segmentation.hpp"
#include "skynet_Segmenter_SingleImageReturn.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"
#include "SegmentationUtils.hpp"




Segmentation_CSEG_MultiReturn::Segmentation_CSEG_MultiReturn()
{
    my_segmenter_singleimage_algorithm = new Skynet::Segmenter_SingleImageReturn();
}
Segmentation_CSEG_MultiReturn::~Segmentation_CSEG_MultiReturn()
{
    if(my_segmenter_singleimage_algorithm != nullptr){delete my_segmenter_singleimage_algorithm;}
}


void Segmentation_CSEG_MultiReturn::DoModule(cv::Mat cropped_target_image,
    std::vector<cv::Mat>* input_SSEGs,
    std::vector<cv::Scalar>* input_colors_to_ignore,
    std::vector<cv::Mat>* returned_char_segmentations,
    std::vector<cv::Scalar>* returned_char_colors,
	float HistSeg_CROP_RESIZE_AMOUNT,

    std::string* folder_path_of_output_saved_images/*=nullptr*/,
    bool save_images_and_results/*=false*/,
    std::string* name_of_crop/*=nullptr*/,
    std::vector<test_data_results_segmentation*>* optional_results_info_vec/*=nullptr*/,
    test_data_results_segmentation* master_results_info_segmentation_only/*=nullptr*/,
    bool this_crop_has_a_real_target/*=false*/)
{
    if(input_SSEGs != nullptr && input_colors_to_ignore != nullptr && returned_char_segmentations != nullptr && returned_char_colors != nullptr && my_segmenter_singleimage_algorithm != nullptr)
    {
		if(input_SSEGs->empty())
		{
			consoleOutput.Level3() << "CSEG module given no SSEGs, so it didn't do anything" << std::endl;
			return;
		}
		
//=======================================================================
        consoleOutput.Level2() << std::endl << std::endl << "doing character segmentation" << std::endl << std::endl;
        int test_number = 0;
        if(optional_results_info_vec == nullptr || optional_results_info_vec->size() < settings.size()) {
            test_number = -1; //don't save to optional results info
        }
        test_data_results_segmentation* all_segmentations_test_data_checker = (master_results_info_segmentation_only != nullptr) ? new test_data_results_segmentation() : nullptr;
        int setting_number = -1;
//=======================================================================


        cv::Scalar mean_color_of_all_input_SSEGs;
        cv::Mat avg_input_sseg;

        if(input_SSEGs->size() > 1)
        {
            avg_input_sseg = Average_Several_SingleChannel_CVMats(input_SSEGs, 0.06f, "CSEG at the beginning of MultiReturn, doing input SSEGs");
            mean_color_of_all_input_SSEGs = Average_Several_CVColors(input_colors_to_ignore);

//=======================================================================
            //saveImage(avg_input_sseg,
            //    *folder_path_of_output_saved_images + std::string("/CSEG__") + (*name_of_crop) + std::string("__AVGd__SegSetting") + to_istring(test_number) + std::string(".bmp"));
//=======================================================================

            cv::threshold(avg_input_sseg, avg_input_sseg, 127, 255, cv::THRESH_BINARY);
            avg_input_sseg.convertTo(avg_input_sseg, CV_8U);

//=======================================================================
            //saveImage(avg_input_sseg,
            //    *folder_path_of_output_saved_images + std::string("/CSEG__") + (*name_of_crop) + std::string("__AVGd222__SegSetting") + to_istring(test_number) + std::string(".bmp"));
//=======================================================================
        }
        else
        {
            input_SSEGs->begin()->copyTo(avg_input_sseg);
            mean_color_of_all_input_SSEGs = (*input_colors_to_ignore->begin());
        }
		
		consoleOutput.Level4() << "about to draw some contours" << std::endl;
		
		
//is the next step (between underscores) necessary?
//reduce noise of SSEG's tiny speckles... redundant... but... tries to crop around only
//									the major regions of the SSEG... not sure how helpful)
//_______________________________________________________________________________________
        std::vector<std::vector<cv::Point>>* contours_in_avg =
                        GetContoursOfMinimumSize__DestroysInputImage(avg_input_sseg, 0.003f);
        avg_input_sseg.setTo(0);
		
        std::vector<cv::Point> all_points_in_all_contours_of_SSEG;
        std::vector<std::vector<cv::Point>>::iterator contours_in_avg_iter = contours_in_avg->begin();
        for(; contours_in_avg_iter != contours_in_avg->end(); contours_in_avg_iter++)
        {
            all_points_in_all_contours_of_SSEG.insert(all_points_in_all_contours_of_SSEG.end(),
                    contours_in_avg_iter->begin(), contours_in_avg_iter->end());
        }
        cv::drawContours(avg_input_sseg, *contours_in_avg, -1, cv::Scalar(255), CV_FILLED, 8);
        delete contours_in_avg;
        contours_in_avg = nullptr;
//_______________________________________________________________________________________
		
		
		if(all_points_in_all_contours_of_SSEG.empty() == false)
		{
			consoleOutput.Level4() << "about to crop for CSEG - step 0" << std::endl;
			
			cv::Rect RectToCropTo = cv::boundingRect(all_points_in_all_contours_of_SSEG);
			cv::Mat cropped_region(cropped_target_image, RectToCropTo);
			cv::Mat cseg_copied_back_to_fullsize(cv::Size(avg_input_sseg.cols, avg_input_sseg.rows), CV_8U, cv::Scalar(0));
			
			cv::Mat submatrix_of_fullsize_that_represents_crop = cseg_copied_back_to_fullsize(RectToCropTo);
			
			
			consoleOutput.Level4() << "finished cropping for CSEG, about to iterate through settings" << std::endl;

//=======================================================================
#if 0
			if(save_images_and_results && folder_path_of_output_saved_images != nullptr)
			{
			saveImage(cropped_region,
				*folder_path_of_output_saved_images + std::string("/") + (*name_of_crop) + std::string("___CSegCrop.bmp"));
			}
#endif
//=======================================================================



			std::vector<Segmenter_Module_Settings>::iterator settings_iter = settings.begin();
			for(; settings_iter != settings.end(); settings_iter++)
			{
//=======================================================================
				setting_number++;
				cv::Mat foundshape_blob_returned_mask;
				cv::Mat findshape_returned_histogrambins_for_saving;
				cv::Mat findshape_returned_preprocessed_for_saving;
				consoleOutput.Level2() << std::string("--------------character segmentation setting number: ") << to_istring(setting_number) << std::endl;
//=======================================================================


				cv::Scalar returned_blob_color;
				cv::Mat foundshape_cseg_blob = my_segmenter_singleimage_algorithm->findShape(cropped_region,
								*settings_iter,
								HistSeg_CROP_RESIZE_AMOUNT,
								settings_iter->HistSeg_MINIMUM_SPECK_SIZE_THRESHOLD_CSEG,
								&mean_color_of_all_input_SSEGs,
								&returned_blob_color,
								&foundshape_blob_returned_mask,
								std::string("Character Segmentation (CSEG)"),
								(folder_path_of_output_saved_images != nullptr ? *folder_path_of_output_saved_images : ""),
								setting_number,
								&findshape_returned_histogrambins_for_saving,
								&findshape_returned_preprocessed_for_saving);


				if(foundshape_cseg_blob.empty() == false)
				{
					returned_char_segmentations->push_back(foundshape_cseg_blob);
					returned_char_colors->push_back(returned_blob_color);

//=======================================================================
					consoleOutput.Level3() << std::string("--color found by CSEG setting: ")
						<< to_sstring((*returned_char_colors->rbegin())[0]) << std::string(",")
						<< to_sstring((*returned_char_colors->rbegin())[1]) << std::string(",")
						<< to_sstring((*returned_char_colors->rbegin())[2]) << std::string(",")
						<< to_sstring((*returned_char_colors->rbegin())[3]) << std::string(",")
						 << std::endl;
//=======================================================================
				}
				

//=======================================================================
				if(test_number >= 0)
					UpdateResultsAttemptsData_CSEG(&consoleOutput.Level3(), (test_number < 0) ? nullptr : ((*optional_results_info_vec)[test_number]), this_crop_has_a_real_target);

				UpdateResultsAttemptsData_CSEG(&consoleOutput.Level3(), all_segmentations_test_data_checker, this_crop_has_a_real_target);

				if(foundshape_cseg_blob.empty() == false)
				{
					if(test_number >= 0)
						CheckValidityOfResults_CSEG(&consoleOutput.Level3(), (test_number < 0) ? nullptr : ((*optional_results_info_vec)[test_number]),
												foundshape_cseg_blob.empty()==false,
												this_crop_has_a_real_target,
												name_of_crop);
					CheckValidityOfResults_CSEG(&consoleOutput.Level3(), all_segmentations_test_data_checker,
											foundshape_cseg_blob.empty()==false,
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
						(*name_of_crop) + std::string("___CSegSetting") + to_istring(setting_number) + std::string("_HistBin.bmp"));
#endif
					if(foundshape_cseg_blob.empty() == false)
					{
					saveImage(foundshape_cseg_blob,
						*folder_path_of_output_saved_images + std::string("/") +
						(*name_of_crop) + std::string("___CSegSetting") + to_istring(setting_number) + std::string(".png"));
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
				cv::Mat avg_shape = Average_Several_SingleChannel_CVMats(returned_char_segmentations, 0.07f, "CSEG at the end of MultiReturn");

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


//=======================================================================
#if 0
				if(save_images_and_results && folder_path_of_output_saved_images != nullptr)
				{
				saveImage(avg_shape,
					*folder_path_of_output_saved_images + std::string("/") + (*name_of_crop) + std::string("___avg_CSeg.jpg"));
				}
#endif
#if 0
				SaveBlobColorCheckImage(cropped_target_image, avg_shape, avg_color,
					*folder_path_of_output_saved_images + std::string("/") + (*name_of_crop) + std::string("_CSEG_ColorCheck.png"));
#endif
//=======================================================================
			}
		}
		else
		{
			consoleOutput.Level2() << "CSEG given blank SSEG, so CSEG was not run" << std::endl;
			returned_char_segmentations->clear();
			returned_char_colors->clear();
		}
    }
    else
        consoleOutput.Level0() << "CHARACTER SEGMENTER WARNING: some stuff was nullptr, so didn't return any images!" << std::endl;
}

