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
	double percent_difference_of_total_confidence_of_top_two_characters_to_report_a_guess2;
	double fraction_of_top_characters_to_keep_before_tossing_the_rest;



	OCRModuleAlgorithm_Tesseract() :
				num_angles_to_check(72),
				percent_difference_of_total_confidence_of_top_two_characters_to_report_a_guess2(0.25),
				fraction_of_top_characters_to_keep_before_tossing_the_rest(70.0 / 360.0)
		{}


	//if the returned vector (in last_obtained_results) is empty, no character was found with the desired confidence
	//the returned vector may have two letters if there were two close guesses found
	//
	virtual bool do_OCR(cv::Mat letter_binary_mat, std::ostream* PRINTHERE=nullptr);



protected:

	bool InitTesseract();

	bool RotateAndRunOCR(cv::Mat matsrc, double angle_amount);
};


#endif
