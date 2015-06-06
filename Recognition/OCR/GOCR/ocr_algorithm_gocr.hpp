#ifndef ____OCR_ALGORITHM_GOCR_H___
#define ____OCR_ALGORITHM_GOCR_H___


#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include "Recognition/OCR/OCRUtils/ocr_algorithm_template.hpp"

class OCRModuleAlgorithm_GOCR : public OCRModuleAlgorithm_Template {
    public:
        int certainty_lower_bound;
        bool return_empty_characters;

        OCRModuleAlgorithm_GOCR() : OCRModuleAlgorithm_Template() {
            return_empty_characters = false;
            certainty_lower_bound = 80;
            num_angles_to_check = 180; //72, 90, 120, 180
        }

        //"return_raw_tesseract_data" determines if the list of letters is chopped to the fraction above,
        // or if all data at all angles is returned.
	virtual bool do_OCR_on_one_CSEG(cv::Mat letter_binary_mat, std::ostream* PRINTHERE=nullptr, bool return_empty_characters=false);
	virtual bool TryToInitializeMe() {return true;}
        
        std::pair<char, int> ProcessCandidates();

    private:
        void RotateAndRunOCR(cv::Mat matsrc, double angle_amount);

};


#endif
