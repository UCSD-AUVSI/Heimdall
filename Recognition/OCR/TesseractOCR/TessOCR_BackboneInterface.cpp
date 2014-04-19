#include <iostream>
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "TessOCR_BackboneInterface.hpp"
#include "ocr_module_main.hpp"
#include "ocr_algorithm_tesseract.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include <opencv2/highgui/highgui.hpp>

/*extern*/ TessOCR_Module_Main* global_TessOCR_module_instance = new TessOCR_Module_Main();

void TessOCR :: execute(imgdata_t *imdata) {
	std::cout << "Tesseract OCR" << std::endl;

	if(imdata->cseg_image_data->empty() == false)
	{
        std::vector<cv::Mat> given_CSEGs;

        std::vector<std::vector<unsigned char>*>::iterator cseg_iter;
        for(cseg_iter = imdata->cseg_image_data->begin();
            cseg_iter != imdata->cseg_image_data->end();
            cseg_iter++)
        {
            given_CSEGs.push_back(cv::imdecode(**cseg_iter, CV_LOAD_IMAGE_ANYDEPTH));
        }


		global_TessOCR_module_instance->RotateAndGetLetterCandidates(&given_CSEGs);
		global_TessOCR_module_instance->SiftThroughCandidates(4);
		imdata->character = global_TessOCR_module_instance->GetBestCandidate();
		

		//print top 4 results
        if(global_TessOCR_module_instance->last_obtained_results.empty() == false)
        {
            consoleOutput.Level2() << "OCR found:" << std::endl;
            std::vector<OCR_Result>::iterator char_results_iter;
            for(char_results_iter = global_TessOCR_module_instance->last_obtained_results.results.begin();
                char_results_iter != global_TessOCR_module_instance->last_obtained_results.results.end();
                char_results_iter++)
            {
                char_results_iter->PrintMe(&(consoleOutput.Level2()));
            }
        }
	}

	setDone(imdata, TESS_OCR);
}
