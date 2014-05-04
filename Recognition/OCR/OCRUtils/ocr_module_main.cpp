/**
 * @file ocr_module_main.cpp
 * @brief The OCR module main interface. Wraps around an OCR algorithm (i.e. Tesseract).
 * @author Jason Bunk
 */

#include "ocr_module_main.hpp"
#include "test_data_results_ocr.hpp"
//#include "ocr_algorithm_tesseract.hpp"


AnyOCR_Module_Main::AnyOCR_Module_Main() : my_ocr_algorithm(nullptr)
{
    //my_ocr_algorithm = new OCRModuleAlgorithm_Tesseract();
}


void AnyOCR_Module_Main::RotateAndGetLetterCandidates(std::vector<cv::Mat>* input_CSEGs,
								bool return_empty_characters/*=false*/)
{
	if(my_ocr_algorithm == nullptr) {
		std::cout << "Warning: OCR won't do anything because no algorithm was chosen! (nullptr)" << std::endl;
	}
	
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


void AnyOCR_Module_Main::SiftThroughCandidates(int max_num_reported_letters, double cutoff_confidence/*=-99.0*/)
{
	if(my_ocr_algorithm != nullptr) {
		my_ocr_algorithm->do_SiftThroughCandidates(last_obtained_results, max_num_reported_letters, cutoff_confidence);
	}
	
	//UpdateResultsAttemptsData_ocr(PRINT_TO_FILE_HERE, optional_results_info, last_obtained_results, correct_ocr_character);
	//CheckValidityOfResults_ocr(PRINT_TO_FILE_HERE, optional_results_info, last_obtained_results, correct_ocr_character);
}


std::string AnyOCR_Module_Main::GetBestCandidate()
{	
	if(last_obtained_results.empty() == false)
	{
		last_obtained_results.SortByConfidence();
		return last_obtained_results.results.begin()->GetCharacterAsString();
	}
	return std::string("");
}


void AnyOCR_Module_Main::Attempt_OCR_OnOneCSEG(cv::Mat input_CSEG,
												bool return_raw_tesseract_data/*=false*/,
												bool return_empty_characters/*=false*/)//,
	//bool save_images_and_results/*=false*/,
	//std::string* name_of_target_image/*=nullptr*/,
	//test_data_results_ocr* optional_results_info/*=nullptr*/,
	//std::string* correct_shape_name/*=nullptr*/,
	//const char* correct_ocr_character/*=nullptr*/,
	//int test_number/*=-1*/)
{
	if(my_ocr_algorithm == nullptr) {
		return;
	}
	if(my_ocr_algorithm->TryToInitializeMe() == false) {
		std::cout << "error in OCR, algorithm not initialized!" << std::endl;
		return;
	}


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
			
			
			my_ocr_algorithm->last_obtained_results.clear();


            my_ocr_algorithm->do_OCR(foundshape_CSEG_post_erosion, nullptr, return_empty_characters);
			
			
			if(return_raw_tesseract_data == false) {
				my_ocr_algorithm->last_obtained_results.KeepOnlyTopFractionOfCharacters(my_ocr_algorithm->fraction_of_top_characters_to_keep_before_tossing_the_rest);
			}
			
			
			
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
