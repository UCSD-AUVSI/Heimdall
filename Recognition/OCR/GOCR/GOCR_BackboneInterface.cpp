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
            global_GOCR_module_instance->do_OCR_on_one_CSEG(cv::imdecode(**cseg_iter, CV_LOAD_IMAGE_ANYDEPTH));
        }
        
        imdata->character1 = "";
        
        std::pair<char,int> output = global_GOCR_module_instance->ProcessCandidates();
        char outchar = std::get<0>(output);
        int orientation = std::get<1>(output);

        if(outchar != '_' && outchar != '\0') {
            imdata->character1 += outchar;
            imdata->targetorientation = orientation + to_degrees(imdata->planeheading + kPI);
            while(imdata->targetorientation < 0) imdata->targetorientation += 360;
            while(imdata->targetorientation >= 360) imdata->targetorientation -= 360;
        }
	}

	setDone(imdata, OCR);
}
