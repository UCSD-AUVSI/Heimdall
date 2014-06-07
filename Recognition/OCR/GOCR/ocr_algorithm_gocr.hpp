#ifndef ____OCR_ALGORITHM_GOCR_H___
#define ____OCR_ALGORITHM_GOCR_H___


#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>

class OCRModuleAlgorithm_GOCR {
    public:
        int certainty_lower_bound, num_angles_to_check;
        bool return_empty_characters;

        OCRModuleAlgorithm_GOCR() {
            return_empty_characters = false;
            certainty_lower_bound = 80;
            num_angles_to_check = 180; //72, 90, 120, 180
        }

        //"return_raw_tesseract_data" determines if the list of letters is chopped to the fraction above,
        // or if all data at all angles is returned.
        bool do_OCR(cv::Mat letter_binary_mat);

        char ProcessCandidates(); 

    private:
        void RotateAndRunOCR(cv::Mat matsrc, double angle_amount);

};


#endif
