/**
 * @file ocr_algorithm_tesseract.cpp
 * @brief Tesseract-based OCR algorithm
 * @author Jason Bunk
 */

#include "ocr_algorithm_tesseract.hpp"
#include "Recognition/OCR/OCRUtils/ocr_results.hpp"
#include <opencv2/opencv.hpp>
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"
using std::cout; using std::endl;


bool OCRModuleAlgorithm_Tesseract::TryToInitializeMe()
{
	if(tesseract_was_initialized == false)
	{
#if BUILD_WITH_TESSERACT
//#pragma message("BUILDING WITH TESSERACT")
		const char *language = "eng";
		TessApi.Init( 0, language, tesseract::OEM_DEFAULT );
		//TessApi.SetVariable( "tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
		TessApi.SetVariable( "tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");//0123456789");
		TessApi.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
		tesseract_was_initialized = true;
//#else
//#pragma message("NOT BUILDING WITH TESSERACT????????????????????????????????????????????")
#endif
	}
	if(tesseract_was_initialized == false) {
		consoleOutput.Level0() << "TesseractOCRModule::TryToInitializeMe() error: error initializing tesseract!" << std::endl;
	}
	return tesseract_was_initialized;
}


bool OCRModuleAlgorithm_Tesseract::RotateAndRunOCR(cv::Mat matsrc, double angle_amount, bool return_empty_characters)
{
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
			last_obtained_results.PushBackNew(conf, angle_amount, *symbol);
			return true;
		}
	}
	
	if(return_empty_characters)
		last_obtained_results.PushBackNew(0.0, angle_amount, ' ');
#endif

	return false;
}


bool OCRModuleAlgorithm_Tesseract::do_OCR_on_one_CSEG(cv::Mat letter_binary_mat, std::ostream* PRINTHERE/*=nullptr*/, bool return_empty_characters/*=false*/)
{
	last_obtained_results.clear();


	//rotate a bunch and detect chars

	double angle_current=0.0;
	double angle_delta = (360.0 / static_cast<double>(num_angles_to_check));
	for(int bb=0; bb<num_angles_to_check; bb++)
	{
		RotateAndRunOCR(letter_binary_mat, angle_current, return_empty_characters);
		angle_current += angle_delta;
	}


	return (last_obtained_results.empty() == false);
}




