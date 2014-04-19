#ifndef ______OCR_ALGORITHM_TEMPLATE_H___
#define ______OCR_ALGORITHM_TEMPLATE_H___


#include "ocr_results.hpp"


class OCRModuleAlgorithm_Template
{
public:
    OCR_ResultsContainer last_obtained_results;


//-------------
//settings:
	double cutoff_confidence_of_final_result;
//-------------


	//if the returned vector (in last_obtained_results) is empty, no character was found with the desired confidence
	//the returned vector may have two letters if there were two close guesses found
	virtual bool do_OCR(cv::Mat letter_binary_mat, std::ostream* PRINTHERE, bool return_raw_tesseract_data, bool return_empty_characters) = 0;


	OCRModuleAlgorithm_Template() :
		cutoff_confidence_of_final_result(70.0)
	{}
	virtual ~OCRModuleAlgorithm_Template() {}
};


#endif
