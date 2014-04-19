#ifndef ______OCR_ALGORITHM_TESSERACT_H___
#define ______OCR_ALGORITHM_TESSERACT_H___


#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "ocr_algorithm_template.hpp"

#define BUILD_WITH_TESSERACT 1

#if BUILD_WITH_TESSERACT
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#endif


class OCRModuleAlgorithm_Tesseract : public OCRModuleAlgorithm_Template
{
protected:
	OCR_ResultsContainer all_letter_guesses__internal_to_module;

	bool tesseract_was_initialized;

#if BUILD_WITH_TESSERACT
	tesseract::TessBaseAPI TessApi;
#endif


public:
    //OCR_ResultsContainer last_obtained_results; //from template


	int num_angles_to_check;
	double fraction_of_top_characters_to_keep_before_tossing_the_rest;



	OCRModuleAlgorithm_Tesseract() :
				num_angles_to_check(72),
				fraction_of_top_characters_to_keep_before_tossing_the_rest(70.0 / 360.0)
		{}


	//"return_raw_tesseract_data" determines if the list of letters is chopped to the fraction above,
	// or if all data at all angles is returned.
	//
	virtual bool do_OCR(cv::Mat letter_binary_mat, std::ostream* PRINTHERE=nullptr, bool return_raw_tesseract_data=false, bool return_empty_characters=false);



protected:

	bool InitTesseract();

	bool RotateAndRunOCR(cv::Mat matsrc, double angle_amount, bool return_empty_characters);
};


#endif
