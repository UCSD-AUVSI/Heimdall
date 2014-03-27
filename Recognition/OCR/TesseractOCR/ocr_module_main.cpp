/**
 * @file ocr_module_main.cpp
 * @brief The OCR module main interface. Wraps around an OCR algorithm (i.e. Tesseract).
 * @author Jason Bunk
 */

#include "ocr_module_main.hpp"
#include "test_data_results_ocr.hpp"
#include "ocr_algorithm_tesseract.hpp"


TessOCR_Module_Main::TessOCR_Module_Main()
{
    my_ocr_algorithm = new OCRModuleAlgorithm_Tesseract();
}


void TessOCR_Module_Main::DoModule(std::vector<cv::Mat>* input_CSEGs,

        std::ostream* PRINT_TO_FILE_HERE/*=nullptr*/,
        std::string* folder_path_of_output_saved_images/*=nullptr*/,
		bool save_images_and_results/*=false*/,
		std::string* name_of_target_image/*=nullptr*/,
		test_data_results_ocr* optional_results_info/*=nullptr*/,
		std::string* correct_shape_name/*=nullptr*/,
		const char* correct_ocr_character/*=nullptr*/)
{
	last_obtained_results.clear();


    if(input_CSEGs->empty() == false && my_ocr_algorithm != nullptr)
    {
        std::vector<cv::Mat>::iterator CSEGs_iter = input_CSEGs->begin();
        for(; CSEGs_iter != input_CSEGs->end(); CSEGs_iter++)
        {
            Attempt_OCR_OnOneCSEG(*CSEGs_iter,
                save_images_and_results,
                name_of_target_image);
        }

        last_obtained_results.EliminateDuplicates();


        if(last_obtained_results.empty() == false)
        {
            last_obtained_results  =
            last_obtained_results.GetTopNResults(
                my_ocr_algorithm->max_num_characters_to_report,
                my_ocr_algorithm->cutoff_confidence_of_final_result);

            last_obtained_results.SortByConfidence();

            if(PRINT_TO_FILE_HERE != nullptr)
            {
                (*PRINT_TO_FILE_HERE) << std::endl << "============== printing OCR final results:" << std::endl;
                last_obtained_results.PrintMyResults(PRINT_TO_FILE_HERE);
            }
        }
        UpdateResultsAttemptsData_ocr(PRINT_TO_FILE_HERE, optional_results_info, last_obtained_results, correct_ocr_character);
        CheckValidityOfResults_ocr(PRINT_TO_FILE_HERE, optional_results_info, last_obtained_results, correct_ocr_character);
    }
}


void TessOCR_Module_Main::Attempt_OCR_OnOneCSEG(cv::Mat input_CSEG,

	bool save_images_and_results/*=false*/,
	std::string* name_of_target_image/*=nullptr*/,
	test_data_results_ocr* optional_results_info/*=nullptr*/,
	std::string* correct_shape_name/*=nullptr*/,
	const char* correct_ocr_character/*=nullptr*/,
	int test_number/*=-1*/)
{
    //bool I_was_given_a_real_character_and_shape_name_to_compare = (correct_shape_name != nullptr && correct_ocr_character != nullptr);
    //if(I_was_given_a_real_character_and_shape_name_to_compare)
    //    I_was_given_a_real_character_and_shape_name_to_compare = (*correct_ocr_character != 0 && correct_shape_name->empty()==false);


	cv::Mat foundshape_CSEG_post_erosion;


	int erosion_size = 1;
	cv::Mat element;

    std::string guessed_shape_for_saving_file;
    std::string guessed_letter_for_saving_file;



	if(input_CSEG.empty() == false)
	{


//=================================*/
//do OCR
//
		//cv::imshow("OCR_shape_binary_before_dilate", input_CSEG);
		//cv::waitKey(0);



        for(int bbb=0; bbb<=1; bbb++)
        {
            if(bbb > 0)
            {
                erosion_size = bbb;
                element = cv::getStructuringElement(cv::MORPH_CROSS,
                                        cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
                                        cv::Point(erosion_size, erosion_size) );
                cv::erode(input_CSEG, foundshape_CSEG_post_erosion, element);
            }
            else
                input_CSEG.copyTo(foundshape_CSEG_post_erosion);

            //cv::imshow("OCR_shape_binary", foundshape_CSEG_post_erosion);
            //cv::waitKey(0);



            my_ocr_algorithm->do_OCR(foundshape_CSEG_post_erosion, nullptr);


//================================================
#if 0
if(save_images_and_results && name_of_target_image != nullptr  &&  folder_path_of_output_saved_images.empty() == false)
{
    std::string temp___guessed_letter_for_saving_file;
    if(my_ocr_algorithm->last_obtained_results.empty()==false)
    {
        char new2str[2];
        new2str[0] = my_ocr_algorithm->last_obtained_results[0].character;
        new2str[1] = 0;
        temp___guessed_letter_for_saving_file = std::string(new2str);
    }

    saveImage(foundshape_CSEG_post_erosion,
        folder_path_of_output_saved_images + std::string("/CSEG__") + (*name_of_target_image) + std::string("___SegSetting") + to_istring(test_number)
        + std::string("_Erosion") + to_istring(bbb) + std::string("__") + guessed_shape_for_saving_file
        + std::string(",") + temp___guessed_letter_for_saving_file + std::string(".jpg"));
}
#endif
//================================================


            if(my_ocr_algorithm->last_obtained_results.empty() == false)
            {
                //bool found_the_correct_letter = false;

                char newstr[2];
                newstr[0] = my_ocr_algorithm->last_obtained_results[0].character;
                newstr[1] = 0;
                guessed_letter_for_saving_file = std::string(newstr);


				last_obtained_results.results.insert(
					last_obtained_results.results.end(),
					my_ocr_algorithm->last_obtained_results.results.begin(),
					my_ocr_algorithm->last_obtained_results.results.end());
            }
        }
	}
    else
        std::cout << std::string("--- char segmentation fail (so ocr couldn't do anything)") << std::endl;
}
