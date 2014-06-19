#include <iostream>
#include <opencv2/highgui/highgui.hpp>

#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "GOCR_BackboneInterface.hpp"
#include "ocr_algorithm_gocr.hpp"
#include "SharedUtils/SharedUtils.hpp"

/*extern*/ OCRModuleAlgorithm_GOCR* global_GOCR_module_instance = new OCRModuleAlgorithm_GOCR();

using std::cout;
using std::endl;

void GOCRBackboneInterface :: execute(imgdata_t *imdata, std::string args) {
	std::cout << "GOCR, ID: " << imdata->id  << ", CropID: " << imdata->cropid << std::endl;

	if(global_GOCR_module_instance == nullptr) {
		global_GOCR_module_instance = new OCRModuleAlgorithm_GOCR();
	}
	
	if(imdata->cseg_image_data->empty() == false) {
        std::vector<cv::Mat> given_CSEGs;

        std::vector<std::vector<unsigned char>*>::iterator cseg_iter;
        for(cseg_iter = imdata->cseg_image_data->begin();
            cseg_iter != imdata->cseg_image_data->end();
            cseg_iter++) {
            global_GOCR_module_instance->do_OCR(cv::imdecode(**cseg_iter, CV_LOAD_IMAGE_ANYDEPTH));
        }
        
        imdata->character = "";
        
        char output = global_GOCR_module_instance->ProcessCandidates();
        if(output != '_' && output != '\0') {
            imdata->character += output;
        }
	}

	setDone(imdata, OCR);
}
