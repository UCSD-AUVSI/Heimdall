#ifndef ____OCR_ALGORITHM_GOCR_H___
#define ____OCR_ALGORITHM_GOCR_H___


#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "Recognition/OCR/OCRUtils/ocr_algorithm_template.hpp"


class OCRModuleAlgorithm_GOCR : public OCRModuleAlgorithm_Template
{
public:
	int certainty_lower_bound;
	

	OCRModuleAlgorithm_GOCR()
	{
		certainty_lower_bound = 80;
		num_angles_to_check = 180; //72, 90, 120, 180
		fraction_of_top_characters_to_keep_before_tossing_the_rest = (70.0 / 360.0);
	}


	virtual bool TryToInitializeMe();


	//"return_raw_tesseract_data" determines if the list of letters is chopped to the fraction above,
	// or if all data at all angles is returned.
	//
	virtual bool do_OCR(cv::Mat letter_binary_mat, std::ostream* PRINTHERE=nullptr, bool return_empty_characters=false);
	
	virtual void do_SiftThroughCandidates(OCR_ResultsContainer & given_results,
											int max_num_reported_letters,
											double cutoff_confidence);

protected:

	void RotateAndRunOCR(cv::Mat matsrc, double angle_amount, bool return_empty_characters);
};


#endif
