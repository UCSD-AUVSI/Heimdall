#ifndef ______OCR_ALGORITHM_TEMPLATE_H___
#define ______OCR_ALGORITHM_TEMPLATE_H___


#include "ocr_results.hpp"
#include <opencv2/opencv.hpp>


class OCRModuleAlgorithm_Template
{
public:
    OCR_ResultsContainer last_obtained_results;


//-------------
//settings:
	double cutoff_confidence_of_final_result;
	int num_angles_to_check;
	double fraction_of_top_characters_to_keep_before_tossing_the_rest;
//-------------


	//if the returned vector (in last_obtained_results) is empty, no character was found with the desired confidence
	//the returned vector may have two letters if there were two close guesses found
	virtual bool do_OCR(cv::Mat letter_binary_mat, std::ostream* PRINTHERE, bool return_empty_characters) = 0;
	
	virtual void do_SiftThroughCandidates(OCR_ResultsContainer & given_results,
											int max_num_reported_letters,
											double cutoff_confidence);
	
	
	virtual bool TryToInitializeMe() {return false;}
	
	
	OCRModuleAlgorithm_Template() :
		cutoff_confidence_of_final_result(70.0),
		num_angles_to_check(72),
		fraction_of_top_characters_to_keep_before_tossing_the_rest(70.0 / 360.0)
	{}
	virtual ~OCRModuleAlgorithm_Template() {}
};


#endif
