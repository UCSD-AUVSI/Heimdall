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


void TessOCR_Module_Main::RotateAndGetLetterCandidates(std::vector<cv::Mat>* input_CSEGs,
								bool return_empty_characters/*=false*/)
{
	last_obtained_results.clear();


    if(input_CSEGs->empty() == false && my_ocr_algorithm != nullptr)
    {
        std::vector<cv::Mat>::iterator CSEGs_iter = input_CSEGs->begin();
        for(; CSEGs_iter != input_CSEGs->end(); CSEGs_iter++)
        {
            Attempt_OCR_OnOneCSEG(*CSEGs_iter,
				true,//return_raw_tesseract_data,
				return_empty_characters);
        }
    }
}


void TessOCR_Module_Main::SiftThroughCandidates(int max_num_reported_letters, double cutoff_confidence/*=-99.0*/)
{
	last_obtained_results.EliminateDuplicates_ByAveraging();
	
	if(last_obtained_results.empty() == false)
	{
		last_obtained_results  =
		last_obtained_results.GetTopNResults(
			max_num_reported_letters,
			cutoff_confidence >= 0.0 ? cutoff_confidence : my_ocr_algorithm->cutoff_confidence_of_final_result);
		
		last_obtained_results.SortByConfidence();
	}
	//UpdateResultsAttemptsData_ocr(PRINT_TO_FILE_HERE, optional_results_info, last_obtained_results, correct_ocr_character);
	//CheckValidityOfResults_ocr(PRINT_TO_FILE_HERE, optional_results_info, last_obtained_results, correct_ocr_character);
}


std::string TessOCR_Module_Main::GetBestCandidate()
{	
	if(last_obtained_results.empty() == false)
	{
		last_obtained_results.SortByConfidence();
		return last_obtained_results.results.begin()->GetCharacterAsString();
	}
	return std::string("");
}


void TessOCR_Module_Main::Attempt_OCR_OnOneCSEG(cv::Mat input_CSEG,
												bool return_raw_tesseract_data/*=false*/,
												bool return_empty_characters/*=false*/)//,
	//bool save_images_and_results/*=false*/,
	//std::string* name_of_target_image/*=nullptr*/,
	//test_data_results_ocr* optional_results_info/*=nullptr*/,
	//std::string* correct_shape_name/*=nullptr*/,
	//const char* correct_ocr_character/*=nullptr*/,
	//int test_number/*=-1*/)
{
	cv::Mat foundshape_CSEG_post_erosion;
	int erosion_size = 1;
	cv::Mat element;


	if(input_CSEG.empty() == false)
	{
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



            my_ocr_algorithm->do_OCR(foundshape_CSEG_post_erosion, nullptr, return_raw_tesseract_data, return_empty_characters);

			
			//possibility: do "GetTopNResults" here, to keep only the best possibilities between erosions and CSEGs


            if(my_ocr_algorithm->last_obtained_results.empty() == false)
            {
				last_obtained_results.results.insert(
					last_obtained_results.results.end(),
					my_ocr_algorithm->last_obtained_results.results.begin(),
					my_ocr_algorithm->last_obtained_results.results.end());
            }
            
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
        }
	}
    else
        std::cout << std::string("--- char segmentation fail (so ocr couldn't do anything)") << std::endl;
}
