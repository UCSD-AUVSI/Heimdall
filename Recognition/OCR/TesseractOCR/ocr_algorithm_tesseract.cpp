/**
 * @file ocr_algorithm_tesseract.cpp
 * @brief Tesseract-based OCR algorithm
 * @author Jason Bunk
 */

#include "ocr_algorithm_tesseract.hpp"
#include "ocr_results.hpp"
#include <opencv2/opencv.hpp>
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"



bool OCRModuleAlgorithm_Tesseract::RotateAndRunOCR(cv::Mat matsrc, double angle_amount, bool return_empty_characters)
{
	if(tesseract_was_initialized == false)
	{
		if(InitTesseract() == false)
			return false;
	}

	cv::Mat mat_rotated;
	Rotate_CV_Mat(matsrc, angle_amount, mat_rotated);


#if BUILD_WITH_TESSERACT

	TessApi.SetImage(mat_rotated.data,		//image data (in the form of an array of uchars
		mat_rotated.rows,		//image width (pixels)
		mat_rotated.cols,		//image height (pixels)
		mat_rotated.channels(),		//bytes per pixel
		mat_rotated.rows*mat_rotated.channels());	//bytes per line

	TessApi.Recognize(NULL);
	tesseract::ResultIterator* ri = TessApi.GetIterator();
	if(ri != 0)
	{
		const char* symbol = ri->GetUTF8Text(tesseract::RIL_SYMBOL);
		if(symbol != 0)
		{
			float conf = ri->Confidence(tesseract::RIL_SYMBOL);
			all_letter_guesses__internal_to_module.PushBackNew(conf, angle_amount, *symbol);
			return true;
		}
	}
	
	if(return_empty_characters)
		all_letter_guesses__internal_to_module.PushBackNew(0.0, angle_amount, ' ');
#endif

	return false;
}



bool OCRModuleAlgorithm_Tesseract::InitTesseract()
{
	if(tesseract_was_initialized == false)
	{
#if BUILD_WITH_TESSERACT

		const char *language = "eng";
		TessApi.Init( 0, language, tesseract::OEM_DEFAULT );
		//TessApi.SetVariable( "tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
		TessApi.SetVariable( "tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
		TessApi.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
		tesseract_was_initialized = true;

#endif
	}
	return tesseract_was_initialized;
}


bool OCRModuleAlgorithm_Tesseract::do_OCR(cv::Mat letter_binary_mat, std::ostream* PRINTHERE/*=nullptr*/, bool return_raw_tesseract_data/*=false*/, bool return_empty_characters/*=false*/)
{
	all_letter_guesses__internal_to_module.clear();
	last_obtained_results.clear();


	//initialize tesseract, tell it to look for single characters

	if(tesseract_was_initialized == false)
	{
		if(InitTesseract() == false)
		{
			consoleOutput.Level0() << "TesseractOCRModule::do_OCR() error: error initializing tesseract!" << std::endl;
			return false;
		}
	}


	//rotate a bunch and detect chars

	double angle_current=0.0;
	double angle_delta = (360.0 / static_cast<double>(num_angles_to_check));
	for(int bb=0; bb<num_angles_to_check; bb++)
	{
		RotateAndRunOCR(letter_binary_mat, angle_current, return_empty_characters);
		angle_current += angle_delta;
	}


	
	if(return_raw_tesseract_data == false) {
		all_letter_guesses__internal_to_module.KeepOnlyTopFractionOfCharacters(fraction_of_top_characters_to_keep_before_tossing_the_rest);
	}


	if(PRINTHERE != nullptr) {
		(*PRINTHERE) << "----------------------------------------------" << std::endl <<
		"top guesses for this image:" << std::endl << "------------------------" << std::endl;
		all_letter_guesses__internal_to_module.PrintMyResults(PRINTHERE);
		(*PRINTHERE) << "------------------------" << std::endl;
	}


	last_obtained_results = all_letter_guesses__internal_to_module;
	
	
    if(PRINTHERE != nullptr) {
        (*PRINTHERE) << "------------------------------------- top few char results:" << std::endl;
		last_obtained_results.PrintMyResults(PRINTHERE);
	}


	return (last_obtained_results.empty() == false);
}

